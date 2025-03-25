#include <msp430.h>
#include <stdio.h>
#include <stdbool.h>
#include "../src/keypad.h"
#include "msp430fr2355.h"

#define LED_BAR_ADDR 0x42     // I2C address of LED bar
#define PATTERN_KEY '1'       // test pattern key (you can expand this later)

void init_I2C_Master(void) {
    // Set P1.2 (SDA) and P1.3 (SCL) as I2C function
    P1SEL1 |= BIT2 | BIT3;
    P1SEL0 &= ~(BIT2 | BIT3);

    // Put eUSCI_B0 into reset
    UCB0CTLW0 |= UCSWRST;

    // Configure as I2C Master, synchronous mode, use SMCLK
    UCB0CTLW0 = UCMST | UCMODE_3 | UCSYNC | UCSSEL__SMCLK | UCSWRST;

    // Set bit rate (assuming SMCLK ~1MHz)
    UCB0BRW = 10;  // 1 MHz / 10 = 100 kHz I2C

    // Set slave address
    UCB0I2CSA = LED_BAR_ADDR;

    // Release from reset
    UCB0CTLW0 &= ~UCSWRST;
}


void send_byte_I2C(unsigned char byte) {
    while (UCB0CTLW0 & UCTXSTP); // Wait for any previous stop to complete
    UCB0CTLW0 |= UCTR | UCTXSTT; // Set transmitter mode and start condition

    while (!(UCB0IFG & UCTXIFG0)); // Wait for buffer ready
    UCB0TXBUF = byte;             // Send byte

    while (!(UCB0IFG & UCTXIFG0)); // Wait for ACK
    UCB0CTLW0 |= UCTXSTP;          // Send stop
    while (UCB0CTLW0 & UCTXSTP);   // Wait for stop to complete
}

// for blinking LED on keypress
volatile int prev_state = 0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setup red LED
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    // Setup P6.6 as output for LED test
    P6DIR |= BIT6;
    P6OUT &= ~BIT6;  // Start off


    init_keypad_ports();
    init_I2C_Master();

    // Unlock GPIO pins
    PM5CTL0 &= ~LOCKLPM5;

    // Timer B0 for periodic LED blink (optional)
    TB0CTL |= TBCLR;
    TB0CTL |= TBSSEL__ACLK;
    TB0CTL |= MC__CONTINUOUS;

    TB0CTL &= ~TBIFG;
    TB0CTL |= TBIE;

    init_keypad_irqs();
    __enable_interrupt();

    int i;
    while (true) {
        col_masking();
        for (i = 0; i < 10000; i++) {}

        lock_state();
        if (password_unlock && current_key == PATTERN_KEY && prev_key != PATTERN_KEY) {
            send_byte_I2C(0x55);   // send dummy command to LED bar
            P6OUT ^= BIT6;         // toggle status LED to show it sent
        }
        // // Turn on LED on P6.6 if unlock code is correctly entered
        // if (locked_state == 2 && password_unlock) {
        //     P6OUT |= BIT6;
        // } else {
        //     P6OUT &= ~BIT6;
        // }


        for (i = 0; i < 10000; i++) {}

        prev_key = current_key;
        prev_state = locked_state;

        for (i = 0; i < 10000; i++) {}

        row_cycle();

        for (i = 0; i < 10000; i++) {}
    }
}

//---- Port 2 Interrupt (Column ISR)
#pragma vector = PORT2_VECTOR
__interrupt void ISR_Port2_Column(void) {
    col_masking();
    if (col_1 == 0) {
        if (current_row == 1) current_key = '1';
        else if (current_row == 2) current_key = '4';
        else if (current_row == 3) current_key = '7';
        else if (current_row == 4) current_key = '*';
    } else if (col_2 == 0) {
        if (current_row == 1) current_key = '2';
        else if (current_row == 2) current_key = '5';
        else if (current_row == 3) current_key = '8';
        else if (current_row == 4) current_key = '0';
    } else if (col_3 == 0) {
        if (current_row == 1) current_key = '3';
        else if (current_row == 2) current_key = '6';
        else if (current_row == 3) current_key = '9';
        else if (current_row == 4) current_key = '#';
    } else if (col_4 == 0) {
        if (current_row == 1) current_key = 'A';
        else if (current_row == 2) current_key = 'B';
        else if (current_row == 3) current_key = 'C';
        else if (current_row == 4) current_key = 'D';
    } else {
        current_key = 'N';
    }

    // Clear column interrupt flags
    P2IFG &= ~(BIT4 | BIT5 | BIT6 | BIT7);

    // Indicate keypress with LED blink
    P1OUT ^= BIT0;
}

// Timer Overflow (optional LED heartbeat or indicator)
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_Overflow(void) {
    P1OUT ^= BIT0;
    TB0CTL &= ~TBIFG;
}