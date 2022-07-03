#include <msp430.h>

int button1State = 0;
int button2State = 0;

#pragma vector = PORT1_VECTOR
__interrupt void buttonPush1(void) 
{
    //button1 ↓
    if (button1State == 0)
    {
    	//button2 down
        if (button2State == 1)
        {
            P1OUT |= BIT3;	//enable ledA
        }

        P1IES &= ~BIT7;
        button1State = 1;
    } 
	//button1 ↑
	else 
	{
        P1IES |= BIT7;
        button1State = 0;
    }
    volatile int i = 0;
    for (i = 0; i < 1000; i++) {}
    P1IFG = 0;
}

#pragma vector = PORT2_VECTOR
__interrupt void buttonPush2(void) 
{
    //button2 ↓
	if (button2State == 0)
    { 
		//button1 up
        if (button1State == 0)
        {
            P1OUT &= ~BIT4;	//disable ledA
        }

        P2IES &= ~BIT2;
        button2State = 1;
    } 
	//button2 ↑
	else 
	{
		//button1 up
        if (button1State == 0)
        {
        	P1OUT |= BIT4;	//enable ledA
        }

		P1OUT &= ~BIT3;	//disable ledB
        P2IES |= BIT2;
        button2State = 0;
    }
    volatile int i = 0;
    for (i = 0; i < 1000; i++) {}
    P2IFG = 0;
}

int main(void) 
{
	//disable watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

	//setup leds
    P1DIR = BIT4;
    P1DIR = BIT3;
    P1OUT = 0;

	//setup buttons
    P1REN |= BIT7;
    P1OUT |= BIT7;

    P2REN |= BIT2;
    P2OUT |= BIT2;

    __bis_SR_register(GIE);
	
	//setup interruptions 
    P1IE |= BIT7;
    P2IE |= BIT2;
    P1IES |= BIT7;
    P2IES |= BIT2;
    P1IFG = 0;
    P2IFG = 0;

    __no_operation();

    return 0;
}