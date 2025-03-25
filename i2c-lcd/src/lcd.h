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
    P1DIR |= BIT4;
    P1OUT &= ~BIT4;

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
    set_nibble(0b0010);
    enable_pulse();
    // 2 line mode, display on
    __delay_cycles(100000);

    // display on, cursor on, blink on
    set_nibble(0b0000);
    enable_pulse();
    set_nibble(0b1111);
    enable_pulse();
    __delay_cycles(100000);

    // clear display
    set_nibble(0b0000);
    enable_pulse();
    set_nibble(0b0001);
    enable_pulse();
    __delay_cycles(100000);

    // entry mode set, increment mode, shift on
    set_nibble(0b0000);
    enable_pulse();
    set_nibble(0b0111);
    enable_pulse();
    __delay_cycles(100000);
}
