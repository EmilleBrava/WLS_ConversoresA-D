# WLS_ConversoresA-D


Este projeto tem como objetivo explorar o funcionamento do conversor analógico-digital (ADC) no RP2040 e utilizar a placa de desenvolvimento **BitDogLab Pico W** para controlar LEDs RGB com um joystick, além de exibir a posição do joystick em um display SSD1306. O sistema utiliza **PWM** para controlar os LEDs e **I2C** para integrar o display.

## Objetivos do Projeto

- **Compreender o funcionamento do conversor analógico-digital (ADC)** no RP2040.
- **Controlar a intensidade de LEDs RGB** com base nos valores do joystick utilizando PWM.
- **Exibir a posição do joystick** no display SSD1306 por meio de um quadrado móvel.
- **Aplicar o protocolo de comunicação I2C** para integração com o display SSD1306.
- **Implementar interrupções** para o controle dos botões.

## Funcionalidades

### Controle de LEDs RGB

- **LED Azul**: O brilho do LED azul é controlado pelo eixo Y do joystick.
  - Quando o joystick estiver na posição central (valor 2048), o LED ficará apagado.
  - À medida que o joystick for movido para cima ou para baixo, o LED aumentará seu brilho de 0 a 4095.

- **LED Vermelho**: O brilho do LED vermelho é controlado pelo eixo X do joystick.
  - Quando o joystick estiver na posição central (valor 2048), o LED ficará apagado.
  - Movendo o joystick para a esquerda ou para a direita, o LED aumentará seu brilho de 0 a 4095.

### Display SSD1306

- Um **quadrado de 8x8 pixels** será exibido no display SSD1306, com a posição centralizada inicialmente.
- O quadrado se moverá proporcionalmente aos valores dos eixos do joystick.

### Botões

- **Botão do Joystick**:
  - Alterna o estado do **LED Verde** (liga/desliga).
  - Modifica a borda do display SSD1306, alternando entre diferentes estilos de borda a cada novo acionamento.

- **Botão A**:
  - Ativa ou desativa os LEDs PWM a cada acionamento.

## Componentes Utilizados

- **BitDogLab Pico W** (Raspberry Pi Pico W)
- **Joystick** com 2 eixos analógicos (X e Y) e 1 botão
- **LED RGB** (LED Vermelho, Verde e Azul)
- **Display SSD1306** (128x64, via I2C)
- **Botão A** (para controle do LED Verde)

## Diagrama de Conexões

| Componente         | Pino GPIO | Função                                    |
|--------------------|-----------|-------------------------------------------|
| Joystick Eixo X    | GPIO 26   | Leitura analógica do eixo X               |
| Joystick Eixo Y    | GPIO 27   | Leitura analógica do eixo Y               |
| Joystick Botão     | GPIO 22   | Leitura digital (botão pressionado)       |
| LED Vermelho       | GPIO 11   | Controle PWM do LED Vermelho             |
| LED Verde          | GPIO 12   | Controle do LED Verde (botão A)          |
| LED Azul           | GPIO 13   | Controle PWM do LED Azul                 |
| Botão A            | GPIO 5    | Controle digital do LED Verde            |
| Display SSD1306    | GPIO 14, GPIO 15 | Comunicação I2C com o display  |

## Requisitos

- **Uso de Interrupções (IRQ)**: Todas as funcionalidades relacionadas aos botões devem ser implementadas utilizando rotinas de interrupção.
- **Debouncing**: Implementação do tratamento de debounce dos botões via software.
- **Uso do Display 128x64**: Demonstração do funcionamento do display e do protocolo I2C.
- **Organização do Código**: Código bem estruturado, modularizado e comentado.

