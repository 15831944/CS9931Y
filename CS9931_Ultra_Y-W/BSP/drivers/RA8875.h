/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�RA8875.H
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 *
 */
#ifndef __RA8875_H
#define __RA8875_H

#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
		
#include "stm32f4xx.h"
#include "ui_config.h"
#include <rtthread.h>

#define RA8875_BASE		((uint32_t)(0x60000000))

#define RA8875_REG		*(__IO uint16_t *)(RA8875_BASE +  (1 << (0 + 1)))	/* FSMC 16λ����ģʽ�£�FSMC_A18���߶�Ӧ�����ַA19 */
#define RA8875_RAM		*(__IO uint16_t *)(RA8875_BASE)


/* �ɹ��ⲿģ����õĺ��� */
void RA8875_InitHard(void);
void RA8875_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void window_updata(struct panel_type *parent,struct rect_type *rect);

void RA8875_DrawICO(u16 x,u16 y,u16 c,u16 bc,const unsigned char* data);
		
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth);		
/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
