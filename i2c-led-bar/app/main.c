00#include <msp430.h>

// Define LED Pins for MSP430FR2310
#define LED1 BIT0  // P2.0
#define LED2 BIT7  // P2.7
#define LED3 BIT6  // P2.6
#define LED4 BIT7  // P1.7
#define LED5 BIT6  // P1.6
#define LED6 BIT5  // P1.5
#define LED7 BIT4  // P1.4
#define LED8 BIT1  // P1.1

void initLEDs(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;  // Ensure GPIO is enabled

    // Force P2.6 & P2.7 into GPIO mode
    P2SEL0 &= ~(BIT6 | BIT7);
    P2SEL1 &= ~(BIT6 | BIT7);

    // Set all LED pins as outputs
    P1DIR |= (BIT7 | LED5 | LED6 | LED7 | LED8);  
    // P2DIR |= (LED1 | LED2 | LED3);                

    // // Turn all LEDs ON for testing
    P1OUT |= BIT7 | LED5 | LED6 | LED7 | LED8;
    // P2OUT = LED1 | LED2 | LED3;

    // P2DIR |= BIT7;
    // P2OUT |= BIT7;

    // P2DIR |= BIT6;
    // P2OUT |= BIT6;

    //PM5CTL0 &= ~LOCKLPM5;  // Ensure GPIO is enabled
}

int main(void) {
    initLEDs();  // Initialize LED GPIOs
    while (1);   // Stay in an infinite loop
}
