#include "OneWire.h"
#include    "stm32f10x_gpio.h"
#include    "stm32f10x_rcc.h"

void Onewire_Enable_GPIO_Port(void)
{
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
}

void Onewire_OUT_PULL_UP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Onewire_OUT_FLOATING(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Onewire_IN_FLOATING(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Onewire_IN_PULL_UP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void delay_us(u16 us)
{
	us=15*us;
	while(--us);
}

/* Reset */
void resetOnewire(void) 
{
	Onewire_OUT_PULL_UP();
	CLR_DQ();
	delay_us(450);
	SET_DQ();
	delay_us(60);
	Onewire_IN_PULL_UP(); 
	delay_us(10);
	while(!(GET_DQ()));
	Onewire_OUT_PULL_UP();
	SET_DQ();
}

/* Read */
u8 rOnewire(void)
{
    u8 data=0,i=0;
    for(i=0;i<8;i++)
    {
	Onewire_OUT_PULL_UP();
	CLR_DQ();
	data=data>>1;
	SET_DQ();
	Onewire_IN_PULL_UP();
	delay_us(8);
	if(GET_DQ())data|=0x80;
	Onewire_OUT_PULL_UP();
	SET_DQ();
	delay_us(60);
    }
    return(data);
}

/* Write */
void wOnewire(u8 data)
{
	u8 i=0;
	Onewire_OUT_PULL_UP();
	SET_DQ();
	delay_us(16);
    	for(i=0;i<8;i++)
    	{
		CLR_DQ();
		if(data&0x01)
		{
			SET_DQ();
		}
   	 	else
   	 	{
			CLR_DQ();
    		}
    		data=data>>1;
		delay_us(40);  //65
		SET_DQ();
    }
}
