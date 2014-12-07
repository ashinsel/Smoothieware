/*
 * Servo.h
 *
 *  Created on: Dec 6, 2014
 *      Author: heidi
 */

#ifndef SRC_MODULES_TOOLS_SERVO_SERVO_H_
#define SRC_MODULES_TOOLS_SERVO_SERVO_H_

#include <stdint.h>
#include "libs/Module.h"
#include "Pin.h"

class Servo: public Module {
	public:
		Servo();
		Servo(uint16_t name);
		void on_module_loaded(void);
		void on_config_reload(void* argument);

	private:
		uint16_t name_checksum;
		Pin      output_pin;
};

#endif /* SRC_MODULES_TOOLS_SERVO_SERVO_H_ */
