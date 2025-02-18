#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
int lab4_main(void) {
    HAL_Init();
    SystemClock_Config();
    My_HAL_RCC_GPIOA_C_CLK_ENABLE();
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStr);
    USART3_init();

    init_UART3_PC10_PC11(); //PC 10 is TX, PC11 is RX

    volatile char message;
    volatile char character;
    //while((USART3->ISR) & (1<<7)){ //while the Transmit Data register is full

    //}
    while(~(USART3->ISR & (1<<5))){ //while the Read Data register is not empty
        char character = (USART3->RDR |= 0xFF);
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
};