#include "dht11.h"
#include "msp.h"



void main(void) {
    while(1){
        int result[2];
        readTempSensor(result);
        //printf("%d, %d\n", result[0], result[1]);
        __delay_cycles(3000000); // delay 1 sec
    }

}
