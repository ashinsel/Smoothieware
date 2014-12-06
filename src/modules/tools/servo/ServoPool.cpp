/*
 * ServoPool.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: heidi
 */

using namespace std;
#include <vector>
#include <stdint.h>

#include "ServoPool.h"
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "Servo.h"

#define servo_checksum CHECKSUM("servo")
#define enable_checksum CHECKSUM("enable")

void ServoPool::load_tools()
{
	vector<uint16_t> modules;
	THEKERNEL->config->get_module_list( &modules, servo_checksum );

	for( unsigned int i = 0; i < modules.size(); i++ ) {
		// If the module is enabled
		if( THEKERNEL->config->value(servo_checksum, modules[i], enable_checksum )->as_bool() == true ) {
			Servo *servo = new Servo(modules[i]);
			THEKERNEL->add_module(servo);
		}
	}
}

