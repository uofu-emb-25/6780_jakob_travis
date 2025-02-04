#include <stm32f0xx_hal.h>
#include <assert.h>
int lab2_main(void) {
    HAL_Init(); // Reset of all peripherals, init the Flash and Systick
    SystemClock_Config(); //Configure the system clock
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStr);
    GPIO_InitTypeDef initStr_A = {GPIO_PIN_0,
    GPIO_MODE_INPUT,
    GPIO_SPEED_FREQ_LOW,
    GPIO_PULLDOWN,
    };
    My_HAL_GPIO_Init(GPIOC, &initStr);
    My_HAL_GPIO_Init(GPIOA, &initStr_A);


    assert((EXTI->RTSR & 0x1) == 0);
    EXTI_rising_button_toggle();
    assert((EXTI ->RTSR & 0x1) ==1);

    SYSCFG_Initialization();
    assert(((SYSCFG->EXTICR[0]) == 0));

    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_SetPriority(EXTI0_1_IRQn,1);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);





    while (1) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        HAL_Delay(600); // Delay 200ms
        SysTick_Handler();

        //int button_check = My_HAL_GPIO_ReadPin(GPIOA ,GPIO_PIN_0);
        
        //if (button_check) {
        //    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7); // Toggle LEDs
        //}

    }
    
}

void EXTI0_1_IRQHandler(void){
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    EXTI -> PR |= 0b1;
}