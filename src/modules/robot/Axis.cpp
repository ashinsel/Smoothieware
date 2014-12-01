/*
 * Axis.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: heidi
 */

#include "Axis.h"

Axis::Axis() {}

Axis::Axis(uint16_t name)
{
	this->name_checksum = name;

}

void Axis::on_module_loaded()
{

    this->on_config_reload(this);
}

void Axis::on_config_reload(void* argument)
{

	THEKERNEL->streams->printf("Loaded Axis %d\n", this->name_checksum);
}
