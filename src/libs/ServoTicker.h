/*
 * ServoTicker.h
 *
 *  Created on: Nov 30, 2014
 *      Author: heidi
 */

#ifndef SRC_LIBS_SERVOTICKER_H_
#define SRC_LIBS_SERVOTICKER_H_

using namespace std;
#include <vector>
#include <stdint.h>

class ServoTicker {
public:
	static ServoTicker* global_servo_ticker;

	ServoTicker();
	virtual ~ServoTicker();

	void set_period( float period );

private:
	uint32_t period;
	Servo* currentServo;
};

#endif /* SRC_LIBS_SERVOTICKER_H_ */
