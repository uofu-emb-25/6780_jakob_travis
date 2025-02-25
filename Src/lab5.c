#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
int lab3_main(void) {
    HAL_Init();
    //SystemClock_Config();
    
    //------------------------5.2 Setting GPIO Modes-------------------
    My_HAL_RCC_GPIO_BC_CLK_ENABLE(); //enabling clocks B and C

    GPIO_InitTypeDef I2C2_Pin_init = { GPIO_PIN_11| GPIO_PIN_13, // SETTINGS FOR PB11
                                    GPIO_MODE_AF_OD,
                                    GPIO_AF1_I2C2
    };

    HAL_GPIO_Init(GPIOB, &I2C2_Pin_init); //initialing the I2C2 pins PB11, and PB13


    GPIO_InitTypeDef PB14 = { GPIO_PIN_14,
                            GPIO_MODE_OUTPUT_PP,

    };

    HAL_GPIO_Init(GPIOB, &PB14); //Initializing PB14 as Output Mode, and Push pull type!

    GPIO_InitTypeDef PC0 = { GPIO_PIN_0,
            GPIO_MODE_OUTPUT_PP
    };
    
    Hal_GPIO_Init(GPIOC, &PC0); //Initiliazing PC0 as Output Mode, and Push pull type!

    My_HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0, 1); //Writing PC0 and PB14 high
    My_HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,1);

    //------------Initiliazing LEDs-----------------
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStr);

    //------------I2C Initiliazition--------------
    RCC -> APB1ENR |= RCC_APB1ENR_I2C2EN; //enabling the I2C2 clock in RCC peripheral
    // TIMINGR Registrer for 100kHz...SCLL = 0x13, SCLH - 0xF, SDADEL = 0x2, SCLDEL = 0x4, PRESC = 1.
    I2C2 -> TIMINGR |= 0x13;
    I2C2 -> TIMINGR |= (0xF<<8);
    I2C2 -> TIMINGR |= (0x2<<16);
    I2C2 -> TIMINGR |= (0x4<<20);
    I2C2 -> TIMINGR |= (1<<28);
    
    I2C2 -> CR1 = 1; //enabling I2C peripheral using the PE bit in CR1

    //---------------Reading the Register---------------
    //------1.--------
    uint8_t L3addy = 0x6B;
    I2C2 -> CR2 |= (L3addy<<1); // setiing L3GD20's slave address into the CR2 shifted by 1
    I2C2 -> CR2 |= (1<<16);//number of bits to transmit =1
    I2C2 -> CR2 &= ~(1<<10); //Master Requests a write transfer
    I2C2 -> CR2 |= (1<<13); //START GENERATION!

    //-----5.4------
    while(~(I2C2->ISR & I2C_ISR_NACKF)|(I2C2->ISR & I2C_ISR_TXIS)){//if either TXIS or NACKF are not set - stay in while loop
        if (I2C2->ISR & I2C_ISR_NACKF){
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //toggle blue LED if NACKF flag is set...slave did not respond.
        }
        else{
            My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // toggle pin 6
            I2C2 -> CR1 |= (1<<1); //enabling TXIE
            I2C2 -> TXDR |= (I2C2->CR2 & I2C_CR2_SADD); //Writing the address into the Transmit data repo

        };
    }


};