
//     
/***************************************************/

#include<reg51.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#define uchar unsigned char
#define uint unsigned int

sbit cs        =P3^5;
sbit rs        =P2^3;
sbit sda       =P2^2;
sbit scl       =P2^1;
sbit reset     =P3^4;

uchar bdata bitdata;
sbit bit7=bitdata^7;
sbit bit6=bitdata^6;
sbit bit5=bitdata^5;
sbit bit4=bitdata^4;
sbit bit3=bitdata^3;
sbit bit2=bitdata^2;
sbit bit1=bitdata^1;
sbit bit0=bitdata^0;


void  delay(uint t);


void delay(uint time)
{
 uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<250;j++);
}




void LCD_CtrlWrite_IC(uchar c)
{
bitdata=c;
cs=0;
rs=0;
sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;


}
void  LCD_DataWrite_IC(uchar d)  
{

bitdata=d;
cs=0;
rs=1;
sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;
}

void LCD_DataWrite(uchar LCD_DataH,uchar LCD_DataL)
{
LCD_DataWrite_IC(LCD_DataH);
LCD_DataWrite_IC(LCD_DataL);
}


void  write_command(uchar c)
{
bitdata=c;
cs=0;
rs=0;
sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;      
}

void  write_data(uchar d)
{
bitdata=d;
cs=0;
rs=1;
sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;
}


void Reset()
{
    reset=0;
    delay(100);
    reset=1;
    delay(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////

void lcd_initial()
{
/*
   reset=0;
   delay(100);
   reset=1;
   delay(100);
  */
//------------------------------------------------------------------//  
//-------------------Software Reset-------------------------------//
//------------------------------------------------------------------//
 write_command(0x01);   //Sofeware setting
  delay(10);
  write_command(0x11);   //sleep out
  delay(120);

    write_command(0x3A); 
    write_data(0x05);  //  18 bit  RGB color  format  select
    delay(10);

   write_command(0xB1);  // Frame rate setting   95hz
   write_data(0x00);
   write_data(0x06);
   write_data(0x03);
    delay(10);

    write_command(0x36);  // MV,MX,MY 
   write_data(0xC8); 

   write_command(0xB6);  // Display control non-overlap
   write_data(0x15); 
   write_data(0x02); 
   
  

   write_command(0xB4);  // line inverion
   write_data(0x00); 

   //POWER ON SETTING 

   write_command(0xC0);    //VCI1=2.65V
   write_data(0x02);
   write_data(0x70);
   delay(10);
   write_command(0xC1);   
   write_data(0X05);  // VGH,VGL
   write_command(0xC2);  
   write_data(0X01);
   write_data(0X02);   // DC/DC SET
   write_command(0xC5); // VCOMH=3.925V;VCOML=0.875V, 
   write_data(0x3C);  
   write_data(0x38);
    delay(10);

   write_command(0xFC); 
   write_data(0x11);
   write_data(0x15);
    //GAMMA SETTING
    
  write_command(0xE0);// Positive
  write_data(0x09);//+1
  write_data(0x16);//+2
  write_data(0x09);//+3
  write_data(0x20);//+4
  write_data(0x21);//+5
  write_data(0x1B);//+6
  write_data(0x13);//+7
  write_data(0x19);//+8
  write_data(0x17);//+9
  write_data(0x15);//+10
  write_data(0x1E);//+11
  write_data(0x2B);//+12
  write_data(0x04);//+13
  write_data(0x05);//+14
  write_data(0x02);//+15
  write_data(0x0E);//+16



  write_command(0xE1);// Negative
  write_data(0x0B);//-1
  write_data(0x14);//-2
  write_data(0x08);//-3
  write_data(0x1E);//-4
  write_data(0x22);//-5
  write_data(0x1D);//-6
  write_data(0x18);//-7
  write_data(0x1E);//-8
  write_data(0x1B);//-9
  write_data(0x1A);//-10
  write_data(0x24);//-11
  write_data(0x2B);//-12
  write_data(0x06);//-13
  write_data(0x06);//-14
  write_data(0x02);//-15
  write_data(0x0F);//-16
  delay(10);

  //window setting
   write_command(0x2A);
   write_data(0x00);
   write_data(0x02);
   write_data(0x00);
   write_data(0x81);
  
   write_command(0x2B);
   write_data(0x00);
   write_data(0x01);
   write_data(0x00);
   write_data(0xA0);

   //write_command(0x26);  // default gamma setting 
   //write_command(0x30);  // partial area setting 
   
    write_command(0x13);
    delay(10);
    
    write_command(0x2C);

    delay(20);

    write_command(0x29);  //display  on setting 

    delay(120);
}




void dsp_single_colour(DH,DL)
{
 uchar i,j;
 //RamAdressSet();
 for (i=0;i<160;i++)
    for (j=0;j<128;j++)
        LCD_DataWrite(DH,DL);
}



main()
{
 lcd_initial();

      while(1)
  {
 write_command(0x2C);
  //  Disp_gradscal(); //灰阶
   // delay(500); 
      
//    dsp_single_colour(0x84,0x10);//灰色
//    delay(500);	

    dsp_single_colour(0xff,0xff);//白色
    delay(500);	
    	
    dsp_single_colour(0x00,0x00);//黑色
    delay(500);	
    	
    dsp_single_colour(0xf8,0x00);//红色
    delay(500);	
    	
    dsp_single_colour(0x07,0xe0);//绿色
    delay(500);	
    	
    dsp_single_colour(0x00,0x1f);//蓝色
    delay(500);	

    }

 }



