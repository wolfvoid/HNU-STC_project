/***********************************************************************************
*This program is a demo of how to use some U8glib functions to draw some graph.
*This demo was made for OLED modules with a screen resolution of 128x64 pixels.
*This program requires the U8glib library.

* File                : Example01-graph_test.ino
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
**********************************************************************************/
#include "U8glib.h" ////U8g lib

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE); //IIC

void prepare(void) 
{
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

//draw rectangle
void draw_rectangle(void)
{
   int i; 
   for(i = -10;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(22, 0, "draw rectangle");
        u8g.drawFrame(48,15,30,20);
        u8g.drawFrame(4*i,40,30,20);
      }while(u8g.nextPage());       
   }  
}
//fill rectangle
void fill_rectangle(void)
{
   int i; 
   for(i = -10;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(22, 0, "fill rectangle");
        u8g.drawBox(48,15,30,20);
        u8g.drawBox(4*i,40,30,20);
      }while(u8g.nextPage());       
   }  
}

//draw circle
void draw_circle(void)
{
   int i; 
   for(i = -10;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(31, 0, "draw circle");
        u8g.drawCircle(64,25,10);
        u8g.drawCircle(4*i+10,50,10);
      }while(u8g.nextPage());       
   }  
}

//fill circle
void fill_circle(void)
{
   int i; 
   for(i = -10;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(31, 0, "fill circle");
        u8g.drawDisc(64,25,10);
        u8g.drawDisc(4*i+10,50,10);
      }while(u8g.nextPage());       
   }  
}

//draw round rectangle
void draw_round_rectangle(void)
{
   int i; 
   for(i = -10;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(4, 0, "draw round rectangle");
        u8g.drawRFrame(48,15,30,20,5);
        u8g.drawRFrame(4*i,40,30,20,5);
      }while(u8g.nextPage());       
   }   
}

//fill round rectangle
void fill_round_rectangle(void)
{ 
   int i; 
   for(i = -10;i<=32;i++) //X from -128~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(4, 0, "fill round rectangle");
        u8g.drawRBox(48,15,30,20,5);
        u8g.drawRBox(4*i,40,30,20,5);
      }while(u8g.nextPage());       
   } 
}

//draw lines
void draw_line(void)
{
     int i; 
   for(i = 0;i<=32;i++) //X from 0~128 
   {
      u8g.firstPage();  
      do 
      {
        u8g.drawStr( 37, 0, "draw line");
        u8g.drawLine(i, 10, 0, 60);
        u8g.drawLine(i*2, 10, 0, 60);
        u8g.drawLine(i*3, 10, 0, 60);
        u8g.drawLine(i*4, 10, 0, 60);
        u8g.drawLine(u8g.getWidth()-i, 10, 127, 60);
        u8g.drawLine(u8g.getWidth()-i*2, 10, 127, 60);
        u8g.drawLine(u8g.getWidth()-i*3, 10, 127, 60);
        u8g.drawLine(u8g.getWidth()-i*4, 10, 127, 60);
        u8g.drawLine(u8g.getHeight()+i, 10, 63, 60);
        u8g.drawLine(u8g.getHeight()+i*2, 10, 63, 60); 
        u8g.drawLine(u8g.getHeight()-i, 10, 63, 60);
        u8g.drawLine(u8g.getHeight()-i*2, 10, 63, 60);       
      }while(u8g.nextPage());       
   } 
}

//show rotation
void show_rotation(void)
{
    int i; 
    u8g.setFont(u8g_font_unifont);
    for(i=0;i<4;i++)
    {
       switch(i)
       {
          case 0:
            u8g.undoRotation();
            break;
          case 1:
            u8g.setRot90();
            break;
          case 2:
            u8g.setRot180();
            break;
          case 3: 
            u8g.setRot270();
            break;
       } 
      u8g.firstPage();  
      do 
      {
        u8g.drawStr(10, 10, "hello!");
      }while(u8g.nextPage()); 
      delay(1500);
    }
}

//show zoom
void display_zoom(void)
{
    int i; 
    u8g.setFont(u8g_font_9x18);
    for(i=0;i<4;i++)
    {
       switch(i)
       {
          case 0:
            u8g.setFontPosTop();
            break;
          case 1:
            u8g.setFontPosBottom();
            break;
          case 2:
            u8g.setFontPosBaseline();
            break;
          case 3: 
            u8g.setFontPosCenter();
            break;
       } 
      u8g.firstPage();  
      do 
      {
         u8g.drawStr(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
         u8g.drawStr90(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
         u8g.drawStr180(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
         u8g.drawStr270(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
      }while(u8g.nextPage()); 
      delay(500);
    }
     for(i=0;i<4;i++)
    {
       switch(i)
       {
          case 3:
            u8g.setFontPosTop();
            break;
          case 2:
            u8g.setFontPosBottom();
            break;
          case 1:
            u8g.setFontPosBaseline();
            break;
          case 0: 
            u8g.setFontPosCenter();
            break;
       } 
      u8g.firstPage();  
      do 
      {
         u8g.drawStr(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
         u8g.drawStr90(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
         u8g.drawStr180(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
         u8g.drawStr270(u8g.getWidth()>>1, u8g.getHeight()>>1, "FR");
      }while(u8g.nextPage()); 
      delay(500);
    } 
}

void setup() 
{
    prepare();
}

void loop() 
{
  //loop
  prepare();
  u8g.undoRotation();
  draw_rectangle();
  delay(1000);
  fill_rectangle();
  delay(1000);
  draw_circle();
  delay(1000);
  fill_circle();
  delay(1000);
  draw_round_rectangle();
  delay(1000);
  fill_round_rectangle();
  delay(1000);
  draw_line();
  delay(1000);
  display_zoom();
  delay(1000);
  show_rotation();
  delay(1000);
}
