/*
 * ServoPool.cpp
 *
 *  Created on: Dec 6, 2014
 *      Author: heidi
 */

#include "libs/Module.h"
#include "libs/Kernel.h"
using namespace std;
#include <vector>
#include "ServoPool.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "StreamOutputPool.h"

#define servo_checksum  CHECKSUM("servo")
#define enable_checksum CHECKSUM("enable")

void ServoPool::load_tools()
{
	vector<uint16_t> modules;
	THEKERNEL->config->get_module_list( &modules, servo_checksum );
	THEKERNEL->streams->printf("Loaded some servos, maybe\n");
}
