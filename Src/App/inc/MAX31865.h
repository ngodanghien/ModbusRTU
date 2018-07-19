#ifndef __MAX31865_H
#define __MAX31865_H

#define CR1_SPE_Set          ((uint16_t)0x0040)
#define CR1_SPE_Reset        ((uint16_t)0xFFBF)

#define RREF      470.0
#define RNOMINAL  100.0

#define MAX31856_CONFIG_REG            0x00
#define MAX31856_CONFIG_BIAS           0x80
#define MAX31856_CONFIG_MODEAUTO       0x40
#define MAX31856_CONFIG_MODEOFF        0x00
#define MAX31856_CONFIG_1SHOT          0x20
#define MAX31856_CONFIG_3WIRE          0x10
#define MAX31856_CONFIG_24WIRE         0x00
#define MAX31856_CONFIG_FAULTSTAT      0x02
#define MAX31856_CONFIG_FILT50HZ       0x01
#define MAX31856_CONFIG_FILT60HZ       0x00

#define MAX31856_RTDMSB_REG           0x01
#define MAX31856_RTDLSB_REG           0x02
#define MAX31856_HFAULTMSB_REG        0x03
#define MAX31856_HFAULTLSB_REG        0x04
#define MAX31856_LFAULTMSB_REG        0x05
#define MAX31856_LFAULTLSB_REG        0x06
#define MAX31856_FAULTSTAT_REG        0x07

#define RTD_A 3.9083e-3
#define RTD_B -5.775e-7

void MAX31865_3WIRE_Init(void);
uint16_t MAX31865_readRTD (void);
float MAX31865_temperature(float RTDnominal, float refResistor);

#endif
