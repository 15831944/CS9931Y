/*
 * Copyright(c) 2013,SSC
 * All rights reserved
 * �ļ����ƣ�ui_config.h
 * ժ  Ҫ  ��Һ�����豸����ͷ�ļ�
 * ��ǰ�汾��V1.0�������ɱ�д
 * �޸ļ�¼��
 *
 */

#ifndef    _UI_CONFIG_H_
#define    _UI_CONFIG_H_

#if defined(__cplusplus)

    extern "C" {     /* Make sure we have C-declarations in C++ programs */

#endif

#include <rtthread.h>		
#include "stm32f4xx.h"
		
/******************************************************************************
 *                             �����ļ�����
******************************************************************************/ 



/******************************************************************************
 *                           �ļ��ӿ���Ϣ�궨��
******************************************************************************/
#define		ExternSramAddr		(__IO uint8_t *)((uint32_t)(0x64000000))
#define		ExternSramTitleAddr		(ExternSramAddr)
#define		ExternSramTitleSize		(48000)
#define		ExternSramTitleBkAddr   (ExternSramAddr + ExternSramTitleSize)
#define		ExternSramTitleBkSize   (48000)
#define		ExternSramMenuAddr		(ExternSramTitleBkAddr + ExternSramTitleBkSize)
#define		ExternSramMenuSize		(38400)
#define		ExternSramHomeAddr		(ExternSramMenuAddr + ExternSramMenuSize)
#define		ExternSramHomeSize		(546720)
#define		ExternSramKeyAddr		(ExternSramHomeAddr + ExternSramHomeSize)
#define		ExternSramKeySize		(16080)
#define		ExternSramKeyUpAddr		(ExternSramKeyAddr + ExternSramKeySize*6)
#define		ExternSramKeyDownAddr	(ExternSramKeyAddr + ExternSramKeySize*7)
#define		ExternSramStatusAddr	(ExternSramKeyAddr + ExternSramKeySize*8)
#define		ExternSramStatusSize	(38400)
#define		ExternSramNullAddr		(ExternSramStatusAddr + ExternSramStatusSize)
#define		ExternSramWinAddr		(ExternSramNullAddr + 546720)


enum{
	UPDATE_TITLE   = 0x10000000,
	UPDATE_MENU    = 0x20000000,
	UPDATE_KEY     = 0x30000000,
	UPDATE_HOME    = 0x40000000,
	UPDATE_STATUS  = 0x50000000,
};

enum{
	STATUS_KEYLOCK_EVENT	=	0x01000000,
	STATUS_OFFSET_EVENT		=	0x02000000,
	STATUS_CONTENT_EVENT	=	0x03000000,
	STATUS_STATUS_EVENT		=	0x04000000,
	STATUS_INTERFACE_EVENT	=	0x05000000,
	STATUS_TIME_EVENT		=	0x06000000,
};

/* LCD ��ɫ���룬CL_��Color�ļ�д */
enum
{
	CL_WHITE        = 0xFFFF,	/* ��ɫ */
	CL_BLACK        = 0x0000,	/* ��ɫ */
	CL_GREY         = 0xF7DE,	/* ��ɫ */
	CL_RED          = 0xF800,	/* ��ɫ */
	CL_BLUE2        = 0x051F,	/* ǳ��ɫ */
	CL_BLUE         = 0x001F,	/* ��ɫ */
	CL_MAGENTA      = 0xF81F,	/* ����ɫ�����ɫ */
	CL_GREEN        = 0x07E0,	/* ��ɫ */
	CL_CYAN         = 0x7FFF,	/* ����ɫ����ɫ */
	CL_YELLOW       = 0xFFE0,	/* ��ɫ */
	CL_MASK			= 0x9999,	/* ��ɫ���룬�������ֱ���͸�� */
	CL_orange		= 0xFD20, /* ��ɫ */
};
/******************************************************************************
 *                         �ļ��ӿ����ݽṹ������
******************************************************************************/
struct rtc_time_type
{
	char	year;
	char	month;
	char	date;
	char	day;
	char	hours;
	char	minutes;
	char	seconds;
};


struct rect_type
{
	u16 x;u16 y;u16 h;u16 w;
};

struct panel_type
{
	u16 *data;
	u16	h;
	u16 w;
	u16 x;
	u16 y;
};

struct num_format
{
	u8	_int;
	u8	_dec;
	u32	num;
	u32 min;
	u32 max;
	const char *unit;
};

extern struct rt_mailbox screen_mb;
extern struct panel_type panel_title;
extern struct panel_type panel_title_back;
extern struct panel_type panel_menu;
extern struct panel_type panel_key;
extern struct panel_type panel_home;
extern struct panel_type panel_homeback;
extern struct panel_type panel_status;

/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/ 
#define		panel_update(t)				RA8875_DrawBMP(t.x,t.y,t.h,t.w,t.data)

void clr_mem(u16 *p,u16 d,u32 size);
void clr_win(struct panel_type *p,u16 c,u16 x,u16 y,u16 h,u16 w);

rt_uint8_t text_input(struct panel_type *parent,struct rect_type *rect,char *str);
// rt_uint32_t	num_input(struct font_info_t *font,struct rect_type *rect,struct num_format *num);


u8	log10(u32 num);
u32 exp10(u8 n);
u32 strtonum(u8 _int,u8 _dec,char *str);

#if defined(__cplusplus)

    }

#endif

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
