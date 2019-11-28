#include "sen0189.h"

void setup_turbidity(void)
{
    DATA_PORT->DIR &= ~DATA_PIN;                // Analog Input
    DATA_PORT->SEL1 |= DATA_PIN;                // Configure P4.0 for ADC
    DATA_PORT->SEL0 |= DATA_PIN;

    /*Some bits can only be configured while the enable bit is 0*/
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
    ADC14->CTL0 = ADC14_CTL0_PDIV_0 |           // Pre-divided by 1
        ADC14_CTL0_SHP |                        // SAMPCON the sampling timer
        ADC14_CTL0_SSEL__ACLK |                 // ACLK
        ADC14_CTL0_DIV__1 |                     // /1
        ADC14_CTL0_CONSEQ_0 |                   // Single-channel, single-conversion
        ADC14_CTL0_SHT0__8 |                    // Sample and hold time 4 CLKs
        ADC14_CTL0_ON;                          // powered up

    ADC14->CTL1 |= ADC14_CTL1_RES__14BIT;       //Select 14-bit resolution
    ADC14->CTL1 &= ~0x10000;

    ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_0 |      // V(R+) = AVCC, V(R-) = AVSS
                     ADC14_MCTLN_INCH_13;       // A13, P4.0

    ADC14->IER0 = ADC14_IER0_IE0;               // Enable ADC conv complete interrupt

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] |= 1 << ((ADC14_IRQn) & 31);
}

//Calculate turbidity %. Higher the percentage, the higher the turbidity of water.
int calculate_turbidity(int turbidityADC_value){

    volatile int ADC = 0;
    if(turbidityADC_value > 16384)
    {
        ADC = 16384;
    }
    else if(turbidityADC_value < 0)
    {
        ADC = 0;
    }
    else
    {
        ADC = turbidityADC_value;
    }

    float adc_Volt = ((3.3 * ADC)/(16384.0));
    return 100 - (adc_Volt/volt_clear) * 100;
}