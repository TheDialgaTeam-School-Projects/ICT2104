#include "msp.h"
#include "hc_sr04.h"
#include "delay.h"

volatile uint8_t capture_index = 0;
volatile uint32_t capture_offset = 0;
volatile uint32_t capture_time[2];
volatile uint16_t capture_distance = 0;

hc_sr04_config *ultrasonic_config;

void setup_ultrasonic_pins();
void setup_ultrasonic_capture_timer(void);

void initialize_ultrasonic_sensor(hc_sr04_config *config)
{
    ultrasonic_config = config;

    setup_ultrasonic_pins();
    setup_ultrasonic_capture_timer();
}

void setup_ultrasonic_pins(void)
{
    // This set the trigger pin as a GPIO.
    *ultrasonic_config->trigger_port_sel0 &= ~ultrasonic_config->trigger_port_pin;
    *ultrasonic_config->trigger_port_sel1 &= ~ultrasonic_config->trigger_port_pin;

    // This set the trigger pin as output direction.
    *ultrasonic_config->trigger_port_dir |= ultrasonic_config->trigger_port_pin;

    // This set the trigger pin to 0 to prevent trigger.
    *ultrasonic_config->trigger_port_out &= ~ultrasonic_config->trigger_port_pin;

    // This set the echo pin to Timer A capture input.
    *ultrasonic_config->echo_port_sel0 |= ultrasonic_config->echo_port_pin;
    *ultrasonic_config->echo_port_sel1 &= ~ultrasonic_config->echo_port_pin;

    // This set the echo pin to TAx.CCIxA.
    *ultrasonic_config->echo_port_dir &= ~ultrasonic_config->echo_port_pin;
}

void setup_ultrasonic_capture_timer(void)
{
    /*
     * Timer A0 Control Register
     * TASSEL: SMCLK
     * ID: Input divider for clock /1
     * MC: Stop
     * TAIE: Enable overflow interrupt
     */
    TIMER_A0->CTL |= TASSEL_2 | ID_0 | MC_0 | TAIE;

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

void trigger_ultrasonic(void)
{
    // Trigger the ultrasonic sensor for 10us.
    *ultrasonic_config->trigger_port_out |= ultrasonic_config->trigger_port_pin;

    DELAY_US(10);

    // Reset the trigger.
    *ultrasonic_config->trigger_port_out &= ~ultrasonic_config->trigger_port_pin;

    // Start and Reset timer to zero.
    TIMER_A0->CTL |= MC_2 | TACLR;

    capture_index = 0;
    capture_offset = 0;
}

uint8_t is_object_found(void)
{
    return capture_distance <= ultrasonic_config->threshold;
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CTL & TIMER_A_CTL_IFG) {
        // Overflow interrupt triggered.
        if (capture_index == 1) {
            if (capture_offset == 0) {
                capture_offset = (1 << 16) - 1 - capture_time[0];
            } else {
                capture_offset += (1 << 16) - 1;
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
                // Second interrupt happened before the input is read causing the data to be corrupted. Ignore this.
                TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_COV;
            } else {
                capture_distance = (capture_time[1] + capture_offset - capture_time[0]) / (ultrasonic_config->frequency / 1000000) / 58;
            }

            // Stop Timer A0 as it is done to conserve some electricity :)
            TIMER_A0->CTL &= ~MC_3;
        }

        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
