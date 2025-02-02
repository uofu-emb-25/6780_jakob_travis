#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    GPIOC -> MODER |= 0b01010101000000000000;
    GPIOA -> MODER |= 0b00;
    GPIOC -> OTYPER |= 0b0000000000;
    GPIOC -> OSPEEDR |= 0b00000000000000000000;
    GPIOC -> PUPDR |= 0b00000000000000000000;
    GPIOA -> OSPEEDR |= 0b00;
    GPIOA -> PUPDR |= 0b00;


}


void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}



GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return -1;
}



void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
}



void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
}

