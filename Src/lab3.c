#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
int lab3_main(void) {
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_PULLDOWN};
    HAL_GPIO_Init(GPIOC, &initStr);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
    TIM2_UEV_Interrupt_Setup();
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn,1);
    while(1){

    }
}

void TIM2_IRQnHandler(void){
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    TIM2 -> SR &= ~1;
}

