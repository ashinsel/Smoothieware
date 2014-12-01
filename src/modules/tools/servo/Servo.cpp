/*
 * Servo.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: heidi
 */

#include "libs/Module.h"
#include "libs/Kernel.h"
#include "libs/nuts_bolts.h"
#include "libs/Pin.h"

#include "Servo.h"
#include "StreamOutputPool.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "PwmOut.h"
#include "Gcode.h"

#define servo_module_enable_checksum				CHECKSUM("servo_module_enable")
#define servo_module_pin_checksum					CHECKSUM("servo_module_pin")
#define servo_module_period_checksum				CHECKSUM("servo_module_period")
#define servo_module_deactivation_delay_s_checksum	CHECKSUM("servo_module_deactivation_delay_s")

Servo::Servo() {
	this->deactivation_delay_s = 0;
	this->deactivation_delay_s_value = 0;
	this->angle = 0;
	this->servo_pin = NULL;
}

void Servo::on_module_loaded() {
	if (!THEKERNEL->config->value( servo_module_enable_checksum )->by_default(false)->as_bool() ) {
		// This module is not loaded, free up it's RAM.
		delete this;
		return;
	}

	// Get the smoothie-style pin from config
	Pin* dummy_pin = new Pin();
	dummy_pin->from_string(THEKERNEL->config->value(servo_module_pin_checksum)->by_default("nc")->as_string())->as_output();

	// Convert the smoothie-style pin into an mbed PwmOut, then free up the
	// dummy pin's memory.
	servo_pin = dummy_pin->as_output()->hardware_pwm();
	delete dummy_pin;
	dummy_pin = NULL;

	// Did we get a valid PwmOut object? If not, the config file probably
	// specified a pin that doesn't have hardware PWM capability.
	if (servo_pin == NULL)
	{
		THEKERNEL->streams->printf("ERROR: Servo cannot use P%d.%d (P2.0 - P2.5, P1.18, P1.20, P1.21, P1.23, P1.24, P1.26, P3.25, P2.26 only). Servo module disabled.\n", dummy_pin->port_number, dummy_pin->pin);
		delete this;
		return;
	}

	// Retrieve the pwm period from config
	int period = THEKERNEL->config->value(servo_module_period_checksum)->by_default(2000)->as_int();
	this->servo_pin->period_us(period);

	this->register_for_event(ON_GCODE_RECEIVED);

	this->deactivation_delay_s_value =
			THEKERNEL->config->value(servo_module_deactivation_delay_s_checksum)->by_default(0)->as_int();
	if (this->deactivation_delay_s_value > 0)
	{
		this->register_for_event(ON_SECOND_TICK);
	}
}

void Servo::on_gcode_received(void* argument) {
	Gcode* gcode = static_cast<Gcode*>(argument);
	if( gcode->has_g ) {
		int code = gcode->g;
		if (code == 401) {
			this->set_angle(120);
		} else if (code == 402) {
			this->set_angle(70);
		} else if (code == 403) {
			if ( gcode->has_letter('S' )) {
				int degrees = gcode->get_uint('S');
				this->set_angle(degrees);
			} else {
				gcode->stream->printf("Servo at %d degrees.\n", this->get_angle());
			}
		} else if (code == 404) {
			this->servo_pin->write(1);
		}
	}
}

void Servo::on_second_tick(void *)
{
    // we are timing out to release the servo
    if (this->deactivation_delay_s > 0) {
        if (--this->deactivation_delay_s == 0) {
            this->servo_pin->write(0);
        }
    }
}

void Servo::set_angle(int angle)
{
	// TODO: check to see if angle is valid
	float writeVal = (float)angle/180.0F;
	this->servo_pin->write(writeVal);

	this->angle = angle;
	this->deactivation_delay_s = this->deactivation_delay_s_value;
}

int Servo::get_angle(void)
{
	return this->angle;
}
