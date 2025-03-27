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

    while (true) {

        start_up();
        start_up();

        while (true) {
            switch (current_pattern) {
                case 0:
                    write_word("static        ");
                    break;
                case 1:
                    write_word("toggle        ");
                    break;
                case 2:
                    write_word("up counter    ");
                    break;
                case 3:
                    write_word("in and out    ");
                    break;
                case 4:
                    write_word("down counter  ");
                    break;
                case 5:    
                    write_word("rotate 1 left ");
                    break;
                case 6:
                    write_word("rotate 7 right");
                    break;
                case 7:
                    write_word("fill left     ");
                    break;
                case 8:
                    write_word("              ");
            }

            switch (last_key) {
                case '1':
                    pressed_char('1');
                    current_pattern = 1;
                    break;
                case '2':
                    pressed_char('2');
                    current_pattern = 2;
                    break;
                case '3':
                    pressed_char('3');
                    current_pattern = 3;
                    break;
                case '4':
                    pressed_char('4');
                    current_pattern = 4;
                    break;
                case '5':
                    pressed_char('5');
                    current_pattern = 5;
                    break;
                case '6':
                    pressed_char('6');
                    current_pattern = 6;
                    break;
                case '7':
                    pressed_char('7');
                    current_pattern = 7;
                    break;
                case '8':
                    pressed_char('8');
                    current_pattern = 8;
                    break;
                case '9':
                    pressed_char('9');
                    cursor_blink = !cursor_blink;
                    update_cursor_status();
                    break;
                case '0':
                    pressed_char('0');
                    current_pattern = 0;
                    break;
                case 'A':
                    pressed_char('A');
                    break;
                case 'B':
                    pressed_char('B');
                    break;
                case 'C':
                    pressed_char('C');
                    cursor_on = !cursor_on;
                    update_cursor_status();
                    break;
                case 'D':
                    pressed_char('D');
                    break;
                case '*':
                    pressed_char('*');
                    break;
                case '#':
                    pressed_char('#');
                    break;
                case 'N':
                    break;
            }
        }

        

        // Delay for 100000*(1/MCLK)=0.1s
        //__delay_cycles(100000);
    }
}
