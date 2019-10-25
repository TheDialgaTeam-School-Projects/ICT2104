#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "msp.h"

/*
 * Ultrasonic HC-SR04 PIN CONFIGURATION
 *
 * Trigger: Any port that can be used as I/O.
 * Echo: Require Timer A capture input ports.
 */

typedef struct {
    volatile uint8_t *trigger_port_sel0;
    volatile uint8_t *trigger_port_sel1;
    volatile uint8_t *trigger_port_dir;
    volatile uint8_t *trigger_port_out;
    uint16_t trigger_port_pin;

    volatile uint8_t *echo_port_sel0;
    volatile uint8_t *echo_port_sel1;
    volatile uint8_t *echo_port_dir;
    uint16_t echo_port_pin;
} hc_sr04_config;

void initialize_ultrasonic_sensor(const hc_sr04_config *config, const uint16_t threshold);
void trigger_ultrasonic_sensor(void);

uint8_t is_object_found(void);
uint16_t get_object_distance(void);

void delay_us(const uint32_t amount);

void TA0_N_IRQHandler(void);

#endif
