#include "MQ135.h"

void setup_CO2Sesnsor(void);
float getADCresistance(void);
float getCO2PPM(void);

void setup_CO2Sesnsor(void)
{
    // GPIO Setup
    P1->OUT &= ~BIT0;                       // Clear LED to start
    P1->DIR |= BIT0;                        // Set P1.0/LED to output

    // Configure P6.0 for ADC
    P6->DIR &= ~BIT0;
    P6->SEL1 |= BIT0;
    P6->SEL0 |= BIT0;

    CS->KEY = CS_KEY_VAL;                // Unlock CS module for register access
    CS->CTL0 = 0;                               // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_0; // Set DCO to 1.5MHz (nominal, center of 1-2MHz range)

    // Select ACLK = REFO, SMCLK = MCLK = DCO
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;

    /*Some bits can only be configured while the enable bit is 0*/
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
    ADC14->CTL0 = ADC14_CTL0_PDIV_0 |           // Pre-divided by 1
            ADC14_CTL0_SHP |                      // SAMPCON the sampling timer
            ADC14_CTL0_SSEL__ACLK |                 // ACLK
            ADC14_CTL0_DIV__1 |                     // /1
            ADC14_CTL0_CONSEQ_0 |          // Single-channel, single-conversion
            ADC14_CTL0_SHT0__4 |                 // Sample and hold time 4 CLKs
            ADC14_CTL0_ON;                          // powered up

    ADC14->CTL1 |= ADC14_CTL1_RES__14BIT;       //Select 14-bit resolution

    ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_0 |      // V(R+) = AVCC, V(R-) = AVSS
            ADC14_MCTLN_INCH_15;       // A15, P6.0

    ADC14->IER0 = ADC14_IER0_IE0;          // Enable ADC conv complete interrupt

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
}

float getADCresistance(void)
{
    unsigned int v = ADC14->MEM[0];
    float r = (((4096.0 / (float) v) - 1.0) * 10.0);
    return r;
}

float getCO2PPM(void)
{
    float ppm = 116.6020682 * pow((getADCresistance() / 76.63), -2.769034857);
    return ppm;
}
