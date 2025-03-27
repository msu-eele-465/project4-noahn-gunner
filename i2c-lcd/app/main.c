#include <msp430fr2310.h>
#include <stdint.h>
#include "../src/lcd.h"
#include "intrinsics.h"

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    define_ports();

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;

    while (true) {

        start_up();
        start_up();

        while (true) {
            key_input('0');
            key_input('1');
            key_input('2');
            key_input('3');
            key_input('4');
            key_input('5');
            key_input('6');
            key_input('7');
            key_input('8');
            key_input('9');
            key_input('0');
            key_input('A');
            key_input('B');
            key_input('C');
            key_input('D');
            key_input('*');
            key_input('H');
            
        }
    }
}

