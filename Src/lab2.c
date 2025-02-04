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
    My_HAL_GPIO_Init(GPIOA, &initStr_A);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
    uint32_t debouncer = 0;

    while (1) {
        HAL_Delay(200); // Delay 200ms

        int button_check = My_HAL_GPIO_ReadPin(GPIOA ,GPIO_PIN_0);
        debouncer = (debouncer << 1) | (button_check & 0x01);
        
        if (debouncer == 0x7FFFFFFF) {
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7); // Toggle LEDs

            
            while (My_HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
            debouncer = 0;
        }
    }
    HAL_Delay(10);
    
}