#include <msp430.h>

int selected_timer = 0;
int counter = 0;
int count = 0;
int count_wdt = 0;
int canChange = 1;
const int timeD = 1000;
int second = 32768;

int bDelay = 0;

void initTA()
{
    TA2CCTL0 |= CCIE;   //interrupt en
    TA2CTL = TASSEL__SMCLK | ID__1 | MC__UP | TACLR;    //source = smclk | input div = 1 | count mode = up | clear timer
    TA2CCR0 = second;   //ccr0 value
}

//delay for value ticks with ccr0
void delay(int value)
{
    TA1CTL = TASSEL_1 | ID__1 | MC__UP | TACLR;
    TA1CCR0 = value;
    TA1CCTL0 |= CCIE;
    bDelay= 1;
}

//end of delay
#pragma vector = TIMER1_A0_VECTOR
__interrupt void ta1_isr (void)
{
    bDelay = 0;
}

//task 2
void blink(void)
{
    switch(counter)
    {

    //turn 1 off
    case 0:
        P1OUT &= ~BIT2;
        counter++;
        break;

    //turn 2 off
    case 1:
        P1OUT &= ~BIT3;
        counter++;
        break;

    //turn 3 off
    case 2:
        P1OUT &= ~BIT4;
        counter++;
        break;

    //all on
    case 3:
        P1OUT |= BIT2;
        P1OUT |= BIT3;
        P1OUT |= BIT4;
        counter = 0;
        break;

    default:
        break;
    }
}

//task 2 with wdt
#pragma vector = WDT_VECTOR
interrupt void wdt_isr(void)
{
    if( selected_timer == 1 && count_wdt == 140)
    {
        blink();
        count_wdt=0;
    }
    else
    {
        count_wdt++;
    }
}

//task 2 with ta
#pragma vector = TIMER2_A0_VECTOR
__interrupt void ta2_isr (void)
{
    blink();
}

#pragma vector = PORT1_VECTOR
__interrupt void button_s1_isr(void)
{
   if(bDelay == 0)
   {
        delay(timeD);    //delay for button swithcing

        //suspend blinking on btn pressed
        if(canChange == 1)
        {
            if(selected_timer == 0)
            {
                TA2CCTL0 &= ~CCIE;  //disable ccr interrupt
                TA2CCTL0 &= ~OUT;   //ccr out to low
            }
            else
            {
                WDTCTL = WDTPW + WDTHOLD;   //disable wdt
            }
            
            canChange = 0;
            P1IES &=~ BIT7; //change interrupt direction
        }
        //resume blinking on btn released
        else 
        {
            if(selected_timer==0)
            {
                initTA();
            }
            else
            {
                SFRIE1 |= WDTIE;    //wdt interrupt enable
                WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL__SMCLK | WDTIS_5; //pass | iterval mode | clear | select SMCLK | select interval
            }

            canChange = 1;
            P1IES |= BIT7;  //change interrupt direction
        }
    }

   P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
interrupt void button_s2_isr (void)
{
    if(bDelay== 0)
    {
        delay(timeD);   //delay for button swithcing

        if(canChange == 1)
        {
            //change timer for task 2
            if (selected_timer == 0)
            {
                //disable TA
                TA2CCTL0 &= ~CCIE;
                TA2CCTL0 &= ~OUT;

                //enable wdt
                SFRIE1 |= WDTIE;
                WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL__SMCLK | WDTIS_5;

                selected_timer = 1;
            }
            else if (selected_timer ==  1)
            {
                WDTCTL = WDTPW + WDTHOLD;   //disable wdt

                initTA();   //enable TA

                selected_timer = 0;
            }
         }
         //if btn 1 is pressed change timer without starting it
         else
         {
            if(selected_timer == 1)
            {
                selected_timer = 0;
            }
            else
            {
                selected_timer = 1;
            }
         }
    }

    P2IFG &= ~BIT2;
}

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    
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

    //setup leds
    P1DIR |= BIT2;
    P1OUT |= BIT2;

    P1DIR |= BIT3;
    P1OUT |= BIT3;

    P1DIR |= BIT4;
    P1OUT |= BIT4;

    P1DIR |= BIT5;
    P1OUT &=~ BIT5; //off
    P1SEL |= BIT5;  //work with timer

    //select refoclk as source for ACLK, SMCLK
    UCSCTL4 |= SELS__REFOCLK;
    UCSCTL4 |= SELA__REFOCLK;

    //task 1
    TA0CTL = TASSEL_1 | ID__2 | MC__UP | TACLR; // source = aclk | input div = 2 | count mode = up | clear timer
    TA0CCR0 = second;
    TA0CCR4 = second/5;
    TA0CCTL4 |= (OUTMOD_6); //outmod_6 = output switches on CCRn, sets to '1' on CCR0

    initTA()

    _bis_SR_register(GIE);

    _no_operation();
    return 0;
}