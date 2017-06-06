/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�BMP.H
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 *
 */
#ifndef __BMP_H
#define __BMP_H

#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "stm32f4xx.h"
#include <rtthread.h>
#include "CS99xx.h"		
		
struct tag_bmp_file_header
{
	rt_uint16_t	type;
	rt_uint32_t	size;
	rt_uint16_t	reserved1;
	rt_uint16_t	reserved2;
	rt_uint32_t	offbits;
};


#define RTGUI_RGB(R,G,B) 	(((R>>3)<<11) | ((G>>2)<<5) | ((B>>3)))


/******************* �������� *********************/
void bmptorgb(u16 h,u16 w,u16 *p1,u8 *p2);
void bmp16torgb(u16 h,u16 w,u16 *p1,u8 *p2);
u8 loadbmptosram(const char *path,u16 *data);
u8 loadbmpbintosram(const char *path,u16 *data);

/********************** �ⲿ�õ��ı��� **************************/


/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
