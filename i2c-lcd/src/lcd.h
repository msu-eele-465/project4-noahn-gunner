void define_ports(void) {
    // data nibble DB7-BD4 (P1.0-1.3)
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;
    P1OUT &= ~BIT1;
    P1DIR |= BIT1;
    P1OUT &= ~BIT2;
    P1DIR |= BIT2;
    P1OUT &= ~BIT3;
    P1DIR |= BIT3;

    // logic circuit power supply (P1.4)
    P1OUT &= ~BIT4;
    P1DIR |= BIT4;

    // RS (P1.5)
    P1OUT &= ~BIT5;
    P1DIR |= BIT5;
    
    // RW (P1.6)
    P1OUT &= ~BIT6;
    P1DIR |= BIT6;

    // Enable (P1.7)
    P1OUT &= ~BIT7;
    P1DIR |= BIT7;

    // Contrast (P2.0)
    P2OUT &= ~BIT0;
    P2DIR |= BIT0;

    // i2c pins
    // SDA (P2.6)
    P2OUT &= ~BIT6;
    P2DIR |= BIT6;
    // SCL (P2.7)
    P2OUT &= ~BIT7;
    P2DIR |= BIT7;
}
