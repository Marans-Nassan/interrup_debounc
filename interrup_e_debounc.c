#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
//fazendo a ligação entre nomenclatura e pinos.
#define red_led 13
#define botao_A 5
#define botao_B 6
#define matriz 7
#define matriz_led 25
//defindo estrutura para a matriz.
typedef struct pixeis {
  uint8_t G, R, B;
} pixeis;

pixeis leds[matriz_led];
//config PIO e state machine.
PIO pio; 
uint sm;

struct repeating_timer timer; // contribuinte na configuração do led rgb.
uint8_t i; //constante global com intuito de facilitar o uso do for
static volatile uint64_t last_time; // contribuinte para a configuração do debounce.
static volatile uint8_t numerotela = 0; // definção inicial do numero que será exibido na tela e permitir a mudança para o próximo.
static volatile bool atualiza_display; // responsável por facilitar a atualização do display.
uint64_t ultimo_tempo_A = 0; //contribuiu na configuração do debounce.
uint64_t ultimo_tempo_B = 0; //contribuiu na configuração do debounce.
volatile bool botao_A_pressionado = false; //contribuiu na ativação da interrupção.
volatile bool botao_B_pressionado = false; //contribuiu na ativação da interrupção.
// iniciando led.
void ledinit(){ 
        gpio_init(13);
        gpio_set_dir(13, 1);
        gpio_put(13, 0);
}   
// iniciando botões.
void botinit(){
    for(i = 5; i < 7; i ++){ 
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }
}
//timer responsável por ditar o funcionamento do led rgb vermelho piscando 5x por segundo.
bool repeating_timer_callback(struct repeating_timer *t){
gpio_put(13, !gpio_get(13));
return true;
}
//Iniciando o programa do PIO
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
//configuração para permitir o uso da função display. (configuração da matriz de led)
void setled(const uint index, const uint8_t r, const uint8_t g, const uint8_t b){
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}
//esta é a função responsável por permitir ditar qual led vai acender e apagar.
void display(){
    for (uint i = 0; i < matriz_led; ++i) {
        pio_sm_put_blocking(pio, sm, leds[i].G);
        pio_sm_put_blocking(pio, sm, leds[i].R);
        pio_sm_put_blocking(pio, sm, leds[i].B);
    }
sleep_us(100); 
}
//Parte da configuração dos números, coloquei em função pq era uma mera repetição.
void digit_complement(const uint8_t *digit_leds, uint16_t count){
    for (size_t i = 0; i < count; ++i) {
        setled(digit_leds[i], 0, 0, 1);
    }
        display();    
}
// configuração dos números de 0 à 9.
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

static void (*digitos[10])() = { //função ponteiro para ajudar a numerostela.
    digito0, digito1, digito2, digito3, digito4,
    digito5, digito6, digito7, digito8, digito9
};

void led_clear(){ // limpar o led para possibilitar a adição do próximo número sem colidir com o anterior.
    for(i = 0; i < matriz_led; i++){
        setled(i, 0, 0, 0);
    }
}

void numerostela(){ //Responsável por definir o próximo número ao acionar a interrupção.
led_clear();
digitos[numerotela]();
}

static void gpio_irq_handler(uint gpio, uint32_t events) { //checando se houve interrupção ou não.
    (gpio == botao_A) ? botao_A_pressionado = true : (void)0;
    (gpio == botao_B) ? botao_B_pressionado = true : (void)0;
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
        uint64_t tempo_atual = to_us_since_boot(get_absolute_time()) / 2000; 
        //debounce em conjunto com os efeitos da interrupção
        if (botao_A_pressionado && (tempo_atual - ultimo_tempo_A > 1000)) { 
        ultimo_tempo_A = tempo_atual;
        
        numerotela = (numerotela + 1) % 10;
        atualiza_display = true;
        botao_A_pressionado = false;
        }

        if (botao_B_pressionado && (tempo_atual - ultimo_tempo_B > 1000)) {
        ultimo_tempo_B = tempo_atual;
        
        numerotela = (numerotela + 9) % 10;
        atualiza_display = true;
        botao_B_pressionado = false;
        }

            if (atualiza_display) { // atualização do display
            numerostela();
            atualiza_display = false;
            }
    sleep_ms(1);
    }
}