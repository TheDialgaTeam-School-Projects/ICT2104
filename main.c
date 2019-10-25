#include "msp.h"
#include "hc_sr04.h"
#include "timer32_delay.h"

void stop_watchdog_timer(void);

void main(void)
{
    stop_watchdog_timer();

    hc_sr04_config config;

    config.trigger_port_sel0 = &P4SEL0;
    config.trigger_port_sel1 = &P4SEL1;
    config.trigger_port_dir = &P4DIR;
    config.trigger_port_out = &P4OUT;
    config.trigger_port_pin = BIT3;

    config.echo_port_sel0 = &P2SEL0;
    config.echo_port_sel1 = &P2SEL1;
    config.echo_port_dir = &P2DIR;
    config.echo_port_pin = BIT4;

    initialize_ultrasonic_sensor(&config, 400);

    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

    __enable_interrupts();

    while (1) {
        trigger_ultrasonic_sensor(&config);
        delay_us(60000);

        if (is_object_found()) {
            P1->OUT |= BIT0;
        } else {
            P1->OUT &= ~BIT0;
        }
    }
}

void stop_watchdog_timer(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
}
