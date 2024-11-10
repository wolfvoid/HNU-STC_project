#include "main.h"

//静音测试：用于在图书馆等安静的地方做测试
//方法：注释掉下面的BeepInit();并解除该句void SetBeep(char a,char b);的注释
//void SetBeep(char a,char b);

//用户自定义函数声明
void Initial_interface();//初始界面
void myKey_callback();//Key按键触发回调函数
void my100mS_callback();//定时回调函数（用于显示数码管）
void myKN_callback();//导航按键＆K3按键的回调函数
void myVib_callback();//震动事件回调函数
unsigned int tem_calculate(unsigned int tem_adc);//温度转换辅助函数
void oled_display();//OLED显示屏切换
void uart_callback();//通信接收回调函数
void pc_preview();//计算机展示

//用户程序全局变量定义
char state;//操作系统此时的状态
char previous_state;//陷入切换模式之前的状态
char vib_old_state;//陷入Vib之前的状态
//char maxstate=10;//state的最大编号，修改这个之后需要同时在main.h中修改两个数组//为节省空间，直接在程序里改
bit key_flag = 0;
bit transfer = 0;//state切换时为1，切换后OLED变化后归为0。
//这里是因为OLED若一直刷新，将导致LED闪烁，无法正常显示，OLED短时间内只能设置一次
bit oled_change = 0;

char count=0;//用于计数分割时间，达成闪烁效果

//mode1:Clock实时时钟初始默认值
code struct_DS1302_RTC rtc_origin = {0x0,0x16,0x17,0x7,0x9,0x4,0x23};//  秒 分 时 日 月 星期 年
struct_DS1302_RTC rtc;
char clock_mode = 0;//0-时分秒  1-年月日   2-星期

//mode2:stopwatch秒表模式全局变量
unsigned int time1=0,time2 = 0;//累积量：两个秒表分别计时的值。
bit stopwatch_1=0,stopwatch_2 = 0;//信号量：两个秒表的状态，是否开始计时。该信号量为1时，定时回调函数被调用时会累加时间。

//mode3:timer定时器
int timer = 3481;//注意这里为什么不能用unsigned int
//初始定时时间（默认值在这里设置）
bit timer_change = 0;//当前是否处于change态，初始不在change态
bit timer_minute = 0;//当前是否在change态的调节“分”
bit timer_second = 0;//当前是否在change态的调节“秒”
bit timer_run = 0;//当前是否处于“计时态”

//mode4：light_intensity
char light_intensity_lowerlimit = 20;//光照报警值下阈值
char light_intensity_upperlimit = 100;//光照报警值上阈值
bit light_intensity_mode = 1;//光照报警模式（是否报警，默认报警）
bit light_intensity_change = 0;//当前是否处于change态
bit light_intensity_lowerlimit_change = 0;//当前是否处于change态的调节“下阈值”
bit light_intensity_upperlimit_change = 0;//当前是否处于change态的调节“上阈值”
bit light_intensity_mode_change = 0;//当前是否处于change态的调节“报警模式”

//mode5：temperature
unsigned int temperature_lowerlimit = 200;//温度报警值下阈值
unsigned int temperature_upperlimit = 400;//温度报警值上阈值
bit temperature_mode = 1;//温感报警模式（是否报警，默认报警）
bit temperature_change = 0;//当前是否处于change态
bit temperature_lowerlimit_change = 0;//当前是否处于change态的调节“下阈值”
bit temperature_upperlimit_change = 0;//当前是否处于change态的调节“上阈值”
bit temperature_mode_change = 0;//当前是否处于change态的调节“报警模式”

//mode6:FM收音机全局变量
code struct_FMRadio radio_origin = {886,3,0,1,1};//频率*10 音量 灯 灯 灯 (0亮1灭)
//107.0浙江广播 96.8浙江音乐广播
//88.6长沙音乐广播 89.3湖南音乐之声
bit FM_frequency = 0; //是否在调节频率
bit FM_volume = 0;    //是否在调节音量
bit FM_mutex = 0; //对于导航按键访问的锁

//mode7:Mp3

//mode8：RemoteControl

//与计算机通信
code char datahead[]={0xaa,0x55};//包头
char datanow[];//接收数据保存地址
code char back[]={0xaa,0x55,0xa1,0xa1,0xa1,0xa1};//初次通讯返回值

//主函数
void main()
{
	//加载模块
	//Initialization
	KeyInit();
	BeepInit();//静音测试时注释此句
	OLED_Init();//启动OLED模块
	AdcInit(ADCexpEXT);//由于ADC模块自带对EXT口的控制，以该参数启动以解除控制，释放EXT口给OLED
	DisplayerInit();
	DS1302Init(rtc_origin);
	FMRadioInit(radio_origin);//FM收音机模块
	MusicPlayerInit();
	//IrInit(NEC_R05d);
	VibInit();
	Uart1Init(1200);
	SetUart1Rxd(datanow,6,datahead,0);


	//设置事件回调函数
	SetEventCallBack(enumEventKey, myKey_callback);
	SetEventCallBack(enumEventSys100mS, my100mS_callback);
	SetEventCallBack(enumEventNav, myKN_callback);//负责导航键和K3键★★★的回调
	SetEventCallBack(enumEventVib, myVib_callback);
	SetEventCallBack(enumEventUart1Rxd,uart_callback);

	//用户程序状态初始化


	//用户程序变量初始化
	state = 0;
	previous_state = 0;
	vib_old_state = 0;
	Initial_interface();
	SetDisplayerArea(0,7);
	//RTC_Write(rtc_origin);  //修改系统时

	//OS主循环
	MySTC_Init();
	while(1)
		{
			MySTC_OS();
			//用户主循环程序
		}
}

//初始界面
void Initial_interface()
{
		OLED_DrawBMP(0, 0, 128, 8, BMP0);
		OLED_ShowString(0, 6, "HUNAN UNIVERSITY", 16);
		state=2;
}

//导航按键和K3按键的回调函数
void myKN_callback()
{
	char K3=GetAdcNavAct(enumAdcNavKey3);
	char up=GetAdcNavAct(enumAdcNavKeyUp);
	char down=GetAdcNavAct(enumAdcNavKeyDown);
	char left=GetAdcNavAct(enumAdcNavKeyLeft);
	char right=GetAdcNavAct(enumAdcNavKeyRight);

	//mode2: stopwatch1  秒表模式1
	if (state == 4)
	{
		if (K3 == enumKeyPress)
     {
			 stopwatch_1 =~stopwatch_1;
       if (stopwatch_1) time1=0;
       SetBeep(1000,20);
     }
	}

	//mode3: timer-nav  定时器模式导航按键调节
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

	//mode3: timer 定时器模式(正常状态与计时状态转换)
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
			if (light_intensity_lowerlimit_change == 1) //下限增大
			{
				if (light_intensity_lowerlimit+1 < light_intensity_upperlimit) light_intensity_lowerlimit++;
			}
			else if (light_intensity_upperlimit_change == 1) //上限增大
			{
				if (light_intensity_upperlimit+1 < 255) light_intensity_upperlimit++;
			}
			else if (light_intensity_mode_change == 1) //修改模式
			{
				light_intensity_mode = ~light_intensity_mode;
			}
			SetBeep(1000,20);
		}
		else if (down == enumKeyPress)
		{
			if (light_intensity_lowerlimit_change == 1) //下限减小
			{
				if (light_intensity_lowerlimit-1 > 0) light_intensity_lowerlimit--;
			}
			else if (light_intensity_upperlimit_change == 1) //上限减小
			{
				if (light_intensity_upperlimit-1 > light_intensity_lowerlimit) light_intensity_upperlimit--;
			}
			else if (light_intensity_mode_change == 1) //修改模式
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
			if (temperature_lowerlimit_change == 1) //下限增大
			{
				if (temperature_lowerlimit+1 < temperature_upperlimit) temperature_lowerlimit++;
			}
			else if (temperature_upperlimit_change == 1) //上限增大
			{
				if (temperature_upperlimit+1 < 999) temperature_upperlimit++;
			}
			else if (temperature_mode_change == 1) //修改模式
			{
				temperature_mode = ~temperature_mode;
			}
			SetBeep(1000,20);
		}
		else if (down == enumKeyPress)
		{
			if (temperature_lowerlimit_change == 1) //下限减小
			{
				if (temperature_lowerlimit-1 > 0) temperature_lowerlimit--;
			}
			else if (temperature_upperlimit_change == 1) //上限减小
			{
				if (temperature_upperlimit-1 > temperature_lowerlimit) temperature_upperlimit--;
			}
			else if (temperature_mode_change == 1) //修改模式
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

	//mode6: FM收音机模式(K3)
	if (state == 8)
	{
		if (K3 == enumKeyPress && FM_mutex == 0)//未选中状态进入选中状态
		{
			FM_mutex = 1;//获取锁
			FM_volume = 1;
			SetBeep(1500,20);
		}
		else if (K3 == enumKeyPress && FM_mutex == 1 && FM_volume == 1)//选中状态恢复未选中状态
		{
			FM_mutex = 0;//释放锁
			FM_volume = 0;
			SetBeep(1500,20);
		}
	}


	//mode6: FM收音机模式导航按键调节
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


//Key 按键触发回调函数
void myKey_callback()
{
	char K1=GetKeyAct(enumKey1);
	char K2=GetKeyAct(enumKey2);
	//char K3=GetKeyAct(enumKey3);
	//char K3=GetAdcNavAct(enumAdcNavKey3);
	//特别注意：
	//由于导航按键与K3键共用了单片机同一个端口（P1.7），
	//启用ADC模块后，P1.7口IO功能失效，
	//只能用GetAdcNavAct(char Nav_button)函数获取K3按键的事件或状态。

	if (K1 == enumKeyPress)
	{
		oled_change = 1;
	}

	//切换模式
	if (state != 0)//排除Vib模式
	{
		if (key_flag == 0 && K1 == enumKeyPress)
		{
			previous_state=state;//保存原状态
			state=1;
			key_flag=1;
		}
		else if (key_flag == 1 && K1 == enumKeyPress)
		{
			if (previous_state < 9)
			{
				SetBeep(600,10);
				state=previous_state+1;//切换到下一个模式
				transfer=1;
			}
			else
			{
				SetBeep(600,10);
				state=3;//模式到尽头，切换回开头
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

	//mode1: 实时时钟模式-切换查看 时分秒/年月日/星期
	if (state == 3)
	{
		if (K2 == enumKeyPress)
		{
			if (clock_mode<2) clock_mode++;
			else clock_mode = 0;
		}
	}

	//mode2: 秒表模式2
	if (state == 4)
	{
		if (K2 == enumKeyPress)
    {
			stopwatch_2 =~stopwatch_2;
      if (stopwatch_2) time2=0;
      SetBeep(1500,20);
    }
	}

	//mode3: 定时器模式(normal state <=> change state)
	if (state == 5 && timer_run == 0)
	{
		if (K2 == enumKeyPress)
		{
			if (timer_change == 0) timer_second = 1;
			timer_change = ~timer_change;
		}
	}

	//mode4: 光感模式(normal state <=> change state)
	if (state == 6)
	{
		if (K2 == enumKeyPress)
		{
			light_intensity_change = ~light_intensity_change;
			light_intensity_lowerlimit_change = 1;
		}
	}

	//mode5: 温感模式(normal state <=> change state)
	if (state == 7)
	{
		if (K2 == enumKeyPress)
		{
			temperature_change = ~temperature_change;
			temperature_lowerlimit_change = 1;
		}
	}

	//mode6: FM收音机模式(K2)
	if (state == 8)
	{
		if (K2 == enumKeyPress && FM_mutex == 0)//未选中状态进入选中状态
		{
			FM_mutex = 1;//获取锁
			FM_frequency = 1;
			SetBeep(1500,20);
		}
		else if (K2 == enumKeyPress && FM_mutex == 1 && FM_frequency == 1)//选中状态恢复未选中状态
		{
			FM_mutex = 0;//释放锁
			FM_frequency = 0;
			SetBeep(1500,20);
		}
	}

	//mode7: MP3播放器
	if (state == 9)
	{
		if (K2 == enumKeyPress && GetPlayerMode() == enumModeStop)//停止状态，初次按下K2开始
		{
			SetMusic(90,0xFA,&song_TruE,sizeof(song_TruE),enumMscNull);
			//改动时注意两个要一起改动
			SetPlayerMode(enumModePlay);
		}
		else if (K2 == enumKeyPress && GetPlayerMode() == enumModePlay)//仍在播放，按下K2暂停
		{
			SetPlayerMode(enumModePause);
		}
		else if (K2 == enumKeyPress && GetPlayerMode() == enumModePause)//暂停状态，按下K2继续
		{
			SetPlayerMode(enumModePlay);
		}
	}

	//mode7(end): 离开音乐播放模式，自动停止播放
	if (state != 9 && (GetPlayerMode() == enumModePause || GetPlayerMode() == enumModePlay))
		SetPlayerMode(enumModeStop);

	/*该功能废止
	//mode8: 遥控模式
	if (state == 10)
	{
		if (K2 == enumKeyPress)
		{
			if(IrTxdSet(&controller,22) == enumIrTxOK) SetBeep(1500,20);
		}
	}
	*/

}




//定时回调函数(用于显示数码管)
void my100mS_callback()
{
	if (oled_change == 1)
	{
		oled_display();
		pc_preview();
		oled_change = 0;
	}

	//秒表模式，在秒表模式之外也计时
	if(stopwatch_1 ==1)  time1++;
	if(stopwatch_2 ==1)  time2++;

	//定时器模式，在定时器之外也进行
	if (timer_run == 1 ) timer--;
	if (timer == 0 && timer_change != 1)
	{
		timer_run = 0;
		SetBeep(1400,200);
		timer = 3000;

	}

	switch(state)
	{


		//Vib处理模式界面
		case 0:
		{
			Seg7Print(30,13,1,11,11,1,13,30);
			break;
		}


		//切换模式界面
		case 1:
		{
			if (transfer == 1)//state转移，需要改变屏幕
			{
				//OLED_ShowString(0, 6, "Next Mode：", 16);//中间步骤使用
				transfer=0;
				oled_change=1;
			}
			Seg7Print(32,37,44,44,48,34,12,next_mode[previous_state]);
			LedPrint(0);
			break;
		}


		//初始欢迎界面:HNU-CSEE+流水灯
		case 2:
		{
			unsigned char a;
			Seg7Print(37,43,50,12,32,48,34,34);

			if( a != 0) a=a<<1;
				else a=0x01;
			LedPrint(a);
			break;
		}


		//Mode1:实时时钟
		//初始显示时间，使用K2键依次切换显示 时分秒/年月日/星期(英文+数字)
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


		//Mode2:双通道秒表
		//使用K3/K2键分别独立记两个表，切换模式不影响计时，将在后台继续
		case 4:
		{
			unsigned char d0,d1,d2,d3,d4,d5,d6,d7;

			d7 = (time1/1000)%10;	if (d7 == 0) d7 = 10;
			d6 = (time1/100)%10;	if (d6 == 0) d6 = 10;
			d5 = (time1/10)%10+20;//这里要显示小数点
			d4 = (time1%10);

			d3 = (time2/1000)%10;	if (d3 == 0) d3 = 10;
			d2 = (time2/100)%10;	if (d2 == 0) d2 = 10;
			d1 = (time2/10)%10+20;//这里要显示小数点
			d0 = (time2%10);

			Seg7Print(d7,d6,d5,d4,d3,d2,d1,d0);

			break;
		}


		//Mode3:定时器
		case 5:
		{
			count++;
			if (count<=3)//用计数分割时间，达成闪烁效果
			{
				Seg7Print(10,10,timer/10/60/10,timer/10/60%10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
			}
			else
			{
				if (timer_change == 0)//正常状态 或 计数模式
				{
					if (timer_run == 0) Seg7Print(10,10,timer/10/60/10,timer/10/60%10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
					else if (timer_run == 1) Seg7Print(45,10,timer/10/60/10,timer/10/60%10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
				}
				else if (timer_minute == 1)//调节分钟，分钟闪烁
				{
					Seg7Print(32,10,10,10,12,timer/10%60/10,timer/10%60%10+20,timer%10);
				}
				else if (timer_second == 1)//调节秒钟，秒钟闪烁
				{
					Seg7Print(32,10,timer/10/60/10,timer/10/60%10,12,10,10,10);
				}
			}
			if (count > 5) count=0;
			break;
		}


		//Mode4:光照监测与报警
		case 6:
		{
			unsigned int num = GetADC().Rop;

			if (light_intensity_change == 0)//正常模式
			{
				Seg7Print(34,4,12,10,10,num/100,num/10%10,num%10);
				if (light_intensity_mode == 1)
				{
					if (num < light_intensity_lowerlimit)
					{
						SetBeep(1200,200);
					}
					else if (num > light_intensity_upperlimit)
					{
						SetBeep(2000,200);
					}
				}
			}
			else if (light_intensity_change == 1)//change模式
			{
				count++;

				if (count<=3)//用计数分割时间，达成闪烁效果
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
					if (light_intensity_lowerlimit_change == 1) //下限改动
					{
						Seg7Print(light_intensity_mode,
										10,10,10,
										12,
										light_intensity_upperlimit/100%10,
										light_intensity_upperlimit/10%10,
										light_intensity_upperlimit%10);
					}
					else if (light_intensity_upperlimit_change == 1) //上限改动
					{
						Seg7Print(light_intensity_mode,
										light_intensity_lowerlimit/100%10,
										light_intensity_lowerlimit/10%10,
										light_intensity_lowerlimit%10,
										12,
										10,10,10);
					}
					else if (light_intensity_mode_change == 1) //模式改动
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


		//Mode5:温度监测与报警
		case 7:
		{
			unsigned int tem_adc = GetADC().Rt;
			unsigned int tem = tem_calculate(tem_adc);

			if (temperature_change == 0)//正常模式
			{
				Seg7Print(34,5,12,10,10,tem/100%10,20+tem/10%10,tem%10);
				//第二位显示小数点
				if (temperature_mode == 1)
				{
					if (tem/10 < 20)
					{
						SetBeep(1200,200);
					}
					else if (tem/10 > 40)
					{
						SetBeep(2000,200);
					}
				}
			}
			else if (temperature_change == 1)//change模式
			{
				count++;

				if (count<=3)//用计数分割时间，达成闪烁效果
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
					if (temperature_lowerlimit_change == 1) //下限改动
					{
						Seg7Print(temperature_mode,
										10,10,10,
										12,
										temperature_upperlimit/100%10,
										20+temperature_upperlimit/10%10,
										temperature_upperlimit%10);
					}
					else if (temperature_upperlimit_change == 1) //上限改动
					{
						Seg7Print(temperature_mode,
										temperature_lowerlimit/100%10,
										20+temperature_lowerlimit/10%10,
										temperature_lowerlimit%10,
										12,
										10,10,10);
					}
					else if (temperature_mode_change == 1) //模式改动
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


		//Mode6:FM收音机
		//按K1或K2进入选中状态，分别调节音量和频率，按上下导航键调节值大小
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
			if (count<=3)//用计数分割时间，达成闪烁效果
			{
				Seg7Print(50,vol1,vol0,10,35,fre2,fre1,fre0);
			}
			else
			{
				if (FM_frequency == 0 && FM_volume == 0)//正常情况
				{
					Seg7Print(50,vol1,vol0,10,35,fre2,fre1,fre0);
				}
				else if (FM_frequency == 1)//调节频率，频率闪烁
				{
					Seg7Print(50,vol1,vol0,10,35,10,10,10);
				}
				else if (FM_volume == 1)//调节音量，音量闪烁
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

		/*该功能最后没有实现
		//Mode8:（格力）空调遥控
		case 10:
		{
			Seg7Print(12,12,43,44,12,8,12,12);
			break;
		}
		*/

		default:
		{
			//do nothing
			break;
		}

	};

}

//震动事件回调函数
void myVib_callback()
{
	if (state != 0) vib_old_state = state;//加判断条件防止连续撞击导致state变为0，锁死出不去
	state = 0;
	SetBeep(600,10);
	oled_change = 1;
}

//温度转换辅助函数
unsigned int tem_calculate(unsigned int tem_adc)
{
	float vccx=tem_adc/1000.0;
	float lnx=log(vccx/(1-vccx));
	float tem=1/((lnx/3950)+(1/298.15))-273.15;
	float tem10=tem*10;
	unsigned int ans=(int)tem10;
	return ans;
}

//oled显示屏切换
void oled_display()
{
	OLED_Clear();
	//OLED_ColorTurn(0);
	switch(state)
	{
		case 0:
		{
			//震动警告
			OLED_DrawBMP1(0, 0, 64, 64, vib_avater);
			OLED_ShowString(0, 0, "  Vib Detected  ", 16);
			OLED_ShowCHinese(64, 3, 29);
			OLED_ShowCHinese(80, 3, 30);
			OLED_ShowCHinese(96, 3, 31);
			OLED_ShowCHinese(112, 3, 32);
			break;
		}
		case 1:
		{
			//选择模式
			OLED_DrawBMP1(0, 0, 64, 64, choose_avater);
			OLED_ShowString(0, 0, "CHOOSE YOUR MODE", 16);
			OLED_ShowCHinese(64, 3, 33);
			OLED_ShowCHinese(80, 3, 34);
			OLED_ShowCHinese(96, 3, 35);
			OLED_ShowCHinese(112, 3, 36);
			break;
		}
		case 2:
		{
			//由初始化函数代为处理，此处不作操作
			break;
		}
		case 3:
		{
			//MODE-1 实时时钟
			OLED_DrawBMP1(0, 0, 64, 64, clock_avater);
			OLED_ShowString(0, 0, " MODE:01  CLOCK ", 16);
			OLED_ShowCHinese(64, 3, 0);
			OLED_ShowCHinese(80, 3, 1);
			OLED_ShowCHinese(96, 3, 2);
			OLED_ShowCHinese(112, 3, 3);
			break;
		}
		case 4:
		{
			//MODE-2 双通道秒表
			OLED_DrawBMP1(0, 0, 64, 64, stopwatch_avater);
			OLED_ShowString(0, 0, "MODE:02STOPWATCH", 16);
			OLED_ShowCHinese(64, 3, 4);
			OLED_ShowCHinese(80, 3, 5);
			OLED_ShowCHinese(96, 3, 6);
			OLED_ShowCHinese(64, 5, 7);
			OLED_ShowCHinese(80, 5, 8);
			break;
		}
		case 5:
		{
			//MODE-3 定时器
			OLED_DrawBMP1(0, 0, 64, 64, timer_avater);
			OLED_ShowString(0, 0, " MODE:03  TIMER ", 16);
			OLED_ShowCHinese(64, 3, 9);
			OLED_ShowCHinese(80, 3, 10);
			OLED_ShowCHinese(96, 3, 11);
			break;
		}
		case 6:
		{
			//MODE-4 光感监测与报警
			OLED_DrawBMP1(0, 0, 64, 64, light_avater);
			OLED_ShowString(0, 0, " MODE:04 light  ", 16);
			OLED_ShowCHinese(64, 3, 12);
			OLED_ShowCHinese(80, 3, 13);
			OLED_ShowCHinese(96, 3, 14);
			OLED_ShowCHinese(112, 3, 15);
			OLED_ShowCHinese(64, 5, 16);
			OLED_ShowCHinese(80, 5, 17);
			OLED_ShowCHinese(96, 5, 18);
			break;
		}
		case 7:
		{
			//MODE-5 温度监测与报警
			OLED_DrawBMP1(0, 0, 64, 64, temperature_avater);
			OLED_ShowString(0, 0, " MODE:05  Temp  ", 16);
			OLED_ShowCHinese(64, 3, 19);
			OLED_ShowCHinese(80, 3, 20);
			OLED_ShowCHinese(96, 3, 21);
			OLED_ShowCHinese(112, 3, 22);
			OLED_ShowCHinese(64, 5, 23);
			OLED_ShowCHinese(80, 5, 24);
			OLED_ShowCHinese(96, 5, 25);
			break;
		}
		case 8:
		{
			//MODE-6 收音机FM
			OLED_DrawBMP1(0, 0, 64, 64, FM_avater);
			OLED_ShowString(0, 0, " MODE:06   FM   ", 16);
			OLED_ShowCHinese(64, 3, 26);
			OLED_ShowCHinese(80, 3, 27);
			OLED_ShowCHinese(96, 3, 28);
			break;
		}
		case 9:
		{
			//MODE-7 音乐
			//OLED_ColorTurn(1);
			OLED_DrawBMP1(0, 0, 64, 64, music_avater);
			OLED_ShowString(0, 0, " MODE:07  MUSIC ", 16);
			break;
		}
		default:
		{
			//do nothing
			break;
		}
	}
}

//通信接收回调函数
void uart_callback()
{
	//if (datanow[2] == 0x00) SetBeep(200,10);
	Uart1Print(back,6);
	SetBeep(2000,10);
}

//计算机展示
void pc_preview()
{
	switch(state)
	{
		case 0:
		{
			//震动警告
			code char vib_pc_1[]={0xaa,0x55,0x10,0x00,0x00,0x00};
			code char vib_pc_2[]={0xaa,0x55,0x20,0x00,0x00,0x00};
			code char vib_pc_3[]={0xaa,0x55,0x30,0x00,0x00,0x00};
			code char vib_pc_4[]={0xaa,0x55,0x40,0x00,0x00,0x00};
			code char vib_pc_5[]={0xaa,0x55,0x50,0x00,0x00,0x00};
			code char vib_pc_6[]={0xaa,0x55,0x60,0x00,0x00,0x00};
			code char vib_pc_7[]={0xaa,0x55,0x70,0x00,0x00,0x00};
			code char vib_pc_8[]={0xaa,0x55,0x80,0x00,0x00,0x00};
			code char vib_pc_9[]={0xaa,0x55,0x90,0x00,0x00,0x00};
			switch (vib_old_state)
			{
				case 1: {Uart1Print(vib_pc_1,6); break; }
				case 2: {Uart1Print(vib_pc_2,6); break; }
				case 3: {Uart1Print(vib_pc_3,6); break; }
				case 4: {Uart1Print(vib_pc_4,6); break; }
				case 5: {Uart1Print(vib_pc_5,6); break; }
				case 6: {Uart1Print(vib_pc_6,6); break; }
				case 7: {Uart1Print(vib_pc_7,6); break; }
				case 8: {Uart1Print(vib_pc_8,6); break; }
				case 9: {Uart1Print(vib_pc_9,6); break; }
				default: {break;}
			}
			break;
		}
		case 1:
		{
			//选择模式
			code char choose_pc_2[]={0xaa,0x55,0x3a,0x00,0x00,0x00};
			code char choose_pc_3[]={0xaa,0x55,0x4a,0x00,0x00,0x00};
			code char choose_pc_4[]={0xaa,0x55,0x5a,0x00,0x00,0x00};
			code char choose_pc_5[]={0xaa,0x55,0x6a,0x00,0x00,0x00};
			code char choose_pc_6[]={0xaa,0x55,0x7a,0x00,0x00,0x00};
			code char choose_pc_7[]={0xaa,0x55,0x8a,0x00,0x00,0x00};
			code char choose_pc_8[]={0xaa,0x55,0x9a,0x00,0x00,0x00};
			code char choose_pc_9[]={0xaa,0x55,0x3a,0x00,0x00,0x00};
			switch (previous_state)
			{
				case 2: {Uart1Print(choose_pc_2,6); break; }
				case 3: {Uart1Print(choose_pc_3,6); break; }
				case 4: {Uart1Print(choose_pc_4,6); break; }
				case 5: {Uart1Print(choose_pc_5,6); break; }
				case 6: {Uart1Print(choose_pc_6,6); break; }
				case 7: {Uart1Print(choose_pc_7,6); break; }
				case 8: {Uart1Print(choose_pc_8,6); break; }
				case 9: {Uart1Print(choose_pc_9,6); break; }
				default: {break;}
			}
			break;
		}
		case 2:
		{
			code char welcome_pc[]={0xaa,0x55,0xa1,0xa1,0xa1,0xa1};
			Uart1Print(welcome_pc,6);
			break;
		}
		case 3:
		{
			//MODE-1 实时时钟
			code char clock_pc[]={0xaa,0x55,0x01,0x00,0x00,0x00};
			Uart1Print(clock_pc,6);
			break;
		}
		case 4:
		{
			//MODE-2 双通道秒表
			code char stopwatch_pc[]={0xaa,0x55,0x02,0x00,0x00,0x00};
			Uart1Print(stopwatch_pc,6);
			break;
		}
		case 5:
		{
			//MODE-3 定时器
			code char timer_pc[]={0xaa,0x55,0x03,0x00,0x00,0x00};
			Uart1Print(timer_pc,6);
			break;
		}
		case 6:
		{
			//MODE-4 光感监测与报警
			code char light_pc[]={0xaa,0x55,0x04,0x00,0x00,0x00};
			Uart1Print(light_pc,6);
			break;
		}
		case 7:
		{
			//MODE-5 温度监测与报警
			code char temperature_pc[]={0xaa,0x55,0x05,0x00,0x00,0x00};
			Uart1Print(temperature_pc,6);
			break;
		}
		case 8:
		{
			//MODE-6 收音机FM
			code char FM_pc[]={0xaa,0x55,0x06,0x00,0x00,0x00};
			Uart1Print(FM_pc,6);
			break;
		}
		case 9:
		{
			//MODE-7 音乐
			code char music_pc[]={0xaa,0x55,0x07,0x00,0x00,0x00};
			Uart1Print(music_pc,6);
			break;
		}
		default:
		{
			//do nothing
			break;
		}
	}
}
