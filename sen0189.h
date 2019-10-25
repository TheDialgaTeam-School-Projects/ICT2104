#include "msp.h"

/*
 * SEN0189 Turbidity Sensor PIN CONFIGURATION
 *
 * DOut: Data can be configured to Analog or Digital in the hardware itself.
 * For this project, Analog output will be used.
 */

#define DATA_PORT P4
#define DATA_PIN BIT0

void setup_turbidity(void);
void turbidity_capture(void);
