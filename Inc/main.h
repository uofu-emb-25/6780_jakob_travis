#pragma once
#include <stdint.h>
#include <hal_gpio.h>

void SystemClock_Config(void);
void TIM2_UEV_Interrupt_Setup(void);
void AF_init_lab3(void);
void TIM3_UEV_Setup(void);
void ASCII_write_USART3(int);
void USART3_init(void);
void init_UART3_PC10_PC11_PC12(void);

int lab1_main(void);
int lab2_main(void);
int lab3_main(void);
int lab4_main(void);
int lab5_main(void);
int lab6_main(void);
int lab7_main(void);