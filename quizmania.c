#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
//Fontes
#include "inc/ssd1306.h"
#include "inc/font.h"
//PIO
#include "quizmania.pio.h"
//Display
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
//Configuração dos LEDs
#define LED_GREEN 11
#define LED_RED 13
//Configuração da matriz de LEDs
#define NUM_PIXELS 25 
#define OUT_PIN 7 
//Configurações dos botões
#define BUTTON_A 5
#define BUTTON_B 6
//Configuração do buzzer
#define BUZZER 21
//Definição das notas musicais em Hz
#define NOTE_E  329
#define NOTE_G  392
#define NOTE_A  440
#define NOTE_B  494
#define NOTE_C  523


//Variáveis globais 
PIO pio;
uint sm;
bool cor = true;
//Estrutura do display
ssd1306_t ssd;


//Protótipo da função de interrupção
//static void gpio_irq_handler(uint gpio, uint32_t events);
void init_hardware();
void led_welcome_effect();
void play_welcome_music();

//Variáveis voláteis
static volatile uint32_t last_time = 0; //armazena o último evento de temo (microssegundos)

//Vetores com animação dos números
double numero_zero[25] =    {0.0, 0.0, 0.3, 0.3, 0.3,
                             0.3, 0.0, 0.3, 0.0, 0.0, 
                             0.0, 0.0, 0.3, 0.0, 0.3,
                             0.3, 0.0, 0.3, 0.0, 0.0,
                             0.0, 0.0, 0.3, 0.3, 0.3};

double numero_um[25] =      {0.0, 0.0, 0.0, 0.0, 0.3,
                             0.3, 0.0, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.0, 0.0, 0.3,
                             0.3, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.3};

double numero_dois[25] =    {0.0, 0.0, 0.3, 0.3, 0.3,
                             0.3, 0.0, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.5, 0.3, 0.3,
                             0.0, 0.0, 0.5, 0.0, 0.0,
                             0.0, 0.0, 0.3, 0.3, 0.3};  

double numero_tres[25] =    {0.0, 0.0, 0.3, 0.3, 0.3,
                             0.3, 0.0, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.3, 0.3, 0.3,
                             0.3, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.3, 0.3, 0.3};  

double numero_quatro[25] =  {0.0, 0.0, 0.3, 0.0, 0.3,
                             0.3, 0.0, 0.3, 0.0, 0.0, 
                             0.0, 0.0, 0.3, 0.3, 0.3,
                             0.3, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.3}; 

double numero_cinco[25] =  {0.0, 0.0, 0.3, 0.3, 0.3,
                            0.0, 0.0, 0.3, 0.0, 0.0, 
                            0.0, 0.0, 0.3, 0.3, 0.3,
                            0.3, 0.0, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.3, 0.3, 0.3}; 

double setas[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.1, 0.0, 0.1, 0.0, 
                      0.1, 0.1, 0.0, 0.1, 0.1,
                      0.0, 0.1, 0.0, 0.1, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0}; 

double *numeros[10] = {numero_zero, numero_um, numero_dois};

//Rotina pra definição de cores do led
uint32_t matrix_rgb(double r, double g, double b){
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}  

//Rotina para acionar a matriz de LEDs - ws2812b
void desenho_pio(double *desenho, int cor){
    uint32_t valor_led;
    if (cor == 1){
        for (int16_t i = 0; i < NUM_PIXELS; i++){
            valor_led = matrix_rgb(desenho[24-i], 0.0, 0.0); //cor vermelha
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }else if (cor == 2) {
        for (int16_t i = 0; i < NUM_PIXELS; i++){
            valor_led = matrix_rgb(0.0, desenho[24-i], 0.0); //cor verde
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }    
}

// Mostra tela de boas-vindas no display
void show_welcome_screen() {
    // Toca música de boas-vindas
    play_welcome_music();
    // Efeito na matriz de LEDs
    led_welcome_effect();

    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd,"QUIZMANIA", 27, 24); // Centralizado no meio
    ssd1306_draw_string(&ssd,"Pressione um",18, 45); // Abaixo no centro
    ssd1306_draw_string(&ssd,"botao",44, 53); // Abaixo no centro (ajuste a posição)
    ssd1306_send_data(&ssd);
}

// Função que toca a música de boas-vindas
void play_welcome_music() {
    pwm_config config = pwm_get_default_config();
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER);
    pwm_init(slice_num, &config, true);

    int notes[] = {NOTE_E, NOTE_G, NOTE_A, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_C, NOTE_B, NOTE_A, NOTE_G, NOTE_A};
    int durations[] = {400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400};

    for (int i = 0; i < sizeof(notes)/sizeof(notes[0]); i++) {
        pwm_set_gpio_level(BUZZER, 32768);
        pwm_set_clkdiv(slice_num, (float)clock_get_hz(clk_sys) / (notes[i] * 32768));
        sleep_ms(durations[i]);
        pwm_set_gpio_level(BUZZER, 0);
        sleep_ms(50); // Pequena pausa entre as notas
    }
}

// Efeito na matriz de LEDs
void led_welcome_effect() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        desenho_pio(setas, 2);
        sleep_ms(50);
    }  
}

//Função Principal
int main(){
    
    //Incializa o hardware
    init_hardware();
        
    while (true){
               
        //Mostra tela de boas-vindas
        show_welcome_screen();        
     
    }    
    return 0;
}


// Inicialização do hardware
void init_hardware() {
    //Inicializa a biblioteca padrão
    stdio_init_all();

    //Configura PIO para matriz de LEDs WS2812
    pio = pio0;
    uint offset = pio_add_program(pio, &quizmania_program);
    sm = pio_claim_unused_sm(pio, true);
    quizmania_program_init(pio, sm, offset, OUT_PIN);  

    //Configura SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    //Configura LEDs
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

    //Configura botões
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    //gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    //gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);    

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd); 
    ssd1306_fill(&ssd, false); 
    ssd1306_send_data(&ssd); 
}
