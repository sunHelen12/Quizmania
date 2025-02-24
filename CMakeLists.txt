cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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
set(PICO_BOARD pico CACHE STRING "Board type")

# pull in raspberry pi pico sdk (must be before project)
include(pico_sdk_import.cmake)

project(quizmania C CXX ASM)

# initialise the raspberry pi pico sdk
pico_sdk_init()

# add executable. default name is the project name, version 0.1
add_executable(quizmania quizmania.c inc/ssd1306.c)

pico_set_program_name(quizmania "quizmania")
pico_set_program_version(quizmania "0.1")

# modify the below lines to enable/disable output over uart/usb
pico_enable_stdio_uart(quizmania 0)  
pico_enable_stdio_usb(quizmania 1)

pico_generate_pio_header(quizmania ${CMAKE_CURRENT_LIST_DIR}/quizmania.pio)

# adicionar as bibliotecas necessárias ao projeto
target_link_libraries(quizmania
        pico_stdlib
        hardware_pio
        hardware_clocks
        hardware_adc
        hardware_uart
        hardware_i2c
        pico_bootrom
        hardware_pwm
        pico_time
)

# adicionar os diretórios de inclusão
target_include_directories(quizmania PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# geração dos arquivos binários extras
pico_add_extra_outputs(quizmania)