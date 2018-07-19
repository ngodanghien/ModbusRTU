/**
**--------------DS18b20 Configuration -------------------------------------------------------------------------------
GPIO STM32F103C8T6 Support FT = PB2->15\{5}; PA8->15
**------------------------------------------------------------------------------------------------------*/
#include    "stm32f10x.h"
#include    "stm32f10x_rcc.h"
#include    "stm32f10x_gpio.h"
#include "MAX6675.h"

//User init ....
//CC NSS
#define Pin_NSS GPIO_Pin_12
#define GPIO_NSS GPIOB
#define GPIO_RCC_NSS RCC_APB2Periph_GPIOB
//CLK
#define Pin_CLK GPIO_Pin_13
#define GPIO_CLK GPIOB
#define GPIO_RCC_CLK RCC_APB2Periph_GPIOB
//MISO
#define Pin_MISO GPIO_Pin_14
#define GPIO_MISO GPIOB
#define GPIO_RCC_MISO RCC_APB2Periph_GPIOB


#define CS_pinON           GPIO_NSS->BSRR = Pin_NSS //GPIO_SetBits(GPIO_NSS,Pin_NSS)
#define CS_pinOFF					 GPIO_NSS->BRR = Pin_NSS //GPIO_ResetBits(GPIO_NSS,Pin_NSS)
#define SCK_pinON          GPIO_CLK->BSRR = Pin_CLK //GPIO_SetBits(GPIO_CLK,Pin_CLK)
#define SCK_pinOFF				 GPIO_CLK->BRR = Pin_CLK// GPIO_ResetBits(GPIO_CLK,Pin_CLK)
#define SO_pin             GPIO_ReadInputDataBit(GPIO_MISO,Pin_MISO) //MISO (GPIO_MISO->IDR & Pin_MISO) //



void MAX6675_init(void)
{
  //Configuration GPIO.... 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = Pin_CLK; //SCK
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_CLK, &GPIO_InitStructure);	//
	
	GPIO_InitStructure.GPIO_Pin = Pin_NSS; //NSS
  GPIO_Init(GPIO_NSS, &GPIO_InitStructure);	//
	
	GPIO_InitStructure.GPIO_Pin = Pin_MISO; //MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_MISO, &GPIO_InitStructure);	//
	//
	CS_pinON;
	SCK_pinON;//
}
void delay_ms(uint32_t us)	// cai nay se ko chuan ..../
{
	us=10000*us;	//12000 ~ 2ms
	while(--us);
}
uint8_t spiread(void) 
{ 
  int i;
  uint8_t d = 0;

  for (i=7; i>=0; i--)
  {
    SCK_pinOFF; //digitalWrite(sclk, LOW);
    delay_ms(1);
    if (SO_pin) //digitalRead(miso)) 
		{
      //set the bit to 0 no matter what
      d |= (1 << i);
    }

    SCK_pinON; //digitalWrite(sclk, HIGH);
    delay_ms(1);
  }

  return d;
}
double readCelsius(void) 
{

  uint16_t v;

  CS_pinOFF; //digitalWrite(cs, LOW);
  delay_ms(1);

  v = spiread();
  v <<= 8;
  v |= spiread();

  CS_pinON; //digitalWrite(cs, HIGH);

  if (v & 0x4) {
    // uh oh, no thermocouple attached!
    return -1; 
    //return -100;
	}
	v >>= 3;
  return v*0.25;
}
unsigned char MAX6657_get_ADC(unsigned long *ADC_data)
{
   unsigned char clk_pulses = 0x00;
   unsigned char samples = no_of_samples;
   unsigned long temp_data = 0x0000;
   unsigned long long avg_value = 0x00000000;
   
   
   while(samples > 0)
   {
         clk_pulses = no_of_pulses; //16
         temp_data = 0x0000;
         
         CS_pinOFF;
         
         while(clk_pulses > 0)
         {    
            temp_data <<= 1;
            if(SO_pin == 1)
            {
                temp_data |= 1;
            }
            SCK_pinON;
            SCK_pinOFF;
            
            clk_pulses--;
         };   
   
         CS_pinON;
         temp_data &= 0x7FFF;
         
         avg_value += ((unsigned long long)temp_data);
         
         samples--;
         delay_ms(10);
   };
   
   temp_data = ((unsigned long)(avg_value >> 4));// /16
   
   if((temp_data & 0x04) == close_contact)
   {
      *ADC_data = (temp_data >> 3);
      return close_contact;
   }
   
   else
   {
      *ADC_data = (count_max + 1);
      return open_contact;
   }
   
}

float MAX6675_get_T(unsigned long ADC_value, unsigned char T_unit)
{
   float tmp = 0.0;
   
   tmp = (ADC_value * scalar_deg_C);
   
   switch(T_unit)
   {
      case deg_F:
      {
         tmp *= scalar_deg_F_1;
         tmp += scalar_deg_F_2;
         break;
      }
      case tmp_K:
      {
        tmp += scalar_tmp_K;
        break;
      }
      default:
      {
        break;
      }
   }

   return tmp;
}
