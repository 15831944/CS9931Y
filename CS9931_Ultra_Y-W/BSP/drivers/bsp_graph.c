/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�graph.C
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 */

/******************* �����ļ� *********************/
#include "bsp_graph.h"

void draw_fillrect(struct panel_type *p,struct rect_type *rect,u16 c)
{
	u16 *sc,i,j;
	for(i=0;i<rect->h;i++)
	{
		sc = p->data + p->w*(i+rect->y) + rect->x;//�����е�ַ
		for(j=0;j<rect->w;j++)
		{
			*sc++ = c;
		}
	}
}

void draw_rect(struct panel_type *p,struct rect_type *rect,u16 c,u8 bold)
{
	
}

void draw_bmp(struct panel_type *p,struct rect_type *rect,u16 *d)
{
	u16 *sc,i,j;
	for(i=0;i<rect->h;i++)
	{
		sc = p->data + p->w*(i+rect->y) + rect->x;//�����е�ַ
		for(j=0;j<rect->w;j++)
		{
			*sc++ = *d++;
		}
	}
}

void draw_alphabmp(struct panel_type *p,struct rect_type *rect,u16 *d,u16 ac)
{
	u16 *sc,i,j;
	for(i=0;i<rect->h;i++)
	{
		sc = p->data + p->w*(i+rect->y) + rect->x;//�����е�ַ
		for(j=0;j<rect->w;j++)
		{
			if(*d != ac)
				*sc = *d;
				d++;
				sc++;
		}
	}
}

void window_copy(struct panel_type *p1,struct panel_type *p2,struct rect_type *rect)
{
	u16 *sc1,*sc2,i,j;
	for(i=0;i<rect->h;i++)
	{
		sc1 = p1->data + p1->w*(i+rect->y) + rect->x;//�����е�ַ
		sc2 = p2->data + p2->w*(i+rect->y) + rect->x;//�����е�ַ
		for(j=0;j<rect->w;j++)
		{
			*sc1++ = *sc2++;
		}
	}
}

/********************************************************************************************/
