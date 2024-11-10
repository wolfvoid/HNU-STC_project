#ifndef __OLED_H
#define __OLED_H

#include <STC15F2K60S2.H>		  	 
 
#define  u8 unsigned char 
#define  u16 unsigned int
#define  u32 unsigned int
	
#define OLED_CMD  0	//
#define OLED_DATA 1	//

sbit OLED_SCL=P1^0;//SCL
sbit OLED_SDA=P1^1;//SDA
sbit OLED_RES =P1^2;//RES

//-----------------OLED----------------

#define OLED_SCL_Clr() OLED_SCL=0
#define OLED_SCL_Set() OLED_SCL=1

#define OLED_SDA_Clr() OLED_SDA=0
#define OLED_SDA_Set() OLED_SDA=1

#define OLED_RES_Clr() OLED_RES=0
#define OLED_RES_Set() OLED_RES=1



//OLED模块函数
void OLED_Write_GRAM(u8 x,u8 y,bit value);//显示地图上的点
void OLED_Refresh(void);//更新显存到OLED
void delay_ms(unsigned int ms);//延时多少ms
void OLED_ColorTurn(u8 i);//反显函数
void OLED_DisplayTurn(u8 i);//屏幕旋转180°
void OLED_WR_Byte(u8 dat,u8 cmd);//发送字节，后面表示模式，0为命令，1为数据
void OLED_Set_Pos(u8 x, u8 y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);//清屏函数
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey);
u32 oled_pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 sizey);//★
void OLED_ShowCHinese(u8 x,u8 y,u8 no);//★
void OLED_DrawBMP(u8 x,u8 y,u8 sizex, u8 sizey,u8 BMP[]);//★
void OLED_DrawBMP1(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_Init(void);
#endif  
	 



