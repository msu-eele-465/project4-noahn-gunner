#include <msp430fr2310.h>
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

        start_up();
        start_up();

        cursor_on = true;
        cursor_blink = false;
        update_cursor_status();
        write_word("static          ");
        pressed_char('A');
        write_word("toggle          ");
        pressed_char('B');
        write_word("up counter      ");
        pressed_char('9');
        write_word("in and out      ");
        pressed_char('*');
        
        cursor_on = true;
        cursor_blink = true;
        update_cursor_status();

        clear_display();

        write_word("down counter    ");
        pressed_char('#');
        write_word("rotate 1 left   ");
        pressed_char('7');

        cursor_on = false;
        cursor_blink = false;
        update_cursor_status();

        write_word("rotate 7 right   ");
        pressed_char('3');
        write_word("fill left        ");
        pressed_char('D');

        char_test();

        // Delay for 100000*(1/MCLK)=0.1s
        //__delay_cycles(100000);
    }
}
