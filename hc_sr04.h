#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "msp.h"

/*
 * Ultrasonic HC-SR04 PIN CONFIGURATION
 *
 * Trigger: Any port that can be used as I/O.
 * Echo: Require Timer A capture input ports.
 */

#define TRIGGER_PORT P4
#define TRIGGER_PIN BIT3

#define ECHO_PORT P2
#define ECHO_PIN BIT4

void setup_ultrasonic_pins(void);
void setup_ultrasonic_capture_timers(void);
void trigger_ultrasonic_sensor(void);

void delay_us(int n);

void TA0_N_IRQHandler(void);

#endif
