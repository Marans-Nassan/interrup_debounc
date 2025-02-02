#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define red_led 13
#define botao_A 5
#define botao_B 6
#define matriz 7
#define matriz_led 25

typedef struct pixeis {
  uint8_t G, R, B;
} pixeis;

pixeis leds[matriz_led];

PIO pio;
uint sm;

struct repeating_timer timer;
uint8_t i;
static volatile uint64_t last_time;
static volatile uint8_t numerotela = 0;

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

bool repeating_timer_callback(struct repeating_timer *t){ // Manutenção do led piscando 5x por segundo
gpio_put(13, !gpio_get(13));
}

void minit(uint pin){

uint offset = pio_add_program(pio0, &ws2812_program);
pio = pio0;

sm = pio_claim_unused_sm(pio, false);
    if(sm < 0){
        pio = pio1;
        sm = pio_claim_unused_sm(pio, true);
    }

ws2812_program_init(pio, sm, offset, pin, 800000.f);
}

void setled(const uint index, const uint8_t r, const uint8_t g, const uint8_t b){
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

void display(){
    for (uint i = 0; i < matriz_led; ++i) {
        pio_sm_put_blocking(pio, sm, leds[i].G);
        pio_sm_put_blocking(pio, sm, leds[i].R);
        pio_sm_put_blocking(pio, sm, leds[i].B);
    }
sleep_us(100); 
}

void digit_complement(const uint8_t *digit_leds, uint16_t count){
    for (size_t i = 0; i < count; ++i) {
        setled(digit_leds[i], 0, 1, 1);
    }    
display();
}

void digito0(){
const uint8_t digit_leds[] = {6, 7, 8, 11, 13, 16, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito1(){
const uint8_t digit_leds[] = {8, 13, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito2(){
const uint8_t digit_leds[] = {6, 7, 8, 13, 16, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito3(){
const uint8_t digit_leds[] = {6, 7, 8, 13, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito4(){
const uint8_t digit_leds[] = {6, 7, 8, 13, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito5(){
const uint8_t digit_leds[] = {6, 7, 8, 11, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito6(){
const uint8_t digit_leds[] = {6, 7, 8, 11, 13, 16, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito7(){
const uint8_t digit_leds[] = {6, 7, 8, 13};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito8(){
const uint8_t digit_leds[] = {6, 7, 8, 11, 13, 16, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito9(){
const uint8_t digit_leds[] = {6, 7, 8, 11, 13, 17, 18};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void (*digitos[10])() = {
    digito0, digito1, digito2, digito3, digito4,
    digito5, digito6, digito7, digito8, digito9
};

void led_clear(){
    for(i = 0; i < matriz_led; i++){
        setled(i, 0, 0, 0);
    }
    display();
}

void numerostela(){
led_clear();
digitos[numerotela]();
display();
}

static void gpio_irq_handler(uint gpio, uint32_t events){ // eventos da interrupção
uint32_t current_time = to_us_since_boot(get_absolute_time()); //debounce
    if(current_time - last_time > 200000){
        last_time = current_time;
        if(gpio == botao_A){
            numerotela = (numerotela + 1) % 10;
        }
            else if(gpio == botao_B) {
               numerotela = (numerotela + 9) % 10; 
            }
            numerostela();
    }
}
    

int main(){
ledinit();
botinit();
minit(matriz);
digito0();
gpio_set_irq_enabled_with_callback (botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Interrupção A
gpio_set_irq_enabled_with_callback (botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Interrupção A
add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer); //timer led 
    while (true) {
        sleep_ms(1);
    }
}