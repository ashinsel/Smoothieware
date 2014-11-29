/*
 * Servo.h
 *
 *  Created on: Nov 28, 2014
 *      Author: heidi
 */

#ifndef SRC_MODULES_TOOLS_SERVO_SERVO_H_
#define SRC_MODULES_TOOLS_SERVO_SERVO_H_

#include "libs/Module.h"

namespace mbed {
	class PwmOut;
}

class Servo: public Module {
	public:
		Servo();
		virtual ~Servo() {};
		void on_module_loaded();
		void on_gcode_received(void* argument);
		void on_second_tick(void* argument);

	private:
		mbed::PwmOut *servo_pin;	// PWM output for the servo
		uint16_t angle;
		void set_angle(int angle);
		int get_angle(void);
		uint8_t deactivation_delay_s;
		uint8_t deactivation_delay_s_value;
};

#endif /* SRC_MODULES_TOOLS_SERVO_SERVO_H_ */
