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
    GPIOA -> PUPDR |= 0b10;


}


void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}



GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{  
   return (GPIOx->IDR & GPIO_Pin);
}



void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int PinState)
{
    if (PinState == 1){
        GPIOx->ODR |= GPIO_Pin;
    }
    if (PinState == 0){
        GPIOx->ODR &= ~(GPIO_Pin);
    }
}



void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->ODR ^= GPIO_Pin;
}

void My_HAL_RCC_GPIOC_CLK_ENABLE(void){
    RCC->AHBENR |= (1<<19);
    RCC->AHBENR |= (1<<17);
}

void EXTI_rising_button_toggle(void){
    EXTI->IMR |= 0b1;
    EXTI -> RTSR |= 0b1;
}

void SYSCFG_Initialization(void){
    RCC->APB2ENR |= 0b1;
    SYSCFG -> EXTICR[0] |= 0b000; //setting to PA0
}

void TIM2_UEV_Interrupt_Setup(void){
    RCC->APB1ENR |= 0b11;
    TIM2->PSC = 7999; //7999 in Hex
    TIM2->ARR = 250; //ARR is 250
    TIM2->DIER |= 0b1; //Update interrupt enable!
    TIM2->CR1 |= 0b1; // enabled the CEN (counter enable)



    //SYSCFG->ITLINE16 Timer 2 Interrupt Status Register
    //Interrupt Vector Table: TIM2 / TIM3 - Global Interrupt
}