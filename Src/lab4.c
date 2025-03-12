#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
#include <hal_gpio.h>

volatile char character;

int lab4_main(void) {
    HAL_Init();
    SystemClock_Config();

    // Ungate peripheral clocks
    My_HAL_RCC_GPIOA_C_CLK_ENABLE(); // GPIOA and GPIOC
    RCC->APB1ENR |= (1<<18); // USART3

    // Making sure the global character for receiving characters in a known value.
    character = '&';

    // Initialize LEDs
    GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
    GPIO_MODE_OUTPUT_PP,
    GPIO_SPEED_FREQ_LOW,
    GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStr);

    // Setup USART3 to use PC4 and PC5.
    init_UART3_PC4_PC5(); //PC 5 is TX, PC4 is RX Boardside
    USART3_init();

//----------------------------------Checkoff 1-------------------------
    volatile char* message;

    HAL_Delay(1000);
    Transmit_USART3("Hello\r\n");
    while(1){
        //Do nothing while the RECEIVE data register is empty, otherwise handle the (received) data inside it.
		if( (USART3->ISR & (1<<5) ) ) {
			Process_TDR_Part_I( USART3->RDR & (0xFF) ); // Bottom 8 bits is the character.
		}
    };
}

void Process_TDR_Part_I(char c) {
	
	switch(c) {
		case '\0':
			break;
		case 'r':
			GPIOC->ODR ^= RED;
			break;
		case 'g':
			GPIOC->ODR ^= GREEN;
			break;
		case 'b':
			GPIOC->ODR ^= BLUE;
			break;
		case 'o':
			GPIOC->ODR ^= ORANGE;
			break;
		default:
			Transmit_USART3("You're only allowed to type one of the 4 colors. Try again nerd.\r\n");
	}
	
}


//     while(1){
//         while(USART3->ISR & (1<<5)){ //while the Read Data register is not empty
//             character = (USART3->RDR |= 0xFF);
//         }
//         if (character == 'r'){
//                 My_HAL_GPIO_TogglePin(GPIOC, (GPIO_PIN_7));
//                 message = 'Red LED toggled';
//                 Transmit_USART3(message);

//         }
//         else{
//             message = 'Not valid instruction';
//             Transmit_USART3(message);
//         }
//     }
// };