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
static volatile bool atualiza_display;
uint64_t ultimo_tempo_A = 0;
uint64_t ultimo_tempo_B = 0;
volatile bool botao_A_pressionado = false;
volatile bool botao_B_pressionado = false;

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
return true;
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
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 19, 14, 10, 5, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito1(){
const uint8_t digit_leds[] = {22, 17, 12, 7, 2};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito2(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 18, 12, 6, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito3(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 19, 14, 13, 12, 11, 10, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito4(){
const uint8_t digit_leds[] = {24, 15, 14, 13, 12, 11, 10, 20, 19, 9, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito5(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 14, 13, 12, 11, 10, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito6(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 14, 13, 12, 11, 10, 9, 5, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito7(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 18, 12, 6, 4};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito8(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 19, 14, 13, 12, 11, 10, 5, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito9(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 19, 14, 13, 12, 11, 10, 9, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

static void (*digitos[10])() = {
    digito0, digito1, digito2, digito3, digito4,
    digito5, digito6, digito7, digito8, digito9
};

void led_clear(){
    for(i = 0; i < matriz_led; i++){
        setled(i, 0, 0, 0);
    }
}

void numerostela(){
led_clear();
digitos[numerotela]();
}

static void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == botao_A) {
        botao_A_pressionado = true;
    } 
    else if (gpio == botao_B) {
        botao_B_pressionado = true;
    }
}

    
int main(){
ledinit();
botinit();
minit(matriz);
digitos[numerotela]();

gpio_set_irq_enabled_with_callback (botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Interrupção A
gpio_set_irq_enabled_with_callback (botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Interrupção A
add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer); //timer led 
    while (true) {
        uint64_t tempo_atual = to_us_since_boot(get_absolute_time()) / 1000;  // Tempo em ms

        if (botao_A_pressionado && (tempo_atual - ultimo_tempo_A > 2000)) {
        ultimo_tempo_A = tempo_atual;
        numerotela = (numerotela + 1) % 10;
        atualiza_display = true;
        botao_A_pressionado = false;
        }

        if (botao_B_pressionado && (tempo_atual - ultimo_tempo_B > 2000)) {
        ultimo_tempo_B = tempo_atual;
        numerotela = (numerotela + 9) % 10;
        atualiza_display = true;
        botao_B_pressionado = false;
        }

            if (atualiza_display) {
            numerostela();
            atualiza_display = false;
            }
    sleep_ms(1);
    }
}