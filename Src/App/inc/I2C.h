#ifndef __I2C_H
#define __I2C_H

void I2C_Configuration(void);
void I2C_ReadS_24C(u8 addr ,u8* pBuffer,u16 no);
void I2C_Standby_24C(void);
void I2C_ByteWrite_24C(u8 addr,u8 dat);
void I2C_PageWrite_24C(u8 addr,u8* pBuffer, u8 no);
void I2C_WriteS_24C(u8 addr,u8* pBuffer,  u16 no);
void I2C_Test(void);
void I2C_Scanner(void);
#endif
