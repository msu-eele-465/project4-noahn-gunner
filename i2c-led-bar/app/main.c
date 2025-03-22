#include "src/LED_Patterns.h"
#include <msp430.h>

// Timing Variables
static float base_period = 1.0;         // Default to 1 second
static unsigned int pattern_step1 = 0;
static unsigned int pattern_step2 = 0;
static unsigned int pattern_step3 = 0;
//static unsigned int count = 0;
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


static const unsigned char Pattern_3_P1[6] = {  
    (LED4 | LED5),      // Step 0: 0 0 0 1 1 0 0 0
    (LED6),      // Step 1: 0 0 1 0 0 1 0 0
    (LED7),      // Step 2: 0 1 0 0 0 0 1 0
    (LED8),      // Step 3: 1 0 0 0 0 0 0 1
    (LED7),      // Step 4: 0 1 0 0 0 0 1 0
    (LED6)       // Step 5: 0 0 1 0 0 1 0 0
};

static const unsigned char Pattern_3_P2[6] = {  
    (0),                // Step 0
    (LED3),                // Step 1
    (LED2),             // Step 2
    (LED1),                // Step 3
    (LED2),             // Step 4
    (LED3)                 // Step 5
};




// Timer_B0 Configuration (Updated)
void setupTimer(void) {
    TB0CTL = TBCLR;  // Clear Timer
    TB0CTL |= TBSSEL__SMCLK;  // Use SMCLK
    TB0CTL |= MC__UP;  // Up mode
    TB0CTL |= ID__8;   // Divide input clock by 8
    TB0EX0 = TBIDEX__7; // Further divide by 7

    TB0CCR0 = 18858;    // Maximum period (~1 sec) (NEEDS REFINING)

    TB0CCTL0 = CCIE;   // Enable interrupt
    TB0CCTL0 &= ~CCIFG;  // Clear interrupt flag

    __enable_interrupt();
}

void updateTimerPeriod(float new_period) {
    TB0CTL |= TBCLR;  // Stop and clear the timer
    TB0CCR0 = (unsigned int)(new_period * 18858);  // Scale timer
    TB0CTL |= MC__UP;  // Resume in Up mode
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

    // Update timer period based on pattern's spec
    switch (pattern) {
        case Pattern_1_Toggle:
            updateTimerPeriod(base_period * 1.0);  // Toggle every 1 sec
            break;
        case Pattern_2_Up:
        case Pattern_3_In_Out:
            updateTimerPeriod(base_period * 0.5);  // Step every 0.5 sec
            break;
        default:
            updateTimerPeriod(base_period);  // Default
            break;
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

        case Pattern_2_Up: {
            uint8_t count = pattern_step2;

            // Debug: Show count value
            //P1OUT ^= (count & 1) ? BIT4 : 0;  // Blink P1.4 when LSB is 1

            // Map bits to Port 2
            LED_PORT2 = 0;
            if (count & (1 << 0)) LED_PORT2 |= LED1;
            if (count & (1 << 1)) LED_PORT2 |= LED2;
            if (count & (1 << 2)) LED_PORT2 |= LED3;

            // Map bits to Port 1
            LED_PORT1 = 0;
            if (count & (1 << 3)) LED_PORT1 |= LED4;
            if (count & (1 << 4)) LED_PORT1 |= LED5;
            if (count & (1 << 5)) LED_PORT1 |= LED6;
            if (count & (1 << 6)) LED_PORT1 |= LED7;
            if (count & (1 << 7)) LED_PORT1 |= LED8;


            pattern_step2 = (pattern_step2 + 1) & 0xFF;  // Wrap after 255
            break;
        }

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

