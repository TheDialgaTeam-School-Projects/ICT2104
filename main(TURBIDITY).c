#include "msp.h"
#include "sen0189.h"

unsigned short turbidityADC_value = 0;
float adc_Volt = 0;

int main(void) {
    volatile unsigned int i;

    WDT_A->CTL = WDT_A_CTL_PW |  WDT_A_CTL_HOLD;           // Stop WDT

    setup_turbidity();

    // Enable global interrupt
    __enable_irq();

    // Ensures SLEEPONEXIT takes effect immediately
    __DSB();

    while (1)
    {
        for (i = 20000; i > 0; i--);        // Delay

        turbidity_capture();

        __sleep();
        __no_operation();                   // For debugger
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    turbidityADC_value = ADC14->MEM[0];
    adc_Volt = ((3.3 * turbidityADC_value)/(16384.0));     //Vr+ = AVcc = 3.3V, Vr- = AVss = GND

    if (turbidityADC_value >= 0x7FF)             // ADC12MEM0 = A1 > 0.5AVcc?
      P1->OUT |= BIT0;                      // P1.0 = 1
    else
      P1->OUT &= ~BIT0;                     // P1.0 = 0
}
