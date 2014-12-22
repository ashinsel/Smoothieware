/*
 * Servo.cpp
 *
 *  Created on: Dec 6, 2014
 *      Author: heidi
 */

#include "libs/Kernel.h"
#include "libs/Module.h"

#include "modules/robot/Conveyor.h"
#include "system_LPC17xx.h"

#include "Servo.h"
#include "StreamOutputPool.h"
#include "ServoPublicAccess.h"
#include "Pin.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "Gcode.h"
#include "PwmOut.h"
#include "SlowTicker.h"


#define output_pin_checksum		       CHECKSUM("output_pin")
#define set_angle_command_checksum     CHECKSUM("set_angle_command")
#define min_angle_checksum             CHECKSUM("min_angle")
#define max_angle_checksum             CHECKSUM("max_angle")
#define deactivation_delay_ms_checksum CHECKSUM("deactivation_delay_ms")

Servo::Servo() {}

/**
 * Servo Constructor
 * Called from ServoPool::load_tools().  The servo will be (re)initialized every
 * time that on_config_reload() is called, so in this constructor, we just need
 * to save the name checksum.
 * @param name the checksum of the name of this Servo from the config file
 */
Servo::Servo(uint16_t name)
{
	this->name_checksum = name;
	THEKERNEL->streams->printf("Created servo %04X\n", name);
}

/**
 * Load the configuration for a Servo.  Registers for the required events.
 */
void Servo::on_module_loaded(void)
{
	this->register_for_event(ON_GCODE_RECEIVED);
	this->register_for_event(ON_GCODE_EXECUTE);
	this->register_for_event(ON_MAIN_LOOP);

	// Load the settings from the Config Source.
	this->on_config_reload(this);
}

/**
 * Loads the configuration source, and configures the pin.
 * @param argument
 */
void Servo::on_config_reload(void* argument)
{
	// Get a smoothie style pin to convert to a mbed pwm pin
	this->output_pin.from_string(THEKERNEL->config->value(servo_checksum, this->name_checksum, output_pin_checksum )->by_default("nc")->as_string())->as_output();
	pwm_pin = output_pin.hardware_pwm();
	if (pwm_pin == NULL) {
		THEKERNEL->streams->printf("ERROR: SERVO %04X: Must use Hardware PWM pin.\n", this->name_checksum);
		delete this;
		return;
	} else {
		THEKERNEL->streams->printf("SERVO %04X: Connected P%d.%02d to Servo.\n", this->name_checksum, this->output_pin.port_number, this->output_pin.pin);
		this->pwm_pin->period_us(20000);
		this->pwm_pin->write(0);
	}

	this->set_angle_command = THEKERNEL->config->value(servo_checksum, this->name_checksum, set_angle_command_checksum )->by_default("")->as_string();
	set_angle_command_letter = 0;
	set_angle_command_code = 0;
	if (!set_angle_command.empty()) {
		Gcode gc(set_angle_command, NULL);
		if (gc.has_g) {
			set_angle_command_letter = 'G';
			set_angle_command_code = gc.g;
		} else if (gc.has_m) {
			set_angle_command_letter = 'M';
			set_angle_command_code = gc.m;
		}
	}
	THEKERNEL->streams->printf("SERVO %04X: Set Angle Command: %s\n", this->name_checksum, this->set_angle_command.c_str());

	this->deactivation_delay_ms = THEKERNEL->config->value(servo_checksum, this->name_checksum, deactivation_delay_ms_checksum)->by_default(0)->as_int();
//	if (this->deactivation_delay_ms > 0)
//	{
//		// See if the systick timer has already been set up
//		if (SysTick->LOAD == 0)
//		{
//			// Set up the system tick to be at 1ms.
//		    SysTick_Config((SystemCoreClock / 1000) - 1, true);
//		}
//	}
	THEKERNEL->streams->printf("SERVO %04X: Deactivation delay is %dms.\n", this->name_checksum, this->deactivation_delay_ms);

	this->min_angle = THEKERNEL->config->value(servo_checksum, this->name_checksum, min_angle_checksum)->by_default(80)->as_int();
	this->max_angle = THEKERNEL->config->value(servo_checksum, this->name_checksum, max_angle_checksum)->by_default(100)->as_int();
	THEKERNEL->streams->printf("SERVO %04X: Min angle:%d, Max angle:%d\n", this->name_checksum, min_angle, max_angle);
}

/**
 * Checks to see if a Gcode is the Gcode used to set the servo angle.
 * @param gcode the Gcode to check
 * @return true if the Gcode is the code that sets the servo angle.
 */
bool Servo::match_set_angle_gcode(const Gcode *gcode) const
{
	return ((set_angle_command_letter == 'M' && gcode->has_m && gcode->m == set_angle_command_code) ||
			(set_angle_command_letter == 'G' && gcode->has_g && gcode->g == set_angle_command_code));
}

/**
 * Checks for a relevant GCode.
 * This will add the gcode to the gcode queue if it is used by us.
 * @param argument the GCode received
 */
void Servo::on_gcode_received(void* argument)
{
	// Get the GCode.
	Gcode *gcode = static_cast<Gcode *>(argument);

	// Add the code to the queue if we need it.
	if (match_set_angle_gcode(gcode)) {
		THEKERNEL->conveyor->append_gcode(gcode);
	}
}

/**
 * Checks to see if the angle is in the range of specified max and min angles
 * from the config source
 * @param angle the angle to test
 * @return true if the angle is valid.
 */
bool Servo::is_valid_angle(uint16_t angle)
{
	return (angle >= this->min_angle && angle <= this->max_angle);
}

/**
 * Called when a GCode is executed
 * @param argument the gcode that is being executed
 */
void Servo::on_gcode_execute(void* argument)
{
	Gcode *gcode = static_cast<Gcode *>(argument);

	if (match_set_angle_gcode(gcode))
	{
		if (gcode->has_letter('S')) {
			uint16_t angle = gcode->get_value('S');
			if (is_valid_angle(angle)) {
				this->set_angle(angle);
			} else {
				gcode->stream->printf("Invalid angle for Servo. Min=%d, Max=%d, Requested=%d\n", this->min_angle, this->max_angle, angle);
			}
		}
	}
}

void Servo::on_main_loop(void* argument)
{
//	if (this->isActive && this->deactivation_delay_ms > 0)
//	{
//		// Convert the deactivation delay into ticks.
//		uint32_t deactivation_ticks = SystemCoreClock / 1000 * deactivation_delay_ms;
//		// Calculate the turn off time
//		uint32_t off_time = this->on_timestamp + deactivation_ticks;
//		uint32_t now_timestamp = SysTick->LOAD - SysTick->VAL;
//	}
}

float scale(float val, float oldmin, float oldmax, float newmin, float newmax)
{
	float oldrange = oldmax - oldmin;
	float newrange = newmax - newmin;
	return (((val - oldmin) * newrange) / oldrange) + newmin;
}

uint32_t Servo::deactivate(uint32_t data)
{
	(void)data;
	this->pwm_pin->write(0);
	THEKERNEL->streams->printf("SERVO %04X: Deactivated\n", this->name_checksum);
	return 0;
}

/**
 * Sets the angle of a servo
 * @param angle the new angle for the servo
 */
void Servo::set_angle(uint16_t angle)
{
	THEKERNEL->streams->printf("SERVO %04X: Setting Angle to %d\n", this->name_checksum, angle);
	// Make sure the angle is valid
	if  (this->is_valid_angle(angle)) {
		float value = scale(angle, 0, 180, 544, 2400);
		THEKERNEL->streams->printf("SERVO %04X: Setting PulseWidth to %.1fus\n", this->name_checksum, value);
		this->pwm_pin->pulsewidth_us(value);
		this->angle = angle;

		if (this->deactivation_delay_ms > 0)
		{
//			this->on_timestamp = SysTick->LOAD - SysTick->VAL;

//			uint32_t frequency = (1000 / this->deactivation_delay_ms);
//            THEKERNEL->streams->printf("Frequency:%d, Interval:%d\n", frequency, int(floor((SystemCoreClock/4)/frequency)));
			THEKERNEL->streams->printf("Attaching timer at frequency %.1fHz\n", 1000.0 / this->deactivation_delay_ms);
			THEKERNEL->slow_ticker->attach(1000.0 / this->deactivation_delay_ms, this, &Servo::deactivate, true);
		}
	}
}
