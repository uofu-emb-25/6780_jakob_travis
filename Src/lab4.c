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
    //My_HAL_GPIO_Init(GPIOC, &initStr);
    //My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
    init_UART3_PC10_PC11(); //PC 10 is TX, PC11 is RX
    //AF_init_lab3();
    TIM2_UEV_Interrupt_Setup();
    TIM3_UEV_Setup();
    AF_init_lab3();
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn,3);

    while(1){

    }
}

void TIM2_IRQHandler(void){
    My_HAL_GPIO_TogglePin(GPIOC, (GPIO_PIN_8 | GPIO_PIN_9));
    TIM2->SR &= ~1;
}