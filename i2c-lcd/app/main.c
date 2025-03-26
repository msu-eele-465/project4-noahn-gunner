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

        text_static();
        pressed_char('A');
        text_toggle();
        pressed_char('B');
        text_up_counter();
        pressed_char('9');
        text_in_and_out();
        pressed_char('*');
        
        cursor_on = true;
        cursor_blink = true;
        update_cursor_status();

        text_down_counter();
        pressed_char('#');
        text_rotate_1_left();
        pressed_char('7');

        cursor_on = false;
        cursor_blink = false;
        update_cursor_status();

        text_rotate_7_right();
        pressed_char('3');
        text_fill_left();
        pressed_char('D');

        // Delay for 100000*(1/MCLK)=0.1s
        //__delay_cycles(100000);
    }
}
