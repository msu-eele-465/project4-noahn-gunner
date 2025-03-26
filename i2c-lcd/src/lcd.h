#include <stdbool.h>

bool cursor_on = true;
bool cursor_blink = true;

void define_ports(void) {
    // data nibble DB7-BD4 (P1.0-1.3)
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    P1DIR |= BIT1;
    P1OUT &= ~BIT1;
    P1DIR |= BIT2;
    P1OUT &= ~BIT2;
    P1DIR |= BIT3;
    P1OUT &= ~BIT3;

    // logic circuit power supply (P1.4)
    // this does nothing bc I'm not powering from GPIO pin anymore lol
    P1OUT &= ~BIT4;
    P1DIR |= BIT4;

    // RS (P1.5)
    P1DIR |= BIT5;
    P1OUT &= ~BIT5;
    
    // RW (P1.6)
    P1DIR |= BIT6;
    P1OUT &= ~BIT6;

    // Enable (P1.7)
    P1DIR |= BIT7;
    P1OUT &= ~BIT7;

    // Contrast (P2.0)
    P2DIR |= BIT0;
    P2OUT &= ~BIT0;

    // i2c pins
    // SDA (P2.6)
    P2DIR |= BIT6;
    P2OUT &= ~BIT6;
    // SCL (P2.7)
    P2DIR |= BIT7;
    P2OUT &= ~BIT7;
}

// turns power on
void power_on(void) {
    P1OUT |= BIT4;
}

// turns power off
void power_off(void) {
    P1OUT &= ~BIT4;
}

// turns enable on
void enable_high(void) {
    P1OUT |= BIT7;
}

// turns enable off
void enable_low(void) {
    P1OUT &= ~BIT7;
}

void enable_pulse() {
    enable_high();
    __delay_cycles(50000);
    enable_low();
}

void rs_high(void) {
    P1OUT |= BIT5;
}

void rs_low(void) {
    P1OUT &= ~BIT5;
}

void rw_high(void) {
    P1OUT |= BIT6;
}

void rw_low(void) {
    P1OUT &= ~BIT6;
}

void set_nibble(int nibbleInt) {
    int nib0 = nibbleInt;
    if ((nib0 &= BIT0) != 0) {
        P1OUT |= BIT0;
    } else {
        P1OUT &= ~BIT0;
    }
    
    int nib1 = nibbleInt;
    if ((nib1 &= BIT1) != 0) {
        P1OUT |= BIT1;
    } else {
        P1OUT &= ~BIT1;
    }
    
    int nib2 = nibbleInt;
    if ((nib2 &= BIT2) != 0) {
        P1OUT |= BIT2;
    } else {
        P1OUT &= ~BIT2;
    }
    
    int nib3 = nibbleInt;
    if ((nib3 &= BIT3) != 0) {
        P1OUT |= BIT3;
    } else {
        P1OUT &= ~BIT3;
    }
}

void start_up() {
    power_on();
    __delay_cycles(100000);
    // set to 4 bit mode
    set_nibble(0b0010);
    enable_pulse();
    __delay_cycles(100000);
    set_nibble(0b0010);
    enable_pulse();
    __delay_cycles(100000);

    // 2 line mode, display on
    set_nibble(0b1100);
    enable_pulse();
    __delay_cycles(100000);

    // display on, cursor on, blink on
    set_nibble(0b0000);
    enable_pulse();
    __delay_cycles(100000);
    set_nibble(0b1111);
    enable_pulse();
    __delay_cycles(100000);

    // clear display
    set_nibble(0b0000);
    enable_pulse();
    __delay_cycles(100000);
    set_nibble(0b0001);
    enable_pulse();
    __delay_cycles(100000);

    // entry mode set, increment mode, shift off
    set_nibble(0b0000);
    enable_pulse();
    __delay_cycles(100000);
    set_nibble(0b0110);
    enable_pulse();
    __delay_cycles(100000);
}

void clear_display() {
    set_nibble(0b0000);
    enable_pulse();
    __delay_cycles(5000);
    set_nibble(0b0001);
    enable_pulse();
    __delay_cycles(5000);
}

void set_cursor_location(int upperNibble, int lowerNibble) {
    // expecting full address given to be 1 concatenated with 7 bit address, split into 2 nibbles
    // e.g. for first char in first line, upperNibble = 1000, lowerNibble = 0000
    set_nibble(upperNibble);
    enable_pulse();
    __delay_cycles(5000);
    set_nibble(lowerNibble);
    enable_pulse();
    __delay_cycles(5000);
}

void write_character(int upperNibble, int lowerNibble) {
    // expecting full address split into 2 nibbles
    // e.g. for 0, upperNibble = 0011, lowerNibble = 0000
    rs_high();
    set_nibble(upperNibble);
    enable_pulse();
    __delay_cycles(5000);
    set_nibble(lowerNibble);
    enable_pulse();
    __delay_cycles(5000);
    rs_low();
}

void update_cursor_status() {
    const int upper = 0b0000;
    int lower = 0b1100;
    if (cursor_on) {
        if (cursor_blink) {
            lower = 0b1111;
        } else {
            lower = 0b1110;
        }
    } else {
            lower = 0b1100;
    }
    set_nibble(upper);
    enable_pulse();
    __delay_cycles(5000);
    set_nibble(lower);
    enable_pulse();
    __delay_cycles(5000);
}

void write_by_ascii(char ch) {
    int ascii_ch = ch;
    int ascii_ch_upper = ch >> 4;
    write_character(ascii_ch_upper, ascii_ch);
}

void location_by_coords(int row, int column) {
    int upper = 0b1000;
    int lower = column - 1;
    if (row == 1) {
        upper = 0b1000;
    } else if (row == 2) {
        upper = 0b1100;
    } else {
        upper = 0b1000;
    }
    set_cursor_location(upper, lower);
} 

void pressed_char(char ch) {
    int ascii_ch = ch;
    location_by_coords(2, 16);
    write_by_ascii(ascii_ch);
}

void write_word(char *word) {
    location_by_coords(1, 1);
    int i;
    for (i = 0; word[i] != '\0'; i++) {
        write_by_ascii(word[i]);
    }
}
