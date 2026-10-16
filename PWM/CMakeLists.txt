cmake_minimum_required(VERSION 3.10)
project(pwm_led C)

# Libreria de PWM
add_library(pwm pwm.c)
target_include_directories(pwm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Programa principal
add_executable(led main.c)
target_link_libraries(led pwm gpiod)