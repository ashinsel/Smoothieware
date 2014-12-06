/*
 * Servo.h
 *
 *  Created on: Nov 28, 2014
 *      Author: heidi
 */

#ifndef SRC_MODULES_TOOLS_SERVO_SERVO_H_
#define SRC_MODULES_TOOLS_SERVO_SERVO_H_

#include "Pin.h"
using namespace std;
#include <string>

namespace mbed {
	class PwmOut;
}

class Servo: public Module {
	public:
		Servo();
		Servo(uint16_t name);

		virtual ~Servo() {};
		void on_module_loaded();
		void on_config_reload(void* argument);
		void on_gcode_received(void* argument);
		void on_second_tick(void* argument);

	private:
		uint16_t name_checksum;
		Pin			output_pin;
		mbed::PwmOut *servo_pin;	// PWM output for the servo
		uint16_t angle;
		void set_angle(int angle);
		int get_angle(void);
		uint16_t deactivation_delay_ms;
		uint8_t deactivation_delay_s_value;
		string set_angle_command;
};

#endif /* SRC_MODULES_TOOLS_SERVO_SERVO_H_ */
