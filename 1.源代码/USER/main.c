#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "dht11.h"
#include "bump.h"
#include "LDR.h"
#include "oled.h"
#include "key.h"
#include "Modules.h"
#include "TIM2.h"
#include "TIM3.h"
#include "adcx.h"
#include "flash.h"
#include "stepmotor.h"
#include "fan.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "stm32f10x_iwdg.h"

 

#define KEY_Long1	11

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

//传感器变量声明
u8 temp;//温度变量
u8 humi;//湿度变量
u16 lux;//光照值变量
u16 soil_Humi;//土壤湿度变量
u16 CO2;//CO2变量


SensorModules sensorData;								//声明传感器数据结构体变量
SensorThresholdValue Sensorthreshold;		//声明传感器阈值结构体变量
DriveModules driveData;									//声明驱动器状态结构体变量

int mode = 0;	//系统模式  0自动  1手动  设置

//系统静态变量
uint8_t count_a = 1;  //自动模式按键数
uint8_t count_m = 1;  //手动模式按键数
static uint8_t count_s = 1;	 //设置模式按键数
extern volatile uint8_t motor_busy;  // 电机忙标志
extern uint8_t valueFlashFlag;
/**
  * @brief  显示菜单内容
  * @param  无
  * @retval 无
  */
enum 
{
	AUTO_MODE = 0,
	MANUAL_MODE,
	SETTINGS_MODE
	
}MODE_PAGES;

/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_autoPage1(void)		//自动模式菜单第一页
{
	//显示“环境温度：  C”
	OLED_ShowChinese(0,0,13,16,1); 	//环
	OLED_ShowChinese(16,0,14,16,1);	//境
	OLED_ShowChinese(32,0,0,16,1);	//温
	OLED_ShowChinese(48,0,2,16,1);	//度
	OLED_ShowChar(64,0,':',16,1);
	 
	
	OLED_Refresh();
	
}
void SensorDataDisplay1(void)		//传感器数据显示第一页
{
	//显示温度数据
	OLED_ShowNum(90,0,sensorData.temp,2,16,1);
	 
}

/**
  * @brief  显示手动模式设置界面1
  * @param  无
  * @retval 无
  */
void OLED_manualPage1(void)
{
	
	//显示“灯光”
	OLED_ShowChinese(16,0,28,16,1);//灯
	OLED_ShowChinese(32,0,29,16,1);//光
	OLED_ShowChar(64,0,':',16,1);

	  
}



/**
  * @brief  显示手动模式设置参数界面1
  * @param  无
  * @retval 无
  */
void ManualSettingsDisplay1(void)
{
	if(driveData.LED_Flag)
	{
		OLED_ShowChinese(96,0,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,0,42,16,1); 	//关
	}
	
 
	
	if(driveData.NOW_StepMotor_Flag)
	{
		OLED_ShowChinese(96,48,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,48,42,16,1); 	//关
	}
}


/**
  * @brief  显示系统阈值设置界面1
  * @param  无
  * @retval 无
  */
void OLED_settingsPage1(void)
{
	//显示“温度阈值”
	OLED_ShowChinese(16,0,0,16,1);	
	OLED_ShowChinese(32,0,2,16,1);	
	OLED_ShowChinese(48,0,50,16,1);	
	OLED_ShowChinese(64,0,52,16,1);	
	OLED_ShowChar(80,0,':',16,1);

	 
}


void SettingsThresholdDisplay1(void)
{
	//显示温度阈值数值
	OLED_ShowNum(90,0, Sensorthreshold.tempValue, 2,16,1);
	//显示光照上限阈值数值
	OLED_ShowNum(90, 16, Sensorthreshold.lux_Max, 3,16,1);
 
}

	
/**
  * @brief  记录自动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetAuto(void)  
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_a++;
		if (count_a > 2)
		{
			count_a = 1;
		}
		OLED_Clear();
	}
	return count_a;
}

/**
  * @brief  记录手动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetManual(void)  
{

	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_m++;
		if (count_m > 4)  		//一共可以控制的外设数量
		{
			count_m = 1;
		}
	}
	return count_m;
}

/**
  * @brief  记录阈值界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_s++;
	}
	else if (count_s > 4)
		{
			OLED_Clear();
			count_s = 1;
		}
	return count_s;
}


/**
  * @brief  显示手动模式界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_manualOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
	 
		default: break;
	}
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_settingsOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		 
		default: break;
	}
}

/**
  * @brief  自动模式控制函数
  * @param  无
  * @retval 无
  */
void AutoControl(void)
{
	
	if(sensorData.temp>Sensorthreshold.tempValue)//当前值小于阈值
		driveData.Fan_Flag = 1;
	else
		driveData.Fan_Flag = 0;
	 
}

/**
  * @brief  手动模式控制函数
  * @param  无
  * @retval 无
  */
void ManualControl(uint8_t num)
{
	switch(num)
	{
		case 1:	
			//显示外设开关
			if(KeyNum == KEY_3)
			{
				KeyNum=0;
				driveData.LED_Flag = 1;
			}
		 
			break;
		case 2:	
			
			if(KeyNum == KEY_3)
			{
				KeyNum=0;
				driveData.Fan_Flag = 1;
			}
			if(KeyNum == KEY_4)
			{
				KeyNum=0;
				driveData.Fan_Flag = 0;
			}
		
			break;
		 
		default: break;
	}

}

/**
  * @brief  控制函数
  * @param  无
  * @retval 无
  */
void Control_Manager(void)
{
	if(driveData.LED_Flag)//LED控制
	{
		LED_On();
	}
	else
	{
		LED_Off();
	}

	 
	
}

/**
  * @brief  阈值设置函数
  * @param  无
  * @retval 无
  */
void ThresholdSettings(uint8_t num)
{
	switch (num)
	{
		case 1:
			if (KeyNum == KEY_3)//按键三按下
			{
				KeyNum = 0;
				Sensorthreshold.tempValue += 1;//阈值增加
				if (Sensorthreshold.tempValue > 35)//阈值上限
				{
					Sensorthreshold.tempValue = 35;
				}
			}
			else if (KeyNum == KEY_4)//按键四按下
			{
				KeyNum = 0;				
				if(Sensorthreshold.tempValue>2)
					Sensorthreshold.tempValue -= 1;//阈值减小			
			}
			break;
			
	 
		default: break;		
	}
}
/**
  * @brief  机智云按键配网
  * @param  无
  * @retval 无
  */
void ScanGizwitsMode(void)
{
	if(!KEY3)
	{
		delay_ms(20);
		if(!KEY3)
		{
			//显示“热点配网”
	    OLED_ShowChinese(32,16,44,16,1); 	//热
	    OLED_ShowChinese(48,16,45,16,1);	//点                               
	    OLED_ShowChinese(64,16,48,16,1);	//配
	    OLED_ShowChinese(80,16,49,16,1);  //网
			gizwitsSetMode(WIFI_SOFTAP_MODE);
			OLED_Refresh();
		}
	}
	else if(!KEY4)
	{
		delay_ms(20);
		if(!KEY4)
		{
			//显示“一键配网”
	    OLED_ShowChinese(32,16,46,16,1); 	//一
	    OLED_ShowChinese(48,16,47,16,1);	//键
	    OLED_ShowChinese(64,16,48,16,1);	//配
	    OLED_ShowChinese(80,16,49,16,1);  //网
			gizwitsSetMode(WIFI_AIRLINK_MODE);
			OLED_Refresh();
		}
	}
}
/**
  * @brief  flash阈值读取函数
  * @param  无
  * @retval 无
  */
void FLASH_ReadThreshold()
{
	Sensorthreshold.tempValue	= FLASH_R(FLASH_START_ADDR);	//从指定页的地址读FLASH
	Sensorthreshold.lux_Max = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH
	Sensorthreshold.lux_Min = FLASH_R(FLASH_START_ADDR+4);	//从指定页的地址读FLASH
	Sensorthreshold.soilHumiValue = FLASH_R(FLASH_START_ADDR+6);	//从指定页的地址读FLASH
	if(Sensorthreshold.tempValue > 35 || Sensorthreshold.lux_Max > 200 || Sensorthreshold.lux_Min> 150|| Sensorthreshold.soilHumiValue > 90)//flash读取乱码重置flash中的数值
	{
		FLASH_W(FLASH_START_ADDR,28,200,130,50);
		FLASH_ReadThreshold();//调用flash阈值读取函数
	}
}

uint8_t OLED_ClearFlag_New;
uint8_t OLED_ClearFlag_OLD;

int main(void)
{ 
  SystemInit();//配置系统时钟为72M	
	delay_init(72);
	ADCx_Init();//ADC初始化
	LED_Init();//LED初始化
	LED_On();//打开LED
	Key_Init();//按键初始化
	 
	OLED_Clear();
	
  while (1)
  {	
		do
		{
			currentDataPoint.valuegz_max = Sensorthreshold.lux_Max;
			currentDataPoint.valuegz_min = Sensorthreshold.lux_Min;
			currentDataPoint.valuewd = Sensorthreshold.tempValue;
			currentDataPoint.valuetrsd = Sensorthreshold.soilHumiValue;
		}while(0);//阈值上传机智云
		
		SensorScan();	//获取传感器数据
		userHandle();//上报
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//下发
		switch(mode)//模式选择
		{
			case AUTO_MODE://自动模式;
					OLED_autoPage1();	//显示主页面1固定信息
					SensorDataDisplay1();	//显示传感器1数据
					AutoControl();//自动模式控制
				
				/*按键1按下时切换模式*/
				if (KeyNum == KEY_1)   //系统模式mode  0自动  1手动  2设置
				{
					KeyNum = 0;//按键清零
					mode = MANUAL_MODE;//切换至手动模式
					count_m = 1;//手动模式下按键次数赋值
					OLED_Clear();//清屏
				}
				
				if (KeyNum == KEY_Long1)//长按按键1三秒
				{
					KeyNum = 0;//按键清零
					mode = SETTINGS_MODE;//切换至阈值调节模式
					count_s = 1;//阈值切换模式下按键次数赋值
					OLED_Clear();//清屏
				}
				
				Control_Manager();//控制函数执行相应的执行器
				break;
				
				case MANUAL_MODE://手动模式
			
			 
				Control_Manager();//控制函数执行相应的执行器
				break;
				
				case SETTINGS_MODE://阈值调节模式
					
				 	
					//判断是否退出阈值设置界面
					if (KeyNum == KEY_1)
					{
						KeyNum = 0;
						driveData.Mode_Flag=0;  
						mode = AUTO_MODE;	//跳转到主界面
						count_a = 1;//自动模式下按键次数赋值
						OLED_Clear();	//清屏
						//存储修改的传感器阈值至flash内	
						FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue,
																			Sensorthreshold.lux_Max,
																			Sensorthreshold.lux_Min,
																			Sensorthreshold.soilHumiValue);
					}
				 
				break;
				default: break;
					
		}
		if (valueFlashFlag)
		{
			valueFlashFlag = 0;
			FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue, 
																			Sensorthreshold.lux_Max,
																			Sensorthreshold.lux_Min, 
																			Sensorthreshold.soilHumiValue);			
		}
		 
		
  }
}
