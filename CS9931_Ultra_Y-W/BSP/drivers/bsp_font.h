/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�bsp_font.H
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 *
 */
#ifndef __BSP_FONT_H
#define __BSP_FONT_H

#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "stm32f4xx.h"
#include <rtthread.h>
#include "CS99xx.h"		
		
/*
 * bitmap font engine
 */
struct rtgui_font_bitmap
{
    const rt_uint8_t  *bmp;         /* bitmap font data */
    const rt_uint8_t  *char_width;  /* each character width, NULL for fixed font */
    const rt_uint32_t *offset;      /* offset for each character */

    rt_uint16_t width;              /* font width  */
    rt_uint16_t height;             /* font height */

    rt_uint8_t first_char;
    rt_uint8_t last_char;
};


/*
 * bitmap font engine
 */
struct font_info_t
{
    struct panel_type *panel;
	
	rt_uint16_t	fontcolor;
	rt_uint16_t	backcolor;
	
	rt_uint8_t	alpha;// ͸��
	rt_uint8_t	center;//�Ƿ����
	
	rt_uint8_t	high;
	
};

/******************* �������� *********************/
// void font_info_set(struct panel_type *p,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t a,rt_uint8_t h);

void fontasctosram(u16 x, u16 y, struct font_info_t *font_info,struct rtgui_font_bitmap *font,const char ch);
void fonthztosram(u16 x, u16 y, struct font_info_t *font_info,struct rtgui_font_bitmap *bmp_font,const char *text);
void font_draw(u16 x, u16 y, struct font_info_t *font_info,const char *text);
void ui_text_draw(struct font_info_t *font_info,struct rect_type *rect,char *str);
void ui_text_draw_alpha(struct font_info_t *font_info,struct rect_type *rect,char *str);

/********************** �ⲿ�õ��ı��� **************************/

/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
