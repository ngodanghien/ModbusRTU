
/**
**-----------------------------------------------------------------------------------------------
** Coder: HIENCLUBVN
**------------------------------------------------------------------------------------------------------*/

#ifndef __MAX6675_H
#define __MAX6675_H 

#include "stm32f10x.h"


#define deg_C              0
#define deg_F              1
#define tmp_K              2

#define T_min              0
#define T_max              1024

#define count_max          4096

#define open_contact       0x04
#define close_contact      0x00

#define no_of_pulses       16
#define no_of_samples      16
#define scalar_deg_C      0.25
#define scalar_deg_F_1     1.8
#define scalar_deg_F_2     32.0
#define scalar_tmp_K       273.0


void MAX6675_init(void);
unsigned char MAX6657_get_ADC(unsigned long *ADC_data);// check xem adc_data co loi hay khong
float MAX6675_get_T(unsigned long ADC_value, unsigned char T_unit);//doc nhiet do tu max6675
double readCelsius(void); 

#endif


















