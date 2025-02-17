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
    RCC->APB1ENR |= 0b1;
    TIM2->PSC = 7999; //7999 in Hex
    TIM2->ARR = 1500; //ARR is 1500
    TIM2->DIER |= 0b1; //Update interrupt enable!
    TIM2->CR1 |= 0b1; // turning timer on.



    //SYSCFG->ITLINE16 Timer 2 Interrupt Status Register
    //Interrupt Vector Table: TIM2 / TIM3 - Global Interrupt
}

void TIM3_UEV_Interrupt_Setup(void){
    RCC->APB1ENR |= (1<<1);
    TIM3->PSC = 1250; //7999 in Hex
    TIM3->ARR = 25; //ARR is 250
    //TIM3->DIER |= 0b1; //Update interrupt enable!
    TIM3->CCMR1 &= ~0b11; //setting CCMR1 Channel 1 and 2 to output
    TIM3 ->CCMR1 &= ~((1<<9)|(1<<8));
    TIM3 ->CCMR2 &= ~0b11; //setting CCMR2 Channel 3 and 4 to output
    TIM3 ->CCMR2 &= ~((1<<9)|(1<<8));
    TIM3 ->CCMR1 |= ((1<<4)|(1<<5)|(1<<6)); //setting the Outpare Compare Mode to PWM Mode 2
    TIM3 ->CCMR1 |= ((1<<14)|(1<<13)); //setting the Output Compare Mode to PWM MOde 1
    TIM3 ->CCMR1 &= ~(1<<12);
    TIM3 ->CCMR1 |= ((1<<11)|(1<<3));
    TIM3 -> CCER |= 0b10001; //enabling capture/compare enable register
    TIM3 -> CCR1 = 5;
    TIM3 -> CCR2 = 5;
    //TIM3->CR1 |= 0b1;
}

void AF_init_lab3(void){
    GPIOC -> MODER |= 0b1111000000000000;
    GPIOC -> AFR[0] = 0;
}



    //SYSCFG->ITLINE16 Timer 2 Interrupt Status Register
    //Interrupt Vector Table: TIM2 / TIM3 - Global Interrupt}