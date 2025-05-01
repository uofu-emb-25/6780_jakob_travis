//Jakob Travis 6780 Partner: Clay Stanley


#include "stm32f0xx.h"
#include <stdint.h>


#define L3GD20_ADDR       0x69
#define CTRL_REG1         0x20
#define OUT_X_L           0x28
#define OUT_X_H           0x29
#define OUT_Y_L           0x2A
#define OUT_Y_H           0x2B
#define THRESHOLD         100
#define MULTI_READ     0x80
uint8_t ctrl1 = 0xB;

volatile int16_t globe_x = 0;
volatile int16_t globe_y = 0;
uint8_t i2c_read(uint8_t addr, uint8_t reg);
void    i2c_write(uint8_t addr, uint8_t reg, uint8_t value);

void LED_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~((0b11 << (6 * 2)) | (0b11 << (7 * 2)) | (0b11 << (8 * 2)) | (0b11 << (9 * 2)));
    GPIOC->MODER |= (0b01 << (6 * 2)) | (0b01 << (7 * 2)) | (0b01 << (8 * 2)) | (0b01 << (9 * 2));
}

void Gyro_Init(void) {
    // Enable X and Y axis, set PD = 1, others = 0
    // CTRL_REG1 = 0b00000011 = 0x03 (Y enabled, X enabled, PD = 1)
    //I2C2_WriteRegister(CTRL_REG1, 0x03);
//slave address #bytes
i2c_write(L3GD20_ADDR, CTRL_REG1, ctrl1);
}

/* 1) GPIO + AF setup for I2C2 */
static void I2C2_GPIO_Init(void) {
    /* Enable GPIOB & GPIOC clocks */
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    
    /* PB11 & PB13: AF mode (10), open-drain, no pull, AF1=I2C2_SDA / AF5=I2C2_SCL */
    GPIOB->MODER &= ~((3UL<<22)|(3UL<<26));
    GPIOB->MODER |=  ((2UL<<22)|(2UL<<26));
    GPIOB->OTYPER |=  (1UL<<11)|(1UL<<13);
    GPIOB->OSPEEDR &= ~((3UL<<22)|(3UL<<26));
    GPIOB->PUPDR   &= ~((3UL<<22)|(3UL<<26));
    /* AF selection on AFR[1] (pins 8–15) */
    GPIOB->AFR[1] &= ~((0xFUL<<12)|(0xFUL<<20));
    GPIOB->AFR[1] |=  ((1UL<<12)|(5UL<<20));  // PB11→AF1, PB13→AF5 :contentReference[oaicite:0]{index=0}&#8203;:contentReference[oaicite:1]{index=1}

    /* PB14: push-pull output, high (SDO high → 0x6Bh) */
    GPIOB->MODER   &= ~(3UL<<(14*2));
    GPIOB->MODER   |=  (1UL<<(14*2));
    GPIOB->OTYPER  &= ~(1UL<<14);
    GPIOB->ODR     |=  (1UL<<14);

    /* PC0: push-pull output, high (I2C mode select) */
    GPIOC->MODER   &= ~(3UL<<(0*2));
    GPIOC->MODER   |=  (1UL<<(0*2));
    GPIOC->OTYPER  &= ~(1UL<<0);
    GPIOC->ODR     |=  (1UL<<0);             // :contentReference[oaicite:2]{index=2}&#8203;:contentReference[oaicite:3]{index=3}

    //section 5.7.1 line 133
        // Set PC8 and PC9 to output mode
    GPIOC->MODER &= ~((0b11 << (8 * 2)) | (0b11 << (9 * 2)));
    GPIOC->MODER |= (0b01 << (8 * 2)) | (0b01 << (9 * 2));

    // Set initial state: PC8 ON, PC9 OFF
    GPIOC->ODR |= (1 << 8);
    GPIOC->ODR &= ~(1 << 9);
}

/* 2) I2C2 peripheral init at 100 kHz standard-mode */
static void I2C2_Init(void) {
    /* Enable I2C2 clock */
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    /* Disable peripheral to configure */
    I2C2->CR1 &= ~I2C_CR1_PE;
    /* TIMINGR for 8 MHz → 100 kHz: PRESC=1, SCLDEL=4, SDADEL=2, SCLH=15, SCLL=19 */
    I2C2->TIMINGR = 0x10420F13;
    /* Enable I2C2 */
    I2C2->CR1 |= I2C_CR1_PE;
}

// int lab5_main(void) {
//     uint8_t who = 0;
//     I2C2_GPIO_Init();
//     I2C2_Init();

//     who = L3GD20_WHO_AM_I(0x0F);
//     /* At this point, `who` should be 0xD4 if the sensor is present. */
//     if (who == 0xD3) {
//         GPIOC->ODR |= (1 << 9);  // Turn on LED to indicate success
//         GPIOC->ODR &= ~(1 << 8);  // Turn on error LED
//     } else {
//         GPIOC->ODR &= ~(1 << 9);  // Turn on error LED
//         GPIOC->ODR |= (1 << 8);  // Turn on error LED

//     }

//     while (1) {
//         /* your post-lab: maybe blink an LED if who==0xD4 */
     
//     }
// }

int lab5_main(void) {
    I2C2_GPIO_Init();
    LED_Init();
    I2C2_Init();
    Gyro_Init();

    while (1) {
    int16_t gyro_x, gyro_y;
    int8_t x_l, x_h, y_l, y_h;

    x_l = i2c_read(0x69,0x28); // Read OUT_X_H register
    y_l = i2c_read(0x69,0x2A); // Read OUT_Y_H register
    x_h = i2c_read(0x69,0x29); // Read OUT_X_H register
    y_h = i2c_read(0x69,0x2B); // Read OUT_Y_H register

    gyro_x = (int16_t)((x_h<<8) | x_l); // Combine high and low bytes for X-axis
    gyro_y = (int16_t)((y_h<<8) | y_l); // Combine high and low bytes for Y-axis

    globe_x += gyro_x;
    globe_y += gyro_y;
        
    Update_LEDs(globe_x, globe_y);
    Delay_ms(10);  // Adjust delay as needed
    }
}

static uint8_t L3GD20_WHO_AM_I(uint8_t wanted_address){
    uint8_t value;

    /* --- Write phase: send register address --- */
    /* Clear SADD[9:0], NBYTES[7:0], RD_WRN, AUTOEND */
    I2C2->CR2 &= ~((0x3FFUL<<0)|(0xFFUL<<16)|(1UL<<25)|(1UL<<10));
    /* Set SADD=0x6B<<1, NBYTES=1, write (RD_WRN=0) */
    I2C2->CR2 |= ((0x69<<1)<<0)|(1UL<<16);
    /* Start */
    I2C2->CR2 |= I2C_CR2_START;
    /* Wait TXIS or NACK */
    while (!(I2C2->ISR & (I2C_ISR_TXIS|I2C_ISR_NACKF)));
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR |= I2C_ICR_NACKCF;            // clear NACK
        return 0xFF;                           // error
    }
    /* Send register address 0x0F */
    I2C2->TXDR = wanted_address;
    /* Wait Transfer Complete */
    while (!(I2C2->ISR & I2C_ISR_TC));

    /* --- Read phase: restart for 1-byte read --- */
    I2C2->CR2 &= ~((0x3FFUL<<0)|(0xFFUL<<16)|(1UL<<25)|(1UL<<10));
    /* SADD=0x6B<<1, NBYTES=1, read (RD_WRN=1 at bit-10) */
    I2C2->CR2 |= ((0x69<<1)<<0)|(1UL<<16)|(1UL<<10);
    I2C2->CR2 |= I2C_CR2_START;
    /* Wait RXNE or NACK */
    while (!(I2C2->ISR & (I2C_ISR_RXNE|I2C_ISR_NACKF)));
    if (I2C2->ISR & I2C_ISR_NACKF) {
        I2C2->ICR |= I2C_ICR_NACKCF;
        I2C2->CR2 |= I2C_CR2_STOP;
        return 0xFF;
    }
    value = (uint8_t)I2C2->RXDR;               // read the byte
    /* Wait TC then STOP */
    while (!(I2C2->ISR & I2C_ISR_TC));
    I2C2->CR2 |= I2C_CR2_STOP;

    return value;                             // expected 0xD4
}


void i2c_write(uint8_t addr, uint8_t reg,uint8_t value) {
    // 1) Configure CR2 for a 2-byte write: [reg, value]
    I2C2->CR2 = (addr << 1)                // 7-bit address + write
              | (2 << I2C_CR2_NBYTES_Pos)  // two bytes to send
              | I2C_CR2_START;             // send START
    // 2) Send register address
    while (!(I2C2->ISR & I2C_ISR_TXIS)) { }
    I2C2->TXDR = reg;
    // 3) Send data
    while (!(I2C2->ISR & I2C_ISR_TXIS)) { }
    I2C2->TXDR = value;
    // 4) Wait for transfer complete, then STOP
    while (!(I2C2->ISR & I2C_ISR_TC))  { }
    I2C2->CR2 |= I2C_CR2_STOP;
    // 5) Clear STOP flag
    while (!(I2C2->ISR & I2C_ISR_STOPF)) { }
    I2C2->ICR |= I2C_ICR_STOPCF;
}

// Read OUT_X_L, OUT_X_H, OUT_Y_L, OUT_Y_H into data[0..3]
uint8_t i2c_read(uint8_t addr, uint8_t reg) {
    uint8_t value = 0;
        // --- write the register address ---
        I2C2->CR2 = (addr << 1)                // write mode
                  | (1 << I2C_CR2_NBYTES_Pos)  // 1 byte
                  | I2C_CR2_START;
        while (!(I2C2->ISR & I2C_ISR_TXIS)) { }
        I2C2->TXDR = reg;
        while (!(I2C2->ISR & I2C_ISR_TC))  { }
        // --- restart for read ---
        I2C2->CR2 = (addr << 1)
                  | I2C_CR2_RD_WRN            // read mode
                  | (1 << I2C_CR2_NBYTES_Pos)
                  | I2C_CR2_START;
        // get the data
        while (!(I2C2->ISR & I2C_ISR_RXNE)) { }
        value = I2C2->RXDR;
        // finish
        while (!(I2C2->ISR & I2C_ISR_TC))  { }
        I2C2->CR2 |= I2C_CR2_STOP;
        while (!(I2C2->ISR & I2C_ISR_STOPF)) { }
        I2C2->ICR |= I2C_ICR_STOPCF;

        return value;
}

void Update_LEDs(int16_t x, int16_t y) {
    GPIOC->ODR &= ~((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9));
    if (x > THRESHOLD)      GPIOC->ODR |= (1 << 9); // Orange LED = +X
    else if (x < -1*THRESHOLD) GPIOC->ODR |= (1 << 8); // Green LED = -X
    if (y > THRESHOLD)      GPIOC->ODR |= (1 << 7); // Blue LED = +Y
    else if (y < -1*THRESHOLD) GPIOC->ODR |= (1 << 6); // Red LED = -Y
}


void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 8000; ++i) __NOP(); // rough delay for 8MHz clock
}



void I2C2_WriteRegister(uint8_t reg, uint8_t value) {
    I2C2->CR2 = (L3GD20_ADDR << 1) | (1 << 16); // 1 byte, write
    I2C2->CR2 |= I2C_CR2_START;
    while (!(I2C2->ISR & I2C_ISR_TXIS));
    I2C2->TXDR = reg;
    while (!(I2C2->ISR & I2C_ISR_TXIS));
    I2C2->TXDR = value;
    while (!(I2C2->ISR & I2C_ISR_TC));
    I2C2->CR2 |= I2C_CR2_STOP;
}
