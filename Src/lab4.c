#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
#include <hal_gpio.h>

int lab4_main(void) {
    HAL_Init();
    //SystemClock_Config();
    My_HAL_RCC_GPIOA_C_CLK_ENABLE();
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStr);
    GPIO_InitTypeDef initAF = {GPIO_PIN_10 | GPIO_PIN_11,
                        GPIO_MODE_AF_PP,
                        GPIO_SPEED_FREQ_LOW,
                        GPIO_NOPULL,
                        GPIO_AF1_USART3};
    HAL_GPIO_Init(GPIOC, &initAF);
    
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);

    //init_UART3_PC10_PC11(); //PC 10 is TX, PC11 is RX Boardside
    USART3_init();

    volatile char message;
    volatile char character;
    //while((USART3->ISR) & (1<<7)){ //while the Transmit Data register is full
    Transmit_USART3('Hello');
    //}
    while(1){
        while(USART3->ISR & (1<<5)){ //while the Read Data register is not empty
            character = (USART3->RDR |= 0xFF);
        }
        if (character == 'r'){
                My_HAL_GPIO_TogglePin(GPIOC, (GPIO_PIN_7));
                message = 'Red LED toggled';
                Transmit_USART3(message);

        }
        else{
            message = 'Not valid instruction';
            Transmit_USART3(message);
        }
    }
};