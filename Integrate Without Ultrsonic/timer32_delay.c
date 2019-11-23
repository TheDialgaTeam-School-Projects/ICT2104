#include "msp.h"
#include "timer32_delay.h"

void delay_ms(const uint32_t amount)
{
    delay_us(amount * 1000);
}

void delay_us(const uint32_t amount)
{
    // Set Timer32 to 1 microseconds.
    TIMER32_1->LOAD = SystemCoreClock / 1000000;

    // Set Timer32 to periodic warping mode.
    TIMER32_1->CONTROL = TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_MODE | TIMER32_CONTROL_SIZE;

    volatile int delay_amount = amount;

    for (delay_amount = amount; delay_amount > 0; delay_amount--) {
        while((TIMER32_1->RIS & TIMER32_RIS_RAW_IFG) == 0);

        // Clear the interrupt flag.
        TIMER32_1->INTCLR = 0;
    }
}
