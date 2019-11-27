#include "msp.h"
#include "onewire.h"

volatile uint8_t scratchpad[9];
volatile uint16_t temp_f;
volatile float wholeTemp;
void setup_dsb(void);
float read_dsb();

onewire_t ow;
