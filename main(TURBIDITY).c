#include "msp.h"
#include "sen0189.h"

unsigned short turbidityADC_value = 0;
float adc_Volt = 0;
signed int turbidity = 0;

int main(void) {
    volatile unsigned int i;

    WDT_A->CTL = WDT_A_CTL_PW |  WDT_A_CTL_HOLD;           // Stop WDT

    setup_turbidity();                                     // Configure pins for SEN0189

    __enable_irq();                                        // Enable global interrupt

    __DSB();                                               // Ensures SLEEPONEXIT takes effect immediately

    while (1)
    {
        for (i = 20000; i > 0; i--);                       // Delay

        turbidity_capture();

        __sleep();
        __no_operation();                                  // For debugger
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    turbidityADC_value = ADC14->MEM[0];
    adc_Volt = ((3.3 * turbidityADC_value)/(16384.0));     //Vr+ = AVcc = 3.3V, Vr- = AVss = GND
    turbidity = calculate_turbidity(adc_Volt, voltCLEAR);

    if (turbidity >= 70){                                  //Red LED, high turbidity
        P2->OUT |= BIT0;
        P2->OUT &= ~BIT1;
    }
    else if (turbidity < 70 && turbidity > 30){            //Orange LED, medium turbidity
        P2->OUT = BIT0 + BIT1;
    }
    else{                                                  //Greed LED, low turbidity
        P2->OUT |= BIT1;
        P2->OUT &= ~BIT0;
    }

}
