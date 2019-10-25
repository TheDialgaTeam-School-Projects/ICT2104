#include "MQ135.h"

/**
 * main.c
 */


void main(void)
{
    volatile unsigned int i;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    setup_CO2Sesnsor();
    // Enable global interrupt
    __enable_irq();

    // Ensures SLEEPONEXIT takes effect immediately
    __DSB();

    while (1)
    {
        for (i = 2000; i > 0; i--); // Delay

        // Start sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

        __sleep();

        __no_operation();
        // For debugger
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0)
    {
        getCO2PPM();
        ADC14->CLRIFGR0;
    }
}
