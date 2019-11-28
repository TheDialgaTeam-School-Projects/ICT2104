#include "MQ135.h"

void setup_CO2Sesnsor(void);
float getCO2percentage(int);

void setup_CO2Sesnsor(void)
{
    // GPIO Setup
    // Configure P6.0 for ADC
    P6->DIR &= ~BIT0;
    P6->SEL1 |= BIT0;
    P6->SEL0 |= BIT0;

    /*Some bits can only be configured while the enable bit is 0*/
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
    ADC14->CTL0 = ADC14_CTL0_PDIV_0 |           // Pre-divided by 1
            ADC14_CTL0_SHP |                      // SAMPCON the sampling timer
            ADC14_CTL0_SSEL__ACLK |                 // ACLK
            ADC14_CTL0_DIV__1 |                     // /1
            ADC14_CTL0_CONSEQ_0 |          // Single-channel, single-conversion
            ADC14_CTL0_SHT0__4 |                 // Sample and hold time 4 CLKs
            ADC14_CTL0_ON;                          // powered up

    ADC14->CTL1 |= ADC14_CTL1_RES__14BIT | 1 << 16;   //Select 14-bit resolution

    ADC14->MCTL[1] = ADC14_MCTLN_VRSEL_0 |      // V(R+) = AVCC, V(R-) = AVSS
            ADC14_MCTLN_INCH_15;       // A15, P6.0

    ADC14->IER0 = ADC14_IER0_IE1;          // Enable ADC conv complete interrupt

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] |= 1 << ((ADC14_IRQn) & 31);
}

float getCO2percentage(int adcValue)
{
    volatile int ADC = 0;
    if(adcValue > 16384)
    {
        ADC = 16384;
    }
    else if(adcValue < 0)
    {
        ADC = 0;
    }
    else
    {
        ADC = adcValue;
    }

    /*
     * This formulae calculate ADC resistance in Kohm's law
     * 2^14 = 16384 is the max resolution output
     * 10.0 is the max resistor load on the board
     * */

    float r = (((16384 / (float) adcValue) - 1.0) * 10.0);

    /*ppm value = CO2 max *(CO2min^(resistance/calibration of resistance at CO2 air)) */
    float ppm = 116.6020682 * pow((r / 76.63), -2.769034857);

    /*
     * CO2 Percentage in current air is CO2ppm/100ppm
     * 100ppm is the RS which is 100pm of NH3 in normal air
     * */
    float percent = (ppm / 100.0);
    return percent;
}