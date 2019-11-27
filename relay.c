#include "relay.h"

void setup_relay(void){
    P3->DIR |= ( BIT2 | BIT3);
}

void activate_relay(void){
    P3->OUT |= BIT2;
}

void deactivate_relay(void){
    P3->OUT &= ~BIT2;
}

void toggle_relay(void){
    P3->OUT ^= BIT2;
}
