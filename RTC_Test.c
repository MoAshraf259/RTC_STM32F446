/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include	<stdio.h>
#include	<string.h>
#include 	<stdint.h>
#include "stm32f446.h"
#include "ds1307.h"
#include "LCD.h"

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1000); i++);
}

extern void initialise_monitor_handles(void);
#define SYSTICK_TIM_CLK	16000000UL
void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

    /* calculation of reload value */
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter

}

char* Date_To_String(RTC_Date_t *pRTCDate);
char* Time_To_String(RTC_Time_t *pRTCTime);
void Number_To_String(uint8_t number,char *Buffer);
char* Get_Day_Of_Week(uint8_t day);


int main(void)
{

	LCD_Init();

	initialise_monitor_handles();
	RTC_Time_t CurrentTime;
	RTC_Date_t CurrentDate;


	LCD_SendString("Mohamed Ashraf");
	lcd_set_cursor(2, 1);
	LCD_SendString("RTC Test");
	mdelay(1200);
	clr_lcd();
	init_systick_timer(1);
	if(DS1307_Init())
	{
		printf("Initialize failed !\n");
		while(1);
	}
	CurrentDate.date=15;
	CurrentDate.day=Saturday;
	CurrentDate.month=11;
	CurrentDate.year=23;

	CurrentTime.hours=11;
	CurrentTime.minutes=59;
	CurrentTime.seconds=49;
	CurrentTime.time_format=TIMER_FORMAT_12PM;

	DS1307_SetCurrentTime(&CurrentTime);
	DS1307_SetCurrentDate(&CurrentDate);

	DS1307_GetCurrentTime(&CurrentTime);
	DS1307_GetCurrentDate(&CurrentDate);

	char *am_pm;

	if(CurrentTime.time_format != TIMER_FORMAT_24)
	{
		am_pm = (CurrentTime.time_format) ? "PM" : "AM";
		//printf("Current time = %s %s\n",Time_To_String(&CurrentTime),am_pm);
		LCD_SendString(Time_To_String(&CurrentTime));
		LCD_SendString(am_pm);
	}
	else
	{
		//printf("Current time = %s\n",Time_To_String(&CurrentTime));
		LCD_SendString(Time_To_String(&CurrentTime));
	}

	//printf("Current Date: %s <%s> \n",Date_To_String(&CurrentDate),Get_Day_Of_Week(CurrentDate.day)	);
	lcd_set_cursor(2, 1);
	LCD_SendString(Date_To_String(&CurrentDate));
	LCD_SendString(Get_Day_Of_Week(CurrentDate.day));

	while(1)
	{

	}
	return 0;
}

char* Get_Day_Of_Week(uint8_t day)
{
	char* Days[]={"Saturday" , "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday"};

	return Days[day-1];
}

void Number_To_String(uint8_t number,char *Buffer)
{
	if(number<10)
	{
		Buffer[0]='0';
		Buffer[1]=number+48;
	}
	else
	{
		Buffer[0]=(number/10)+48;
		Buffer[1]=(number%10)+48;
	}
}


char* Time_To_String(RTC_Time_t *pRTCTime)
{
	static char time[9];
	time[2]=':';
	time[5]=':';

	Number_To_String(pRTCTime->hours,time);
	Number_To_String(pRTCTime->minutes,&time[3]);
	Number_To_String(pRTCTime->seconds,&time[6]);

	time[8]='\0';

	return time;
}
char* Date_To_String(RTC_Date_t *pRTCDate)
{
	static char date[9];
	date[2]='/';
	date[5]='/';

	Number_To_String(pRTCDate->date, date);
	Number_To_String(pRTCDate->month, &date[3]);
	Number_To_String(pRTCDate->year, &date[6]);

	date[8]='\0';

	return date;
}

void SysTick_Handler(void)
{
	RTC_Time_t CurrentTime;
	RTC_Date_t CurrentDate;
	char *am_pm;
	DS1307_GetCurrentTime(&CurrentTime);

	if(CurrentTime.time_format != TIMER_FORMAT_24)
	{
		am_pm = (CurrentTime.time_format) ? "PM" : "AM";
		//printf("Current time = %s %s\n",Time_To_String(&CurrentTime),am_pm);
		lcd_set_cursor(1, 1);
		LCD_SendString(Time_To_String(&CurrentTime));
		LCD_SendString(am_pm);
	}
	else
	{
		//printf("Current time = %s\n",Time_To_String(&CurrentTime));
		lcd_set_cursor(1, 1);
		LCD_SendString(Time_To_String(&CurrentTime));
	}

	DS1307_GetCurrentDate(&CurrentDate);
	lcd_set_cursor(2, 1);
	LCD_SendString(Date_To_String(&CurrentDate));
	LCD_SendChar('<');
	LCD_SendString(Get_Day_Of_Week(CurrentDate.day));
	LCD_SendChar('>');
	//printf("Current Date: %s <%s> \n",Date_To_String(&CurrentDate),Get_Day_Of_Week(CurrentDate.day)	);
}
