#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definição de pinos
#define VRX 26
#define VRY 27
#define SW 22
#define BUTTON_A 5
#define LED_B 13
#define LED_R 11
#define LED_G 12

// Parâmetros do PWM
#define DIVIDER_PWM 16.0
#define PERIOD 4095

// Parâmetros do display SSD1306
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define SSD1306_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SQUARE_SIZE 8

// Centro do joystick para calibração
#define JOYSTICK_CENTER 2048

// Estado dos LEDs e borda do display
bool led_g_state = false;
bool leds_pwm_state = true;
uint8_t border_state = 0;

// Variáveis do display e joystick
ssd1306_t ssd;
int square_pos_x = SCREEN_WIDTH / 2 - SQUARE_SIZE / 2;
int square_pos_y = SCREEN_HEIGHT / 2 - SQUARE_SIZE / 2;
uint slice_led_b, slice_led_r;

// Configuração do joystick, botões e ADC
void setup_joystick_and_buttons() {
    adc_init();
    adc_gpio_init(VRX);
    adc_gpio_init(VRY);
    
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
    
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
}

// Configuração do PWM para os LEDs
void setup_pwm_led(uint led, uint *slice) {
    gpio_set_function(led, GPIO_FUNC_PWM);
    *slice = pwm_gpio_to_slice_num(led);
    pwm_set_clkdiv(*slice, DIVIDER_PWM);
    pwm_set_wrap(*slice, PERIOD);
    pwm_set_gpio_level(led, 0);
    pwm_set_enabled(*slice, true);
}

// Inicialização do I2C e do display SSD1306
void setup_display() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, SCREEN_WIDTH, SCREEN_HEIGHT, false, SSD1306_ADDR, I2C_PORT);
}

// Configuração inicial
void setup() {
    stdio_init_all();
    setup_joystick_and_buttons();
    setup_pwm_led(LED_B, &slice_led_b);
    setup_pwm_led(LED_R, &slice_led_r);
    
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    
    setup_display();
}

// Leitura do joystick
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    adc_select_input(0);
    sleep_us(2);
    *vrx_value = adc_read();

    adc_select_input(1);
    sleep_us(2);
    *vry_value = adc_read();
}

// Mapeamento do joystick para PWM (suporte para variação de intensidade)
uint16_t map_joystick_to_pwm(uint16_t value) {
    return (value >= JOYSTICK_CENTER) ? (value - JOYSTICK_CENTER) * 2 : (JOYSTICK_CENTER - value) * 2;
}

// Atualização dos LEDs RGB conforme posição do joystick
void update_leds(uint16_t vrx, uint16_t vry) {
    if (leds_pwm_state) {
        pwm_set_gpio_level(LED_B, map_joystick_to_pwm(vry));
        pwm_set_gpio_level(LED_R, map_joystick_to_pwm(vrx));
    } else {
        pwm_set_gpio_level(LED_B, 0);
        pwm_set_gpio_level(LED_R, 0);
    }
}

// Alternância do LED verde
void toggle_green_led() {
    led_g_state = !led_g_state;
    gpio_put(LED_G, led_g_state);
}

// Alternância dos LEDs PWM
void toggle_leds_pwm() {
    leds_pwm_state = !leds_pwm_state;
}

// Alternância do estilo da borda
void toggle_display_border() {
    border_state = (border_state + 1) % 3;  // Alterna entre 3 estilos de borda
}

// Mapeamento do ADC para a tela
int map_adc_to_screen(int adc_value, int center_value, int screen_max) {
    int offset = adc_value - center_value;
    int mapped_value = ((offset * (screen_max / 2)) / 2048) + (screen_max / 2);
    if (mapped_value < 0) mapped_value = 0;
    if (mapped_value > screen_max) mapped_value = screen_max;
    return mapped_value;
}

// Atualização da posição do quadrado na tela
void update_square_position() {
    uint16_t adc_x, adc_y;
    joystick_read_axis(&adc_x, &adc_y);

    square_pos_x = map_adc_to_screen(adc_x, JOYSTICK_CENTER, SCREEN_WIDTH - SQUARE_SIZE);
    square_pos_y = SCREEN_HEIGHT - SQUARE_SIZE - map_adc_to_screen(adc_y, JOYSTICK_CENTER, SCREEN_HEIGHT - SQUARE_SIZE);
}

// Atualização da tela SSD1306
void update_display() {
    ssd1306_fill(&ssd, false);

    // Desenha borda conforme o estado
    if (border_state == 1) {
        ssd1306_rect(&ssd, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, true, false);
    } else if (border_state == 2) {
        ssd1306_rect(&ssd, 2, 2, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 4, true, false);
    }

    // Desenha o quadrado do joystick
    ssd1306_rect(&ssd, square_pos_x, square_pos_y, SQUARE_SIZE, SQUARE_SIZE, true, false);
    
    ssd1306_send_data(&ssd);
}

// Interrupção para o botão A e o botão do joystick
void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A) {
        toggle_leds_pwm();
    }
    if (gpio == SW) {
        toggle_green_led();
        toggle_display_border();
    }
}

// Loop principal
int main() {
    setup();
    
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (1) {
        uint16_t vrx, vry;
        joystick_read_axis(&vrx, &vry);
        
        update_leds(vrx, vry);
        update_square_position();
        update_display();
        
        sleep_ms(10);
    }
}

