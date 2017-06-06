#ifndef __CALIBRATION__H__
#define __CALIBRATION__H__


/*������ͷ�ļ�*/
#include <stdint.h>
#include "stm32f4xx.h"

uint8_t AC_Calibration(uint8_t data);
uint8_t DC_Calibration(uint8_t data);
uint8_t AC_Current_Calibration(uint8_t data);
uint8_t DC_Current_Calibration(uint8_t data);


#endif



