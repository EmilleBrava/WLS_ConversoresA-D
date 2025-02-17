# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Define the Raspberry Pi Pico board type
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialize the project
project(joysticadc C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(joysticadc 
    joysticadc.c   # Arquivo principal do código
)

# Define the program name and version
pico_set_program_name(joysticadc "joysticadc")
pico_set_program_version(joysticadc "0.1")

# Enable output over UART and USB
pico_enable_stdio_uart(joysticadc 1)
pico_enable_stdio_usb(joysticadc 1)

# Link libraries necessary for the project
target_link_libraries(joysticadc
    pico_stdlib     # Biblioteca padrão (GPIO, UART, etc.)
    hardware_adc    # Biblioteca do conversor ADC (joystick)
    hardware_pwm    # Biblioteca para controle PWM (LEDs)
    hardware_i2c    # Biblioteca para comunicação I2C (SSD1306)
)

# Specify the directories containing the header files
target_include_directories(joysticadc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}  # Diretório atual, onde o código-fonte está localizado
)

# Add any extra libraries if necessary
# target_link_libraries(joysticadc <other-libraries>)

# Generate extra outputs (for example, UF2 file)
pico_add_extra_outputs(joysticadc)
