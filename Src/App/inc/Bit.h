/********************************************************************************
  * @File   : Bit.h
  * @Author : worldsing
  * @Version: V0.1
  * @Date   : 2013/05/05
  * @Brief  :
  *******************************************************************************
  * @Attention:
  */
#ifndef __BIT_H__
#define __BII_H__


#define RevBit(port, bitn)           (port ^=  (1<<(bitn)))          // ODR                       // Dao bit
#define SetBit(port, bitn)           (port |=  (1<<(bitn)))          // ODR                       //Set Port at bitn = 1
#define ClrBit(port, bitn)           (port &= ~(1<<(bitn)))          // ODR                      //Reset Port at bitn = 0
// #define GetBit(port, bitn)           (port &   (1<<(bitn)))                                    // Read bitn at Port
#define GetBit(port, bitn)           ((port >> bitn) & 0x1)          // IDR || ODR                        // Read bitn at Port
#define OutBit(port, bitn, value)    ((value) ? (SetBit(port, bitn)) : (ClrBit(port, bitn))) // IDR|ODR  - Set (1) hoac Reset(0).

#endif

//end of file


