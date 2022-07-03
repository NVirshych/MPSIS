#include <msp430.h>

int main(void) 
{
    //disable watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    //setup leds
    P1DIR |= BIT4;
    P1DIR |= BIT3;
    P1OUT = 0;

    //setup buttons
    P1REN |= BIT7;
    P1OUT |= BIT7;

    P2REN |= BIT2;
    P2OUT |= BIT2;


    int button1State = 0;
    int button2State = 0;

    while (1) 
    {
        int button1 = (P1IN & BIT7) == 0;
        int button2 = (P2IN & BIT2) == 0;
        
        if (button1 == 0 && button2 == 1 && button2State == 0)  //button1 U and button2 ↑ 
        {
            P1OUT &= ~BIT4; //disable ledA
        }
        if (button1 == 0 && button2 == 0 && button2State == 1)  //button1 U and button2 ↓
        {
            P1OUT |= BIT4;  //enable ledA
        }
        if (button1 == 1 && button1State == 0 && button2 == 1)  //button2 D and button1 ↓
        {
            P1OUT |= BIT3;  //enable ledB
        }
        if (button2 == 0 && button2State == 1)  //button2 ↑ 
        {
            P1OUT &= ~BIT3; //disable ledB
        }
        button1State = button1;
        button2State = button2;   
    }
    return 0;
}