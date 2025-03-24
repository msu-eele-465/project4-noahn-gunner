#include <msp430fr2310.h>
#include <stdbool.h>
#include <stdint.h>
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

        power_on();
        power_off();
        enable_high();
        enable_low();
        rs_high();
        rs_low();
        rw_high();
        rw_low();

        // Delay for 100000*(1/MCLK)=0.1s
        __delay_cycles(100000);
    }
}
