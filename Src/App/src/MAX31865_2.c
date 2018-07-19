#include  "stm32f10x.h"
#include "stm32f10x_spi.h"
#include  "stm32f10x_gpio.h"
#include "MAX31865.h"
#include <math.h>

void delay( uint32_t num) //ms
{
	num *=8000; //Chuan ...ms
	while(num--);
}
// Send 2 BYTE - quan tam den BYTE dau`; READ Byte cuoi.
uint8_t readRegister8(uint8_t addr) {
  //Write 1 && Read 1
  uint8_t ret;
  addr &= 0x7F;
	
	 //((uint16_t)0x0001)‬; //~GPIO_Pin_12; //reset
	//GPIOB->ODR &= ~GPIO_Pin_12;
  //SPI2->CR1 |= CR1_SPE_Set; //SPI_Cmd(SPI2, ENABLE);
	
	GPIOB->ODR &= ~GPIO_Pin_12;
  SPI2->CR1 |= CR1_SPE_Set; //SPI_Cmd(SPI2, ENABLE);
	
	  
  /* Send SPI2 data 1 BYTE */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET){} 
  SPI2->DR = addr; //SPI_I2S_SendData(SPI2, addr); 
  //Ko quan tam ....
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_RXNE)==RESET){}
  ret = SPI2->DR; //ko quan tam.
  
  /* Send SPI2 data 2 BYTE = ko quan tam */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_TXE)==RESET){}
  SPI2->DR = 0x7F; //SPI_I2S_SendData(SPI2, addr); 
  /* Read SPIy received data */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_RXNE)==RESET){}
  ret = SPI2->DR; //Nhan ve.

  SPI2->CR1 &= CR1_SPE_Reset;//SPI_Cmd(SPI2, DISABLE);
	GPIOB->ODR |= GPIO_Pin_12; //set
  
  return ret;
}
//Write 2 BYTE (ko quan tam Read)....
uint8_t writeRegister8(uint8_t addr, uint8_t data) {
  uint8_t ret;
	//uint16_t n=500; 
	//Start SPI2
	GPIOB->ODR &= ~GPIO_Pin_12; //reset
  SPI2->CR1 |= CR1_SPE_Set; //SPI_Cmd(SPI2, ENABLE);
  /* Send SPI2 2 data */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET){} 
  SPI2->DR = addr|0x80; //SPI_I2S_SendData(SPI2, addr);
	//while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_RXNE)==RESET){}
  ret = SPI2->DR; //Nhan ve.
	
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET){} 
  SPI2->DR = data; //SPI_I2S_SendData(SPI2, addr);
	//while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_RXNE)==RESET){}
  ret = SPI2->DR; //Nhan ve.
  //End SPI2
  SPI2->CR1 &= CR1_SPE_Reset;//SPI_Cmd(SPI2, DISABLE);
	GPIOB->ODR |= GPIO_Pin_12; //set
	return ret; 
}

uint16_t readRegister16(uint8_t addr) {
	uint16_t ret;
  ret = readRegister8(addr)<<8; 
	ret |= readRegister8(addr+1); 
  return ret;
}

void MAX31865_3WIRE_Init(void) //MAX31865_3WIRE = 1,
{  
  //0001.0010 = BIAS(ON)+CONV(AUTO)+1-SHOT(0)+3WIRE(1)..FAULT_DET(00)+FAULT_STA(Clear)+60Hz;/
  //setWires(wires);   //MAX31856_CONFIG_3WIRE=0x10 =‭00010000‬
  
  //BIN[0][7F][C0][7F][80][7F][][][][]
	//uint8_t n=255;
  uint8_t t = readRegister8(MAX31856_CONFIG_REG); //Read .... [0][7F]=>[0]
	//while(n--);
  t |= MAX31856_CONFIG_3WIRE; //MAX31856_CONFIG_3WIRE=0x10 
  writeRegister8(MAX31856_CONFIG_REG, t); //Write ...[80][10] =>[]
	//while(n--);
  
  //enableBias(false); //clear bit : MAX31856_CONFIG_BIAS=0x80=‭10000000‬
  t = readRegister8(MAX31856_CONFIG_REG); 
  t &= ~MAX31856_CONFIG_BIAS;       // disable bias
  writeRegister8(MAX31856_CONFIG_REG, t);
  
  //autoConvert(false); //clear MAX31856_CONFIG_MODEAUTO = 0x40=‭01000000‬
  t = readRegister8(MAX31856_CONFIG_REG);
  t &= ~MAX31856_CONFIG_MODEAUTO;       // disable autoconvert
  writeRegister8(MAX31856_CONFIG_REG, t);
  
  //clearFault(); //MAX31856_CONFIG_FAULTSTAT = 0x02=‭00000010‬
  t = readRegister8(MAX31856_CONFIG_REG);
  t &= ~0x2C; //reset 0x2C = ‭00101100‬ (~0x2C)=1101.0011 | 10 = 1101.0011
  t |= MAX31856_CONFIG_FAULTSTAT; //0x02 ==>  xx0x.00xx
  writeRegister8(MAX31856_CONFIG_REG, t);
} 
uint16_t MAX31865_readRTD (void) {
  //clearFault();
	uint16_t rtd;
  uint8_t t = readRegister8(MAX31856_CONFIG_REG);
  t &= ~0x2C; //reset 0x2C = ‭00101100‬ (~0x2C)=1101.0011 | 10 = 1101.0011
  t |= MAX31856_CONFIG_FAULTSTAT; //0x02 ==>  xx0x.00xx
  writeRegister8(MAX31856_CONFIG_REG, t);
  
  //enableBias(true);
  t = readRegister8(MAX31856_CONFIG_REG);
  t |= MAX31856_CONFIG_BIAS;       // enable bias
  writeRegister8(MAX31856_CONFIG_REG, t);
  
  delay(1); //delay(10); //6
  t = readRegister8(MAX31856_CONFIG_REG);
  t |= MAX31856_CONFIG_1SHOT;      
  writeRegister8(MAX31856_CONFIG_REG, t);
  delay(1);//delay(60);

  //Read...
  rtd = readRegister16(MAX31856_RTDMSB_REG);

  // remove fault
  rtd >>= 1;

  return rtd;
}
float MAX31865_temperature(float RTDnominal, float refResistor) {
  // http://www.analog.com/media/en/technical-documentation/application-notes/AN709_0.pdf

  float Z1, Z2, Z3, Z4, Rt, temp;
  float rpoly;
		
  Rt = MAX31865_readRTD();
  Rt /= 32768;
  Rt *= refResistor;
  
  // Serial.print("\nResistance: "); Serial.println(Rt, 8);

  Z1 = -RTD_A;
  Z2 = RTD_A * RTD_A - (4 * RTD_B);
  Z3 = (4 * RTD_B) / RTDnominal;
  Z4 = 2 * RTD_B;

  temp = Z2 + (Z3 * Rt);
  temp = (sqrt(temp) + Z1) / Z4;
  
  if (temp >= 0) return temp;

  // ugh.
  Rt /= RTDnominal;
  Rt *= 100;      // normalize to 100 ohm

  //float rpoly = Rt;
  rpoly = Rt;
	
  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt;  // square
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt;  // ^3
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt;  // ^4
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt;  // ^5
  temp += 1.5243e-10 * rpoly;

  return temp;
} //End
