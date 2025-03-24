#include <msp430fr2310.h>
#include <stdbool.h>
#include "../src/lcd.h"

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    define_ports();

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;

    while (true)
    {
        P1OUT ^= BIT0;
        P1OUT ^= BIT1;
        P1OUT ^= BIT2;
        P1OUT ^= BIT3;
        P1OUT ^= BIT4;
        P1OUT ^= BIT5;
        P1OUT ^= BIT6;
        P1OUT ^= BIT7;
        
        P2OUT ^= BIT0;
        P2OUT ^= BIT6;
        P2OUT ^= BIT7;

        // Delay for 100000*(1/MCLK)=0.1s
        __delay_cycles(100000);
    }
}
