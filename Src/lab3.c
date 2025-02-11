#include <stm32f0xx_hal.h>
#include <assert.h>
int lab3_main(void) {
    HAL_Init(); // Reset of all peripherals, init the Flash and Systick
    SystemClock_Config(); //Configure the system clock
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    My_HAL_GPIO_Init(GPIOC, &initStr);
    //RCC->APB1ENR |= 0b11;
    TIM2_UEV_Interrupt_Setup();
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn,1);
}

void TIM2_IRQnHandler(void){
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    TIM2 -> SR |= 0x0;
}

