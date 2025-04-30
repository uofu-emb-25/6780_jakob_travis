#include <stm32f0xx_hal.h>
#include <assert.h>
#include <main.h>
#include <hal_gpio.h>

// Global variables for USART reception via interrupt
volatile uint8_t rx_data = 0;
volatile uint8_t rx_flag = 0;

// Function prototypes
void USART3_Init(void);
void LED_Init(void);
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Green_On(void);
void LED_Green_Off(void);
void LED_Blue_On(void);
void LED_Blue_Off(void);
void Toggle_Red(void);
void Toggle_Green(void);
void Toggle_Blue(void);
void USART3_Transmit(const char *str);


// 

#define LED_GREEN_PIN   6  // PC6
#define LED_ORANGE_PIN  7  // PC7
#define LED_RED_PIN     8  // PC8
#define LED_BLUE_PIN    9  // PC9

typedef enum {
    WAIT_CMD,
    WAIT_ARG
} RX_State_t;

static volatile RX_State_t rx_state = WAIT_CMD;
static volatile char rx_cmd = 0;
static void process_command(char cmd, char arg);

 void LED_Init(void) {
    // 1) Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // 2) Clear mode bits & set PC6–PC9 to general‑purpose output
    uint32_t mask = (3U << (LED_GREEN_PIN*2))
                  | (3U << (LED_ORANGE_PIN*2))
                  | (3U << (LED_RED_PIN*2))
                  | (3U << (LED_BLUE_PIN*2));
    GPIOC->MODER &= ~mask;
    GPIOC->MODER |= ((1U << (LED_GREEN_PIN*2))
                   | (1U << (LED_ORANGE_PIN*2))
                   | (1U << (LED_RED_PIN*2))
                   | (1U << (LED_BLUE_PIN*2)));

    // 3) Push‑pull, low speed, no pull‑up/pull‑down
    GPIOC->OTYPER  &= ~((1U<<LED_GREEN_PIN)
                      | (1U<<LED_ORANGE_PIN)
                      | (1U<<LED_RED_PIN)
                      | (1U<<LED_BLUE_PIN));
    GPIOC->OSPEEDR &= ~mask;
    GPIOC->PUPDR   &= ~mask;

    // 4) Start with all LEDs off
    GPIOC->ODR &= ~((1U<<LED_GREEN_PIN)
                  | (1U<<LED_ORANGE_PIN)
                  | (1U<<LED_RED_PIN)
                  | (1U<<LED_BLUE_PIN));
}

void LED_Red_On(void)   { GPIOC->ODR |=  (1U<<LED_RED_PIN); }
void LED_Red_Off(void)  { GPIOC->ODR &= ~(1U<<LED_RED_PIN); }
void Toggle_Red(void)   { GPIOC->ODR ^=  (1U<<LED_RED_PIN); }

void LED_Green_On(void) { GPIOC->ODR |=  (1U<<LED_GREEN_PIN); }
void LED_Green_Off(void){ GPIOC->ODR &= ~(1U<<LED_GREEN_PIN); }
void Toggle_Green(void) { GPIOC->ODR ^=  (1U<<LED_GREEN_PIN); }

void LED_Blue_On(void)  { GPIOC->ODR |=  (1U<<LED_BLUE_PIN); }
void LED_Blue_Off(void) { GPIOC->ODR &= ~(1U<<LED_BLUE_PIN); }
void Toggle_Blue(void)  { GPIOC->ODR ^=  (1U<<LED_BLUE_PIN); }

void USART3_GPIO_Config(void) {
    // 1) Enable GPIOC and USART3 clocks
    RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;     // GPIOC clock enable
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;   // USART3 clock enable

    // 2) Configure PC4 (USART3_TX) and PC5 (USART3_RX) to AF mode (MODER = 10)
    GPIOC->MODER &= ~((3U << (4*2)) | (3U << (5*2))); // clear mode bits
    GPIOC->MODER |=  ((2U << (4*2)) | (2U << (5*2))); // set to AF mode

    // 3) Select AF1 for PC4 and PC5 in AFRL (pins 0–7)
    GPIOC->AFR[0] &= ~((0xFU << (4*4)) | (0xFU << (5*4))); // clear AF bits
    GPIOC->AFR[0] |=  ((1U << (4*4)) | (1U << (5*4)));     // AF1 = USART3 :contentReference[oaicite:0]{index=0}&#8203;:contentReference[oaicite:1]{index=1}

    // 4) (Optional) Configure pull‑up on RX pin for idle high
    GPIOC->PUPDR &= ~((3U << (5*2)));
    GPIOC->PUPDR |=  (1U << (5*2)); // 01 = pull‑up
}

/* USART3 initialization:
   1. Enable the system clock to USART3.
   2. Set the baud rate to 115200 using the system clock frequency from HAL_RCC_GetHCLKFreq().
   3. Enable transmitter and receiver hardware.
   4. Enable the USART peripheral.
*/
void USART3_Init(void) {
    uint32_t hclk;
    uint32_t baud_div;

    // 1. Enable the system clock for USART3 via the RCC peripheral.
    // Using direct register access; alternatively, you could use the HAL macro: __HAL_RCC_USART3_CLK_ENABLE();
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // 2. Set the Baud rate for communication to 115200 bps.
    // Get the system clock frequency using HAL_RCC_GetHCLKFreq() or similar function.
    hclk = HAL_RCC_GetHCLKFreq();  // This returns the HCLK frequency in Hz.
    // For 16x oversampling, the baud rate divider is:
    baud_div = hclk / 115200;
    // Set the baud rate register (BRR) to the calculated divider.
    USART3->BRR = baud_div;

    // 3. Enable the transmitter and receiver hardware.
    // Set the TE (Transmitter Enable) and RE (Receiver Enable) bits in the Control Register 1.
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
    // Enable RXNE interrupt (Receive Not Empty) for USART3.
    //sec.4.3
    USART3->CR1 |= USART_CR1_RXNEIE;
    // 4. Enable the USART peripheral.
    // Set the UE (USART Enable) bit in the Control Register 1.
    USART3->CR1 |= USART_CR1_UE;

    // Optional: Wait until the USART is ready (e.g., check the TEACK and REACK flags).
    while (!(USART3->ISR & (USART_ISR_TEACK | USART_ISR_REACK))) {
        // Wait until both transmitter and receiver are enabled
    }
}

/* Simple blocking transmit function for USART3.
   It sends a null-terminated string over USART using the TXE flag to indicate readiness. */
   void USART3_Transmit(const char *str)
   {
       while (*str)
       {
           /* Wait until Transmit Data Register is empty (TXE flag is set) */
           while (!(USART3->ISR & USART_ISR_TXE))
           {
               // Wait for TXE
           }
           USART3->TDR = *str++;  // Send the character and move to the next
       }
   }

//section 4.3
   // Interrupt handler for USART3 reception
//void USART3_IRQHandler(void)
void USART3_4_IRQHandler(void){
    if (USART3->ISR & USART_ISR_RXNE) {
        char c = (char)(USART3->RDR & 0xFF);

        if (rx_state == WAIT_CMD) {
            // Accept only r/g/b (case insensitive)
            if (c=='r'||c=='R' || c=='g'||c=='G' || c=='b'||c=='B') {
                rx_cmd   = c;
                rx_state = WAIT_ARG;
                USART3_Transmit("\r\nArg? ");
            } else {
                USART3_Transmit("\r\nERR: Expect r/g/b\r\nCmd? ");
            }
        }
        else {  // WAIT_ARG
            // Accept only '0','1','2'
            if (c=='0'||c=='1'||c=='2') {
                process_command(rx_cmd, c);
            } else {
                USART3_Transmit("\r\nERR: Expect 0/1/2\r\nCmd? ");
            }
            rx_state = WAIT_CMD;
        }
    }
}
void process_command(char cmd, char arg)
{
    if (arg == '0') {
        if (cmd=='r'||cmd=='R') LED_Red_Off();
        else if (cmd=='g'||cmd=='G') LED_Green_Off();
        else /* b/B */                LED_Blue_Off();
        USART3_Transmit("\r\nLED OFF\r\nCmd? ");
    }
    else if (arg == '1') {
        if (cmd=='r'||cmd=='R') LED_Red_On();
        else if (cmd=='g'||cmd=='G') LED_Green_On();
        else /* b/B */                LED_Blue_On();
        USART3_Transmit("\r\nLED ON\r\nCmd? ");
    }
    else {  // arg == '2'
        if (cmd=='r'||cmd=='R') Toggle_Red();
        else if (cmd=='g'||cmd=='G') Toggle_Green();
        else /* b/B */                Toggle_Blue();
        USART3_Transmit("\r\nLED TOGGLE\r\nCmd? ");
    }
}

//section 4.1
// int lab4_main(void)
// {
//     char rx_char;

//     /* Initialize LED GPIO pins and USART3 */
//     LED_Init();
//     USART3_GPIO_Config(); // Configure GPIO for USART3
//     USART3_Init();

//     while (1)
//     {
//         /* 1. Wait for a character to be received.
//            The RXNE flag in USART3->ISR is set when a byte is available in the Receive Data Register (RDR) */
//         while (!(USART3->ISR & USART_ISR_RXNE))
//         {
//             // Wait until data is received
//         }

//         /* 2. Read the received data.
//            (Note: reading the RDR clears the RXNE flag) */
//         rx_char = (char)(USART3->RDR & 0xFF);

//         /* 3. Test the received data and toggle the appropriate LED.
//            If the received character is 'r'/'R', 'g'/'G', or 'b'/'B' toggle the corresponding LED.
//            Otherwise, print an error message via USART3. */
//         switch(rx_char)
//         {
//             case 'r':
//             case 'R':
//                 Toggle_Red();
//                 break;
//             case 'g':
//             case 'G':
//                 Toggle_Green();
//                 break;
//             case 'b':
//             case 'B':
//                 Toggle_Blue();
//                 break;
//             default:
//                 USART3_Transmit("Error: Unrecognized command.\r\n");
//                 break;
//         }
//     }
// }



int lab4_main(void)
{
    uint8_t command[2];
    uint8_t command_index = 0;

    // Initialize peripherals
    LED_Init();
    USART3_Init();
    USART3_GPIO_Config(); // Configure GPIO for USART3
    // Enable USART3 interrupt in NVIC
    NVIC_EnableIRQ(USART3_4_IRQn);

    // Main loop: process two-character commands
    while (1)
    {
        // If waiting for a new command, prompt the user
        // if (command_index == 0)
        // {
        //     USART3_Transmit("CMD? ");
        // }
        
        // Check if new data was received by the interrupt handler
        if (rx_flag)
        {
            rx_flag = 0; // Clear flag
            command[command_index] = rx_data;
            command_index++;

            // Once two characters have been received, process the command
            if (command_index == 2)
            {
                char color = command[0];
                char op    = command[1];

                // Validate command: first char must be one of r, g, b (case-insensitive)
                // and second char must be between '0' and '2'
                if ((color == 'r' || color == 'R' || color == 'g' || color == 'G' || color == 'b' || color == 'B') &&
                    (op >= '0' && op <= '2'))
                {
                    // Process command based on LED color and operation
                    if (op == '0')
                    {
                        // Turn LED off
                        if (color == 'r' || color == 'R') { LED_Red_Off(); }
                        else if (color == 'g' || color == 'G') { LED_Green_Off(); }
                        else if (color == 'b' || color == 'B') { LED_Blue_Off(); }
                        USART3_Transmit("\r\nCommand recognized: Turn Off LED\r\n");
                    }
                    else if (op == '1')
                    {
                        // Turn LED on
                        if (color == 'r' || color == 'R') { LED_Red_On(); }
                        else if (color == 'g' || color == 'G') { LED_Green_On(); }
                        else if (color == 'b' || color == 'B') { LED_Blue_On(); }
                        USART3_Transmit("\r\nCommand recognized: Turn On LED\r\n");
                    }
                    else if (op == '2')
                    {
                        // Toggle LED
                        if (color == 'r' || color == 'R') { Toggle_Red(); }
                        else if (color == 'g' || color == 'G') { Toggle_Green(); }
                        else if (color == 'b' || color == 'B') { Toggle_Blue(); }
                        USART3_Transmit("\r\nCommand recognized: Toggle LED\r\n");
                    }
                }
                else
                {
                    // Unknown command: print error and restart command processing
                    USART3_Transmit("\r\nError: Invalid Command\r\n");
                }
                command_index = 0; // Reset command index for the next command
            }
        }
    }
}




/* Initialize LED GPIO pins.
//    This example assumes three LEDs are connected to GPIOA pins 5 (Red), 6 (Green), and 7 (Blue). */
//    void LED_Init(void) {
//     // Enable clock for GPIOC.
//     RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
//     // Configure PC6, PC7, PC8, and PC9 as general-purpose outputs.
//     GPIOC->MODER &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)) | (3UL << (8 * 2)) | (3UL << (9 * 2)));
//     GPIOC->MODER |=  ((1UL << (6 * 2)) | (1UL << (7 * 2)) | (1UL << (8 * 2)) | (1UL << (9 * 2)));
    
//     // Configure output type as push-pull (default) and low speed, no pull-up/pull-down.
//     GPIOC->OTYPER &= ~((1UL << 6) | (1UL << 7) | (1UL << 8) | (1UL << 9));
//     GPIOC->OSPEEDR &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)) | (3UL << (8 * 2)) | (3UL << (9 * 2)));
//     GPIOC->PUPDR &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)) | (3UL << (8 * 2)) | (3UL << (9 * 2)));
    
    
 
// }

// void LED_Red_On(void)
// {
//     GPIOC->ODR |= (1 << 8);
// }

// void LED_Red_Off(void)
// {
//     GPIOC->ODR &= ~(1 << 8);
// }

// void Toggle_Red(void)
// {
//     GPIOC->ODR ^= (1 << 8);
// }

// /* LED control functions for Green LED on PA6 */
// void LED_Green_On(void)
// {
//     GPIOC->ODR |= (1 << 6);
// }

// void LED_Green_Off(void)
// {
//     GPIOC->ODR &= ~(1 << 6);
// }

// void Toggle_Green(void)
// {
//     GPIOC->ODR ^= (1 << 6);
// }

// /* LED control functions for Blue LED on PA7 */
// void LED_Blue_On(void)
// {
//     GPIOC->ODR |= (1 << 7);
// }

// void LED_Blue_Off(void)
// {
//     GPIOC->ODR &= ~(1 << 7);
// }

// void Toggle_Blue(void)
// {
//     GPIOC->ODR ^= (1 << 7);
// }

/* Blocking transmit function for USART3.
   This function sends a null-terminated string over USART.
*/

// int lab4_main(void) {
//     HAL_Init();
//     SystemClock_Config();
//     My_HAL_RCC_GPIOA_C_CLK_ENABLE();
//     RCC->APB1ENR |= (1<<18);
//     GPIO_InitTypeDef initStr = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
//     GPIO_MODE_OUTPUT_PP,
//     GPIO_SPEED_FREQ_LOW,
//     GPIO_NOPULL};
//     HAL_GPIO_Init(GPIOC, &initStr);
   
//     init_UART3_PC4_PC5(); //PC 5 is TX, PC4 is RX Boardside
//     USART3_init();
// //----------------------------------Checkoff 1-------------------------
//     volatile char* message;
//     volatile char character;
//     HAL_Delay(100);
//     Transmit_USART3("Hello");
//     while(1){

//     };
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