/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			24C02.h
** Descriptions:		24C02 操作函数库 
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-10-29
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/
#ifndef __24C02_H
#define __24C02_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
#define _I2C2

#ifdef _I2C1
	#define I2C_SCL GPIO_Pin_6 //I2C1 
	#define I2C_SDA GPIO_Pin_7 //PB6+7
#else	//I2C2
	#define I2C_SCL GPIO_Pin_10 //I2C1
	#define I2C_SDA GPIO_Pin_11 //PB10+11
#endif



#define SCL_H         GPIOB->BSRR = I2C_SCL	 /* GPIO_SetBits(GPIOB , GPIO_Pin_6)   */
#define SCL_L         GPIOB->BRR  = I2C_SCL   /* GPIO_ResetBits(GPIOB , GPIO_Pin_6) */
   
#define SDA_H         GPIOB->BSRR = I2C_SDA	 /* GPIO_SetBits(GPIOB , GPIO_Pin_7)   */
#define SDA_L         GPIOB->BRR  = I2C_SDA	 /* GPIO_ResetBits(GPIOB , GPIO_Pin_7) */

#define SCL_read      GPIOB->IDR  & I2C_SCL   /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6) */
#define SDA_read      GPIOB->IDR  & I2C_SDA	 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) */

#define I2C_PageSize  8  /* 24C02每页8字节 */

#define ADDR_24LC02		0x50//0xA0

/* Private function prototypes -----------------------------------------------*/
void I2C_Configuration(void);
FunctionalState I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress, uint8_t DeviceAddress);
FunctionalState I2C_ReadByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress);

#endif 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
