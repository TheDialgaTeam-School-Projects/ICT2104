#include "msp.h"
#include "delay.h"
#include "onewire.h"

volatile uint8_t scratchpad[9];
volatile uint16_t temp_f;
volatile float wholeTemp;

int main(void) {


    onewire_t ow;


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

      P1->SEL0 &= ~BIT7;
      P1->SEL1 &= ~BIT7;

      ow.port_out = &P1OUT;
      ow.port_in = &P1IN;
      ow.port_ren = &P1REN;
      ow.port_dir = &P1DIR;
      ow.pin = BIT7;

      CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
      CS->CTL0 = 0;                           // Reset tuning parameters
      CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
      CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
                  CS_CTL1_SELS_3 |                // SMCLK = DCO
                  CS_CTL1_SELM_3;                 // MCLK = DCO
      CS->KEY = 0;                            // Lock CS module from unintended accesses


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

      }

}
