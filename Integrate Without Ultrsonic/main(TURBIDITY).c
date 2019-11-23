#include "msp.h"
#include "clock.h"
#include "sen0189.h"
#include "MQ135.h"
#include "DS18B20.h"
#include "dht11.h"

unsigned short turbidityADC_value = 0;
float adc_Volt = 0;
signed int turbidity = 0;
volatile float percentage;
volatile float dsbTemp;
volatile float result[2]; //[0] DHT11 temp, [1] DHT humidity

int main(void) {

    disable_watchdog();
    setup_DCO();

    setup_dsb();

    setup_turbidity();                                     // Configure pins for SEN0189

    __enable_irq();                                        // Enable global interrupt

    __DSB();                                               // Ensures SLEEPONEXIT takes effect immediately

    while (1)
    {
        turbidity_capture();

        dsbTemp = read_dsb();

        readTempSensor(result);
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    if ((ADC14->IFGR0 & ADC14_IFGR0_IFG0) == ADC14_IFGR0_IFG0)
    {
        turbidityADC_value = ADC14->MEM[0];
        adc_Volt = ((3.3 * turbidityADC_value)/(16384.0));     //Vr+ = AVcc = 3.3V, Vr- = AVss = GND
        turbidity = calculate_turbidity(adc_Volt, voltCLEAR);

        if (turbidity >= 70){                                  //Red LED, high turbidity
            P2->OUT &= ~( BIT0 | BIT1 | BIT2 );                       // Clear LED to start
            P2->OUT |= BIT0;
            P2->OUT &= ~BIT1;
        }
        else if (turbidity < 70 && turbidity > 30){            //Orange LED, medium turbidity
            P2->OUT &= ~( BIT0 | BIT1 | BIT2 );                       // Clear LED to start
            P2->OUT = BIT0 + BIT1;
        }
        else{                                                  //Greed LED, low turbidity
            P2->OUT &= ~( BIT0 | BIT1 | BIT2 );                       // Clear LED to start
            P2->OUT |= BIT1;
            P2->OUT &= ~BIT0;
        }

        setup_CO2Sesnsor();
    }
    ADC14->CLRIFGR0 &= ~ADC14_IFGR0_IFG0;
    if ((ADC14->IFGR0 & ADC14_IFGR0_IFG1) == ADC14_IFGR0_IFG1)
    {
        percentage=getCO2percentage();
        if(percentage>19.0)
        {
            P1->OUT |= BIT0;
        }
        else
        {
            P1->OUT &= ~BIT0;
        }

        setup_turbidity();
    }
    ADC14->CLRIFGR0 &= ~ADC14_IFGR0_IFG1;
}
