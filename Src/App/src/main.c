#include    "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include    "stm32f10x_rcc.h"
#include    "stm32f10x_gpio.h"
#include    "stm32f10x_adc.h"
#include    "stm32f10x_dma.h"
#include 		"stm32f10x_iwdg.h"
#include 		"stm32f10x_usart.h"
#include 		"stm32f10x_spi.h"

#include 	"ModbusRTU.h"

/* Private define ------------------------------------------------------------*/
	
#define RL2_OFF GPIO_SetBits(GPIOB,GPIO_Pin_8)	// Ngoai` mep = Nhiet >70 do C./
#define RL2_ON GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define RL1_OFF GPIO_SetBits(GPIOB,GPIO_Pin_9)	// Dong dien (Ap) lon hon Nguong.
#define RL1_ON GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define CR1_SPE_Set          ((uint16_t)0x0040)
#define CR1_SPE_Reset        ((uint16_t)0xFFBF)

// ------------------------------------------------IFDEFENIE ----------------------
//#define BOARD_USER_01 // 

#define Vrefint  4.083 //96
#define HesoA  14.813
#define HesoB  -0.7405

#define R79 240.0	//Ohm 1%
#define LR (4.0*R79/1000.0)
#define HR (20.0*R79/1000.0)
#define TMPConvert(x) (x-LR)*(200/(HR-LR))

#define R27 100	//100k
#define R30 5	//5k
#define Vin(x) (x*(R27+R30)/R30)	//x=Vout
#define OverTemp 70 //70 do C
#define OverLoad 49 //50A do C

__IO uint16_t nCountAmpe =0,nCountTemp=0; 

#define NUMCHANNEL 3
//__IO uint16_t ADC_ConvertedValue[NUMCHANNEL];  // Votl + Ampe + Temp

// Main
//__IO float tempVolt = 0,tempAmpe=0,realVolt=0,realVolt2=0,realAmpe=0,tempNhiet=280,tempNhiet2,tempNhiet3;
//__IO uint16_t nCountTemp=0, nCountAmpe = 0; 
//__IO uint8_t over70=0, over50A =0; 
//uint32_t nCountReset =0; 
//uint8_t check6s = 0,OKRead=0,OKRead2=0; 


/* Private function prototypes -----------------------------------------------*/
void Delay( uint32_t num);
void Delay_us(__IO uint32_t num);
void GPIO_Configuration(void);
void IWDG_Configuration(void);
void SPI1_Configuration(void);
//void SPI2_Configuration(void);
//void ADC_Configuration(void);

/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SPI1 Read Voltage at Channel x
*******************************************************************************/
/*
MCP3208 8 channel
Configuration bit [00000][Start][Single/Diff][D2]----[D1][D0][6x] ---[8x]
Select Channal Single-ended: (Send Command)
-------------------------------------------
Channel x    BYTE0	BYTE1	BYTE2
				0			0x06		0x00			X (don't care)
				1			0x06		0x40			X (don't care)
				2			0x06		0x80			X (don't care)
				3			0x06		0xC0			X (don't care)

				4			0x07		0x00			X (don't care)
				5			0x07		0x40			X (don't care)
				6			0x07		0x80			X (don't care)
				7			0x07		0xC0			X (don't care)
----------------------------------------------
Read Data ADC 12bit
			BYTE0				 BYTE1										BYTE2
				X			xxxx[11][10][9][8]		[7][6][5][4][3][2][1][0]
(uint16_t)Result = (((uint16_t)BYTE1&0x0F)<<8) | BYTE2 ; // Finish 
*/
#define SAMPLE 1
//uint16_t SAMPLE = 300; 
float ReadVoltSPI1(uint8_t channel)
{
	uint8_t i,Buffer_Rx[3]={0};
	uint8_t code1=0;
	uint16_t j,ret=0;
	float sumVolt=0;
	
switch (channel)
	{
		case 0: code1 = 0x00; break;
		case 1: code1 = 0x40; break;
		case 2: code1 = 0x80; break;
		case 3: code1 = 0xC0; break;		
		case 4: code1 = 0x00; break;
		case 5: code1 = 0x40; break;
		case 6: code1 = 0x80; break;
		case 7: code1 = 0xC0; break;
		
	}
	for (j=0; j<SAMPLE;j++) //
	{
		SPI1->CR1 |= CR1_SPE_Set; //SPI_Cmd(SPI1, ENABLE);
		GPIOA->ODR &= ~GPIO_Pin_4; //A4
		for (i=0;i<3;i++) //NOTE
		{
			
			//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			while((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET){} 
			/* Send SPI1 data */
			if (i==0) SPI_I2S_SendData(SPI1, 0x06);
			if (i==1) SPI_I2S_SendData(SPI1, code1); //40=channel 1; 00=channel0
			if (i==2) SPI_I2S_SendData(SPI1, 0x00);

			/* Wait for SPIy data reception */
			//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
			while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET){}
			/* Read SPIy received data */
			Buffer_Rx[i] = SPI_I2S_ReceiveData(SPI1);
		} 
		GPIOA->ODR |= GPIO_Pin_4; //A4
		SPI1->CR1 &= CR1_SPE_Reset;//SPI_Cmd(SPI1, DISABLE);

		ret =  ((((uint16_t)Buffer_Rx[1]&0x0F)<<8) | Buffer_Rx[2]);
		sumVolt += ret; 	
		//Delay(1); //20*10=100
	}
	sumVolt/=SAMPLE; //Volt TB 
	
	sumVolt *= Vrefint/4096;
	return sumVolt; 
}
//------------------------Max31865 + MCP3208 ---------------------------------------------------
void SPI1_Configuration(void) //For Max31865 + MCP3208
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 |RCC_APB2Periph_AFIO, ENABLE);
	
	/* Configure SPI_MASTER pins: SCK and MOSI ---------------------------------*/
  /* Configure SCK and MOSI pins as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //SCK|MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//GPIOA->ODR &= ~GPIO_Pin_5; 
	/* Configure MISO pins as Input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;//;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4); //GPIO_NSS1
	
	/* SPI_MASTER configuration ------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 8bit
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//_32 thi` PC410L bat dau lam viec (~2.4Mhz)
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
	/* Enable SPI_MASTER TXE interrupt */
  //SPI_I2S_ITConfig(SPI_MASTER, SPI_I2S_IT_TXE, ENABLE);
  /* Enable SPI_SLAVE RXNE interrupt */
  //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
   
  /* Enable SPI_SLAVE */
  //SPI_Cmd(SPI1, ENABLE);
}

/*******************************************************************************
* Function Name  : Main ......
*******************************************************************************/
__IO uint8_t nFlag =0;
__IO uint32_t nTime=0;
__IO float tmpTemp=0, TempSUM=0, realTemp=0;
__IO float tmpAmpe=0, AmpeSUM=0, realAmpe=0;
__IO float tmpVolt=0, VoltSUM=0, realVolt=0;
#define SAMPE 1000
int main(void)
{
	// ------------------ Config & Init ...........
	SysTick_Config(SystemCoreClock / 1000); // /100000=10uS=50Khz; /10000=>Width=0.1ms=>F=5kHz; /1000=1ms=500Hz; /100=10ms=50Hz
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) 	RCC_ClearFlag();
	 
	IWDG_Configuration(); //5s reset if ERROR = OK
	GPIO_Configuration(); 
	SPI1_Configuration(); //MCP3208+MAX31865 (Same Configuration) 
	
	eMBInit(0x01, 9600);	// Address = 0x01 + Baudrate = 9600bps + DE RS485.
	Delay(1000);
	
	usRegHoldingBuf[9]= usRegHoldingBuf[10]= 0; 
	
	// ------------------ FOREVER ........................		
	while(1)  
	{
		if (nFlag)
		{
			nFlag =0; 
			nTime++;
			eMBPoll();  
			if (nTime%200==0) GPIOB->ODR ^= (1<<9); 	//Toggle LED.	
			
			TempSUM += ReadVoltSPI1(2); //Nhiet do
			AmpeSUM += ReadVoltSPI1(0); // Ampe
			VoltSUM += ReadVoltSPI1(3); // Volt
			if (nTime ==SAMPE) 
			{
				//reset + save data ....
				nTime = 0; 
				/* Calculator Nhiet do */
				tmpTemp = TempSUM/SAMPE; 
				TempSUM =0;
				realTemp = (tmpTemp-0.96)*520.8333333; //Da *10;; //view	
				usRegHoldingBuf[14] = realTemp;
				
				/* Calculator Ampe */
				tmpAmpe = AmpeSUM/SAMPE; 
				AmpeSUM =0;
				realAmpe = tmpAmpe/0.08*100; //view	
				usRegHoldingBuf[13] = realAmpe;
				/* Calculator Dien ap*/
				tmpVolt = VoltSUM/SAMPE; 
				VoltSUM =0;
				realVolt = (21.013*tmpVolt+1.1162)*99.586776;//*99.1668028;
				usRegHoldingBuf[12] = realVolt;
				// Public 
				//3. Du vao Volt de tinh toan .... |0-36-42|42-48-52|52-60-100|-------------------------------------
				if (realVolt>=5200) //Volt>52V (60V)
				{
					//check6s =0; nCountReset=0;  
					usRegHoldingBuf[0]=usRegHoldingBuf[1]=usRegHoldingBuf[2]=usRegHoldingBuf[3]=usRegHoldingBuf[4]=usRegHoldingBuf[5] = 0;		
					
					usRegHoldingBuf[6] = realVolt;
					usRegHoldingBuf[7] = realAmpe;
					usRegHoldingBuf[8] = realTemp;	
				}
				//
				if ((realVolt>=4200)&&(realVolt<5200)) //42<Volt<53 (48V)
				{
						//check6s =0; nCountReset=0;
						usRegHoldingBuf[0]=usRegHoldingBuf[1]=usRegHoldingBuf[2]=usRegHoldingBuf[6]=usRegHoldingBuf[7]=usRegHoldingBuf[8] = 0;
						
						usRegHoldingBuf[3] = realVolt;// _realVolt48;
						usRegHoldingBuf[4] = realAmpe;
						usRegHoldingBuf[5] = realTemp;
				}
				//
				if ((realVolt>=300)&&(realVolt<4200)) // 3<Volt<40 (36V)
				{
						usRegHoldingBuf[3]=usRegHoldingBuf[4]=usRegHoldingBuf[5]=usRegHoldingBuf[6]=usRegHoldingBuf[7]=usRegHoldingBuf[8] = 0;  
					
						usRegHoldingBuf[0] = realVolt;//_realVolt;
						usRegHoldingBuf[1] = realAmpe;
						usRegHoldingBuf[2] = realTemp;

					
				}
				if (realVolt<300) //Volt<2V (<300) (2V)
				{
					// Reset + Calib ..../
					usRegHoldingBuf[0]=usRegHoldingBuf[1]=usRegHoldingBuf[2]=usRegHoldingBuf[3]=usRegHoldingBuf[4] = 0;
					usRegHoldingBuf[5]=usRegHoldingBuf[6]=usRegHoldingBuf[7]=usRegHoldingBuf[8] = 0;  
				}
			}	
		}
		//5s = reset
		IWDG_ReloadCounter();
	}
}
/*******************************************************************************
* Function Name  : IWDG_Configuration
*******************************************************************************/
void IWDG_Configuration(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 	/* ??0x5555,????????????? */
  IWDG_SetPrescaler(IWDG_Prescaler_256);            /* ??????256?? 40K/256=156HZ(6.4ms) */ 
  /* ?????????? */
  //IWDG_SetReload(781);							    /* ???? 5s/6.4MS=781 .??????0xfff*/
	IWDG_SetReload(1562); // 312 ~ 2s // 1562 ~10s
  IWDG_ReloadCounter();								/* ??*/
  IWDG_Enable(); 									/* ?????*/
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Attention		 : None
*******************************************************************************/
void GPIO_Configuration(void)	// All are Analog for Volt + Ampe./
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
  						 				
  /* Analog ... */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Run Led.. */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

void Delay( uint32_t num)
{
	num *= 10000; //12000; //Chuan 1ms voi ban toan nay`
	while(num--);
}

void Delay_us(__IO uint32_t num) //1m
{
	__IO uint32_t index = 0;

	/* default system clock is 72MHz */
	for(index = (7* num); index != 0; index--)
	{
	}
}

void SysTick_Handler(void)
{
	//eMBPoll(); //system Tick ~10ms then UART ~4.5ms tra ban tin ModbusRTU
	nFlag = 1; 
}
