// Jakob Travis Embedded Systems - Partner: Clay Stanley

#include "stm32f0xx.h"
#include "hal_gpio.h"
#include "stm32f0xx_hal.h"
#include <assert.h>
#include <main.h>
// LED pins on Discovery: PE8 (LED1), PE9 (LED2), PE10 (LED3), PE11 (LED4)
#define LED_GREEN_PIN   6  // PC6
#define LED_ORANGE_PIN  7  // PC7
#define LED_RED_PIN     8  // PC8
#define LED_BLUE_PIN    9  // PC9


// // ADC channel for PC0
// #define ADC_CHANNEL       10U

// // Thresholds for 8-bit ADC (0–255)
// #define THRESH1  64U
// #define THRESH2  128U
// #define THRESH3  192U
// #define THRESH4  240U

// //part 1
// int lab6_main(void) {
//     // 1) Enable GPIOC (LEDs) and GPIOC (PC0) clocks
//     GPIO_Init6();
//     // 2) Configure PE8–PE11 as general-purpose outputs (00=IN, 01=OUT, 10=AF, 11=AN)
//     GPIOC->MODER &= ~((3U<<6)|(3U<<7)|(3U<<8)|(3U<<9));
//     GPIOC->MODER |=  ((1U<<6)|(1U<<7)|(1U<<8)|(1U<<9));
   
//     // 4) Enable ADC1 clock
//     RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 
//     GPIOC->ODR |= (1<<LED_GREEN_PIN);
//     // 5) ADC1 configuration: 8-bit resolution, continuous mode, software trigger
//     ADC1->CFGR1 = (1<<4)    // 8-bit
//                   | ADC_CFGR1_CONT;     // continuous conversions 
//                   ADC1->CFGR1 &= ~(3UL<<10); //clearing 11 and 10 bit to disable hardware trigger detections
//                   //ADC1->CHSELR = ADC_CHANNEL_10;
//                   //ADC1->CHSELR = ADC_IN10;
//     // 6) Select channel 10 (PC0)
//     ADC1->CHSELR |= (1U << ADC_CHANNEL);

//     // // 7) Calibrate ADC
//     // ADC1->CR |= ADC_CR_ADCAL;                // start calibration
//     // while(ADC1->CR & ADC_CR_ADCAL) { }       // wait until cleared

//     // // 8) Enable ADC
//     // ADC1->CR |= ADC_CR_ADEN;                 // enable
//     // while(!(ADC1->ISR & ADC_ISR_ADRDY)) { }  // wait for ready :contentReference[oaicite:6]{index=6}&#8203;:contentReference[oaicite:7]{index=7}

//     // // 9) Start regular conversion
//     // ADC1->CR |= ADC_CR_ADSTART;              // software trigger
//     adc_self_calibrate_enable_and_start(); // call the function to calibrate and start ADC
//     // 10) Main loop: read ADC, update LEDs
//     GPIOC->ODR &= ~ ((1<<LED_ORANGE_PIN)| (1<<LED_RED_PIN)|(1<<LED_BLUE_PIN)|(1<<LED_GREEN_PIN)); // turn off all LEDs
//     while(1) {
//         // wait for end-of-conversion
//         while(!(ADC1->ISR & ADC_ISR_EOC)) { }
//         uint8_t val = (uint8_t)(ADC1->DR & 0xFF);
//         // clear EOC flag
//         ADC1->ISR |= ADC_ISR_EOC;

//         // // turn LEDs off
//         // GPIOC->ODR &= ~(LED_GREEN_PIN|LED_ORANGE_PIN|LED_RED_PIN|LED_BLUE_PIN);
//         // GPIOC->ODR |= (LED_GREEN_PIN|LED_ORANGE_PIN|LED_RED_PIN|LED_BLUE_PIN);

//         // light up LEDs in sequence
//         if (val < THRESH1) {
//             GPIOC->ODR &= ~ ((1<<LED_ORANGE_PIN)| (1<<LED_RED_PIN)|(1<<LED_BLUE_PIN)|(1<<LED_GREEN_PIN)); // turn off all LEDs
//         }
//         if (val > THRESH1){
//             GPIOC->ODR |= (1<<LED_GREEN_PIN);
//             GPIOC->ODR &= ~ ((1<<LED_ORANGE_PIN)| (1<<LED_RED_PIN)|(1<<LED_BLUE_PIN)); 
//         }
//         if (val > THRESH2){
//             GPIOC->ODR |= ((1<<LED_BLUE_PIN)|(1<<LED_GREEN_PIN));
//             GPIOC->ODR &= ~ ((1<<LED_ORANGE_PIN)| (1<<LED_RED_PIN)); 
//         }
//         if (val > THRESH3){
//             GPIOC->ODR |= ((1<<LED_BLUE_PIN)|(1<<LED_GREEN_PIN)|(1<<LED_RED_PIN));
//             GPIOC->ODR &= ~ ((1<<LED_ORANGE_PIN)); 
//         }

//         if (val > THRESH4) GPIOC->ODR |= ((1<<LED_ORANGE_PIN)|(1<<LED_BLUE_PIN)|(1<<LED_GREEN_PIN)|(1<<LED_RED_PIN)); // all LEDs on

//         delay_ms(50); // debounce noise
//     }
// }


//part 2

#include "stm32f0xx.h"

static const uint8_t sine32[32] = {127,151,175,197,216,232,244,251,254,251,244,
    232,216,197,175,151,127,102,78,56,37,21,9,2,0,2,9,21,37,56,78,102};


void delay_ms6(uint32_t ms) {
    // rough busy‐wait @48MHz
    for(uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

int lab6_main(void) {
    // 1) Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;              
    // 2) Set PA4 to analog mode (MODER4 = 11), no pull‐up/down
    GPIOA->MODER   |=  (3U << (4 * 2));
    //GPIOA->PUPDR   &= ~(3U << (4 * 2));

    // 3) Enable DAC1 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;            

    // 4) Configure DAC channel1:
    //    – Enable channel1 (EN1 bit0)
    //    – Enable trigger (TEN1 bit2)
    //    – Leave TSEL1=111 for software trigger
    DAC1->CR |= (0x7<<3);
    DAC1->CR |= DAC_CR_EN1;  // bit 0: enable output on DAC_OUT1 (PA4)
            // | DAC_CR_TEN1; // bit 2: enable trigger
    DAC1->SWTRIGR = DAC_SWTRIGR_SWTRIG1;
    // 5) Main loop: output waveform
    int idx = 0;
    while (1) {
        // Write next 8‐bit value, right-aligned, to DHR8R1 (offset 0x08)
        DAC1->DHR8R1 = sine32[idx];


        if (idx == 31){
            idx = 0;
        }
        else{
            idx++;
        }

        // 1 ms delay → ~31 Hz output over 32 samples
        delay_ms6(1);
    }
}

void adc_self_calibrate_enable_and_start(void) {
    // 1) Ensure ADC is powered off (ADEN=0) before calibration
    if (ADC1->CR & ADC_CR_ADEN) {
        ADC1->CR |= ADC_CR_ADDIS;               // disable ADC
        while (ADC1->CR & ADC_CR_ADEN) { }      // wait until fully disabled
    }

    // 2) Start calibration
    ADC1->CR |= ADC_CR_ADCAL;                   // set ADCAL bit
    while (ADC1->CR & ADC_CR_ADCAL) { }         // wait until ADCAL bit clears

    // 3) Enable the ADC
    ADC1->CR |= ADC_CR_ADEN;                    // set ADEN bit
    // 4) Wait for ADC ready (ADRDY flag)
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) { }
    // Clear ADRDY by writing 1 (clears the flag in ISR)
    ADC1->ISR |= ADC_ISR_ADRDY;

    // 5) Start conversions (software trigger)
    ADC1->CR |= ADC_CR_ADSTART;                 // set ADSTART bit
    // From here, ADC will continuously convert if in continuous mode,
    // or perform one conversion if in single mode.
}