#include <msp430.h>
#include <math.h>


typedef unsigned char uchar;


#define SCROL_CTL                   0x78	//Scroll image up by SL rows (SL = last 5 bits), range:0-63 //78
#define SCROL_CTL_DOWN              0x58	//Scroll image up by SL rows (SL = last 5 bits), range:0-63 //78

#define SET_MIRROR_COL 				0xA0	//Normal mirror SEG (column) mapping (set bit0 to mirror columns) //A1

#define SET_MIRROR_ROW 				0xC0	//Normal mirror COM (row) mapping (set bit3 to mirror rows) //C8

#define ALL_PIXEL_ON                0xA4	//Disable all pixel on (last bit 1 to turn on all pixels - does not affect memory)

#define LCD_INVERSE                 0xA6	//Inverse display off (last bit 1 to invert display - does not affect memory)
#define LCD_INVERSE_YES             0xA7	//Inverse display on

#define BIAS_RATIO_VCC              0xA2	//Set voltage bias ratio (BR = bit0)

#define POW_CTL                     0x2F	//Set Power control - booster, regulator, and follower on

#define SET_CONTRAST_RESISTOR		0x27	//Set internal resistor ratio Rb/Ra to adjust contrast
#define MSB_SELECT_VOLUME           0x81	//Set Electronic Volume "PM" to adjust contrast
#define LSB_SELECT_VOLUME           0x10	//Set Electronic Volume "PM" to adjust contrast (PM = last 5 bits)

#define ADV_CTL_MSB                 0xFA	//Set temp. compensation curve to -0.11%/C
#define ADV_CTL_LSB                 0x90

#define COLUMN_ADR_MSB              0x12	//Set SRAM col. addr. before write, last 4 bits = ca4-ca7
#define COLUMN_ADR_LSB              0x00	//Set SRAM col. addr. before write, last 4 bits = ca0-ca3
#define PAGE_ADR                    0xB0	//Set SRAM page addr (pa = last 4 bits), range:0-8

#define LCD_EN                      0xAF	//Enable display (exit sleep mode & restore power)

#define ROWS 7
#define COLUMNS 4
#define PAGES 1
#define DELAY 500
#define COLUMN_OFFSET_BIG 80
#define COLUMN_OFFSET_NONE 0


void Delay(long int value);
int GetS1State();
int GetS2State();

void SetupSPI();
void SetupLCD();
void SetupButtons();

void __LCD_SetAddress(uchar page, uchar column);
void Dogs102x6_writeData(uchar *sData, uchar i);
void Dogs102x6_writeCommand(uchar *sCmd, uchar i);

void Clear(void);
void ShowNumber(void);

uchar scroll[1] = {SCROL_CTL};
uchar plus[PAGES][COLUMNS]  = {{0x08, 0x1C, 0x08, 0x00}};
uchar minus[PAGES][COLUMNS] = {{0x08, 0x08, 0x08, 0x00}};
uchar digits[10][COLUMNS] = {
  {0x7F, 0x41, 0x7F, 0x00}, // digit 0
  {0x7F, 0x02, 0x04, 0x00}, // digit 1
  {0x4F, 0x49, 0x79, 0x00}, // digit 2
  {0x7F, 0x49, 0x49, 0x00}, // digit 3
  {0x7F, 0x08, 0x0F, 0x00}, // digit 4
  {0x79, 0x49, 0x4F, 0x00}, // digit 5
  {0x79, 0x49, 0x7F, 0x00}, // digit 6
  {0x0F, 0x71, 0x03, 0x00}, // digit 7
  {0x7F, 0x49, 0x7F, 0x00}, // digit 8
  {0x7F, 0x49, 0x4F, 0x00}  // digit 9
};

int number = 4903;
int subtrahend = 571;
int column_offset = COLUMN_OFFSET_BIG;

uchar LCD_INIT_COMMANDS_PART_1[7] = {
	SCROL_CTL,
	SET_MIRROR_COL,
	SET_MIRROR_ROW,
	ALL_PIXEL_ON,
	LCD_INVERSE,
	BIAS_RATIO_VCC,
	POW_CTL
};
uchar LCD_INIT_COMMANDS_PART_2[6] = {
	SET_CONTRAST_RESISTOR,
	MSB_SELECT_VOLUME,
	LSB_SELECT_VOLUME,
	ADV_CTL_MSB,
	ADV_CTL_LSB,
	LCD_EN,
};
int AMOUNT_OF_COMMANDS_1 = 7;
int AMOUNT_OF_COMMANDS_2 = 6;

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;

  SetupLCD();
  SetupButtons();

  Clear();
  ShowNumber();

  __bis_SR_register(GIE);
  __no_operation();

  return 0;
}

void SetupLCD()
{
	//reset LCD
	P5DIR |= BIT7;	//port init for LCD operations
	P5OUT &= ~BIT7;	//set RST (active low)
	P5OUT |= BIT7;	//reset RST (inactive is high)

	Delay(550);	//delay for at least 5ms

	//choose slave
	P7DIR |= BIT4;	//select LCD for chip
	P7OUT &= ~BIT4;	//CS is active low

	//setup LCD_D/C
	P5DIR |= BIT6;	//Command/Data for LCD
	P5OUT &= ~BIT6;	//CD low for command

	//setup P4.1 -- SIMO, P4.3 -- SCLK (select PM_UCB1CLK)
	P4SEL |= BIT1 | BIT3;
	P4DIR |= BIT1 | BIT3;

	//setup backlit
	P7DIR |= BIT6;	//init
	P7OUT |= BIT6;	//backlit
	P7SEL &= ~BIT6; //use PWM to controll brightness

	//deselect slave
	P7OUT |= BIT4;	//CS = 1 (Deselect LCD) (stop setting it up)

	UCB1CTL1 |= UCSWRST;	//set UCSWRST bit to disabel USCI and change its control registeres

	UCB1CTL0 = (
		UCCKPH 	&	//UCCKPH - 1: change out on second signal change, capture input on first one)
		~UCCKPL |	//UCCKPL - 0: active level is 1
		UCMSB 	|	//MSB comes first, LSB is next
		UCMST 	|	//Master mode
		UCSYNC 	|	//Synchronious mode
		UCMODE_0	//3 pin SPI mode
	);

	//set SMCLK as source and keep RESET
	UCB1CTL1 = UCSSEL_2 | UCSWRST;

	//set frequency divider
	UCB1BR0 = 0x01;	//LSB to 1
	UCB1BR1 = 0;	//MSB to 0

	UCB1CTL1 &= ~UCSWRST;	//enable USCI
	UCB1IFG &= ~UCRXIFG;	//reset int flag (which is set after input shift register gets data)
	Dogs102x6_writeCommand(LCD_INIT_COMMANDS_PART_1, AMOUNT_OF_COMMANDS_1);

	Delay(12500);	//delay to wait at least 120 ms

	Dogs102x6_writeCommand(LCD_INIT_COMMANDS_PART_2, AMOUNT_OF_COMMANDS_2);
}

void SetupButtons()
{
	//setup buttons for reading
	P1DIR &= ~BIT7;
	P2DIR &= ~BIT2;

	//setup pull up resistors
	P1REN |= BIT7;
	P2REN |= BIT2;

	//initialize buttons with inactive level
	P1OUT |= BIT7;
	P2OUT |= BIT2;

	//setup interrupts for S1
	P1IE |= BIT7;
	P1IES |= BIT7;
	P1IFG = 0;

	//setup interrupts for S2
	P2IE |= BIT2;
	P2IES |= BIT2;
	P2IFG = 0;
}

void __LCD_SetAddress(uchar page, uchar column)
{
	uchar cmd[1];

	if (page > 7)
	{
		page = 7;
	}

	if (column > 101)
	{
		column = 101;
	}

	cmd[0] = PAGE_ADR + (7 - page);
	uchar command_high = 0x00;
	uchar command_low = 0x00;
	uchar column_address[] = { COLUMN_ADR_MSB, COLUMN_ADR_LSB };

	command_low = (column & 0x0F);
	command_high = (column & 0xF0);
	command_high = (command_high >> 4);

	column_address[0] = COLUMN_ADR_LSB + command_low;
	column_address[1] = COLUMN_ADR_MSB + command_high;

	Dogs102x6_writeCommand(cmd, 1);
	Dogs102x6_writeCommand(column_address, 2);
}

void Dogs102x6_writeCommand(uchar *sCmd, uchar i)
{
	P7OUT &= ~BIT4;	//CS Low
	P5OUT &= ~BIT6;	//CD Low

	while (i)
	{
		//USCI_B1 TX buffer ready?
		while (!(UCB1IFG & UCTXIFG));

		UCB1TXBUF = *sCmd;	//transmit data 
		sCmd++;	//increment the pointer on the array
		i--;	//decrement the Byte counter
	}

	// Wait for all TX/RX to finish
	while (UCB1STAT & UCBUSY);

	UCB1RXBUF;	//dummy read to empty RX buffer and Clear any overrun conditions
	P7OUT |= BIT4;	//CS High
}

void Dogs102x6_writeData(uchar *sData, uchar i)
{
	P7OUT &= ~BIT4;	//CS Low
	P5OUT |= BIT6;	//CD High

	while (i)
	{
		// USCI_B1 TX buffer ready?
		while (!(UCB1IFG & UCTXIFG));

		UCB1TXBUF = *sData++;	//transmit data and increment pointer
		i--;	//decrement the Byte counter
	}

	// Wait for all TX/RX to finish
	while (UCB1STAT & UCBUSY);

	UCB1RXBUF;	//dummy read to empty RX buffer and Clear any overrun conditions
	P7OUT |= BIT4;	//CS High
}

void ShowNumber(void)
{
	volatile int length = 1;
	volatile int digit = 0;
	volatile int j = 0;
	volatile int i = 10;

	while(1)
	{
		if(number / i != 0)
		{
			i *= 10;
			length++;
		}
		else
		{
			break;
		}
	}

	int temp = number;
	for(j = 0; j < length; j++)
	{
		digit = (int)(temp % 10);

		digit = digit < 0 ? (-1) * digit : digit;

		if (digit < 10)
			{
			__LCD_SetAddress(0, column_offset + j * COLUMNS);
			Dogs102x6_writeData(digits[digit], COLUMNS);
		}

		temp /= 10;
	}

	if (number >= 0)
	{
		__LCD_SetAddress(0, column_offset + length * COLUMNS);
		Dogs102x6_writeData(plus[0], COLUMNS);
	}
	else
	{
		__LCD_SetAddress(0, column_offset + length * COLUMNS);
		Dogs102x6_writeData(minus[0], COLUMNS);
	}
}

void Clear(void)
{
	uchar lcd_data[] = {0x00};
	uchar page, column;

	for (page = 0; page < 8; page++)
	{
		__LCD_SetAddress(page, 0);
		for (column = 0; column < 132; column++)
		{
			Dogs102x6_writeData(lcd_data, 1);
		}
	}
}

#pragma vector = PORT1_VECTOR
__interrupt void __S1_ButtonHandler(void)
{
	Delay(DELAY);

	if(GetS1State())
	{
		Clear();
		number = number - subtrahend;
		if (number<-9999)
			number+=15000;
		ShowNumber();
	}

	P1IFG &= ~BIT7;
}


#pragma vector = PORT2_VECTOR
__interrupt void __S2_ButtonHandler(void)
{
	Delay(DELAY);

	if(GetS2State())
	{
		Clear();
		scroll[0]-=0x20;
		if (*scroll<0x40) 
		{
			*scroll=SCROL_CTL-(0x40-*scroll);
		}
		Dogs102x6_writeCommand(scroll, 1);

		ShowNumber();
	}

	P2IFG &= ~BIT2;
}

void Delay(long int value)
{
	volatile long int i = 0;
	volatile long int temp = 0;
	for (; i < value; i++)
	{
		temp++;
	}
}

int GetS1State()
{
	return (P1IN & BIT7) ? 0 : 1;
}

int GetS2State()
{
	return (P2IN & BIT2) ? 0 : 1;
}
