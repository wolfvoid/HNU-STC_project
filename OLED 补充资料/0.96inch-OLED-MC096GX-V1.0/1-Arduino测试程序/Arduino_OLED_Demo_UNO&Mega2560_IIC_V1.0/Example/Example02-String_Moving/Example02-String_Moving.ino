/****************************************************************************
*This program is a demo of moving string to right,left,down and up.
*This demo was made for OLED modules with a screen resolution of 128x64 pixels.
*This program requires the U8glib library.

* File                : Example02-String_Moving.ino
* Hardware Environment: Arduino UNO&Mega2560
* Build Environment   : Arduino

*Set the pins to the correct ones for your development shield or breakout board.
*This demo use the BREAKOUT BOARD only and use these IIC data lines to the LCD,
*pin usage as follow:
*             SCL SDA
*       Uno  A5  A4  
*   Mega2560 21  20  

*Remember to set the pins to suit your display module!
*
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "U8glib.h" //U8g lib

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE); //IIC

//string for right and left
const char str1[] = "abcdefghik";
const char str2[] = "ABCDEFGHIK";
const char str3[] = "1234567890";

//string for down and up
const char str4[] = "12345";
const char str5[] = "ABCDE";
const char str6[] = "abcde";

//prepare for drawing string
void prepare(void) 
{
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

//show string moving to right
void string_moving_to_right(void)
{
   int i; 
   for(i = -32;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(0,0,"RIGHT");
        u8g.drawStr(i*4,16,(char *)str1); 
        u8g.drawStr(i*4,26,(char *)str2);
        u8g.drawStr(i*4,36,(char *)str3); 
      }while(u8g.nextPage());       
   }
}

//show string moving to right
void string_moving_to_left(void)
{
   int i; 
   for(i = 32;i>=-32;i--) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(0,0,"LEFT");
        u8g.drawStr(i*4,16,(char *)str1);
        u8g.drawStr(i*4,26,(char *)str2);
        u8g.drawStr(i*4,36,(char *)str3); 
      }while(u8g.nextPage());       
   }
}

//show string moving to down 
void string_moving_to_down(void)
{
   int i;  
   for(i = -16;i<=16;i++) //X from -64~64 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(0,0,"DOWN");
        u8g.drawStr90(58,i*4,(char *)str4);
        u8g.drawStr90(68,i*4,(char *)str5);
        u8g.drawStr90(78,i*4,(char *)str6);
      }while(u8g.nextPage());       
   }
}

//show string moving to up 
void string_moving_to_up(void)
{
   int i; 
   for(i = 16;i>=-16;i--) //X from -64~64 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(0,0,"UP");
        u8g.drawStr90(58,i*4,(char *)str4);
        u8g.drawStr90(68,i*4,(char *)str5);
        u8g.drawStr90(78,i*4,(char *)str6);
      }while(u8g.nextPage());       
   }
}

void setup() 
{
  prepare(); 
}

void loop() 
{
  //show moving string loop
  prepare();
  string_moving_to_right();
  delay(150);
  string_moving_to_left();
  delay(150);
  string_moving_to_down();
  delay(150);
  string_moving_to_up();
  delay(150); 
}
