//      biCycles.c
//      
//      Copyright 2011 Josh <muffin@josh-laptop>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#define LCD_SPI_data()		(PORTD |=(1<<4))
#define LCD_SPI_command()	(PORTD &= ~(1<<4))
#define LCD_SPI_CS_High()	(PORTD |= (1<<2))
#define LCD_SPI_CS_Low()	(PORTD &= ~(1<<2))
#define LCD_page(page)		(lcd_spi_command(0xb0+page))
#define MEM_read(byte)	(pgm_read_byte(&(byte)))
#define LCD_RESET (1<<3)
#define SD_pwr_on()		(PORTB |= (1<<2))
#define SD_pwr_off()	(PORTB &= ~(1<<2))
#define SD_CS	(1<<3)
#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"

void lcd_spi_init(void);
void lcd_spi_command(unsigned char command);
void lcd_spi_data(unsigned char data);
void lcd_init(void);
void lcd_draw(unsigned char page, unsigned char col, unsigned char data);
void lcd_col(unsigned char col);
void lcd_clear(void);
void lcd_draw_menu(unsigned char modus, unsigned char battery);
void lcd_draw_bignum(unsigned char digit, unsigned char num);
void lcd_draw_lilnum(unsigned char digit, unsigned char num);

volatile char mode = 0;
volatile unsigned char vread = 0;
volatile unsigned int vol = 0;
volatile unsigned char bat = 0;
volatile unsigned int revs = 0;
volatile char pin0 = 0;
volatile char pin1 = 0;
volatile char pinpush=0;
volatile unsigned int t=0;
volatile unsigned char overf=0;
unsigned int v=0;
unsigned int bdisp;
unsigned int ldisp;
unsigned int avg=0;
unsigned int vel=0;
unsigned int dist=0;
unsigned char contrast = 0x1b;

ISR(ADC_vect)
{
	vread = ADCH;
	vol = 27000/vread;
	bat = (86-vread)/2;
}

ISR(PCINT1_vect)
{
		if((PINC&0x02)!=0x02)
		{
			pin0++;
		}
		if((PINC&0x10)!=0x10)
		{
			pin1++;
		}
		pinpush=1;
}


ISR(PCINT2_vect)
{
	if((PIND&0x20)==0x20)
	{
		t = TCNT1;
		TCNT1=0;
		revs++;
	}
}

ISR(TIMER1_OVF_vect)
{
		if(overf<5)
		{		//display sleep	
			overf++;
		}
}

int main(void)
{
	DDRC |= 0x04; //LED as output
	DDRC &= ~0x12; //Buttons as inputs
	PORTC = 0x16; //pulled high internally
	DDRD |= 0xff;
	DDRD &= ~0x20; //hall0 input
	PORTD |= 0x20;
	PORTD |= 0x80; //turn on hall sensors
	DDRB = 0xff;
	PORTB &= ~0x01;
	
	PCICR = (1<<2)|(1<<1);
	PCMSK2 = 0x20; //enable hall0 pcint
	PCMSK1 = 0x12; //enable button pcint
	
	ADMUX = (1<<5)|0x0e; //VREF source, Left-adjust, select chanel 5
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1); //Enable adc and interrupts, div64 prescale
		
	TCCR1A = 0;
	TCCR1B = (1<<CS12)|(1<<CS10);
	TIMSK1 = (1<<TOIE1);  //enable overflow interrupt
	lcd_spi_init();
	lcd_init();
	lcd_clear();
	lcd_draw_menu(mode,bat);
	sei();
	while(1==1)
	{
		PCICR = 0x04; //disable button interrupts to process button presses
		mode = (mode+pin0)%3;
		pin0=0;
		vel = (15625.0/t)*490;
		dist = revs*0.13635;
		avg=(vel+avg)/2;
		if(mode==0)
		{
			ldisp = vel;
		}
		if(mode==1)
		{
			ldisp = dist;
			if(pin1>0)
			{
				contrast++;
				lcd_spi_command(0x81);
				lcd_spi_command(contrast);
				pin1=0;
			}
		}
		if(mode==2)
		{
			ldisp = avg;
			if(pin1>0)
			{
				lcd_init();
				pin1=0;
				PORTC ^= 0x04;
			}
		}
		if(overf==5)
		{
			lcd_spi_command(0xa5);
			lcd_spi_command(0xae);
			overf=6;
		}
		if(overf==6&&pinpush==1)
		{
			lcd_spi_command(0xa4);
			lcd_spi_command(0xaf);
			overf=0;
			pinpush=0;
		}
		bdisp = ldisp/100.0;
		PCICR = 0x06; //enable button interrupts
		ADCSRA |= (1<<ADSC); //start conversion
		lcd_draw_menu(mode,bat);
		lcd_draw_bignum(0,(bdisp/10)%10);
		lcd_draw_bignum(1,bdisp%10);
		lcd_draw_lilnum(0,(ldisp/10)%10);
		lcd_draw_lilnum(1,ldisp%10);
		_delay_ms(200);
	}
}

void lcd_spi_init(void)
{
	//set SPI enable, master, MSB first, CPOL=0, CPHA=0, f/2
	SPCR = (1<<6)|(1<<4)|(0<<0);
	SPSR |= (1<<0);
	//bring CS high
	LCD_SPI_CS_High();
}

void lcd_spi_command(unsigned char command)
{
	LCD_SPI_command();
	//_delay_us(2);
	LCD_SPI_CS_Low();
	SPDR = command;
	while (!(SPSR & (1<<SPIF)))
	{}
	LCD_SPI_CS_High();
}

void lcd_spi_data(unsigned char data)
{
	LCD_SPI_data();
	//_delay_us(2);
	LCD_SPI_CS_Low();
	SPDR = data;
	while (!(SPSR & (1<<SPIF)))
	{}
	LCD_SPI_CS_High();
}

void lcd_init(void)
{
	//reset lcd
	PORTD &= ~(LCD_RESET);
	_delay_ms(200);
	PORTD |= LCD_RESET;
	_delay_ms(10);

	//initialization sequence
	lcd_spi_command(0x40); //set scroll line
	lcd_spi_command(0xa1); //set display direction
	lcd_spi_command(0xc0); //set COM direction
	lcd_spi_command(0xa4); //turn all pixels on
	lcd_spi_command(0xa6); //turn display inversion off
	lcd_spi_command(0xa2); //set LCD bias ratio
	lcd_spi_command(0x2f); //turn on Booster
	lcd_spi_command(0x25); //set contrast
	lcd_spi_command(0x81); //set contrast
	lcd_spi_command(0x1b); //set contrast
	lcd_spi_command(0xfa); //temp compensation
	lcd_spi_command(0x90); //temp compensation
	lcd_spi_command(0xaf); //Set Display Enable
}

void lcd_draw(unsigned char page, unsigned char col, unsigned char data)
{
	lcd_spi_command(0x10 + ((col&0xf0)>>4));
	lcd_spi_command(col&0x0f);
	LCD_page(page);
	lcd_spi_data(data);
}

void lcd_col(unsigned char col)
{
	lcd_spi_command(0x10 + ((col&0xf0)>>4));
	lcd_spi_command(col&0x0f);
}

void lcd_clear(void)
{
	for (char x=0;x<8;x++)
	{
		for (char y=0;y<103;y++)
		{
			lcd_draw(x,y,0x00);
		}
	}
}
void lcd_draw_menu(unsigned char modus, unsigned char battery)
{
	LCD_page(0);
	char mode0 = (modus==0)*0x7f;
	char mode1 = (modus==1)*0x7f;
	char mode2 = (modus==2)*0x7f;
	for (int x=0;x<31;x++)
	{
		lcd_col(x);
		lcd_spi_data(mode0^MEM_read(menu[x]));
	}
	for (int x=31;x<59;x++)
	{
		lcd_col(x);
		lcd_spi_data(mode1^MEM_read(menu[x]));
	}
	for (int x=59;x<91;x++)
	{
		lcd_col(x);
		lcd_spi_data(mode2^MEM_read(menu[x]));
	}
	for (int x=91;x<(91+battery);x++)
	{
		lcd_col(x);
		lcd_spi_data(0x3c);
	}
	for (int x=91+battery; x<101;x++)
	{
		lcd_col(x);
		lcd_spi_data(0x00);
	}
	lcd_col(101);
	lcd_spi_data(0xff);
}

void lcd_draw_bignum(unsigned char digit, unsigned char num)
{
	for (int y=2; y<8;y++)
	{
		LCD_page(y);
		for (int x=0; x<32; x++)
		{
			lcd_col(x+digit*32);
			lcd_spi_data(MEM_read(big_one[num][y-2][x]));
		}
	}
}

void lcd_draw_lilnum(unsigned char digit, unsigned char num)
{
	for (int y=5; y<8; y++)
	{
		LCD_page(y);
		for (int x=0; x<16; x++)
		{
			lcd_col(x+digit*16+64);
			lcd_spi_data(MEM_read(lil_one[num][y-5][x]));
		}
	}
}
