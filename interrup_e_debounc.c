#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "ws2812.pio"

#define green_led 11
#define blue_led 12
#define red_led 13
#define botao_A 5
#define botao_B 6

uint8_t i;

void ledinit(){
    for(i = 11 ; i < 14 ; i++){
        gpio_init(i);
        gpio_set_dir(i, 1);
        gpio_put(i, 0);
    }   
}

void botinit(){
    for(i = 5; i < 7; i ++){
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }
}

int main(){


    
    while (true) {

    }
}