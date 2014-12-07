/*
 * Servo.cpp
 *
 *  Created on: Dec 6, 2014
 *      Author: heidi
 */

#include "libs/Kernel.h"
#include "Servo.h"
#include "StreamOutputPool.h"
#include "ServoPublicAccess.h"
#include "Pin.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"

#define output_pin_checksum		CHECKSUM("output_pin")

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

	// Load the settings from the Config Source.
	this->on_config_reload(this);
}

/**
 * Loads the configuration source, and configures the pin.
 * @param argument
 */
void Servo::on_config_reload(void* argument)
{
	this->output_pin.from_string(THEKERNEL->config->value(servo_checksum, this->name_checksum, output_pin_checksum )->by_default("nc")->as_string())->as_output();
	THEKERNEL->streams->printf("SERVO: Connected P%d.%02d to Servo %04X\n", this->output_pin.port_number, this->output_pin.pin, this->name_checksum);
}

