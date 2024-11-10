/*******************************************************************************
*This program is a demo of how to show a BMP picture to the screen.
*This demo was made for OLED modules with a screen resolution of 128x64 pixels.
*This program requires the U8glib library.

* File                : Example03-show_BMP.ino
* Hardware Environment: Arduino UNO&Mega2560
* Build Environment   : Arduino

*Set the pins to the correct ones for your development shield or breakout board.
*This demo use the BREAKOUT BOARD only and use these IIC data lines to the LCD,
*pin usage as follow:
*             SCL SDA
*        Uno  A5  A4  
*    Mega2560 21  20  

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
***********************************************************************************/
#include "U8glib.h" //U8g lib
#include "BMP.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);//IIC

char *pic_name[] = {BMP1,BMP2,BMP3}; //BMP pic name

void setup() 
{
  
}

void loop() {
  // show bmp picture loop
  int i;
  for(i=0;i<3;i++)
  {
    u8g.firstPage();  
    do 
    {
      u8g.drawXBMP( 0, 0, 128, 64, pic_name[i]);
    } while(u8g.nextPage());
    delay(3000);
 }  
}
