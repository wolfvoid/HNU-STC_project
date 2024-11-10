#include "STC15F2K60S2.h"       //芯片头文件
#include "sys.h"                //系统头文件
#include "displayer.h" 				  //数码管显示模块
#include "adc.h"								//导航按键/K3键/热敏电阻/光敏电阻
#include "Beep.h"								//蜂鸣器
#include "music.h"							//音乐
#include "Vib.h"								//震动传感器
#include "key.h"								//K1/K2按键
#include "IR.h"									//红外按键
#include "DS1302.h"							//实时时钟模块
#include "FM_Radio.h"						//收音机模块
#include "oled.h"								//OLED显示屏模块
#include "resources.h"					//
#include "math.h"								//调用ln函数


code unsigned long SysClock=11059200;         //必须。定义系统工作时钟频率(Hz)，用户必须修改成与实际工作频率（下载时选择的）一致
#ifdef _displayer_H_                          //显示模块选用时必须。（数码管显示译码表，用戶可修改、增加等） 
code char decode_table[]={
	       0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01,0x41,0x48,0x76,0x00,0x00,0x00,
 /* 序号:   0  1    2	   3    4	   5    6	   7    8	   9	  10	 11		12   13   14   15   16   17   18   19  */
 /* 显示:   0  1    2    3    4    5    6    7    8    9   (无)  下-  中-  上-  上中-  中下-                 */  
	       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80,  
 /* 序号：   20        21        22        23        24        25        26         27        28        29     */
 /* 带小数点 0         1         2         3         4         5         6          7         8         9      */
			   0x77,0x7c,0x39,0x5E,0x79,0x71,0x3D,0x76,0x30,0x1E,0x75,0x38,0x15,0x37,0x3F,0x73,0x6B,0x33,0x6D,0x31,0x3E,0x3E,0x2A,0x76,0x66,0x5B,
 /* 序号：30   31   32   33   34   35   36   37   38   39   40   41   42   43   44   45   46   47   48   49   50   51   52   53   54   55   */
 /*字母		A		 B    C    D    E    F    G    H    I    J    K    L    M    N    O    P    Q    R    S    T    U    V    W    X    Y    Z    */
				 0x37|0x80,0x79|0x80,0x5E|0x80,0x6D|0x80,0x30|0x80,0x31|0x80,0x37|0x80
 /* 序号:    56        57        58        59        60        61        62    */  
 /*字母      N.        E.        d.        s.        I.        t.        n.    */
 /*对应简写 Mon.      TuE.      Wed.     Thurs.     FrI.      Sat.      Sun.   */
};

code int mode[]={0,0,0,1,2,3,4,5,6,7};
code int next_mode[]={0,0,1,2,3,4,5,6,7,1};

//code unsigned char controller[]={0,1,1,2,1,2,1,10,1,6,1,1,1,3,1,2,1,14,1,1,1,4};
	
#endif