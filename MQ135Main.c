#include "MQ135.h"

/**
 * main.c
 */

volatile float percentage;
void main(void)
{
    volatile unsigned int i;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    P1->OUT &= ~BIT0;                       // Clear LED to start
    P1->DIR |= BIT0;                        // Set P1.0/LED to output

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
        percentage=getCO2percentage();
        if(percentage>19.0)
        {
            P1->OUT |= BIT0;
        }
        ADC14->CLRIFGR0;
    }
}
