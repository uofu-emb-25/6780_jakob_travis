#include <stm32f0xx_hal.h>
#include <assert.h>
int lab1_main(void) {
    HAL_Init(); // Reset of all peripherals, init the Flash and Systick
    SystemClock_Config(); //Configure the system clock
    /* This example uses HAL library calls to control
    the GPIOC peripheral. You’ll be redoing this code
    with hardware register access. */
    //__HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock in the RCC
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    // Set up a configuration struct to pass to the initialization function
    GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    My_HAL_GPIO_Init(GPIOC, &initStr);// Initialize pins PC8 & PC9
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
    //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // Start PC8 high
    //assert(GPIOC->MODER == 0 << 14);
    //assert(GPIOC-> MODER == 1 << 16);
    while (1) {
        HAL_Delay(200); // Delay 200ms
        // Toggle the output state of both PC8 and PC9
        //if (GPIOA->IDR & 0x1){
        int check = My_HAL_GPIO_ReadPin(GPIOA ,GPIO_PIN_0);
        if(check) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7);
        }
    
    }
}
