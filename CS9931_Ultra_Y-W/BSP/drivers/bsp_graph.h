/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�bsp_graph.H
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 *
 */
#ifndef __BSP_GRAPH_H
#define __BSP_GRAPH_H

#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "stm32f4xx.h"
#include <rtthread.h>
#include "CS99xx.h"		
		





/******************* �������� *********************/
void draw_fillrect(struct panel_type *p,struct rect_type *rect,u16 c);
void draw_rect(struct panel_type *p,struct rect_type *rect,u16 c,u8 bold);
void draw_bmp(struct panel_type *p,struct rect_type *rect,u16 *d);
void draw_alphabmp(struct panel_type *p,struct rect_type *rect,u16 *d,u16 ac);

void window_copy(struct panel_type *p1,struct panel_type *p2,struct rect_type *rect);


/********************** �ⲿ�õ��ı��� **************************/


/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
