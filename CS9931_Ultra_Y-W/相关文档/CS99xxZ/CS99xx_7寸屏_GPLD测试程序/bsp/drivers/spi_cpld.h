/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�spi_cpld.H
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 *
 */
#ifndef __SPI_CPLD_H
#define __SPI_CPLD_H

#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "stm32f4xx.h"

		
/******************************* ����ѡ�񿪹� ***********************************/


/******************* �������� *********************/
void spi_cpld_init(void);
void cpld_write(u32 SendData);
u16 ReadDataFromCPLD(u32 SendData);


/********************** �ⲿ�õ��ı��� **************************/

		

/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
