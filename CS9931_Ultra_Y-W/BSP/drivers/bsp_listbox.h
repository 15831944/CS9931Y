/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�bsp_listbox.H
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 *
 */
#ifndef __BSP_LISTBOX_H
#define __BSP_LISTBOX_H

#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "stm32f4xx.h"
#include <rtthread.h>
#include "bsp_font.h"		
// #include "CS99xx.h"		
		
struct rtgui_listctrl
{
	struct panel_type *parent;
    /* total number of items */
    rt_uint16_t items_count;
	/* the number of item in a page */
    rt_uint16_t total_items;
    /* current item */
    rt_int16_t current_item;
	/* current item */
    rt_int16_t start_item;
    
	struct rect_type *rect;
    void (*on_item_draw)(struct font_info_t *font,rt_uint16_t index,rt_uint16_t x,rt_uint16_t y);
};


/******************* �������� *********************/
void listbox_draw(struct rtgui_listctrl *box);

/********************** �ⲿ�õ��ı��� **************************/


/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
