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

#define LCD_SPI_data()		(PORTB |=(1<<1))
#define LCD_SPI_command()	(PORTB &= ~(1<<1))
#define LCD_SPI_CS_High()	(PORTB |= (1<<0))
#define LCD_SPI_CS_Low()	(PORTB &= ~(1<<0))
#define LCD_page(page)		(lcd_spi_command(0xb0+page))
#define MEM_read(byte)	(pgm_read_byte(&(byte)))
#define LCD_RESET 0x04
#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "graphics.h"

void lcd_spi_init(void);
void lcd_spi_command(unsigned char command);
void lcd_spi_data(unsigned char data);
void lcd_init(void);
void lcd_draw(unsigned char page, unsigned char col, unsigned char data);
void lcd_col(unsigned char col);
void lcd_clear(void);
void lcd_draw_menu(void);
void lcd_draw_bignum(unsigned char digit, unsigned char num);
void lcd_draw_lilnum(unsigned char digit, unsigned char num);

volatile char mode = 0;

ISR(PCINT2_vect)
{
	if(PIND==0x00)
	{
		mode=(mode+1)%3;
		lcd_draw_menu();
	}
}

int main(void)
{
	DDRD &= ~0x01;
	PORTD |= 0x01;
	DDRB = 0xFF;
	PCICR = (1<<2);
	PCMSK2 = 0x01;
	lcd_init();
	lcd_clear();
	lcd_draw_menu();
	sei();
	while(1==1)
	{
		for (int x=0;x<0xff;x++)
		{
			lcd_draw_bignum(0,1);
			lcd_draw_bignum(1,2);
			lcd_draw_lilnum(0,3);
			lcd_draw_menu();
			_delay_ms(500);
		}
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
	lcd_spi_init();
	//reset lcd
	PORTB &= ~(LCD_RESET);
	_delay_ms(200);
	PORTB |= LCD_RESET;
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
void lcd_draw_menu(void)
{
	LCD_page(0);
	char mode0 = (mode==0)*0x7f;
	char mode1 = (mode==1)*0x7f;
	char mode2 = (mode==2)*0x7f;
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
