#ifndef HC_SR04_H_
#define HC_SR04_H_

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

    uint16_t threshold;
    uint32_t frequency;
} hc_sr04_config;

void initialize_ultrasonic_sensor(hc_sr04_config *config);
void trigger_ultrasonic(void);

uint8_t is_object_found(void);

void TA0_N_IRQHandler(void);
void TA1_N_IRQHandler(void);

#endif
