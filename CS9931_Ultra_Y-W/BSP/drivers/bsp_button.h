/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��    
*	�ļ����� : bsp_button.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v0.1    2009-12-27 armfly  �������ļ���ST�̼���汾ΪV3.1.2
*		v1.0    2011-01-11 armfly  ST�̼���������V3.4.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

/* �����˲�ʱ��50ms, ��λ10ms
 ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
*/
#define BUTTON_FILTER_TIME 	1
#define BUTTON_LONG_TIME 	  100		/* ����1�룬��Ϊ�����¼� */

#define	BUTTON_BACKCOLOR		0xffff

extern struct rt_mailbox key_mb;


/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
	���Ա������ʵ���˲��Ͷ��ְ���״̬�������
*/
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	uint32_t (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */

	uint8_t Count;			/* �˲��������� */
	uint8_t FilterTime;		/* �˲�ʱ��(���255,��ʾ2550ms) */
	uint16_t LongCount;		/* ���������� */
	uint16_t LongTime;		/* �������³���ʱ��, 0��ʾ����ⳤ�� */
	uint8_t  State;			/* ������ǰ״̬�����»��ǵ��� */
	uint8_t KeyCodeUp;		/* ��������ļ�ֵ����, 0��ʾ����ⰴ������ */
	uint8_t KeyCodeDown;	/* �������µļ�ֵ����, 0��ʾ����ⰴ������ */
	uint8_t KeyCodeLong;	/* ���������ļ�ֵ����, 0��ʾ����ⳤ�� */
	uint8_t RepeatSpeed;	/* ������������ */
	uint8_t RepeatCount;	/* �������������� */
}BUTTON_T;

/* �����ֵ����
	�Ƽ�ʹ��enum, ����#define��ԭ��
	(1) ����������ֵ,�������˳��ʹ���뿴���������
	(2)	�������ɰ����Ǳ����ֵ�ظ���
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_DOWN = 0x20,
	KEY_UP = 0x40,
	KEY_LONG = 0x80,
	
	KEY_DISPLAY = 1,			/* SET������ */
	
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	
	KEY_LOCK,
	KEY_OFFSET,
	KEY_ENTER,
	KEY_EXIT,
	
	KEY_NUM0,
	KEY_NUM1,
	KEY_NUM2,
	KEY_NUM3,
	KEY_NUM4,
	KEY_NUM5,
	KEY_NUM6,
	KEY_NUM7,
	KEY_NUM8,
	KEY_NUM9,
	
	KEY_POS,
	KEY_SHIFT,
	
	
	KEY_L,
	KEY_U,
	KEY_D,
	KEY_R,
	
	CODE_LEFT,
	CODE_RIGHT,
}KEY_ENUM;

enum{
	LED_FAIL=0x100,
	LED_PASS=0x200,
	FMQ=0x400,
	LED_KEY1=0x800,
	LED_KEY2=0x1000,
	
	LED_TEST=0x10000,
};
/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	20
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	uint8_t Read;					/* ��������ָ�� */
	uint8_t Write;					/* ������дָ�� */
}KEY_FIFO_T;


/* ���ⲿ���õĺ������� */
void bsp_InitButton(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
void bsp_KeyPro(void);
// void CODE_LEFT_IRQ(void);
// void CODE_RIGHT_IRQ(void);
// void SoftSendEXITKey(void);
// void bsp_KeyMode(u8 key, u8 m);
void ui_key_updata(uint8_t key_disable);
void buzzer(u8 timer);

void bsp_display(u32 cmd,u8 status);

#endif


