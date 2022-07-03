#include <msp430.h>

volatile int short CNANGED_FREQUENCY_MODE = 0;
volatile int short LOW_POWER_MODE_CORE_4_MODE = 0;
volatile int short DEFAULT_DELAY_IN_INTERRUPT = 1000;

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_S1(void) 
{
    volatile int i;
    volatile int k = 1;

    for (i = 0; i < DEFAULT_DELAY_IN_INTERRUPT; i++) 
    {
        k++;
    }

    if (LOW_POWER_MODE_CORE_4_MODE) 
    {
        _bic_SR_register_on_exit(LPM4_bits);    //exit LPM after interrupt
        LOW_POWER_MODE_CORE_4_MODE = 0;
    } 
    else 
    {
        LOW_POWER_MODE_CORE_4_MODE = 1;
        _bis_SR_register_on_exit(LPM4_bits);    //enter LPM after interrupt
    }

    P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_S2(void) 
{
    volatile int i;
    volatile int k = 1;

    for (i = 0; i < DEFAULT_DELAY_IN_INTERRUPT; i++) 
    {
        k++;
    }

    if (CNANGED_FREQUENCY_MODE) 
    {
        UCSCTL4 &= 0;
        UCSCTL5 &= 0; 
        UCSCTL4 |= SELM__DCOCLK;    //select source for MCLK
        UCSCTL5 |= DIVM__32;    //MCLK div
        CNANGED_FREQUENCY_MODE = 0;
    } 
    else 
    {
        UCSCTL4 &= 0;
        UCSCTL5 &= 0;
        UCSCTL4 |= SELM__REFOCLK;   //select source for MCLK
        UCSCTL5 |= DIVM__4; //MCLK div
        CNANGED_FREQUENCY_MODE = 1;
    }

    P2IFG &= ~BIT2;
}

int main(void) 
{
    //disable watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    //setup buttons
    P1DIR &= ~BIT7;
    P1OUT |= BIT7;
    P1REN |= BIT7;

    P2DIR &= ~BIT2;
    P2OUT |= BIT2;
    P2REN |= BIT2;

    __bis_SR_register(GIE);
    
    //setup interrupts on buttons
    P1IES |= BIT7;
    P1IFG &= ~BIT7;
    P1IE |= BIT7;

    P2IES |= BIT2;
    P2IFG &= ~BIT7;
    P2IE |= BIT2;

    //pins for reading MCLK
    P7DIR |= BIT7;
    P7SEL |= BIT7;
    
    //CLK management registers
    UCSCTL1 &= 0;
    UCSCTL2 &= 0;
    UCSCTL3 &= 0; 
    UCSCTL4 &= 0; 
    UCSCTL5 &= 0;

    UCSCTL4 |= SELM__DCOCLK;     //select source for MCLK
    UCSCTL1 |= DCORSEL_0;   //select DCO range
    
    // DCOCLK = 950 kHz
    UCSCTL3 |= SELREF__REFOCLK; //FLL source
    UCSCTL3 |= FLLREFDIV__1;    //FLL input div
    UCSCTL2 |= FLLD__1; //DCOCLK div
    UCSCTL2 |= FLLN4 | FLLN3 | FLLN2 | FLLN1;   //DCOCLK mul
    UCSCTL2 &= ~FLLN0;
    UCSCTL5 |= DIVM__32;     //MCLK div

    return 0;
}

