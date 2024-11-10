//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STC12LE5A60S2,晶振30M  单片机工作电压3.3V或5V
//QDtech-OLED液晶驱动 for C51
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtech.net
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//邮箱:QDtech2008@gmail.com 
//Skype:QDtech2008
//技术交流QQ群:324828016
//创建日期:2018/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2009-2019
//All rights reserved
/****************************************************************************************************
//=========================================电源接线================================================//
//      5V  接DC 5V电源
//     GND  接地
//======================================OLED屏数据线接线==========================================//
//本模块数据总线类型为IIC
//     SCL  接P2.3    // IIC时钟信号
//     SDA  接P2.4    // IIC数据信号
//======================================OLED屏控制线接线==========================================//
//本模块数据总线类型为IIC，不需要接控制信号线    
//=========================================触摸屏接线=========================================//
//本模块本身不带触摸，不需要接触摸屏线
//============================================================================================//
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	

#include "REG51.h"
#include "oled.h"
#include "bmp.h"

 int main(void)
 {	u8 t;
		OLED_Init();			//初始化OLED  
		OLED_Clear(0); 
	
		t=' ';
	while(1) 
	{		
		OLED_Clear(0);
		OLED_ShowCHinese(6,0,0);//全
		OLED_ShowCHinese(26,0,1);//动
		OLED_ShowCHinese(46,0,2);//电
		OLED_ShowCHinese(66,0,3);//子
		OLED_ShowCHinese(86,0,4);//技
		OLED_ShowCHinese(106,0,5);//术
		OLED_ShowString(6,3,"0.96' OLED TEST",16); 
		OLED_ShowString(0,6,"ASCII:",16);  
		OLED_ShowString(63,6,"CODE:",16);  
		OLED_ShowChar(48,6,t,16);//显示ASCII字符	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,6,t,3,16);//显示ASCII字符的码值 	
		delay_ms(5000);
		OLED_DrawBMP(0,0,128,8,BMP1);  //图片显示(图片显示慎用，生成的字表较大，会占用较多空间，FLASH空间8K以下慎用)		
		delay_ms(5000);
		OLED_Clear(0); 
		OLED_DrawBMP(0,0,128,8,BMP2);
		delay_ms(5000);
		OLED_Clear(0); 
		OLED_DrawBMP(0,0,128,8,BMP3);
		delay_ms(5000);
		OLED_Clear(0); 
		OLED_DrawBMP(0,0,128,8,BMP4);
		delay_ms(5000);
	}	  
	
}
	