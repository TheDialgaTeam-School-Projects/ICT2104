#include "sen0189.h"

void setup_turbidity(void){
    // GPIO Setup
    P1->OUT &= ~BIT0;                       // Clear LED to start
    P1->DIR |= BIT0;                        // Set P1.0/LED to output

    DATA_PORT->DIR &= ~DATA_PIN;                // Analog Input
    DATA_PORT->SEL1 |= DATA_PIN;                // Configure P4.0 for ADC
    DATA_PORT->SEL0 |= DATA_PIN;

    CS->KEY = CS_KEY_VAL ;                      // Unlock CS module for register access
    CS->CTL0 = 0;                               // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_0;               // Set DCO to 1.5MHz (nominal, center of 1-2MHz range)

    // Select ACLK = REFO, SMCLK = MCLK = DCO
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;

    /*Some bits can only be configured while the enable bit is 0*/
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
    ADC14->CTL0 = ADC14_CTL0_PDIV_0 |           // Pre-divided by 1
        ADC14_CTL0_SHP |                        // SAMPCON the sampling timer
        ADC14_CTL0_SSEL__ACLK |                 // ACLK
        ADC14_CTL0_DIV__1 |                     // /1
        ADC14_CTL0_CONSEQ_0 |                   // Single-channel, single-conversion
        ADC14_CTL0_SHT0__4 |                    // Sample and hold time 4 CLKs
        ADC14_CTL0_ON;                          // powered up

    ADC14->CTL1 |= ADC14_CTL1_RES__14BIT;       //Select 14-bit resolution

    ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_0 |      // V(R+) = AVCC, V(R-) = AVSS
                     ADC14_MCTLN_INCH_13;       // A13, P4.0

    ADC14->IER0 = ADC14_IER0_IE0;               // Enable ADC conv complete interrupt

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

//    // Sampling time, S&H=16, ADC14 on
//    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
//    ADC14->CTL1 = ADC14_CTL1_RES_2;         // Use sampling timer, 12-bit conversion results
//
//    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_13;   // A13 ADC input select; Vref=AVCC
//    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conv complete interrupt
//
//    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;   // Wake up on exit from ISR
}

void turbidity_capture(void){
    // Start sampling/conversion
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}
