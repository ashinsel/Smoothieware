/*
 * ServoTicker.cpp
 *
 *  Created on: Nov 30, 2014
 *      Author: heidi
 */

#include "ServoTicker.h"

#include "libs/LPC17xx/sLPC17xx.h" // smoothed mbed.h lib
#include "libs/LPC17xx/LPC17xxLib/inc/lpc17xx_timer.h"
#include "libs/LPC17xx/LPC17xxLib/inc/lpc17xx_clkpwr.h"
#include "system_LPC17xx.h" // mbed.h lib
#include <math.h>

ServoTicker* ServoTicker::global_servo_ticker;

// Default servo period to 20ms.  Most R/C servos use this refresh rate
#define SERVO_DEFAULT_PERIOD_US		(20000)

ServoTicker::ServoTicker() {
    ServoTicker::global_servo_ticker = this;

    // Power on the timer peripheral.
    LPC_SC->PCONP |= CLKPWR_PCONP_PCTIM3;

    // Configure the Timer.  Use Timer 3 for the servos
    this->set_period(SERVO_DEFAULT_PERIOD_US);
    LPC_TIM3->MCR = TIM_INT_ON_MATCH(0);
    LPC_TIM3->TCR = 0;
}

ServoTicker::~ServoTicker() {
	// TODO Auto-generated destructor stub
}

void ServoTicker::set_period( float period ){
	// No need to update the period if it is already correct.
	if (this->period == period)	{
		return;
	}

	this->period = period;
	LPC_TIM3->MR0 = int(floor((SystemCoreClock/4.0) * period));

	// Now we need to reset the timer, in case the counter is already higher
	// than the new match value.
	LPC_TIM3->TCR = TIM_ENABLE | TIM_RESET;

	// TODO: turn all servos off, so that one that is active doesn't stay on

	// Let the timer run again.
	LPC_TIM3->TCR = TIM_ENABLE;
}

extern "C" void TIMER3_IRQHandler (void){
	ServoTicker::global_servo_ticker->TIMER3_IRQHandler();
}

void ServoTicker::TIMER3_IRQHandler (void){
	// Get the interrupt status.
	uint32_t intStatus = LPC_TIM3->IR;

	// Clear the interrupts flag
	LPC_TIM3->IR = intStatus;

	// See if we are ready to reset the counter.
	if (0){//currentServo == NULL){
		// reset the counter.
		LPC_TIM3->TC = 0;
	} else {
		// Check the current servo, to see if it is active.  It could be
		// deactivated if servo.{NAME}.deactivation_delay_ms is greater than 0.
		if (0){//currentServo->is_active()) {
			// If currentServo is not null, that means that we are waiting to turn
			// it off.  We matched on it's on-time interval, so it is now time to
			// turn it off.
			//currentServo->turn_off();
		}
	}

	// Increment to the next servo, if there is one.
	if (0){//currentServo != NULL) {
		// There is a valid servo here,
		// refresh the counter value.
		if (0){// (currentServo->is_active()) {
			// currentServo->turn_on();
		}
		// calculate the match value to turn this servo off.
	} else {
		// We have finished with all channels.
		// finished all channels so wait for the refresh period to expire before starting over
		// Update the counter value.
		// If we are not yet past the REFRESH_INTERVAL (allowing some overhead for ticks)
		if (0) {
			/// NEXT_MATCH = REFRESH_INTERVAL
		} else {
			// we are after, or very close to the refresh interval.
			// NEXT_MATCH = a few ticks in the future
		}
		/// Set the flag to show that we are done, and need to reset on next interrupt match.
	}
	/// store NEXT_MATCH into the match register
}
