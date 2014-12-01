/*
 * AxisPool.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: heidi
 */

#include "libs/Module.h"
#include "libs/Kernel.h"
using namespace std;
#include <vector>
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "AxisPool.h"
#include "Axis.h"

#define axis_checksum		CHECKSUM("axis")
#define enable_checksum		CHECKSUM("enable")

void AxisPool::load_tools()
{
	vector<uint16_t> modules;
	THEKERNEL->config->get_module_list( &modules, axis_checksum );

	for( unsigned int i = 0; i < modules.size(); i++ ) {
		// If module is enabled
		if( THEKERNEL->config->value(axis_checksum, modules[i], enable_checksum )->as_bool() == true ) {
			Axis *axis = new Axis(modules[i]);
			THEKERNEL->add_module(axis);
		}
	}
}
