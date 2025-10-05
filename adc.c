#include <LPC21xx.h>
#include "types.h"
#include "delay.h"
#include "adc.h"
#include "adc_defines.h"

void Init_ADC(void)
{
	ADCR=(1<<PDN_BIT)|(CLKDIV<<CLKDIV_BITS);
}

void Read_ADC(u8 chNo,u32 *adcDVal,f32 *eAR)
{
  //clear adc previous channel	
	ADCR&=0xFFFFFF00;
	//set adc new channel
	ADCR|=1<<chNo;
	//initiate sampling & conversion
	ADCR |=1<<ADC_START_CONV_BIT;
	//wait for coversion time >=2.44us
	delay_us(3);
	//check if converstion it completed
	while(((ADDR>>DONE_BIT)&1)==0);
	//stop conversion
	ADCR&=~(1<<ADC_START_CONV_BIT);
	//read/extract 10-bit digital data
	*adcDVal=((ADDR>>ADCDVAL_BITS)&1023);
	//convert to equivalent float representation
	*eAR=((3.3/1023)*(*adcDVal));
}
