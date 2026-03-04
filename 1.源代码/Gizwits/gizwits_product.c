/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits control protocol processing, and platform-related hardware initialization 
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
*
* @note         Gizwits is only for smart hardware
*               Gizwits Smart Cloud for Smart Products
*               Links | Value Added | Open | Neutral | Safety | Own | Free | Ecology
*               www.gizwits.com
*
***********************************************************/
#include <stdio.h>
#include <string.h>
#include "gizwits_product.h"
#include "Modules.h"
#include "oled.h"
#include "led.h"
static uint32_t timerMsCount;
extern uint8_t count_a;
extern uint8_t count_m;
extern int mode;
extern enum 
{
	AUTO_MODE = 0,
	MANUAL_MODE,
	SETTINGS_MODE
	
}MODE_PAGES;
uint8_t valueFlashFlag;


/** Current datapoint */
dataPoint_t currentDataPoint;

/**@} */
/**@name Gizwits User Interface
* @{
*/

/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
    uint8_t i = 0;
    dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
    protocolTime_t *ptime = (protocolTime_t *)gizdata;
    
#if MODULE_TYPE
    gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
    moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif

    if((NULL == info) || (NULL == gizdata))
    {
        return -1;
    }

    for(i=0; i<info->num; i++)
    {
        switch(info->event[i])
        {
        case EVENT_fan:
            currentDataPoint.valuefan = dataPointPtr->valuefan;
            GIZWITS_LOG("Evt: EVENT_fan %d \n", currentDataPoint.valuefan);
            if(0x01 == currentDataPoint.valuefan)
            {
            //user handle
							if(mode == MANUAL_MODE)
							{
								driveData.Fan_Flag = 1;
								count_m = 2;
							}
            }
            else
            {
            //user handle  
							if(mode == MANUAL_MODE)
							{
								driveData.Fan_Flag = 0;
								count_m = 2;
							}							
            }
            break;
        case EVENT_bump:
            currentDataPoint.valuebump = dataPointPtr->valuebump;
            GIZWITS_LOG("Evt: EVENT_bump %d \n", currentDataPoint.valuebump);
            if(0x01 == currentDataPoint.valuebump)
            {
            //user handle
							if(mode == MANUAL_MODE)
							{
								driveData.Bump_Flag = 1;
								count_m = 3;
							}	
            }
            else
            {
            //user handle  
								if(mode == MANUAL_MODE)
							{
								driveData.Bump_Flag = 0;
								count_m = 3;
							}
            }
            break;
        case EVENT_led:
            currentDataPoint.valueled = dataPointPtr->valueled;
            GIZWITS_LOG("Evt: EVENT_led %d \n", currentDataPoint.valueled);
            if(0x01 == currentDataPoint.valueled)
            {
            //user handle
							if(mode == MANUAL_MODE)
							{
								driveData.LED_Flag = 1;
								count_m = 1;
							}
            }
            else
            {
            //user handle    
							if(mode == MANUAL_MODE)
							{
								driveData.LED_Flag = 0;
								count_m = 1;
							}
            }
            break;
        case EVENT_motor:
            currentDataPoint.valuemotor = dataPointPtr->valuemotor;
            GIZWITS_LOG("Evt: EVENT_motor %d \n", currentDataPoint.valuemotor);
            if(0x01 == currentDataPoint.valuemotor)
            {
            //user handle
							if(mode == MANUAL_MODE)
							{
								driveData.NOW_StepMotor_Flag = 1;
								count_m = 4;
							}
            }
            else
            {
            //user handle    
							if(mode == MANUAL_MODE)
							{
								driveData.NOW_StepMotor_Flag = 0;
								count_m = 4;
							}
            }
            break;

        case EVENT_control_mode:
            currentDataPoint.valuecontrol_mode = dataPointPtr->valuecontrol_mode;
            GIZWITS_LOG("Evt: EVENT_control_mode %d\n", currentDataPoint.valuecontrol_mode);
            switch(currentDataPoint.valuecontrol_mode)
            {
            case control_mode_VALUE0:
                //user handle
								mode  = AUTO_MODE;
								OLED_Clear();
                break;
            case control_mode_VALUE1:
                //user handle
								mode = MANUAL_MODE;
								OLED_Clear();
                break;
            default:
                break;
            }
            break;

        case EVENT_gz_max:
            currentDataPoint.valuegz_max = dataPointPtr->valuegz_max;
            GIZWITS_LOG("Evt:EVENT_gz_max %d\n",currentDataPoint.valuegz_max);
            //user handle
							valueFlashFlag = 1;
							Sensorthreshold.lux_Max = currentDataPoint.valuegz_max;
            break;
        case EVENT_gz_min:
            currentDataPoint.valuegz_min = dataPointPtr->valuegz_min;
            GIZWITS_LOG("Evt:EVENT_gz_min %d\n",currentDataPoint.valuegz_min);
            //user handle
						valueFlashFlag = 1;
						Sensorthreshold.lux_Min = currentDataPoint.valuegz_min;
            break;
        case EVENT_wd:
            currentDataPoint.valuewd = dataPointPtr->valuewd;
            GIZWITS_LOG("Evt:EVENT_wd %d\n",currentDataPoint.valuewd);
            //user handle
						valueFlashFlag = 1;
						Sensorthreshold.tempValue = currentDataPoint.valuewd;
            break;
        case EVENT_trsd:
            currentDataPoint.valuetrsd = dataPointPtr->valuetrsd;
            GIZWITS_LOG("Evt:EVENT_trsd %d\n",currentDataPoint.valuetrsd);
            //user handle
						valueFlashFlag = 1;
						Sensorthreshold.soilHumiValue = currentDataPoint.valuetrsd;
            break;


        case WIFI_SOFTAP:
            break;
        case WIFI_AIRLINK:
            break;
        case WIFI_STATION:
            break;
        case WIFI_CON_ROUTER:
 
            break;
        case WIFI_DISCON_ROUTER:
 
            break;
        case WIFI_CON_M2M:
 
            break;
        case WIFI_DISCON_M2M:
            break;
        case WIFI_RSSI:
            GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
            break;
        case TRANSPARENT_DATA:
            GIZWITS_LOG("TRANSPARENT_DATA \n");
            //user handle , Fetch data from [data] , size is [len]
            break;
        case WIFI_NTP:
            GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
            break;
        case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
#if MODULE_TYPE
            GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",gprsInfoData->Type);
#else
            GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
#endif
        break;
        default:
            break;
        }
    }

    return 0;
}

/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{
		currentDataPoint.valuetemp = sensorData.temp;//Add Sensor Data Collection
    currentDataPoint.valuehumi = sensorData.humi;//Add Sensor Data Collection
    currentDataPoint.valuets = sensorData.ts;//Add Sensor Data Collection
    currentDataPoint.valuelux = sensorData.lux;//Add Sensor Data Collection
		currentDataPoint.valuebump = driveData.Bump_Flag;
		currentDataPoint.valuefan = driveData.Fan_Flag;
		currentDataPoint.valueled = driveData.LED_Flag;
		currentDataPoint.valuemotor = driveData.NOW_StepMotor_Flag;
		currentDataPoint.valuecontrol_mode = mode;
}

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
    memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 

    currentDataPoint.valuefan = 0;
    currentDataPoint.valuebump = 0;
    currentDataPoint.valueled = 0;
    currentDataPoint.valuemotor = 0;
    currentDataPoint.valuecontrol_mode = AUTO_MODE;
    currentDataPoint.valuetemp = 0;
    currentDataPoint.valuehumi = 0;
    currentDataPoint.valuets = 0;
    currentDataPoint.valuegz_max = 0;
    currentDataPoint.valuegz_min = 0;
    currentDataPoint.valuewd = 0;
    currentDataPoint.valuetrsd = 0;
    currentDataPoint.valuelux = 0;

}


/**
* @brief  gizTimerMs

* millisecond timer maintenance function ,Millisecond increment , Overflow to zero

* @param none
* @return none
*/
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief gizGetTimerCount

* Read system time, millisecond timer

* @param none
* @return System time millisecond
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief mcuRestart

* MCU Reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
	   __set_FAULTMASK(1);
		NVIC_SystemReset();	
}
/**@} */

/**
* @brief TIMER_IRQ_FUN

* Timer Interrupt handler function

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief UART_IRQ_FUN

* UART Serial interrupt function ，For Module communication

* Used to receive serial port protocol data between WiFi module

* @param none
* @return none
*/
void UART_IRQ_FUN(void)
{
  uint8_t value = 0;
  //value = USART_ReceiveData(USART2);//STM32 test demo
  gizPutData(&value, 1);
}


/**
* @brief uartWrite

* Serial write operation, send data to the WiFi module

* @param buf      : Data address
* @param len       : Data length
*
* @return : Not 0,Serial send success;
*           -1，Input Param Illegal
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    
    if(NULL == buf)
    {
        return -1;
    }
    
//    #ifdef PROTOCOL_DEBUG
//    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
//    for(i=0; i<len; i++)
//    {
//        GIZWITS_LOG("%02x ", buf[i]);
//    }
//    GIZWITS_LOG("\n");
//    #endif

    for(i=0; i<len; i++)
    {
      USART_SendData(USART2,buf[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
			
			if(i >=2 && buf[i] == 0xFF)
			{
					USART_SendData(USART2,0x55);
					while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
			}
    }


    
    return len;
}


