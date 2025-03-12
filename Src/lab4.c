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

void Checkoff_4_2(void) {
	while(1) {
		// Display a prompt to the user to get two characters.
		//Transmit_String("..");
		
		// Receive the first character
		while( !(USART3->ISR & (1<<5) )) {}
		received_byte = USART3->RDR & 0xFF;
		Transmit_String("\r\nFirst character is "); Transmit_Char(received_byte);
		
		// If it's an accepted letter, save it and receive the second character.
		if ( received_byte == 'r' || received_byte == 'g' || received_byte == 'b' || received_byte == 'o') {
			LED_ID = received_byte;
			
			// Recieve the second character
			while( !(USART3->ISR & (1<<5) )) {
			}
			received_byte = USART3->RDR & 0xFF;
			Transmit_String("\r\nSecond character is "); Transmit_Char(received_byte);
			
			// If it's a number then perform the corresponding action.
			if(received_byte >= '0' && received_byte <= '2') {
				action_ID = received_byte;
				Process_TDR_Part_II(LED_ID, action_ID);
				continue;
			}
		}
		// Otherwise the input was invalid. Broadcast an error message and return to the beginning state.
		Transmit_String("\r\nThat's is not a valid command, Try again."); // Error message for invalid character
  }
}

// void Process_TDR_Part_I(char c) {
	
// 	switch(c) {
// 		case '\0':
// 			break;
// 		case 'r':
// 			GPIOC->ODR ^= RED;
// 			break;
// 		case 'g':
// 			GPIOC->ODR ^= GREEN;
// 			break;
// 		case 'b':
// 			GPIOC->ODR ^= BLUE;
// 			break;
// 		case 'o':
// 			GPIOC->ODR ^= ORANGE;
// 			break;
// 		default:
// 			Transmit_USART3("You're only allowed to type one of the 4 colors. Try again nerd.\r\n");
// 	}
	
// }


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