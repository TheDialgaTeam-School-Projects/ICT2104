#include "msp.h"

/*
 * SEN0189 Turbidity Sensor PIN CONFIGURATION
 *
 * DOut: Data can be configured to Analog or Digital in the hardware itself.
 * For this project, Analog output will be used.
 */

#define DATA_PORT P4
#define DATA_PIN BIT0
#define voltCLEAR 3.3f

void setup_turbidity(void);
int calculate_turbidity(float adcVoltage, float clearVoltage);
