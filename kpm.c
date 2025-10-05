#include <LPC21xx.h>
#include "kpm_defines.h"
#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
const int Kpmlut[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
void Initkpm(void)
{
        IODIR1|=15<<ROW0;
}
u32 colScan(void)
{
        u32 t;
        t=(((IOPIN1>>COL0)&15)<15)?0:1;
        return t;
}
u32 rowCheck(void)
{
        u32 rno;
        for(rno=0;rno<4;rno++)
        {
                IOPIN1=((IOPIN1 & ~(15<<ROW0))|(~((1<<rno)<<ROW0)));
                if(colScan()==0)
                {
                        break;
                }
        }
        IOCLR1=(15<<ROW0);
        return rno;
}
u32 colCheck(void)
{
        u32 cno;
        for(cno=0;cno<4;cno++)
        {
                if(((IOPIN1>>(COL0+cno))&1)==0)
                {
                        break;
                }
        }
        return cno;
}
u32 keyScan(void)
{
        u32 Rno,Cno,Keyv;
        while(colScan());
                Rno=rowCheck();
                Cno=colCheck();
                Keyv=Kpmlut[Rno][Cno];

                return Keyv;
}
u32 readnum(void)
{
        u32 sum=0,i=0;
        s8 key;
        while(1)
        {
                key=keyScan();
                cmdLCD(GOTO_LINE2_POS0);
                charLCD(key);
                if((key>='0')&&(key<='9'))
                {
                        sum=(sum*10)+(key-'0');
                        cmdLCD((GOTO_LINE2_POS0)+i);
                        U32LCD(sum);
                        i++;
                        while(colScan()==0);
                }
                else
                {
                        while(colScan()==0);
                        break;
                }
        }
        return sum;
}
u32 mykeyRead(void)
{
        u32 sum=0;
        s8 key;
        while(1)
        {
                key=keyScan();
                if((key>='0')&&(key<='9'))
                {
                        sum=((sum*10)+(key-48));
                        cmdLCD(GOTO_LINE2_POS0);
                        U32LCD(sum);
                        while(colScan()==0);
                }
                else if(key=='-')
                {
                        cmdLCD(GOTO_LINE2_POS0);
                        StrLCD("                ");
                        sum=sum/10;
                        cmdLCD(GOTO_LINE2_POS0);
                        U32LCD(sum);
                        while(colScan()==0);
                }
                else if(key=='=')
                {
                        while(colScan()==0);
                        break;
                }
        }
        return sum;
}

