/**
**--------------DS18b20 Configuration -------------------------------------------------------------------------------
GPIO STM32F103C8T6 Support FT = PB2->15\{5}; PA8->15
**------------------------------------------------------------------------------------------------------*/
#include    "stm32f10x.h"
#include    "stm32f10x_rcc.h"
#include    "stm32f10x_gpio.h"

//User init ....
#define GPIO_Pin_DS GPIO_Pin_8
#define GPIO_Port_DS GPIOA
#define GPIO_RCC_DS RCC_APB2Periph_GPIOA
// End Init

#define DS18B20_DQ_High    GPIO_SetBits(GPIO_Port_DS,GPIO_Pin_DS)  //PC1   DQ=1	  
#define DS18B20_DQ_Low     GPIO_ResetBits(GPIO_Port_DS,GPIO_Pin_DS)//PC1   DQ=0
#define DS18B20_DQ_IN     GPIO_ReadInputDataBit(GPIO_Port_DS,GPIO_Pin_DS)  //ÊäÈëDQ

   	
u8 DS18B20_Init(void);//³õÊ¼»¯DS18B20
short DS18B20_Get_Temp(void);//»ñÈ¡ÎÂ¶È
void DS18B20_Start(void);//¿ªÊ¼ÎÂ¶È×ª»»
void DS18B20_Write_Byte(u8 dat);//Ð´ÈëÒ»¸ö×Ö½Ú
u8 DS18B20_Read_Byte(void);//¶Á³öÒ»¸ö×Ö½Ú
u8 DS18B20_Read_Bit(void);//¶Á³öÒ»¸öÎ»
u8 DS18B20_Check(void);//¼ì²âÊÇ·ñ´æÔÚDS18B20
void DS18B20_Rst(void);//¸´Î»DS18B20   
void DS18B20_test(void);

void delay_us(__IO uint32_t num);


void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);

void delay_us(uint32_t us)	// cai nay se ko chuan ..../
{
	us=10*us;
	while(--us);
}


// RESET PULSE ...
void DS18B20_Rst(void)	   
{                 
	  DS18B20_IO_OUT(); // OUT
    DS18B20_DQ_Low; // BUS = LOW
    delay_us(750);    // Min = 480uS
		DS18B20_DQ_High; // Bus = High
		delay_us(15);     //waiting DS18B20 ... 15-60uS 
}
//CHECK .... (Doan nay` co van de`....)
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();// cofiguration input
	
	// 
  while (DS18B20_DQ_IN&&retry<200) //GPIO_ReadInputDataBit
	{
		retry++;
		delay_us(1);
	};	 
	
	if(retry>=200)return 1;	//wait OK
	else retry=0;
	//waiting DS18B20 = HIGH ~ 60-240us 
  while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    //OK
	return 0;
}
//´ÓDS18B20¶ÁÈ¡Ò»¸öÎ»
//·µ»ØÖµ£º1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT();// Output 
    DS18B20_DQ_Low; // Bus Low
	delay_us(2);	// ~1uS
    DS18B20_DQ_High; // Bus High
	
	DS18B20_IO_IN();//INPUT
	delay_us(12);	// ~15uS
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);   //~45uS        
    return data;
}
// Read 1 Byte From DS18B20
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//Write Data (BYTE) to DS18B20
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET OUTPUT;
	 
  for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) // Write 1
        {
            DS18B20_DQ_Low;// Bus = LOW
            delay_us(2);   // Time Recovery ~ 1uS           	              
            DS18B20_DQ_High; // Bus = High (60-120uS)
            delay_us(60);             
        }
        else  // Write 0
        {
            DS18B20_DQ_Low; // Bus = 0
            delay_us(60);   // ~ 60-120uS          
            DS18B20_DQ_High; // Bus = High
            delay_us(2);     // ~ 1uS                     
        }
    }
}
//Start Convert
void DS18B20_Start(void) // RST + CHECK + 0xCC + 0x44
{   						               
    DS18B20_Rst();	   // OK
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
// Init .... 	 
u8 DS18B20_Init(void) // RCC + BUS = HIGH + RST + CHECK()
{
	RCC_APB2PeriphClockCmd(GPIO_RCC_DS,ENABLE);
    DS18B20_IO_OUT(); // GPIO OUT
    DS18B20_DQ_High; // High
	DS18B20_Rst(); // Fix
	return DS18B20_Check(); // Ko can

}  

void DS18B20_IO_IN(void) // GPIO INPUT
 {
    GPIO_InitTypeDef  GPIO_InitStructure; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_DS; //DQ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_Init(GPIO_Port_DS, &GPIO_InitStructure);	
  }

void DS18B20_IO_OUT(void) // GPIO OUTPUT
 {
	GPIO_InitTypeDef  GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_DS;//DQ  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_Port_DS, &GPIO_InitStructure);
	 
  }
//Read Temp....
//º0.1C
//-550~1250 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
	DS18B20_Start ();                    // ds1820 start convert : RST + Check + 0xCC + 0x44 (Skip + Start)
	
    DS18B20_Rst();	//RST agian
    DS18B20_Check();	// Check Again
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// Read Scratchpad .../	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	    	  
    if(TH>7)	//check Am + Duong thoi.../ Byte High = 1111 thi` temp < 0
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//ÎÂ¶ÈÎª¸º  
    }else temp=1;//ÎÂ¶ÈÎªÕý	  	  
    tem=TH; //»ñµÃ¸ß°ËÎ»
    tem<<=8;    
    tem+=TL;//»ñµÃµ×°ËÎ»
    tem=(float)tem*0.625;// 1/16=0.0625
	if(temp)return tem; //·µ»ØÎÂ¶ÈÖµ
	else return -tem;    
} 
