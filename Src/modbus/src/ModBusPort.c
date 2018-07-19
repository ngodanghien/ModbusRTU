/********************************************************************************
  * @File   : ModbusPort.c
  * @Author : HIENCLUBVN
  * @Version: V0.1
  * @Date   : 25/09/2014
  * @Brief  :
  *******************************************************************************
  * @Attention:
  */
#include "ModBusRTU.h"
#include "ModBusFunc.h"

#include "stdint.h"

//extern __IO  uint8_t temp ;
 
#if(MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0)
// Dinh dang kieu du lieu cho COILs  .... ODR r/w 
MBCoilType MBCoils[REG_COILS_SIZE] = 
{
	&(GPIOB->ODR), 7, 
	&(GPIOB->ODR), 8,
	&(GPIOB->ODR), 12,
	&(GPIOB->ODR), 14,
	&(GPIOD->ODR), 8,
	&(GPIOD->ODR), 9,
	&(GPIOD->ODR), 10,
	&(GPIOD->ODR), 11,
};
#endif

#if(MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0)
// Kieu du lieu cho Input ...    IDR Read Only
MBDiscreteType MBDiscretes[REG_DISCRETE_SIZE] = 
{
	&(GPIOC->IDR), 3, 
	&(GPIOC->IDR), 4,
	&(GPIOC->IDR), 8,
	&(GPIOC->IDR), 11,
	&(GPIOA->IDR), 2,
	&(GPIOA->IDR), 8,
	&(GPIOA->IDR), 12,
	&(GPIOA->IDR), 15,
};
#endif

// Function code 4 (read input registers) Only READ ..... 
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0}; // =10

// Array save data of Funtion 03 Read usRegHoldingBuf (read holding registers) READ & WIRE (Use Configuration)
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = 	{0};//{0,1,2,3,4,5,6,7,8,9}; // =10

/* ------------------------------- Khai bao Queue -------------------------------*/
static eMBEventType eQueuedEvent;
static bool     xEventInQueue;

// Init Event ...
bool xMBPortEventInit( void ) {
    xEventInQueue = false;
    return true;
}

// Chuan bi Queue ....
bool xMBPortEventPost( eMBEventType eEvent ){
    xEventInQueue = true;
    eQueuedEvent = eEvent;
    return true;
}

//Dua vao Queue ... 
bool xMBPortEventGet( eMBEventType * eEvent ){
  
    bool xEventHappened = false;
    if( xEventInQueue ){
        *eEvent = eQueuedEvent;
        xEventInQueue = false;
        xEventHappened = true;
    }
    return xEventHappened;
}

/* ------------------------------- MBPortSerialEnable -----------------------------------*/
// Select RX + TX + DE RS485 .
void vMBPortSerialEnable( bool xRxEnable, bool xTxEnable ){
  
    ENTER_CRITICAL_SECTION();	// Ko can thiet...
    if( xRxEnable ){
        USART_RX_ENABLE();	// RXNE Enable ./
        RS485SWITCH_TO_RECEIVE();	// DE = ResetBits ()./
    }
    else{
       USART_RX_DISABLE();	// RX Disable
       RS485SWITCH_TO_SEND();	// DE = GPIO_SetBits ()
    }
    if( xTxEnable ){
       USART_TX_ENABLE();	// TC IE = Enable ...
    }
    else{
       USART_TX_DISABLE();	// TC IE = Disable . 
    }
    EXIT_CRITICAL_SECTION();	// Ko can thiet./
}

// Init USARTx + Config GPIO (DE RS485)
bool xMBPortSerialInit(uint32_t ulBaudRate){
 	
	bool  bInitialized = true;
	GPIO_InitTypeDef GPIO_InitStruct; 
	USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //UART IRQ
  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQN_N;	// USART2_IRQn
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	// ....
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //UART CLK
	RCC_APB2PeriphClockCmd(UART_GPIO_CLK        |      \
		                     RCC_APB2Periph_AFIO,        \
	                       ENABLE);
  if(UART == USART1)	
		RCC_APB2PeriphClockCmd(UART_CLK, ENABLE);	// USART 1
	else
		RCC_APB1PeriphClockCmd(UART_CLK, ENABLE);	// USART 2,3,4..
	
  //UART GPIO	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UART_TX_PIN;	// PA2
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART_GPIO, &GPIO_InitStruct);	// GPIOA

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStruct.GPIO_Pin = UART_RX_PIN;	// PA3
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART_GPIO, &GPIO_InitStruct);
	
	// Config DE RS485 ..........................................DE........RS485./
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = RS485PIN;
	GPIO_Init(RS485GPIO, &GPIO_InitStruct); // GPIOA
  
	//UART Config
	USART_InitStructure.USART_BaudRate = ulBaudRate;	// 38400 ( Test thu 115200 ? )
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART, &USART_InitStructure);
	USART_Cmd(UART, ENABLE);
	
	return bInitialized;
}
// USART2_IRQHandler ........
void UART_IQR_HANDLER(void){
	if(USART_GetITStatus(UART, USART_IT_TC)){	// USART2 + USART_IT_TC
	  xMBRTUTransmitFSM();	//Init + Transmit ....
	}
	else if(USART_GetITStatus(UART, USART_IT_RXNE)){	// USART2 + RXNE (Recvice)
		xMBRTUReceiveFSM();		// Enable TIME + RCV Data (Max=256)
	}
}

/* ----------------------------- TIM3 -----------------------------------*/
//TIM3 using Interrupt ~ 1.750ms ...
void xMBPortTimersInit(uint16_t usTim1Timeout50us ){
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIME_IRQN_N;	// TIM3_IRQn 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  if(TIEM == TIM1 || TIEM == TIM8 || TIEM == TIM9)
		RCC_APB2PeriphClockCmd(TIME_CLK, ENABLE);
	else		
		RCC_APB1PeriphClockCmd(TIME_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = CPU_CLK - 1;               //72 - 1
  TIM_TimeBaseStructure.TIM_Prescaler = usTim1Timeout50us * 50;	//35*50 = 1750
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		    //0
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //Up
  TIM_TimeBaseInit(TIEM, &TIM_TimeBaseStructure);
	// Dung chuan 1.751us ( ~ 1.7ms == 3.5 lan Ky tu )
  TIM_ClearFlag(TIEM, TIM_FLAG_Update);                         //
}

// Enable Time .... 3.5
void vMBPortTimersEnable( ){
  TIM_ClearFlag(TIEM, TIM_FLAG_Update);                         //
  TIM_SetCounter(TIEM,0x00);			                              //Start at 0x0
  TIM_ITConfig(TIEM,TIM_IT_Update,ENABLE);	// Using Interrupts./
  TIM_Cmd(TIEM,ENABLE);	// Init ...
}

// Disable TIM3 .....
void vMBPortTimersDisable( ){
  TIM_ITConfig(TIEM,TIM_IT_Update,DISABLE);
  TIM_Cmd(TIEM,DISABLE);
}

//TIM3_IRQHandler ....
void TIME_IQR_HANDLER( void ){	// Over TIM3 ( 3.5 Char ~ 1.7ms )
  
  if(eRcvState == STATE_RX_RCV){		// Frame is beeing received
    xMBPortEventPost( EV_FRAME_RECEIVED );	// xEventInQueue = true + eQueuedEvent = EV_FRAME_RECEIVED <=> Frame received./
  }
  vMBPortTimersDisable();	// Disable TIM3 
  eRcvState = STATE_RX_IDLE;	// Receiver is in idle state. ( Nhan roi~ )
}


