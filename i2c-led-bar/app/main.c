#include "src/LED_Patterns.h"
#include <msp430.h>

// Timing Variables
static unsigned int pattern_step1 = 0;
static unsigned int pattern_step2 = 0;
static unsigned int pattern_step3 = 0;
static led_pattern_t current_pattern = Pattern_Off;
static bool pattern_active = false;
volatile bool update_flag = false;

// -- Correct LED Patterns
static const unsigned char Pattern_0_P1 = (LED5 | LED7);  // P1 LEDs
static const unsigned char Pattern_0_P2 = (LED1 | LED3);         // P2 LEDs

static const unsigned char Pattern_1_P1[2] = {  
    (LED5 | LED7),         // Step 0: 1 0 1 0 1 0 1 0 (Same as Pattern 0)
    (LED4 | LED6 | LED8)   // Step 1: 0 1 0 1 0 1 0 1 (Inverted)
};

static const unsigned char Pattern_1_P2[2] = {  
    (LED1 | LED3),         // Step 0: 1 0 1 0 1 0 1 0 (Same as Pattern 0)
    (LED2)                 // Step 1: 0 1 0 1 0 1 0 1 (Inverted)
};


static const unsigned char Pattern_3_P1[6] = {  // In & Out P1
    (LED4 | LED5),
    (LED6 | LED7),
    (LED8),
    (LED6 | LED7),
    (LED4 | LED5),
    (0)
};

static const unsigned char Pattern_3_P2[6] = {  // In & Out P2
    (LED1 | LED2),
    (LED3),
    (0),
    (LED3),
    (LED1 | LED2),
    (0)
};



// Timer_B0 Configuration (Updated)
void setupTimer(void) {
    TB0CTL = TBCLR;  // Clear Timer
    TB0CTL |= TBSSEL__SMCLK;  // Use SMCLK
    TB0CTL |= MC__UP;  // Up mode
    //TB0CTL |= ID__8;   // Divide input clock by 8
    TB0EX0 = TBIDEX__7; // Further divide by 7

    TB0CCR0 = 65535;    // Maximum period (~1 sec) (NEEDS REFINING)

    TB0CCTL0 = CCIE;   // Enable interrupt
    TB0CCTL0 &= ~CCIFG;  // Clear interrupt flag

    __enable_interrupt();
}




// Initialize LED GPIOs
void array_init(void) {
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= (LED4 | LED5 | LED6 | LED7 | LED8);  // Set P1 LEDs as outputs
    P2DIR |= (LED1 | LED2 | LED3);                // Set P2 LEDs as outputs

    // Set P2.6 & P2.7 to GPIO mode
    P2SEL0 &= ~(BIT6 | BIT7);
    P2SEL1 &= ~(BIT6 | BIT7);

    P1OUT = 0x00;  // Turn off P1 LEDs
    P2OUT = 0x00;  // Turn off P2 LEDs
}

void selectPattern(led_pattern_t pattern) {
    if (pattern == current_pattern) {
        switch (current_pattern) {
            case Pattern_1_Toggle:
                pattern_step1 = 0;
                break;
            case Pattern_2_Up:
                pattern_step2 = 0;
                break;
            case Pattern_3_In_Out:
                pattern_step3 = 0;
                break;
        }
    } else {
        current_pattern = pattern;
    }
    pattern_active = true;
}

void updatePattern(void) {
    if (!pattern_active) return;

    switch (current_pattern) {
        case Pattern_0_Static:
            LED_PORT1 = Pattern_0_P1;
            LED_PORT2 = Pattern_0_P2;
            break;
        
        case Pattern_1_Toggle:
            LED_PORT1 = Pattern_1_P1[pattern_step1];
            LED_PORT2 = Pattern_1_P2[pattern_step1];
            pattern_step1 = (pattern_step1 + 1) % 2;  // Toggle between 2 states
            break;

        case Pattern_3_In_Out:
            LED_PORT1 = Pattern_3_P1[pattern_step3];
            LED_PORT2 = Pattern_3_P2[pattern_step3];
            pattern_step3 = (pattern_step3 + 1) % 6;  // Cycle through 6 steps
            break;

        default:
            LED_PORT1 = 0x00;
            LED_PORT2 = 0x00;
            break;    
    }
}



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    array_init();              // Initialize LEDs and timer
    setupTimer();              // Start Timer_B0 for LED patterns
    __enable_interrupt();
    selectPattern(Pattern_1_Toggle);  // Start with a default pattern

    while (1) {
        if (update_flag) {
            update_flag = false;
            updatePattern();
        }
    }
}




// Timer_B0 ISR (Triggers Pattern Updates)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void) {
    update_flag = true;
    //P1OUT ^= BIT1;  // Toggle LED P1.1
    TB0CCTL0 &= ~CCIFG;  
}

