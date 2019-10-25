#include "hc_sr04.h"

#define UINT16_MAX_VALUE (1 << 16) - 1;

volatile uint8_t capture_index = 0;
volatile uint16_t capture_offset = 0;
volatile uint16_t capture_time[2];
volatile uint16_t capture_distance = 0;
volatile uint16_t capture_threshold = 400;

void setup_ultrasonic_pins(const hc_sr04_config *config);
void setup_ultrasonic_capture_timer(void);

void initialize_ultrasonic_sensor(const hc_sr04_config *config, const uint16_t threshold)
{
    capture_threshold = threshold;

    setup_ultrasonic_pins(config);
    setup_ultrasonic_capture_timer();
}

void setup_ultrasonic_pins(const hc_sr04_config *config)
{
    // This set the trigger pin as a GPIO.
    *config->trigger_port_sel0 &= ~config->trigger_port_pin;
    *config->trigger_port_sel1 &= ~config->trigger_port_pin;

    // This set the trigger pin as output direction.
    *config->trigger_port_dir |= config->trigger_port_pin;

    // This set the trigger pin to 0 to prevent trigger.
    *config->trigger_port_out &= ~config->trigger_port_pin;

    // This set the echo pin to Timer A capture input.
    *config->echo_port_sel0 |= config->echo_port_pin;
    *config->echo_port_sel1 &= ~config->echo_port_pin;

    // This set the echo pin to TAx.CCIxA.
    *config->echo_port_dir &= ~config->echo_port_pin;
}

void setup_ultrasonic_capture_timer(void)
{
    /*
     * Timer A0 Control Register
     * SMCLK | Divider 1 | Continuous mode | Clear | Enable overflow interrupt
     */
    TIMER_A0->CTL |= TASSEL_2 | ID_0 | MC_2 | TACLR | TAIE;

    /*
     * Timer A0 Capture Control Register
     * CM: Capture on both rising and falling edges
     * CCIS: Capture input CCIxA
     * SCS: To synchronize the capture input
     * CAP: Capture mode
     * CCIE: Capture interrupt
     */
    TIMER_A0->CCTL[1] |= CM_3 | CCIS_0 | SCS | CAP | CCIE;

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

    capture_index = 0;
    capture_offset = 0;
}

uint8_t is_object_found(void)
{
    return capture_distance <= capture_threshold;
}

uint16_t get_object_distance(void)
{
    return capture_distance;
}

void delay_us(const uint32_t amount)
{
    // Set Timer32 to 1 microseconds.
    TIMER32_1->LOAD = SystemCoreClock / 1000000;

    // Set Timer32 to periodic warping mode.
    TIMER32_1->CONTROL = 0xC2;

    volatile int count = amount;

    for (count = amount; count > 0; count--) {
        /* wait until the RAW_IFG is set */
        while((TIMER32_1->RIS & 1) == 0);

        /* clear RAW_IFG flag */
        TIMER32_1->INTCLR = 0;
    }
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CTL & TIMER_A_CTL_IFG) {
        // Overflow interrupt triggered.
        if (capture_index == 1) {
            if (capture_offset == 0) {
                capture_offset = UINT16_MAX_VALUE - capture_time[0];
            } else {
                capture_offset += UINT16_MAX_VALUE;
            }
        }

        TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
    }

    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG) {
        // Rising or falling edge interrupt triggered.
        if (capture_index == 0) {
            // Rising edge interrupt.
            capture_time[capture_index++] = TIMER_A0->CCR[1];
        } else {
            // Falling edge interrupt.
            capture_time[capture_index] = TIMER_A0->CCR[1];

            if ((TIMER_A0->CCTL[1] & TIMER_A_CCTLN_COV) == TIMER_A_CCTLN_COV) {
                // Second interrupt happened but the software is too slow hence this data is corrupted and shall be ignored.
                TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_COV;
            } else {
                capture_distance = (capture_time[1] + capture_offset - capture_time[0]) / (SystemCoreClock / 1000000) / 58;
            }
        }

        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
