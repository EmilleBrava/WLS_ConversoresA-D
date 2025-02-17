#include <stdio.h>        // Biblioteca padrão de entrada e saída
#include <stdlib.h>       // Necessária para abs()
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040
#include "hardware/pwm.h" // Biblioteca para controle de PWM no RP2040
#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico

// Definição dos pinos usados para o joystick, LEDs e botão A
const int VRX = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
const int VRY = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick
const int SW = 22;           // Pino de leitura do botão do joystick
const int BUTTON_A = 23;     // Pino de leitura do botão A

const int LED_B = 13;                    // Pino para controle do LED azul via PWM
const int LED_R = 11;                    // Pino para controle do LED vermelho via PWM
const int LED_G = 15;                    // Pino para controle do LED verde
const float DIVIDER_PWM = 16.0;          // Divisor fracional do clock para o PWM
const uint16_t PERIOD = 4095;            // Período do PWM (valor máximo do contador)
uint slice_led_b, slice_led_r;           // Variáveis para armazenar os slices de PWM correspondentes aos LEDs

bool led_g_state = false;                // Estado inicial do LED verde (desligado)
bool leds_pwm_state = true;              // Estado inicial dos LEDs PWM (ligados)

// Função para configurar o joystick e os botões (pinos de leitura e ADC)
void setup_joystick_and_buttons()
{
    adc_init();         // Inicializa o módulo ADC
    adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
    adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC

    gpio_init(SW);               // Inicializa o pino do botão do joystick
    gpio_set_dir(SW, GPIO_IN);   // Configura o pino do botão do joystick como entrada
    gpio_pull_up(SW);            // Ativa o pull-up no pino do botão do joystick para evitar flutuações

    gpio_init(BUTTON_A);         // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A, GPIO_IN);   // Configura o pino do botão A como entrada
    gpio_pull_up(BUTTON_A);      // Ativa o pull-up no pino do botão A para evitar flutuações
}

// Função para configurar o PWM de um LED (genérica para azul e vermelho)
void setup_pwm_led(uint led, uint *slice)
{
    gpio_set_function(led, GPIO_FUNC_PWM); // Configura o pino do LED como saída PWM
    *slice = pwm_gpio_to_slice_num(led);   // Obtém o slice do PWM associado ao pino do LED
    pwm_set_clkdiv(*slice, DIVIDER_PWM);   // Define o divisor de clock do PWM
    pwm_set_wrap(*slice, PERIOD);          // Configura o valor máximo do contador (período do PWM)
    pwm_set_gpio_level(led, 0);            // Define o nível inicial do PWM para o LED (apagado)
    pwm_set_enabled(*slice, true);         // Habilita o PWM no slice correspondente ao LED
}

// Função de configuração geral
void setup()
{
    stdio_init_all();                          // Inicializa a porta serial para saída de dados
    setup_joystick_and_buttons();              // Chama a função de configuração do joystick e botões
    setup_pwm_led(LED_B, &slice_led_b);        // Configura o PWM para o LED azul
    setup_pwm_led(LED_R, &slice_led_r);        // Configura o PWM para o LED vermelho
    
    gpio_init(LED_G);                          // Inicializa o pino do LED verde
    gpio_set_dir(LED_G, GPIO_OUT);             // Configura o pino do LED verde como saída
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
    adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

    adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}

// Função para mapear os valores do joystick para PWM de forma suave
uint16_t map_joystick_to_pwm(uint16_t value)
{
    if (value >= 2048)
        return (value - 2048) * 2; // Mapeia de 0 a 4095
    else
        return (2048 - value) * 2; // Mapeia de 0 a 4095
 return value;
}

// Função para alternar o estado do LED verde
void toggle_green_led()
{
    led_g_state = !led_g_state;      // Alterna o estado do LED verde
    gpio_put(LED_G, led_g_state);    // Define o estado do LED verde
    printf("LED verde: %s\n", led_g_state ? "ligado" : "desligado"); // Mensagem de depuração
}

// Função para ativar/desativar os LEDs PWM
// Função para ativar/desativar os LEDs PWM
void toggle_leds_pwm(uint16_t *vry,uint16_t *vrx)
{
    leds_pwm_state = !leds_pwm_state;       // Alterna o estado dos LEDs PWM
    if (leds_pwm_state) {
        // Ativa os LEDs RGB, se o estado for "ligado"
        pwm_set_gpio_level(LED_B, map_joystick_to_pwm(vry)); // Ajusta o brilho do LED azul conforme o joystick
        pwm_set_gpio_level(LED_R, map_joystick_to_pwm(vrx)); // Ajusta o brilho do LED vermelho conforme o joystick
    } else {
        // Desliga os LEDs RGB, se o estado for "desligado"
        pwm_set_gpio_level(LED_B, 0); // Apaga o LED azul
        pwm_set_gpio_level(LED_R, 0); // Apaga o LED vermelho
    }
    printf("LEDs PWM: %s\n", leds_pwm_state ? "ativados" : "desativados"); // Mensagem de depuração
}

// Função principal
int main()
{
    uint16_t vrx_value, vry_value; // Variáveis para armazenar os valores do joystick (eixos X e Y)
    setup();                        // Chama a função de configuração
    printf("Joystick-PWM\n");       // Exibe uma mensagem inicial via porta serial

    // Loop principal
    while (1)
    {
        joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick

        // Verifica se o botão do joystick foi pressionado
        if (gpio_get(SW) == 0)
        {
            toggle_green_led();      // Alterna o estado do LED verde
            sleep_ms(300);           // Pequeno delay para evitar múltiplas detecções de um único pressionamento
        }

        // Verifica se o botão A foi pressionado
        if (gpio_get(BUTTON_A) == 0)
        {
            toggle_leds_pwm(&vrx_value,&vry_value);       // Alterna o estado dos LEDs PWM (desliga/liga os LEDs)
            sleep_ms(300);           // Pequeno delay para evitar múltiplas detecções de um único pressionamento
        }

        // Pequeno delay para uma resposta mais fluida
        sleep_ms(10);
    }
}
