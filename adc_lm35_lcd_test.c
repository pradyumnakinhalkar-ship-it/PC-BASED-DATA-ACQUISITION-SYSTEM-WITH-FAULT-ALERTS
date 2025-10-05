#include <lpc21xx.h>
#include "adc.h"
#include "lcd_defines.h"
#include "delay.h"
#include "lcd.h"
#include "rtc.h"
#include "kpm.h"
#include "kpm_defines.h"
#include "rtc_defines.h"
#include "UART.h"
#include "UART_defines.h"
#include "interrupts_pro.h"
#define LED 19
#define LIMIT_MIN 1
s32 hour,min,sec,date,month,year,day,prev_min,prev_sec;
static u32 limit=45;
u32 fg2=1,fg1=1,j;
f32 eAR;
int main()
{
	
  u32 adcDVal;
	Init_ADC();
	InitLCD();
	StrLCD("Jay shivray");
	delay_ms(1000);
	RTC_Init();
	Initkpm();
	Init_UART0();
	Enable_EINT1();
	IODIR0|=1<<LED;
	SetRTCTimeInfo(22,52,00);
	SetRTCDateInfo(22,5,2025);
	SetRTCDay(3);
	
	while(1)
	{
		u32 value,i;
		Read_ADC(1,&adcDVal,&eAR);
		value=eAR*100;
		if(value<=limit)
		{
			GetRTCTimeInfo(&hour,&min,&sec);
			GetRTCDateInfo(&date,&month,&year);
			IOCLR0|=1<<LED;
			fg1=1;
			if(fg2)
			{
				cmdLCD(CLEAR_LCD);
				fg2=0;
			}
			DisplayRTCTime(hour,min,sec);
			DisplayRTCDate(date,month,year);
			GetRTCDay(&day);
			DisplayRTCDay(day);
			cmdLCD(GOTO_LINE1_POS0+11);
			DisplayRTCDay(day);
			//U32LCD(limit);
			//charLCD(0xDF);
			//charLCD('c');
			cmdLCD(GOTO_LINE2_POS0+11);
      StrLCD("T:");
			U32LCD(value);
			//F32LCD(ear,0);
			charLCD(0xDF);
      charLCD('C');
			if (j)
                        {
                                /*U0_TXStr("Normal Operation (Temperature within safe limit)");
                                  U0_TXStr("\r\n");
                                //32.5°C @ 13:45:20 13/05/2025*/
                                U0_TXStr("[INFO] Temp:");
                                U0_TXU32(value);
                                U0_TXByte('*');
                                U0_TXByte('C');
                                U0_TXByte(' ');
                                DisplayRTCTime_uart(hour,min,sec);
                                U0_TXByte(' ');
                                DisplayRTCDate_uart(date,month,year);
                                U0_TXStr("\r\n");
                                j=0;
                        }
                        if (((min%LIMIT_MIN)==0) && (min!=prev_min))
                        {
                                /*U0_TXStr("Normal Operation (Temperature within safe limit)");
                                  U0_TXStr("\r\n");
                                //32.5°C @ 13:45:20 13/05/2025*/
                                U0_TXStr("[INFO] Temp:");
                                U0_TXU32(value);
                                U0_TXByte('*');
                                U0_TXByte('C');
                                U0_TXByte(' ');
                                DisplayRTCTime_uart(hour,min,sec);
                                U0_TXByte(' ');
                                DisplayRTCDate_uart(date,month,year);
                                U0_TXStr("\r\n");
                                prev_min=min;
                        }
                }
                else
                {
                        IOSET0=1<<LED;
                        GetRTCTimeInfo(&hour,&min,&sec);
                        GetRTCDateInfo(&date,&month,&year);
                        if(fg1)
                        {
                                cmdLCD(CLEAR_LCD);
                                fg1=0;
                                fg2=1;
                                i=1;
                        }
                        cmdLCD(GOTO_LINE1_POS0);
                        StrLCD("Over Temprature");
                        cmdLCD(GOTO_LINE2_POS0);
                        U32LCD(value);
                        charLCD(0xDF);
                        charLCD('C');
                        if (i)
                        {
                                j=1;
                                /*U0_TXStr("Over-Temperature Alert (Temperature exceeds set point : ");
                                  U0_TXU32(limit);
                                  U0_TXByte(')');
                                  U0_TXStr("\r\n");*/
                                U0_TXStr("[ALERT] Temp:");
                                U0_TXU32(value);
                                U0_TXByte('*');
                                U0_TXByte('C');
                                U0_TXByte(' ');
                                DisplayRTCTime_uart(hour,min,sec);
                                U0_TXByte(' ');
                                DisplayRTCDate_uart(date,month,year);
                                U0_TXStr("\r\n");
                                prev_sec=sec;
                                i=0;
                        }
                }
                delay_ms(1000);
        }
}
void eint0_isr(void)__irq
{
        u8 choice,choice2;
        u32 lmt,pass,password=420,i=3;
        U0_TXStr("*** Time Editing Mode Activated ***");
        U0_TXStr("\r\n");
L4:      cmdLCD(CLEAR_LCD);
         cmdLCD(RET_CUR_HOME);
         StrLCD("Enter Password:");
         pass=mykeyRead();
         if(pass==password)
         {
L2:     cmdLCD(CLEAR_LCD);
        cmdLCD(RET_CUR_HOME);
        StrLCD("1:Set Threshhold");
        cmdLCD(GOTO_LINE2_POS0);
        StrLCD("2:Set RTC");

        choice=keyScan();
        if(choice=='1' )
        {
                cmdLCD(CLEAR_LCD);
                cmdLCD(RET_CUR_HOME);
                StrLCD("STV (0 -150)");
                lmt=mykeyRead();
                if(lmt<150)
                {
                        limit=lmt;
                                j=0;
                }
                else
                {
                        cmdLCD(CLEAR_LCD);
                        cmdLCD(RET_CUR_HOME);
                        StrLCD("Out of Range");
                        cmdLCD(GOTO_LINE2_POS0);
                        StrLCD("(00 to 150)");
                        delay_ms(1002);
                        goto L2;
                }
                fg2=1;
		fg1=1;
        }
        else if(choice=='2')
        {
L1:cmdLCD(CLEAR_LCD);
   cmdLCD(RET_CUR_HOME);
   cmdLCD(GOTO_LINE1_POS0);
   StrLCD("1.H 2.M 3.S 4.D");
   cmdLCD(GOTO_LINE2_POS0);
   StrLCD("5.M 6.Y 7.D 8.E");
   choice2=keyScan();
   switch(choice2)
   {
           case'1': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("Set Hour (00-23)");
                    hour=mykeyRead();
                    if(hour>=0 && hour<=23)
                    {
                            HOUR=hour;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("Range (00 to 23)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'2': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("Set Min (00-59)");
                    min=mykeyRead();
                    if(min>=0 && min<=59)
                    {
                            MIN=min;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("Range (00 to 59)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'3': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("Set Sec (00-59)");
                    sec=mykeyRead();
                    if(sec>=0 && sec<=59)
                    {
                            SEC=sec;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("Range (00 to 59)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'4': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("Set Day (0-6)");
                    day=mykeyRead();
                    if(day>=0 && day<=6)
                    {
                            DOW=day;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("Range (0 to 6)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'5': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("Set Month (1-12)");
                    month=mykeyRead();
                    if(month>=1 && month<=12)
                    {
                            MONTH=month;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("Range (1 to 12)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'6': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("STY (2020-3000)");
                    year=mykeyRead();
                    if(year>=2020 && year<=3000)
                    {
                            YEAR=year;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("(2020 to 3000)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'7': cmdLCD(CLEAR_LCD);
                    cmdLCD(GOTO_LINE1_POS0);
                    StrLCD("Set Date (01-31)");
                    date=mykeyRead();
                    if(date>=1 && date<=31)
                    {
                            DOM=date;
                    }
                    else
                    {
                            cmdLCD(CLEAR_LCD);
                            cmdLCD(RET_CUR_HOME);
                            StrLCD("Out of Range");
                            cmdLCD(GOTO_LINE2_POS0);
                            StrLCD("Range (1 to 31)");
                            delay_ms(1000);
                            goto L1;
                    }
                    goto L1;

           case'8':fg2=1;
                   j=0;
                   fg1=1;
                   break;
           default: cmdLCD(CLEAR_LCD);
                    cmdLCD(RET_CUR_HOME);
                    StrLCD("Invalid choice");
                    cmdLCD(GOTO_LINE2_POS0);
                    StrLCD("Select 1 to 8");
                    delay_ms(1000);
                    goto L1;
   }
        }
        else
        {
                cmdLCD(CLEAR_LCD);
                cmdLCD(RET_CUR_HOME);
                StrLCD("Invalid choice");
                cmdLCD(GOTO_LINE2_POS0);
                StrLCD("select 1 or 2");
                delay_ms(1000);
                goto L2;
        }
         }
         else if(i==0)
         {
                 cmdLCD(CLEAR_LCD);
                 cmdLCD(RET_CUR_HOME);
                 StrLCD("0 attempts left");
                 cmdLCD(GOTO_LINE2_POS0);
                 StrLCD("permission denied!");
                 fg2=1;
                 fg1=1;
                 delay_ms(1000);
                         j=0;
         }
         else
         {
                 cmdLCD(CLEAR_LCD);
                 cmdLCD(RET_CUR_HOME);
                 StrLCD("Worng Password!");
                 cmdLCD(GOTO_LINE2_POS0);
                 U32LCD(i);
                 StrLCD(" Attempts left");
                 delay_ms(1000);
                 i--;
                 goto L4;
         }
         EXTINT=1<<1;
         VICVectAddr=0;
}

