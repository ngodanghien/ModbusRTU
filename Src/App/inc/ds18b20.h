
/**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: ds18b20.h
**创   建   人: 王云飞
**最后修改日期: 2011年5月20日
**描        述: 
** 固件库信息：V2.03             
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 王云飞
** 版  本: v0.01
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 王云飞
** 版  本: v0.01
** 日　期:  2011年5月20日
** 描　述: 当前版本
**
**------------------------------------------------------------------------------------------------------*/

#ifndef __DS18B20_H
#define __DS18B20_H 

#include "stm32f10x.h"


////IO方向设置   也可以这样用
//#define DS18B20_IO_IN()  {GPIOC->CRL&=0x000000F0;GPIOC->CRL|=0x00000040;} //pC1	  浮空输入模式
//#define DS18B20_IO_OUT() {GPIOC->CRL&=0x000000F0;GPIOC->CRL|=0x00000030;} //PC1 通用推挽输出，50MHZ



//IO操作函数	 
#define DS18B20_DQ_High    GPIO_SetBits(GPIOC,GPIO_Pin_1)  //PC1   DQ=1	  
#define DS18B20_DQ_Low     GPIO_ResetBits(GPIOC,GPIO_Pin_1)//PC1   DQ=0
#define DS18B20_DQ_IN     GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)  //输入DQ
   	
u8 DS18B20_Init(void);//初始化DS18B20
short DS18B20_Get_Temp(void);//获取温度
void DS18B20_Start(void);//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);//读出一个字节
u8 DS18B20_Read_Bit(void);//读出一个位
u8 DS18B20_Check(void);//检测是否存在DS18B20
void DS18B20_Rst(void);//复位DS18B20   
void DS18B20_test(void);


void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);

#endif


















