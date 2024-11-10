#include "STC15F2K60S2.h"       //оƬͷ�ļ�
#include "sys.h"                //ϵͳͷ�ļ�
#include "displayer.h" 				  //�������ʾģ��
#include "adc.h"								//��������/K3��/��������/��������
#include "Beep.h"								//������
#include "music.h"							//����
#include "Vib.h"								//�𶯴�����
#include "key.h"								//K1/K2����
#include "IR.h"									//���ⰴ��
#include "DS1302.h"							//ʵʱʱ��ģ��
#include "FM_Radio.h"						//������ģ��
#include "oled.h"								//OLED��ʾ��ģ��
#include "resources.h"					//
#include "math.h"								//����ln����


code unsigned long SysClock=11059200;         //���롣����ϵͳ����ʱ��Ƶ��(Hz)���û������޸ĳ���ʵ�ʹ���Ƶ�ʣ�����ʱѡ��ģ�һ��
#ifdef _displayer_H_                          //��ʾģ��ѡ��ʱ���롣���������ʾ������Ñ����޸ġ����ӵȣ� 
code char decode_table[]={
	       0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01,0x41,0x48,0x76,0x00,0x00,0x00,
 /* ���:   0  1    2	   3    4	   5    6	   7    8	   9	  10	 11		12   13   14   15   16   17   18   19  */
 /* ��ʾ:   0  1    2    3    4    5    6    7    8    9   (��)  ��-  ��-  ��-  ����-  ����-                 */  
	       0x3f|0x80,0x06|0x80,0x5b|0x80,0x4f|0x80,0x66|0x80,0x6d|0x80,0x7d|0x80,0x07|0x80,0x7f|0x80,0x6f|0x80,  
 /* ��ţ�   20        21        22        23        24        25        26         27        28        29     */
 /* ��С���� 0         1         2         3         4         5         6          7         8         9      */
			   0x77,0x7c,0x39,0x5E,0x79,0x71,0x3D,0x76,0x30,0x1E,0x75,0x38,0x15,0x37,0x3F,0x73,0x6B,0x33,0x6D,0x31,0x3E,0x3E,0x2A,0x76,0x66,0x5B,
 /* ��ţ�30   31   32   33   34   35   36   37   38   39   40   41   42   43   44   45   46   47   48   49   50   51   52   53   54   55   */
 /*��ĸ		A		 B    C    D    E    F    G    H    I    J    K    L    M    N    O    P    Q    R    S    T    U    V    W    X    Y    Z    */
				 0x37|0x80,0x79|0x80,0x5E|0x80,0x6D|0x80,0x30|0x80,0x31|0x80,0x37|0x80
 /* ���:    56        57        58        59        60        61        62    */  
 /*��ĸ      N.        E.        d.        s.        I.        t.        n.    */
 /*��Ӧ��д Mon.      TuE.      Wed.     Thurs.     FrI.      Sat.      Sun.   */
};

code int mode[]={0,0,0,1,2,3,4,5,6,7};
code int next_mode[]={0,0,1,2,3,4,5,6,7,1};

//code unsigned char controller[]={0,1,1,2,1,2,1,10,1,6,1,1,1,3,1,2,1,14,1,1,1,4};
	
#endif