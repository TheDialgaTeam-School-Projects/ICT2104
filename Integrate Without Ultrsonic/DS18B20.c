#include "DS18B20.h"
#include "delay.h"

void setup_dsb(void){

    P1->SEL0 &= ~BIT6;
    P1->SEL1 &= ~BIT6;

    ow.port_out = &P1OUT;
    ow.port_in = &P1IN;
    ow.port_ren = &P1REN;
    ow.port_dir = &P1DIR;
    ow.pin = BIT6;
}

float read_dsb() {

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

    return wholeTemp;

}
