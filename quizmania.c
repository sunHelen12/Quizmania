#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include <time.h>
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
#define JOYSTICK_BUTTON 22
//Número de perguntas que pode ser alterado
#define NUM_PERGUNTAS 10
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
static void gpio_irq_handler(uint gpio, uint32_t events);
void init_hardware();
void led_welcome_effect();
void play_welcome_music();
void led_setas_effect();

//Variáveis voláteis
static volatile uint32_t last_time = 0; //Armazena o último evento de temo (microssegundos)
static volatile bool is_inicio_screen = false; //Variável para controlar o estado da tela
static volatile bool is_players_screen = false;
static volatile int jogador = 0; // 0 = Nenhum jogador, 1 = Jogador 1, 2 = Jogador 2
static volatile int pontos_jogador1 = 0;
static volatile int pontos_jogador2 = 0;


//Vetores com animação dos números
double numero_zero[25] =    {0.0, 0.0, 0.1, 0.1, 0.1,
                             0.1, 0.0, 0.1, 0.0, 0.0, 
                             0.0, 0.0, 0.1, 0.0, 0.1,
                             0.1, 0.0, 0.1, 0.0, 0.0,
                             0.0, 0.0, 0.1, 0.1, 0.1};

double numero_um[25] =      {0.0, 0.0, 0.0, 0.0, 0.1,
                             0.1, 0.0, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.0, 0.0, 0.1,
                             0.1, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.1};

double numero_dois[25] =    {0.0, 0.0, 0.1, 0.1, 0.1,
                             0.1, 0.0, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.1, 0.1, 0.1,
                             0.0, 0.0, 0.1, 0.0, 0.0,
                             0.0, 0.0, 0.1, 0.1, 0.1};  

double numero_tres[25] =    {0.0, 0.0, 0.1, 0.1, 0.1,
                             0.1, 0.0, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.1, 0.1, 0.1,
                             0.1, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.1, 0.1, 0.1};  

double numero_quatro[25] =  {0.0, 0.0, 0.1, 0.0, 0.1,
                             0.1, 0.0, 0.1, 0.0, 0.0, 
                             0.0, 0.0, 0.1, 0.1, 0.1,
                             0.1, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.1}; 

double numero_cinco[25] =  {0.0, 0.0, 0.1, 0.1, 0.1,
                            0.0, 0.0, 0.1, 0.0, 0.0, 
                            0.0, 0.0, 0.1, 0.1, 0.1,
                            0.1, 0.0, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.1, 0.1, 0.1}; 

double interrogacao[25] =   {0.0, 0.1, 0.1, 0.1, 0.0,
                             0.0, 0.1, 0.0, 0.0, 0.0, 
                             0.0, 0.0, 0.1, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.1, 0.0, 0.0}; 

double setas[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.1, 0.0, 0.1, 0.0, 
                      0.1, 0.1, 0.0, 0.1, 0.1,
                      0.0, 0.1, 0.0, 0.1, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0}; 

double pontos[25] =  {0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0, 
                      0.1, 0.1, 0.1, 0.1, 0.1,
                      0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0};  

//Vetor com os números
double *numeros[6] = {numero_zero, numero_um, numero_dois, numero_tres, numero_quatro, numero_cinco};

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
    } else if (cor == 0) { //Desliga todos os LEDs
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
      
}

void exibir_contagem_regressiva() {
    double *numeros[] = {numero_zero, numero_um, numero_dois, numero_tres, numero_quatro, numero_cinco};

    // Alteração aqui: iniciamos com i = 0 e vamos até i = 5 para contar de 0 a 5
    for (int i = 0; i < 6; i++) {
        // Exibe o número correspondente da contagem crescente de 0 a 5 em verde
        for (int j = 0; j < 25; j++) {
            if (numeros[i][j] > 0.0) {
                // Passando a cor verde para a função desenho_pio
                desenho_pio(numeros[i], 2); // Verde: (0.0, 1.0, 0.0) - cor = 2
            }
        }
        sleep_ms(1000); // Espera 1 segundo entre os números
        
        // Limpa a tela (apaga os LEDs antes de exibir o próximo número)
        for (int j = 0; j < 25; j++) {
            desenho_pio(numeros[i], 0); // Limpa a tela (apagando LEDs)
        }
        sleep_ms(1000);
    }
}

void exibir_segunda_contagem_regressiva() {
    double *numeros[] = {numero_zero, numero_um, numero_dois, numero_tres, numero_quatro, numero_cinco};

    // Alteração aqui: iniciamos com i = 0 e vamos até i = 5 para contar de 0 a 5
    for (int i = 5; i >= 0; i--) {
        // Exibe o número correspondente da contagem crescente de 0 a 5 em verde
        for (int j = 0; j < 25; j++) {
            if (numeros[i][j] > 0.0) {
                // Passando a cor vermelha para a função desenho_pio
                desenho_pio(numeros[i], 1); //Cor Vermelha
            }
        }
        sleep_ms(1000); // Espera 1 segundo entre os números
        
        // Limpa a tela (apaga os LEDs antes de exibir o próximo número)
        for (int j = 0; j < 25; j++) {
            desenho_pio(numeros[i], 0); // Limpa a tela (apagando LEDs)
        }
        sleep_ms(1000);
    }
}

void led_vermelho_piscando(){
    gpio_put(LED_RED,1);
    sleep_ms(200);
    gpio_put(LED_RED, 0);
    sleep_ms(200);
    gpio_put(LED_RED, 1);
    sleep_ms(200);
    gpio_put(LED_RED, 0);
}

void led_verde_piscando(){
    gpio_put(LED_GREEN,1);
    sleep_ms(200);
    gpio_put(LED_GREEN, 0);
    sleep_ms(200);
    gpio_put(LED_GREEN, 1);
    sleep_ms(200);
    gpio_put(LED_GREEN, 0);
}
// Função que toca a música de boas-vindas
void bem_vindo_musica() {
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

void musica_alerta() {
    pwm_config config = pwm_get_default_config();
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER);
    pwm_init(slice_num, &config, true);

    int alert_note = NOTE_B;  // Nota aguda para alerta
    int duration = 150;       // Duração curta em milissegundos

    pwm_set_gpio_level(BUZZER, 32768);
    pwm_set_clkdiv(slice_num, (float)clock_get_hz(clk_sys) / (alert_note * 32768));
    sleep_ms(duration);
    pwm_set_gpio_level(BUZZER, 0);
}
// Mostra tela de boas-vindas no display
void show_welcome_screen() {
   
    // Efeito na matriz de LEDs 
    led_welcome_effect(); 

    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);    
    ssd1306_draw_string(&ssd,"QUIZMANIA", 27, 10); 
    ssd1306_draw_string(&ssd,"Pressione uma ", 12, 30); 
    ssd1306_draw_string(&ssd,"vez o botao", 20, 40);   // Ajuste para segunda linha
    ssd1306_draw_string(&ssd,"joystick", 30, 50);    // Ajuste para terceira linha
    ssd1306_send_data(&ssd);
    // Toca música de boas-vindas
    bem_vindo_musica();
    sleep_ms(1000);
    
}

void show_players_screen() {        
    //Efeito na matriz de LEDs
    
    ssd1306_fill(&ssd, !cor);
    //ssd1306_send_data(&ssd); //atualiza o display         
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "PLAYER 1", 30, 10); // Título no topo
    ssd1306_draw_string(&ssd, "BOTAO A", 35, 20); // Um pouco abaixo
    ssd1306_draw_string(&ssd, "PLAYER 2", 30, 40); // Linha 1 da última frase
    ssd1306_draw_string(&ssd, "BOTAO B", 35, 50); // Linha 2 da última frase
    ssd1306_send_data(&ssd);
    led_setas_effect();
}

void show_ready_screen() {   
    ssd1306_fill(&ssd, !cor);
    desenho_pio(numero_zero,0);     
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "INCIANDO", 30, 25); // Um pouco abaixo
    ssd1306_draw_string(&ssd, "PARTIDA", 36, 40);
    ssd1306_send_data(&ssd);
    exibir_contagem_regressiva();  
}

void show_preparacao_screen() {           
    ssd1306_fill(&ssd, !cor);
    //ssd1306_send_data(&ssd); //atualiza o display         
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "SE PREPAREM", 20, 25); // Um pouco abaixo    
    ssd1306_send_data(&ssd);
    desenho_pio(pontos, 2);
    sleep_ms(500);
    desenho_pio(pontos, 0);
    sleep_ms(500);
    desenho_pio(pontos, 2); 
    sleep_ms(500);
    desenho_pio(pontos, 0);
    sleep_ms(500);
    desenho_pio(pontos, 2); 
    sleep_ms(500);
    desenho_pio(pontos, 0); 
}

void show_jogador(int jogador) {
    desenho_pio(numero_zero, 0); // Limpa o led
    // Limpa a tela e desenha a caixa ao redor
    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    if (jogador == 1) {
        ssd1306_draw_string(&ssd, "PLAYER 1", 30, 20);
        ssd1306_draw_string(&ssd, "RESPONDE", 30, 30); // Exibe jogador 1
    } else if (jogador == 2) {
        ssd1306_draw_string(&ssd, "PLAYER 2", 30, 20); // Atualizado para PLAYER 2
        ssd1306_draw_string(&ssd, "RESPONDE", 30, 30);
    }
    ssd1306_send_data(&ssd); // Atualiza o display
}

void show_apertem_botao_screen() {
    // Exibe a tela inicial apenas se o jogo não tiver começado
    if (jogador == 0) {      
        
        ssd1306_fill(&ssd, !cor);        
        led_verde_piscando();
        sleep_ms(100);
        musica_alerta();
        sleep_ms(100);  
        musica_alerta();
        sleep_ms(100);  
        musica_alerta();
        sleep_ms(100);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
        ssd1306_draw_string(&ssd, "APERTEM", 35, 10); // Título no topo
        ssd1306_draw_string(&ssd, "SEUS BOTOES", 22, 20); // Um pouco abaixo    
        ssd1306_send_data(&ssd); // Atualiza o display
        led_setas_effect();        
    } else {       
        // Se um jogador apertou o botão, exibe quem foi
        show_jogador(jogador);
    }
}

void mostrar_placar() {
    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
    
    char placar1[20];
    char placar2[20];
    
    sprintf(placar1, "PLAYER 1 I %d", pontos_jogador1);
    sprintf(placar2, "PLAYER 2 I %d", pontos_jogador2);
    
    ssd1306_draw_string(&ssd, "PLACAR FINAL", 20, 5);
    ssd1306_draw_string(&ssd, placar1, 10, 20);
    ssd1306_draw_string(&ssd, placar2, 10, 35);
    
    ssd1306_send_data(&ssd);
}

//Pergunta
void show_pergunta() {
    // Limpa a tela e desenha a caixa ao redor
    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "PERGUNTA", 30, 20); 
    ssd1306_send_data(&ssd); // Atualiza o display
}

//Pergunta
void show_resposta() {
    // Limpa a tela e desenha a caixa ao redor
    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "RESPOSTA", 30, 20); 
    ssd1306_send_data(&ssd); // Atualiza o display
}

// Função para verificar se o botão A ou B foi pressionado
int aguardar_aperto_botao() {
    jogador = 0;

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (jogador == 0) {
        tight_loop_contents();  
    }

    gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, false);

    return jogador;
}

int aguardar_aperto_botao_especifico(int jogador_original) {
    int resposta = 0;  

    jogador = 0;  

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (resposta == 0) {
        if (jogador == jogador_original || jogador == (jogador_original == 1 ? 2 : 1)) {
            resposta = jogador;  
        }
        tight_loop_contents();
    }

    gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, false);

    return resposta;
}

void led_welcome_effect() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        desenho_pio(interrogacao, 2);
        sleep_ms(50);
    }  
}
// Efeito na matriz de LEDs
void led_setas_effect() {        
    for (int i = 0; i < NUM_PIXELS; i++) {         
        desenho_pio(setas, 2);
        sleep_ms(50);        

    }   
    
}
typedef struct {
    char pergunta[100];
    char resposta[100];
} Pergunta;

Pergunta perguntas[NUM_PERGUNTAS] = {
    {"CAPITAL DA FRANCA", "PARIS"},
    {"ELEMENTO P DA TABELA PERIODICA", "FOSFORO"},
    {"CONTINENTE DO BRASIL", "AMERICA DO SUL"},
    {"TIME COM MAIS MUNDIAIS ", "REAL MADRID"},
    {"ANO QUE O HOMEM CHEGOU A LUA", "1969"},
    {"FUNDADOR DA APPLE?", "STEVE JOBS"},
    {"FILME VENCEDOR DO OSCAR 2020", "PARASITA"},
    {"O MAIOR PAIS DO MUNDO", "RUSSIA"},
    {"NOVELA MAIS FAMOSA DO BRASIL", "AVENIDA BRASIL"},
    {"CIDADE DO PERSONAGEM BOB ESPONJA", "FENDA DO BIQUINI"}
};
// Exibe a pergunta quebrada no display OLED
void exibir_pergunta(Pergunta pergunta) {
    ssd1306_fill(&ssd, false);  // Limpa a tela OLED
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo

    char linha1[20] = "";
    char linha2[20] = "";
    char linha3[20] = "";
    char linha4[20] = "";

    // Separar a pergunta em palavras
    char temp[100];
    strcpy(temp, pergunta.pergunta);  // Copia a string original para não modificar os dados
    char *palavra = strtok(temp, " ");
    
    char *linhas[4] = {linha1, linha2, linha3, linha4};  // Array para armazenar as linhas
    int linha_atual = 0;
    
    while (palavra != NULL && linha_atual < 4) {
        // Se couber na linha, adiciona
        if (strlen(linhas[linha_atual]) + strlen(palavra) < 14) {
            if (strlen(linhas[linha_atual]) > 0) {
                strcat(linhas[linha_atual], " "); // Adiciona espaço entre palavras
            }
            strcat(linhas[linha_atual], palavra);
        } else {
            // Se não couber, passa para a próxima linha
            linha_atual++;
            if (linha_atual < 4) {
                strcpy(linhas[linha_atual], palavra);
            }
        }
        palavra = strtok(NULL, " ");
    }

    // Exibir as linhas no display
    ssd1306_draw_string(&ssd, linha1, 10, 5);
    ssd1306_draw_string(&ssd, linha2, 10, 15);
    ssd1306_draw_string(&ssd, linha3, 10, 25);
    ssd1306_draw_string(&ssd, linha4, 10, 35);

    ssd1306_send_data(&ssd);  // Atualiza o display
}

void exibir_resposta(Pergunta pergunta) {    
    ssd1306_fill(&ssd, false);  // Limpa a tela OLED
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo

    // Divide a resposta em palavras
    char *palavra1 = strtok(pergunta.resposta, " "); 
    char *palavra2 = strtok(NULL, " "); 

    // Exibe as palavras separadas em duas linhas
    if (palavra1) {
        ssd1306_draw_string(&ssd, palavra1, 10, 10);
    }
    if (palavra2) {
        ssd1306_draw_string(&ssd, palavra2, 10, 25);
    }

    ssd1306_send_data(&ssd);  // Atualiza o display
}

void embaralhar_perguntas() {
    srand(time(NULL));
    for (int i = NUM_PERGUNTAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Pergunta temp = perguntas[i];
        perguntas[i] = perguntas[j];
        perguntas[j] = temp;
    }
}

void verificar_resposta(int jogador) {
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
    ssd1306_draw_string(&ssd, "PRESSIONE", 30, 10);
    ssd1306_draw_string(&ssd, "PLAYER ACERTOU", 10, 20);
    ssd1306_draw_string(&ssd, "SEU BOTAO", 25, 30);
    ssd1306_draw_string(&ssd, "PLAYER ERROU", 15, 40);
    ssd1306_draw_string(&ssd, "BOTAO RIVAL", 20, 50);
    ssd1306_send_data(&ssd);

    int resposta = aguardar_aperto_botao_especifico(jogador);

    if (resposta == jogador) {
        if (jogador == 1) {
            pontos_jogador1++;
        } else {
            pontos_jogador2++;
        }
    } else {
        if (jogador == 1) {
            pontos_jogador2++;  
        } else {
            pontos_jogador1++;  
        }
    }
}

void jogo_perguntas() {
    embaralhar_perguntas();  // Embaralha as perguntas

    for (int i = 0; i < NUM_PERGUNTAS; i++) {
        show_preparacao_screen();
        jogador = 0;        
        show_apertem_botao_screen();
        
        jogador = aguardar_aperto_botao();        
        show_jogador(jogador);
        sleep_ms(2000);
        show_pergunta();
        sleep_ms(2000);
        
        exibir_pergunta(perguntas[i]);        
        exibir_segunda_contagem_regressiva();        
        led_vermelho_piscando();
        sleep_ms(100);
        musica_alerta();
        sleep_ms(100);  
        musica_alerta();
        sleep_ms(100);  
        musica_alerta();
        show_resposta();
        sleep_ms(4000);
        exibir_resposta(perguntas[i]);
        sleep_ms(4000);
        
        verificar_resposta(jogador);// Pergunta se acertou e acumula pontos
        sleep_ms(1000);
        mostrar_placar();
        sleep_ms(4000);  // Atraso de 5 segundos para mostrar a resposta
    }

    mostrar_placar();
}

//Função Principal
int main() {
    // Inicializa o hardware
    init_hardware();
        
    while (true) {
        // Mostra a tela de boas-vindas
        show_welcome_screen();

        if (is_inicio_screen == true) {
            // Mostra a tela dos jogadores
            show_players_screen();
            sleep_ms(5000);
            show_ready_screen();
            sleep_ms(2000);
            show_preparacao_screen();                      
            sleep_ms(3000);            
            jogo_perguntas();            
            // Ao sair do loop_perguntas, volta para a tela inicial
            is_inicio_screen = false;  
        }
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
        
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
        
    gpio_init(JOYSTICK_BUTTON); // Inicializa o pino do botão do joystick
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN); // Configura o pino do botão do joystick como entrada
    gpio_pull_up(JOYSTICK_BUTTON); // Habilita o pull-up interno no pino do botão do joystick
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção para o botão do joystick


    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd); 
    ssd1306_fill(&ssd, false); 
    ssd1306_send_data(&ssd); 
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_time > 200000) {
        last_time = current_time;

        if (gpio == BUTTON_A) {
            jogador = 1;     // Jogador 1 apertou primeiro
                     
        }        
        if (gpio == BUTTON_B) {
          jogador = 2;     // Jogador 2 apertou primeiro               
           
        }
        if (gpio == JOYSTICK_BUTTON) {
            // Se o joystick for pressionado, alterna a tela
            is_inicio_screen = !is_inicio_screen;
        }
    }
}



