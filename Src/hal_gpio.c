#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include <main.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    GPIOC -> MODER |= 0b01010000000000000000;
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

void My_HAL_RCC_GPIOA_C_CLK_ENABLE(void){
    RCC->AHBENR |= (1<<19);
    //RCC->AHBENR |= (1<<20);
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

void TIM3_UEV_Setup(void){
    RCC->APB1ENR |= (1<<1);
    TIM3->PSC = 15; //7999 in Hex
    TIM3->ARR = 4; //ARR is 250
    //TIM3->DIER |= 0b1; //Update interrupt enable!
    TIM3->CCMR1 |= 0b00; //setting CCMR1 Channel 1 and 2 to output
    TIM3 ->CCMR1 &= ~((1<<9)|(1<<8));
    TIM3 ->CCMR2 &= ~0b11; //setting CCMR2 Channel 3 and 4 to output
    TIM3 ->CCMR2 &= ~((1<<9)|(1<<8));
    TIM3 ->CCMR1 |= ((1<<4)|(1<<5)|(1<<6)); //setting the Outpare Compare Mode to PWM Mode 2 for Channel 1
    TIM3 ->CCMR1 |= ((1<<14)|(1<<13)); //setting the Output Compare Mode to PWM MOde 1 for Channel 2
    TIM3 ->CCMR1 &= ~(1<<12);
    TIM3 ->CCMR1 |= ((1<<11)|(1<<3));
    TIM3 -> CCER |= ((1<<4)|(1<<0)); //enabling capture/compare enable register
    TIM3 -> CCR1 = 4;
    TIM3 -> CCR2 = 1;
    TIM3-> CR1 |= 0b1;
}

void AF_init_lab3(void){
    GPIOC -> MODER |= ((1<<13)|(1<<15)); //AF mode for PIN6, PIN7 activated
    GPIOC -> MODER &= ~((1<<12)|(1<<14));
    GPIOC -> AFR[0] = 0;
}



    //SYSCFG->ITLINE16 Timer 2 Interrupt Status Register
    //Interrupt Vector Table: TIM2 / TIM3 - Global Interrupt}

void init_UART3_PC10_PC11(void){
    GPIOC -> MODER |= ((1<<23)|(1<<21)); //AF mode for PIN10, PIN11 activated 10 bits in MODER - CLK
    GPIOC -> MODER &= ~((1<<22)|(1<<20));
    GPIOC -> AFR[1] &= ~((0xF<<8)|(0xF<<12));
    GPIOC -> AFR[1] |= ((1<<8)|(1<<12)); //AF1 for 10,11,12 in AFRH register could also be PD6,PD7,PD8

}

void init_UART3_PC4_PC5(void){
    GPIOC -> MODER |= ((1<<9)|(1<<11)); //AF mode for PIN4, PIN5 activated 10 bits in MODER - CLK
    GPIOC -> MODER &= ~((1<<8)|(1<<10));

    GPIOC -> AFR[0] &= ~((0xF<< (4*4) )|(0xF<< (4*5) )); // clear pins 4 and 5
    GPIOC -> AFR[0] |= 1 << (4*4); // AF1 for PC4
    GPIOC -> AFR[0] |= 1 << (4*5); // AF1 for PC5

}

void init_UART3_PD8_PD9_PD10(void){
    
    GPIOD -> MODER |= ((1<<21)|(1<<19)|(1<<17));
    GPIOD -> MODER &= ~((1<<20)|(1<<18)|(1<<16));
    GPIOD -> AFR[0] = 0;
}

void USART3_init(void){
    RCC->APB1ENR |= (1<<18); //Setting USART3 high in RCC reg
    USART3 -> BRR = ((HAL_RCC_GetHCLKFreq())/115200); //Baud div of 416 with clock of 4.8MHz for 115200 -- do I have to shift this over 3 bits???
    USART3 -> CR1 |= ((1<<2)|(1<<3)); //receiver and transmitter enabled
    USART3-> CR1 |= (1<<5);
    USART3 -> CR1 |= 1; //enabling USART
}

//void ASCII_write_USART3(char symbol){
//    USART3->TDR |= (symbol |= 0xFF);
//}

void Transmit_USART3_helper(char c){
    while(!(USART3->ISR & (1<<7))){
    //HAL_Delay(100);
    }
    USART3->TDR = c;
}

void Transmit_USART3(char* string_array){
    for (int i = 0; string_array[i] != '\0'; i++){
        Transmit_USART3_helper(string_array[i]);//writing the character to the transmit register
        };
}

void My_HAL_RCC_GPIO_BC_CLK_ENABLE(void){
    RCC->AHBENR |= (RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN);
}

void I2C2_Read(uint32_t address, int bytes){
    I2C2->CR2 |= I2C_CR2_RD_WRN; //setting Read bit
    I2C2 -> CR2 |= (1<<13); //starting the bit again -- DO I NEED TO CLEAR IT FIRST?
    while(~((I2C2->ISR & I2C_ISR_NACKF)|(I2C2->ISR & I2C_ISR_RXNE))){
        if (I2C2->ISR & I2C_ISR_NACKF){
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); //toggle red LED if NACKF flag is set...slave did not respond.
            break;
        }
        else{
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // toggle pin 6
            I2C2 -> CR1 |= (1<<2); //enabling RXIE
            if ((I2C2->CR2 & I2C_RXDR_RXDATA) == 0xD4){
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
                I2C2->CR2 |= (1<<14); //setting stop bit
                break;
            }

        };
    }

}

void I2C2_Write(uint32_t address, int bytes){
    while(!((I2C2->ISR & I2C_ISR_NACKF)|(I2C2->ISR & I2C_ISR_TXIS))){//if either TXIS or NACKF are not set - stay in while loop
        if (I2C2->ISR & I2C_ISR_NACKF){
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); //toggle blue LED if NACKF flag is set...slave did not respond.
            break;
        }
        else{
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // toggle pin 7
            I2C2 -> TXDR |= 15; //Writing the WHO AM I address into the Transmit data repo
            while (!(I2C2->ISR & I2C_ISR_TC)){
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
                HAL_Delay(20);
            };
            I2C2 -> CR2 |= I2C_CR2_STOP;
            I2C2 -> ICR |= I2C_ICR_STOPCF;
            break;
        };
    }
}
