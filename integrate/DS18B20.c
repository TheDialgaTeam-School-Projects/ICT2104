#include "msp.h"
#include "delay.h"
#include "onewire.h"
#include "dht11.h"
#include "hc_sr04.h"
#include "timer32_delay.h"

volatile uint8_t scratchpad[9];
volatile uint16_t temp_f;
volatile float wholeTemp;
volatile int result[2]; //[0] DHT11 temp, [1] DHT humidity
volatile int temps[2]; //[0] DS18B20, [1] DHT11

int main(void) {


    onewire_t ow;


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

      P1->SEL0 &= ~BIT6;
      P1->SEL1 &= ~BIT6;

      ow.port_out = &P1OUT;
      ow.port_in = &P1IN;
      ow.port_ren = &P1REN;
      ow.port_dir = &P1DIR;
      ow.pin = BIT6;

      CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
      CS->CTL0 = 0;                           // Reset tuning parameters
      CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
      CS->KEY = 0;                            // Lock CS module from unintended accesses

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

      while(1) {

             volatile uint8_t temp_lsb,temp_msb, i;
             volatile int16_t temp;
             onewire_reset(&ow);
             onewire_write_byte(&ow, 0xcc); // skip ROM command
             onewire_write_byte(&ow, 0x44); // convert T command
             onewire_line_high(&ow);
             DELAY_MS(800); // at least 750 ms for the default 12-bit resolution
             onewire_reset(&ow);
             onewire_write_byte(&ow, 0xcc); // skip ROM command
             onewire_write_byte(&ow, 0xbe); // read scratchpad command

             for (i = 0; i < 9; i++)
             {
                 scratchpad[i] = onewire_read_byte(&ow); //Read the scratchpad bytes
             }

             temp_msb = scratchpad[1];
             temp_lsb = scratchpad[0];
             temp = (temp_msb << 8) + temp_lsb; //Create one 16bit number to modify

             wholeTemp = (float)temp/16.0; // Temp C

             temp_f = ((wholeTemp)* 9)/5 + 32; // Temp F

             readTempSensor(result);
             //printf("%d, %d\n", result[0], result[1]);

             trigger_ultrasonic_sensor(&config);
             delay_us(60000);

             if (is_object_found()) {
                 P1->OUT |= BIT0;
             } else {
                 P1->OUT &= ~BIT0;
             }

             temps[0] = wholeTemp;
             temps[1] = result[0];
             __delay_cycles(3000000); // delay 1 sec

      }

}
