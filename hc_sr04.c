#include "hc_sr04.h"

volatile uint16_t captureTime[2];
volatile uint8_t captureIndex = 0;
volatile uint16_t captureDistance = 0;

void setup_ultrasonic_pins(void)
{
    // This set the trigger pin as a GPIO.
    TRIGGER_PORT->SEL0 &= ~TRIGGER_PIN;
    TRIGGER_PORT->SEL1 &= ~TRIGGER_PIN;

    // This set the trigger pin as output direction.
    TRIGGER_PORT->DIR |= TRIGGER_PIN;

    // This set the trigger pin to 0 to prevent trigger.
    TRIGGER_PORT->OUT &= ~TRIGGER_PIN;

    // This set the echo pin to Timer A capture input.
    ECHO_PORT->SEL0 |= ECHO_PIN;
    ECHO_PORT->SEL1 &= ~ECHO_PIN;

    // This set the echo pin to TAx.CCIxA.
    ECHO_PORT->DIR &= ~ECHO_PIN;
}

void setup_ultrasonic_capture_timers(void)
{
    /*
     * Timer A0 Control Register
     * SMCLK | Divider 1 | Continuous mode | Clear
     */
    TIMER_A0->CTL = TASSEL_2 | ID_0 | MC_2 | TACLR;

    /*
     * Timer A0 Capture Control Register
     * CM: Capture on both rising and falling edges
     * CCIS: Capture input CCIxA
     * SCS: To synchronize the capture input
     * CAP: Capture mode
     * CCIE: Capture interrupt
     */
    TIMER_A0->CCTL[1] = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    NVIC->ISER[0] |= 1 << ((TA0_N_IRQn) & 31);
}

void trigger_ultrasonic_sensor(void)
{
    // Trigger the ultrasonic sensor for 10us.
    P4->OUT |= BIT3;

    delay_us(10);

    // Reset the trigger.
    P4->OUT &= ~BIT3;

    // Reset timer to zero.
    TIMER_A0->CTL |= TACLR;
}

void delay_us(int n)
{
    // Set Timer32 to 1 microseconds.
    TIMER32_1->LOAD = SystemCoreClock / 1000000;

    // Set Timer32 to periodic warping mode.
    TIMER32_1->CONTROL = 0xC2;

    int i = n;

    for (i = n; i > 0; i--) {
        /* wait until the RAW_IFG is set */
        while((TIMER32_1->RIS & 1) == 0);

        /* clear RAW_IFG flag */
        TIMER32_1->INTCLR = 0;
    }
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG) {
        captureTime[captureIndex++] = TIMER_A0->CCR[1];

        if (captureIndex >= 2) {
            captureIndex = 0;
            captureDistance = (captureTime[1] - captureTime[0]) / (SystemCoreClock / 1000000) / 58;
        }

        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_COV;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
