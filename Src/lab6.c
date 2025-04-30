#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
#include <hal_gpio.h>


int lab6_main(void) {

    //1. initilizing LEDs
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    RCC->APB1ENR |= (1<<18);
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStr);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);

    //initilizing GPIO Pin C0 to analog input function
    GPIOC->MODER|=3; // setting 11 to the first register
    GPIOC->PUPDR &= ~3; // setting no pull-up,pull-down to 0

    //3. enable the ADC1 in RCC peripheral
    RCC->APB2ENR |= (1<<9); //enabling ADC interface clock in RCC

    //4. Configuring the ADC to 8-bit resultion, continuous conversion mode, hardware triggers disabled (software triggers only)
    ADC1->CFGR1 &= ~24; //check this bitwise
    ADC1->CFGR1 |= (1<<4); //set to 8 bit resolution
    ADC1->CFGR1 |= (1<<13); //CONT bit set to continous conversion mode
    ADC1->CFGR1 &= ~3072; //clearing 11 and 10 bit to disable hardware trigger detections
    ADC1->CHSELR = ADC_CHANNEL_10;

    //5. setting ADC enable on input pin for ADC conversion
    ADC1->CR |= 1; //enabling ADC
    ADC1->CR &= ~ADC_CR_ADEN; //disabling ADEN
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,1);

    //6. calibration software procedure - APPENDIX A.7.1
    if ((ADC1->CR & ADC_CR_ADEN) != 0){
        ADC1->CR |= ADC_CR_ADDIS;
    }
    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) /* (5) */
    {
    /* For robust implementation, add here time-out management */
    }
    
    //ADC enable sequence code Appendix A.7.2
    if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) /* (1) */
    {
    ADC1->ISR |= ADC_ISR_ADRDY; /* (2) */
    }
    ADC1->CR |= ADC_CR_ADEN; /* (3) */
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (4) */
    {
    /* For robust implementation, add here time-out management */
    }

    //7.read the ADC data resitior and turn off the LEDS depending on the value
    int ADC_value = 0;
    
    ADC1->CR |= (1<<2);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,1);

    while(1){
        ADC_value = ADC_DR_DATA;

        if (ADC_value <= 256/4){
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,0);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,0);
        };
        if ((ADC_value <= 256/2) & (ADC_value > 256/4)){
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,0);
        };
        if ((ADC_value <= 3*(256/4)) & (ADC_value > 256/2)){
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,0);
        };
        if ((ADC_value <= 256) & (ADC_value > 3*(256/4))){
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,1);
        };
    };

}
