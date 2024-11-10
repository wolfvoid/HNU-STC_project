#include "main.h"

//��������
//void SetBeep(char a,char b);

//�û��Զ��庯������
void Initial_interface();
void myKey_callback();
void my100mS_callback();
void myKN_callback();
void myVib_callback();
unsigned int tem_calculate(unsigned int tem_adc);
												 
//�û�����ȫ�ֱ�������
char state;//����ϵͳ��ʱ��״̬
char previous_state;//�����л�ģʽ֮ǰ��״̬
char vib_old_state;//����Vib֮ǰ��״̬
//char maxstate=10;//state������ţ��޸����֮����Ҫͬʱ��main.h���޸���������//Ϊ��ʡ�ռ䣬ֱ���ڳ������
bit key_flag = 0;
bit transfer = 0;//state�л�ʱΪ1���л���OLED�仯���Ϊ0��
//��������ΪOLED��һֱˢ�£�������LED��˸���޷�������ʾ��OLED��ʱ����ֻ������һ��

char count=0;//���ڼ����ָ�ʱ�䣬�����˸Ч��

//mode1:Clockʵʱʱ�ӳ�ʼĬ��ֵ
code struct_DS1302_RTC rtc_origin = {0x30,0x32,0x11,0x25,0x7,0x2,0x23};//  �� �� ʱ �� �� ���� ��
struct_DS1302_RTC rtc;
char clock_mode = 0;//0-ʱ����  1-������   2-����

//mode2:stopwatch���ģʽȫ�ֱ���
unsigned int time1=0,time2 = 0;  
bit stopwatch_1=0,stopwatch_2 = 0;

//mode3:timer��ʱ��
unsigned int timer = 3481;
bit timer_change = 0;
bit timer_minute = 0;
bit timer_second = 0;
bit timer_run = 0;

//mode4��light_intensity
char light_intensity_lowerlimit = 20;
char light_intensity_upperlimit = 100;
bit light_intensity_mode = 1;
bit light_intensity_change = 0;
bit light_intensity_lowerlimit_change = 0;
bit light_intensity_upperlimit_change = 0;
bit light_intensity_mode_change = 0;

//mode5��temperature
unsigned int temperature_lowerlimit = 200;
unsigned int temperature_upperlimit = 400;
bit temperature_mode = 1;
bit temperature_change = 0;
bit temperature_lowerlimit_change = 0;
bit temperature_upperlimit_change = 0;
bit temperature_mode_change = 0;

//mode6:FM������ȫ�ֱ���
code struct_FMRadio radio_origin = {968,3,0,1,1};//Ƶ��*10 ���� �� �� �� (0��1��)
//107.0�㽭�㲥 96.8�㽭���ֹ㲥
bit FM_frequency = 0; //�Ƿ��ڵ���Ƶ��
bit FM_volume = 0;    //�Ƿ��ڵ�������
bit FM_mutex = 0; //���ڵ����������ʵ���

//mode7:Mp3

//mode8��RemoteControl

//������
void main() 
{ 
	//����ģ��
	//Initialization
	KeyInit();
	BeepInit();
	OLED_Init();//����OLEDģ��
	AdcInit(ADCexpEXT);//����ADCģ���Դ���EXT�ڵĿ��ƣ��Ըò��������Խ�����ƣ��ͷ�EXT�ڸ�OLED
	DisplayerInit();
	DS1302Init(rtc_origin);
	FMRadioInit(radio_origin);//FM������ģ��
	MusicPlayerInit();
	IrInit(NEC_R05d);
	VibInit();
	

	//�����¼��ص�����
	SetEventCallBack(enumEventKey, myKey_callback);
	SetEventCallBack(enumEventSys100mS, my100mS_callback);
	SetEventCallBack(enumEventNav, myKN_callback);//���𵼺�����K3������Ļص�
	SetEventCallBack(enumEventVib, myVib_callback);
	
	//�û�����״̬��ʼ��
	
	
	//�û����������ʼ��
	state = 0;	
	previous_state = 0;
	vib_old_state = 0;
	Initial_interface();
	SetDisplayerArea(0,7);
	//RTC_Write(rtc_origin);  //�޸�ϵͳʱ
	
	//OS��ѭ��
	MySTC_Init();
	while(1)             	
		{
			MySTC_OS();
			//�û���ѭ������
		}
}                 

//��ʼ����
void Initial_interface()
{
		OLED_DrawBMP(0, 0, 128, 8, BMP0);
		OLED_ShowString(0, 6, "HUNAN UNIVERSITY", 16);
		state=2;
}

//����������K3�����Ļص�����
void myKN_callback()
{
	char K3=GetAdcNavAct(enumAdcNavKey3);
	char up=GetAdcNavAct(enumAdcNavKeyUp);
	char down=GetAdcNavAct(enumAdcNavKeyDown);
	char left=GetAdcNavAct(enumAdcNavKeyLeft);
	char right=GetAdcNavAct(enumAdcNavKeyRight);
	
	//mode2: stopwatch1  ���ģʽ1
	if (state == 4)
	{
		if (K3 == enumKeyPress) 
     { 
			 stopwatch_1 =~stopwatch_1; 
       if (stopwatch_1) time1=0; 
       SetBeep(1000,20);
     }
	}
	
	//mode3: timer-nav  ��ʱ��ģʽ������������
	if (state == 5 && timer_change == 1)
	{
		if (up == enumKeyPress)
		{
			if (timer_minute == 1) timer += 600;
			else if (timer_second == 1) timer += 10;
			SetBeep(1000,20);
		}
		else if (down == enumKeyPress)
		{
			if (timer_minute == 1) if (timer-600 > 0) timer -= 600; else timer = 0;
			else if (timer_second == 1) if (timer-10 > 0) timer -= 10; else timer = 0;
			SetBeep(1000,20);
		}
		if (left == enumKeyPress)
		{
			if (timer_second == 1) {timer_second = 0; timer_minute = 1;}
			SetBeep(1000,20);
		}
		if (right == enumKeyPress)
		{
			if (timer_minute == 1) {timer_minute = 0; timer_second = 1;}
			SetBeep(1000,20);
		}
	}
	
	//mode3: timer ��ʱ��ģʽ(����״̬���ʱ״̬ת��)
	if (state == 5 && timer_change == 0)
	{
		if (K3  == enumKeyPress)
		{
			timer_run = ~timer_run;
		}
	}
	
	//mode4: light_intensity_change
	if (state == 6 && light_intensity_change == 1)
	{
		if (up == enumKeyPress)
		{
			if (light_intensity_lowerlimit_change == 1) //��������
			{
				if (light_intensity_lowerlimit+1 < light_intensity_upperlimit) light_intensity_lowerlimit++;
			}
			else if (light_intensity_upperlimit_change == 1) //��������
			{
				if (light_intensity_upperlimit+1 < 255) light_intensity_upperlimit++;
			}
			else if (light_intensity_mode_change == 1) //�޸�ģʽ
			{
				light_intensity_mode = ~light_intensity_mode;
			}
			SetBeep(1000,20);
		}
		else if (down == enumKeyPress)
		{
			if (light_intensity_lowerlimit_change == 1) //���޼�С
			{
				if (light_intensity_lowerlimit-1 > 0) light_intensity_lowerlimit--;
			}
			else if (light_intensity_upperlimit_change == 1) //���޼�С
			{
				if (light_intensity_upperlimit-1 > light_intensity_lowerlimit) light_intensity_upperlimit--;
			}
			else if (light_intensity_mode_change == 1) //�޸�ģʽ
			{
				light_intensity_mode = ~light_intensity_mode;
			}
			SetBeep(1000,20);
		}
		if (left == enumKeyPress)
		{
			if (light_intensity_lowerlimit_change == 1) 
			{
				light_intensity_lowerlimit_change = 0;
				light_intensity_mode_change = 1;
			}
			if (light_intensity_upperlimit_change == 1) 
			{
				light_intensity_upperlimit_change = 0;
				light_intensity_lowerlimit_change = 1;
			}
			SetBeep(1000,20);
		}
		if (right == enumKeyPress)
		{
			if (light_intensity_lowerlimit_change == 1) 
			{
				light_intensity_lowerlimit_change = 0;
				light_intensity_upperlimit_change = 1;
			}
			if (light_intensity_mode_change == 1) 
			{
				light_intensity_mode_change = 0;
				light_intensity_lowerlimit_change = 1;
			}
			SetBeep(1000,20);
		}
	}
	
	//mode5: temperature_change
	if (state == 7 && temperature_change == 1)
	{
		if (up == enumKeyPress)
		{
			if (temperature_lowerlimit_change == 1) //��������
			{
				if (temperature_lowerlimit+1 < temperature_upperlimit) temperature_lowerlimit++;
			}
			else if (temperature_upperlimit_change == 1) //��������
			{
				if (temperature_upperlimit+1 < 999) temperature_upperlimit++;
			}
			else if (temperature_mode_change == 1) //�޸�ģʽ
			{
				temperature_mode = ~temperature_mode;
			}
			SetBeep(1000,20);
		}
		else if (down == enumKeyPress)
		{
			if (temperature_lowerlimit_change == 1) //���޼�С
			{
				if (temperature_lowerlimit-1 > 0) temperature_lowerlimit--;
			}
			else if (temperature_upperlimit_change == 1) //���޼�С
			{
				if (temperature_upperlimit-1 > temperature_lowerlimit) temperature_upperlimit--;
			}
			else if (temperature_mode_change == 1) //�޸�ģʽ
			{
				temperature_mode = ~temperature_mode;
			}
			SetBeep(1000,20);
		}
		if (left == enumKeyPress)
		{
			if (temperature_lowerlimit_change == 1) 
			{
				temperature_lowerlimit_change = 0;
				temperature_mode_change = 1;
			}
			if (temperature_upperlimit_change == 1) 
			{
				temperature_upperlimit_change = 0;
				temperature_lowerlimit_change = 1;
			}
			SetBeep(1000,20);
		}
		if (right == enumKeyPress)
		{
			if (temperature_lowerlimit_change == 1) 
			{
				temperature_lowerlimit_change = 0;
				temperature_upperlimit_change = 1;
			}
			if (temperature_mode_change == 1) 
			{
				temperature_mode_change = 0;
				temperature_lowerlimit_change = 1;
			}
			SetBeep(1000,20);
		}
	}
	
	//mode6: FM������ģʽ(K3)
	if (state == 8)
	{
		if (K3 == enumKeyPress && FM_mutex == 0)//δѡ��״̬����ѡ��״̬
		{
			FM_mutex = 1;//��ȡ��
			FM_volume = 1;
			SetBeep(1500,20);
		}
		else if (K3 == enumKeyPress && FM_mutex == 1 && FM_volume == 1)//ѡ��״̬�ָ�δѡ��״̬
		{
			FM_mutex = 0;//�ͷ���
			FM_volume = 0;
			SetBeep(1500,20);
		}
	}
	
	
	//mode6: FM������ģʽ������������
	if (state == 8)
	{
		struct_FMRadio radio;
		radio.frequency = GetFMRadio().frequency;
		radio.volume = GetFMRadio().volume;
		radio.GP1 = GetFMRadio().GP1;
		radio.GP2 = GetFMRadio().GP2;
		radio.GP3 = GetFMRadio().GP3;
		if (up == enumKeyPress)
		{
			if (FM_frequency == 1) radio.frequency += 1;
			else if (FM_volume == 1) radio.volume += 1;
			SetBeep(1000,20);
		}
		else if (down == enumKeyPress)
		{
			if (FM_frequency == 1) radio.frequency -= 1;
			else if (FM_volume == 1) radio.volume -= 1;
			SetBeep(1000,20);
		}
		SetFMRadio(radio);
	}
	
}


//Key ���������ص�����
void myKey_callback()
{ 
	char K1=GetKeyAct(enumKey1);
	char K2=GetKeyAct(enumKey2);
	//char K3=GetKeyAct(enumKey3);
	//char K3=GetAdcNavAct(enumAdcNavKey3);
	//�ر�ע�⣺
	//���ڵ���������K3�������˵�Ƭ��ͬһ���˿ڣ�P1.7����
	//����ADCģ���P1.7��IO����ʧЧ��
	//ֻ����GetAdcNavAct(char Nav_button)������ȡK3�������¼���״̬��
	
	//�л�ģʽ
	if (state != 0)//�ų�Vibģʽ
	{
		if (key_flag == 0 && K1 == enumKeyPress) 
		{	
			previous_state=state;//����ԭ״̬
			state=1;
			key_flag=1;
		}
		else if (key_flag == 1 && K1 == enumKeyPress)
		{
			if (previous_state < 10) 
			{
				SetBeep(600,10);
				state=previous_state+1;//�л�����һ��ģʽ
				transfer=1;
			}
			else 
			{
				SetBeep(600,10);
				state=3;//ģʽ����ͷ���л��ؿ�ͷ
				transfer=1;
			}
			key_flag=0;
		}
	}
	else if (state == 0)
	{
		if (K1 == enumKeyPress)
		{
			state = vib_old_state;
		}
	}
	
	//mode1: ʵʱʱ��ģʽ-�л��鿴 ʱ����/������/����
	if (state == 3)
	{
		if (K2 == enumKeyPress)
		{
			if (clock_mode<2) clock_mode++;
			else clock_mode = 0;
		}
	}

	//mode2: ���ģʽ2
	if (state == 4)
	{
		if (K2 == enumKeyPress)
    { 
			stopwatch_2 =~stopwatch_2; 
      if (stopwatch_2) time2=0; 
      SetBeep(1500,20);
    }
	}
	
	//mode3: ��ʱ��ģʽ(normal state <=> change state)
	if (state == 5 && timer_run == 0)
	{
		if (K2 == enumKeyPress)
		{
			if (timer_change == 0) timer_second = 1;
			timer_change = ~timer_change;
		}
	}
	
	//mode4: ���ģʽ(normal state <=> change state)
	if (state == 6)
	{
		if (K2 == enumKeyPress)
		{
			light_intensity_change = ~light_intensity_change;
			light_intensity_lowerlimit_change = 1;
		}
	}
	
	//mode5: �¸�ģʽ(normal state <=> change state)
	if (state == 7)
	{
		if (K2 == enumKeyPress)
		{
			temperature_change = ~temperature_change;
			temperature_lowerlimit_change = 1;
		}
	}
	
	//mode6: FM������ģʽ(K2)
	if (state == 8)
	{
		if (K2 == enumKeyPress && FM_mutex == 0)//δѡ��״̬����ѡ��״̬
		{
			FM_mutex = 1;//��ȡ��
			FM_frequency = 1;
			SetBeep(1500,20);
		}
		else if (K2 == enumKeyPress && FM_mutex == 1 && FM_frequency == 1)//ѡ��״̬�ָ�δѡ��״̬
		{
			FM_mutex = 0;//�ͷ���
			FM_frequency = 0;
			SetBeep(1500,20);
		}
	}
	
	//mode7: MP3������
	if (state == 9)
	{
		if (K2 == enumKeyPress && GetPlayerMode() == enumModeStop)//ֹͣ״̬�����ΰ���K2��ʼ
		{
			SetMusic(90,0xFA,&song_TruE,sizeof(song_TruE),enumMscNull);
			SetPlayerMode(enumModePlay);
		}
		else if (K2 == enumKeyPress && GetPlayerMode() == enumModePlay)//���ڲ��ţ�����K2��ͣ
		{
			SetPlayerMode(enumModePause);
		}
		else if (K2 == enumKeyPress && GetPlayerMode() == enumModePause)//��ͣ״̬������K2����
		{
			SetPlayerMode(enumModePlay);
		}
	}
	
	//mode7(end): �뿪���ֲ���ģʽ���Զ�ֹͣ����
	if (state != 9 && (GetPlayerMode() == enumModePause || GetPlayerMode() == enumModePlay))
		SetPlayerMode(enumModeStop);
	
	//mode8: ң��ģʽ
	if (state == 10)
	{
		if (K2 == enumKeyPress)
		{
			if(IrTxdSet(&controller,22) == enumIrTxOK) SetBeep(1500,20);
		}
	}
}




//��ʱ�ص�����(������ʾ�����)
void my100mS_callback()
{ 
	
	//���ģʽ�������ģʽ֮��Ҳ��ʱ
	if(stopwatch_1 ==1)  time1++;
	if(stopwatch_2 ==1)  time2++;
	
	//��ʱ��ģʽ���ڶ�ʱ��֮��Ҳ����
	if (timer_run == 1 ) timer--;
	if (timer == 0 )
	{
		timer_run = 0;
		SetBeep(1400,200);
		timer = 3000;
		
	}

	switch(state)
	{
		
		
		//Vib����ģʽ����
		case 0:
		{
			Seg7Print(30,13,1,11,11,1,13,30);
			break;
		}
		
		
		//�л�ģʽ����
		case 1:
		{
			if (transfer == 1)//stateת�ƣ���Ҫ�ı���Ļ
			{
				OLED_ShowString(0, 6, "Next Mode��", 16);
				transfer=0;
			}
			Seg7Print(32,37,44,44,48,34,12,next_mode[previous_state]);
			LedPrint(0);
			break;
		}
		
		
		//��ʼ��ӭ����:HNU-CSEE+��ˮ��
		case 2:
		{
			unsigned char a;
			Seg7Print(37,43,50,12,32,48,34,34);
		
			if( a != 0) a=a<<1;
				else a=0x01;
			LedPrint(a);
			break;
		}
		
		
		//Mode1:ʵʱʱ��
		//��ʼ��ʾʱ�䣬ʹ��K2�������л���ʾ ʱ����/������/����(Ӣ��+����)
		case 3:
		{
			rtc = RTC_Read();
			switch(clock_mode)
			{
				case 0:
				{
					Seg7Print(rtc.hour/16,rtc.hour%16,12,rtc.minute/16,rtc.minute%16,12,rtc.second/16,rtc.second%16);
					break;
				}
				case 1:
				{
					Seg7Print(rtc.year/16,rtc.year%16,12,rtc.month/16,rtc.month%16,12,rtc.day/16,rtc.day%16);
					break;
				}
				case 2:
				{
					unsigned char weekday = rtc.week;
					switch(weekday)
					{
						case 1:{Seg7Print(43,43,44,56,10,10,12,1); break;}
						case 2:{Seg7Print(49,50,57,10,10,10,12,2); break;}
						case 3:{Seg7Print(50,50,34,58,10,10,12,3); break;}
						case 4:{Seg7Print(49,37,50,47,59,10,12,4); break;}
						case 5:{Seg7Print(35,47,60,10,10,10,12,4); break;}
						case 6:{Seg7Print(48,30,61,10,10,10,12,4); break;}
						case 7:{Seg7Print(48,50,62,10,10,10,12,4); break;}
					}
					break;
				}
			}
			
			break;
		}
		
		
		//Mode2:˫ͨ�����
		//ʹ��K3/K2���ֱ�������������л�ģʽ��Ӱ���ʱ�����ں�̨����
		case 4:
		{
			unsigned char d0,d1,d2,d3,d4,d5,d6,d7;	
	
			d7 = (time1/1000)%10;	if (d7 == 0) d7 = 10;
			d6 = (time1/100)%10;	if (d6 == 0) d6 = 10;
			d5 = (time1/10)%10+20;//����Ҫ��ʾС����
			d4 = (time1%10); 	
			
			d3 = (time2/1000)%10;	if (d3 == 0) d3 = 10;
			d2 = (time2/100)%10;	if (d2 == 0) d2 = 10;
			d1 = (time2/10)%10+20;//����Ҫ��ʾС����
			d0 = (time2%10);
			
			Seg7Print(d7,d6,d5,d4,d3,d2,d1,d0);
			
			break;
		}
		
		
		//Mode3:��ʱ��
		case 5:
		{
			count++;
			if (count<=3)//�ü����ָ�ʱ�䣬�����˸Ч��
			{
				Seg7Print(10,10,timer/10/60/10,timer/10/60%10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
			}
			else
			{
				if (timer_change == 0)//����״̬ �� ����ģʽ
				{
					if (timer_run == 0) Seg7Print(10,10,timer/10/60/10,timer/10/60%10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
					else if (timer_run == 1) Seg7Print(45,10,timer/10/60/10,timer/10/60%10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
				}
				else if (timer_minute == 1)//���ڷ��ӣ�������˸
				{
					Seg7Print(32,10,10,10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
				}
				else if (timer_second == 1)//�������ӣ�������˸
				{
					Seg7Print(32,10,timer/10/60/10,timer/10/60%10,12,10,10,10);
				}
			}
			if (count > 5) count=0;
			break;
		}
		
		
		//Mode4:���ռ���뱨��
		case 6:
		{
			unsigned int num = GetADC().Rop;	
			
			if (light_intensity_change == 0)//����ģʽ
			{
				Seg7Print(34,4,12,10,10,num/100,num/10%10,num%10);
				if (num < light_intensity_lowerlimit)
				{
					SetBeep(1200,200);
				}
				else if (num > light_intensity_upperlimit)
				{
					SetBeep(2000,200);
				}
			}
			else if (light_intensity_change == 1)//changeģʽ
			{
				count++;
	
				if (count<=3)//�ü����ָ�ʱ�䣬�����˸Ч��
				{
					Seg7Print(light_intensity_mode,
										light_intensity_lowerlimit/100%10,
										light_intensity_lowerlimit/10%10,
										light_intensity_lowerlimit%10,
										12,
										light_intensity_upperlimit/100%10,
										light_intensity_upperlimit/10%10,
										light_intensity_upperlimit%10);
				}
				else
				{
					if (light_intensity_lowerlimit_change == 1) //���޸Ķ�
					{
						Seg7Print(light_intensity_mode,
										10,10,10,
										12,
										light_intensity_upperlimit/100%10,
										light_intensity_upperlimit/10%10,
										light_intensity_upperlimit%10);
					}
					else if (light_intensity_upperlimit_change == 1) //���޸Ķ�
					{
						Seg7Print(light_intensity_mode,
										light_intensity_lowerlimit/100%10,
										light_intensity_lowerlimit/10%10,
										light_intensity_lowerlimit%10,
										12,
										10,10,10);
					}
					else if (light_intensity_mode_change == 1) //ģʽ�Ķ�
					{
						Seg7Print(10,
										light_intensity_lowerlimit/100%10,
										light_intensity_lowerlimit/10%10,
										light_intensity_lowerlimit%10,
										12,
										light_intensity_upperlimit/100%10,
										light_intensity_upperlimit/10%10,
										light_intensity_upperlimit%10);
					}
				}
				if (count > 5) count=0;
			}
			
			break;
		}
		
		
		//Mode5:�¶ȼ���뱨��
		case 7:
		{
			unsigned int tem_adc = GetADC().Rt;
			unsigned int tem = tem_calculate(tem_adc);
			
			if (temperature_change == 0)//����ģʽ
			{
				Seg7Print(34,5,12,10,10,tem/100%10,20+tem/10%10,tem%10);
				//�ڶ�λ��ʾС����
				if (tem/10 < 20)
				{
					SetBeep(1200,200);
				}
				else if (tem/10 > 40)
				{
					SetBeep(2000,200);
				}
			}
			else if (temperature_change == 1)//changeģʽ
			{
				count++;
	
				if (count<=3)//�ü����ָ�ʱ�䣬�����˸Ч��
				{
					Seg7Print(temperature_mode,
										temperature_lowerlimit/100%10,
										20+temperature_lowerlimit/10%10,
										temperature_lowerlimit%10,
										12,
										temperature_upperlimit/100%10,
										20+temperature_upperlimit/10%10,
										temperature_upperlimit%10);
				}
				else
				{
					if (temperature_lowerlimit_change == 1) //���޸Ķ�
					{
						Seg7Print(temperature_mode,
										10,10,10,
										12,
										temperature_upperlimit/100%10,
										20+temperature_upperlimit/10%10,
										temperature_upperlimit%10);
					}
					else if (temperature_upperlimit_change == 1) //���޸Ķ�
					{
						Seg7Print(temperature_mode,
										temperature_lowerlimit/100%10,
										20+temperature_lowerlimit/10%10,
										temperature_lowerlimit%10,
										12,
										10,10,10);
					}
					else if (temperature_mode_change == 1) //ģʽ�Ķ�
					{
						Seg7Print(10,
										temperature_lowerlimit/100%10,
										20+temperature_lowerlimit/10%10,
										temperature_lowerlimit%10,
										12,
										temperature_upperlimit/100%10,
										20+temperature_upperlimit/10%10,
										temperature_upperlimit%10);
					}
				}
				if (count > 5) count=0;
			}
			
				break;
		}
		
		
		//Mode6:FM������
		//��K1��K2����ѡ��״̬���ֱ����������Ƶ�ʣ������µ���������ֵ��С
		case 8:
		{
			unsigned int frequency = GetFMRadio().frequency;
			unsigned char volume = GetFMRadio().volume;
			unsigned char fre0,fre1,fre2,vol0,vol1;
			fre0 = frequency%10;
			fre1 = frequency/10%10+20;
			fre2 = frequency/100%10; if (fre2 == 0) fre2 = 10;
			vol0 = volume%10;
			vol1 = volume/10%10;
			count++;
			if (count<=3)//�ü����ָ�ʱ�䣬�����˸Ч��
			{
				Seg7Print(50,vol1,vol0,10,35,fre2,fre1,fre0);
			}
			else
			{
				if (FM_frequency == 0 && FM_volume == 0)//�������
				{
					Seg7Print(50,vol1,vol0,10,35,fre2,fre1,fre0);
				}
				else if (FM_frequency == 1)//����Ƶ�ʣ�Ƶ����˸
				{
					Seg7Print(50,vol1,vol0,10,35,10,10,10);
				}
				else if (FM_volume == 1)//����������������˸
				{
					Seg7Print(50,10,10,10,35,fre2,fre1,fre0);
				}
			}
			if (count > 5) count=0;
			break;
		}
		
		
		//Mode7:MP3
		case 9:
		{
			if (GetPlayerMode() == enumModeStop) Seg7Print(34,7,12,10,10,10,10,10);
			if (GetPlayerMode() == enumModePause) Seg7Print(45,30,50,48,34,10,10,10);
			break;
		}
		
		
		//Mode8:���������յ�ң��
		case 10:
		{
			Seg7Print(12,12,43,44,12,8,12,12);
			break;
		}
		
		
		default:
		{
			//do nothing
			break;
		}
		
	};
	
}

//���¼��ص�����
void myVib_callback()
{
	if (state != 0) vib_old_state = state;//���ж�������ֹ����ײ������state��Ϊ0����������ȥ
	state = 0;
	SetBeep(600,10);
}

//�¶�ת����������
unsigned int tem_calculate(unsigned int tem_adc)
{
	float vccx=tem_adc/1000.0;
	float lnx=log(vccx/(1-vccx));
	float tem=1/((lnx/3950)+(1/298.15))-273.15;
	float tem10=tem*10;
	unsigned int ans=(int)tem10;
	return ans;
}
