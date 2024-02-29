#include <xc.h>

// Configuration settings
#pragma config FOSC = INTOSC  // Internal oscillator
#pragma config WDTE = OFF     // Watchdog Timer disabled
#pragma config PWRTE = OFF    // Power-up Timer disabled
#pragma config MCLRE = OFF    // MCLR pin function is digital input
#pragma config CP = OFF       // Code protection off
#pragma config CPD = OFF      // Data code protection off
#pragma config BOREN = ON     // Brown-out Reset enabled
#pragma config CLKOUTEN = OFF // Clock out disabled
#pragma config IESO = OFF     // Internal/External Switchover mode disabled
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor disabled
#pragma config WRT = OFF      // Flash write protection off
#pragma config VCAPEN = OFF   // Voltage Regulator Capacitor Enable bit disabled
#pragma config PLLEN = OFF    // PLL disabled (4x PLL disabled)
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset enabled
#pragma config BORV = LO      // Brown-out Reset Voltage (Vbor), low trip point selected
#pragma config LVP = OFF      // Low Voltage Programming disabled

// I2C Slave Address
#define SLAVE_ADDRESS 0x10

// Variable to store received data
volatile char receivedData;

// Function prototypes
void i2cInitSlave();
void i2cWaitForEvent();
void i2cRespond();

void main(void) {
    OSCCON = 0b01110000; // Set internal oscillator to 8MHz
    ANSELC = 0;          // Digital I/O on port C
    TRISC6 = 1;          // SDA pin (RC6) is input
    TRISC7 = 1;          // SCL pin (RC7) is input

    i2cInitSlave(); // Initialize I2C as slave

    while (1) {
        i2cWaitForEvent(); // Wait for I2C event (start, stop, or data)
        i2cRespond();      // Respond to the I2C event
    }

    return;
}

void i2cInitSlave() {
    SSPSTAT = 0b10000000; // Slew rate control disabled (for standard speed)
    SSPCON1 = 0b00110110; // Enable SSP module in I2C slave mode, 7-bit address
    SSPCON2 = 0b00000001; // Enable clock stretching
    SSPADD = SLAVE_ADDRESS << 1; // Set the slave address

    SSPIF = 0; // Clear the SSP interrupt flag
    SSPIE = 1; // Enable SSP interrupt

    PEIE = 1; // Enable peripheral interrupts
    GIE = 1;  // Enable global interrupts
}

void i2cWaitForEvent() {
    while (!SSPIF); // Wait for I2C interrupt
    SSPIF = 0;      // Clear the SSP interrupt flag
}

void i2cRespond() {
    if (SSPSTATbits.S) {
        // Start condition, reset the flag
    } else if (!SSPSTATbits.P) {
        // Data received
        receivedData = SSPBUF; // Read the received data
        // Process the received data as needed for your application
    }

    if (SSPCON1bits.CKP) {
        // Release SCL line
        SSPCON1bits.CKP = 0;
    }

    // ACK the received data
    SSPCON2bits.ACKDT = 0; // ACK
    SSPCON2bits.ACKEN = 1; // Enable ACK bit
}