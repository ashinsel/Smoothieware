/*
 * Servo.h
 *
 *  Created on: Dec 6, 2014
 *      Author: heidi
 */

#ifndef SRC_MODULES_TOOLS_SERVO_SERVO_H_
#define SRC_MODULES_TOOLS_SERVO_SERVO_H_

#include <stdint.h>
#include "Pin.h"
using namespace std;
#include <string>

class PwmOut;
class Gcode;

class Servo: public Module {
	public:
		Servo();
		Servo(uint16_t name);
		void on_module_loaded(void);
		void on_config_reload(void* argument);
		void on_gcode_received(void* argument);
		void on_gcode_execute(void* argument);
		uint32_t deactivate(uint32_t);

	private:
		bool match_set_angle_gcode(const Gcode* gcode) const;
		void set_angle(uint16_t angle);
		bool is_valid_angle(uint16_t angle);
		int deactivation_delay_ms;

		mbed::PwmOut   *pwm_pin;
		uint16_t name_checksum;
		Pin      output_pin;
		string   set_angle_command;
		char     set_angle_command_letter;
		uint16_t set_angle_command_code;
		uint16_t angle;
		uint16_t min_angle;
		uint16_t max_angle;
};

#endif /* SRC_MODULES_TOOLS_SERVO_SERVO_H_ */
