#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"


#define red_led 13
#define botao_A 5
#define botao_B 6

static uint8_t delay = 100;
struct repeating_timer timer;
uint8_t i;
static volatile uint64_t last_time;
static volatile uint8_t numerotela;

void ledinit(){ // iniciando led
        gpio_init(13);
        gpio_set_dir(13, 1);
        gpio_put(13, 0);
}   

void botinit(){
    for(i = 5; i < 7; i ++){ // iniciando botões
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }
}

bool repeating_timer_callback(struct repeating_timer *t){ // Manutenção do led piscando
gpio_put(13, !gpio_get(13));
}

static void gpio_irq_handler(uint gpio, uint32_t events){ // eventos da interrupção
uint32_t current_time = to_us_since_boot(get_absolute_time()); //debounce
    if(current_time - last_time > 200000){
        last_time = current_time;
        (gpio == botao_A) ? numerotela = (numerotela + 1) % 10 : (void)0; //interrupção A
        (gpio == botao_B) ? numerotela = (numerotela + 9) % 10 : (void)0; //interrupção B
    }
}


int main(){
ledinit();
botinit();
gpio_set_irq_enabled_with_callback (botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Interrupção A
gpio_set_irq_enabled_with_callback (botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Interrupção B
add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);    
    while (true) {

    }
}