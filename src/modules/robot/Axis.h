/*
 * Axis.h
 *
 *  Created on: Nov 29, 2014
 *      Author: heidi
 */

#ifndef SRC_MODULES_ROBOT_AXIS_H_
#define SRC_MODULES_ROBOT_AXIS_H_

#include "libs/Module.h"
#include "libs/Kernel.h"

class Axis: public Module {
public:
	Axis();
	Axis(uint16_t name);

    void on_module_loaded();
    void on_config_reload(void* argument);

private:
	uint16_t name_checksum;
};

#endif /* SRC_MODULES_ROBOT_AXIS_H_ */
