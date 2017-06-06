/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��
*	�ļ����� : bsp_button.c
*	��    �� : V1.0
*	˵    �� : ʵ�ְ����ļ�⣬��������˲����ƣ����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*				(5) ��ϼ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v0.1    2009-12-27 armfly  �������ļ���ST�̼���汾ΪV3.1.2
*		v1.0    2011-01-11 armfly  ST�̼���������V3.4.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f4xx.h"
#include "bsp_button.h"
#include <rtthread.h>
#include <rtgui/event.h>
#include <rtgui/rtgui_server.h>

	/* �����ڶ�Ӧ��RCCʱ�� */
	#define RCC_ALL_KEY 	(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOG)

	#define GPIO_PORT_K1    GPIOI
	#define GPIO_PIN_K1	    GPIO_Pin_8

	#define GPIO_PORT_K2    GPIOC
	#define GPIO_PIN_K2	    GPIO_Pin_13

	#define GPIO_PORT_K3    GPIOI
	#define GPIO_PIN_K3	    GPIO_Pin_11

	#define GPIO_PORT_K4    GPIOH
	#define GPIO_PIN_K4	    GPIO_Pin_2

	#define GPIO_PORT_K5    GPIOH
	#define GPIO_PIN_K5	    GPIO_Pin_3

	#define GPIO_PORT_K6    GPIOF
	#define GPIO_PIN_K6	    GPIO_Pin_11

	#define GPIO_PORT_K7    GPIOG
	#define GPIO_PIN_K7	    GPIO_Pin_7

	#define GPIO_PORT_K8    GPIOH
	#define GPIO_PIN_K8	    GPIO_Pin_15


static BUTTON_T s_BtnUp;		/* ҡ��UP�� */
static BUTTON_T s_BtnDown;		/* ҡ��DOWN�� */
static BUTTON_T s_BtnLeft;		/* ҡ��LEFT�� */
static BUTTON_T s_BtnRight;		/* ҡ��RIGHT�� */

static KEY_FIFO_T s_Key;		/* ����FIFO����,�ṹ�� */

static void bsp_InitButtonVar(void);
static void bsp_InitButtonHard(void);
static void bsp_DetectButton(BUTTON_T *_pBtn);

/*
	���庯���жϰ����Ƿ��£�����ֵ1 ��ʾ���£�0��ʾδ����
*/

static uint8_t IsKeyDownUp(void) 		{if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0) return 1; return 0;}
static uint8_t IsKeyDownDown(void) 		{if ((GPIO_PORT_K2->IDR & GPIO_PIN_K2) == 0) return 1; return 0;}
static uint8_t IsKeyDownLeft(void) 		{if ((GPIO_PORT_K3->IDR & GPIO_PIN_K3) == 0) return 1; return 0;}
static uint8_t IsKeyDownRight(void) 	{if ((GPIO_PORT_K4->IDR & GPIO_PIN_K4) == 0) return 1; return 0;}


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButton
*	����˵��: ��ʼ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitButton(void)
{
	bsp_InitButtonVar();		/* ��ʼ���������� */
	bsp_InitButtonHard();		/* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    �Σ�_KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_Key.Buf[s_Key.Write] = _KeyCode;

	if (++s_Key.Write  >= KEY_FIFO_SIZE)
	{
		s_Key.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_Key.Read == s_Key.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_Key.Buf[s_Key.Read];

		if (++s_Key.Read >= KEY_FIFO_SIZE)
		{
			s_Key.Read = 0;
		}
		return ret;
	}
}


void SoftSendEXITKey(void)
{
	/* ��ֵ���밴��FIFO */
	bsp_PutKey(KEY_DOWN_EXIT);
}
	

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonHard
*	����˵��: ��ʼ������Ӳ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitButtonHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
			
	/* ��1������GPIOA GPIOC GPIOD GPIOF GPIOG��ʱ��
	   ע�⣺����ط�����һ����ȫ��
	*/
	RCC_AHB1PeriphClockCmd(RCC_ALL_KEY, ENABLE);
	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K1;
	GPIO_Init(GPIO_PORT_K1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K2;
	GPIO_Init(GPIO_PORT_K2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K3;
	GPIO_Init(GPIO_PORT_K3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K4;
	GPIO_Init(GPIO_PORT_K4, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K5;
	GPIO_Init(GPIO_PORT_K5, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K6;
	GPIO_Init(GPIO_PORT_K6, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K7;
	GPIO_Init(GPIO_PORT_K7, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K8;
	GPIO_Init(GPIO_PORT_K8, &GPIO_InitStructure);
}
	
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonVar
*	����˵��: ��ʼ����������
*	��    �Σ�strName : ���������ַ���
*			  strDate : ���̷�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitButtonVar(void)
{
	/* �԰���FIFO��дָ������ */
	s_Key.Read = 0;
	s_Key.Write = 0;
	

	/* ��ʼ��Up����������֧�ְ��¡����𡢳��� */
	s_BtnUp.IsKeyDownFunc	= IsKeyDownUp;				/* �жϰ������µĺ��� */
	s_BtnUp.FilterTime		= BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_BtnUp.LongTime		= BUTTON_LONG_TIME;			/* ����ʱ�� */
	s_BtnUp.Count			= s_BtnUp.FilterTime / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnUp.State			= 0;						/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnUp.KeyCodeDown		= KEY_DOWN_JOY_UP;			/* �������µļ�ֵ���� */
	s_BtnUp.KeyCodeUp		= 0;						/* ��������ļ�ֵ���� */
	s_BtnUp.KeyCodeLong		= 0;						/* �������������µļ�ֵ���� */
	s_BtnUp.RepeatSpeed		= 10;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnUp.RepeatCount		= 0;						/* ���������� */		

	/* ��ʼ��Down����������֧�ְ��¡����𡢳��� */
	s_BtnDown.IsKeyDownFunc	= IsKeyDownDown;			/* �жϰ������µĺ��� */
	s_BtnDown.FilterTime	= BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_BtnDown.LongTime		= BUTTON_LONG_TIME;			/* ����ʱ�� */
	s_BtnDown.Count			= s_BtnDown.FilterTime / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnDown.State			= 0;						/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnDown.KeyCodeDown	= KEY_DOWN_JOY_DOWN;		/* �������µļ�ֵ���� */
	s_BtnDown.KeyCodeUp		= 0;						/* ��������ļ�ֵ���� */
	s_BtnDown.KeyCodeLong	= 0;						/* �������������µļ�ֵ���� */
	s_BtnDown.RepeatSpeed	= 10;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnDown.RepeatCount	= 0;						/* ���������� */

	/* ��ʼ��Left����������֧�ְ��¡����𡢳��� */
	s_BtnLeft.IsKeyDownFunc	= IsKeyDownLeft;			/* �жϰ������µĺ��� */
	s_BtnLeft.FilterTime	= BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_BtnLeft.LongTime		= BUTTON_LONG_TIME;			/* ����ʱ�� */
	s_BtnLeft.Count			= s_BtnLeft.FilterTime / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnLeft.State			= 0;						/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnLeft.KeyCodeDown	= KEY_DOWN_JOY_LEFT;		/* �������µļ�ֵ���� */
	s_BtnLeft.KeyCodeUp		= 0;						/* ��������ļ�ֵ���� */
	s_BtnLeft.KeyCodeLong	= 0;						/* �������������µļ�ֵ���� */
	s_BtnLeft.RepeatSpeed	= 0;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnLeft.RepeatCount	= 0;						/* ���������� */		

	/* ��ʼ��Right����������֧�ְ��¡����𡢳��� */
	s_BtnRight.IsKeyDownFunc= IsKeyDownRight;			/* �жϰ������µĺ��� */
	s_BtnRight.FilterTime	= BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_BtnRight.LongTime		= BUTTON_LONG_TIME;			/* ����ʱ�� */
	s_BtnRight.Count		= s_BtnRight.FilterTime / 2;/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnRight.State		= 0;						/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnRight.KeyCodeDown	= KEY_DOWN_JOY_RIGHT;		/* �������µļ�ֵ���� */
	s_BtnRight.KeyCodeUp	= 0;						/* ��������ļ�ֵ���� */
	s_BtnRight.KeyCodeLong	= 0;						/* �������������µļ�ֵ���� */
	s_BtnRight.RepeatSpeed	= 0;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnRight.RepeatCount	= 0;						/* ���������� */		

}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectButton
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    �Σ������ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectButton(BUTTON_T *_pBtn)
{
	/* ���û�г�ʼ�������������򱨴� */
// 	if (_pBtn->IsKeyDownFunc == 0)
// 	{
// 		return;//"Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine";
// 	}

	if (_pBtn->IsKeyDownFunc())
	{
		if (_pBtn->Count < _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				if (_pBtn->KeyCodeDown > 0)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey(_pBtn->KeyCodeDown);
				}
			}

			if (_pBtn->LongTime > 0)
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey(_pBtn->KeyCodeLong);						
					}
				}
				else
				{
					if (_pBtn->RepeatSpeed > 0)
					{
						if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)
						{
							_pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							bsp_PutKey(_pBtn->KeyCodeDown);														
						}
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey(_pBtn->KeyCodeUp);			
				}
			}
		}

		_pBtn->LongCount = 0;
		_pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyPro
*	����˵��: ������а�����������״̬�����뱻�����Եĵ��á�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_KeyPro(void)
{
	bsp_DetectButton(&s_BtnUp);			/* ҡ��UP�� */
	bsp_DetectButton(&s_BtnDown);		/* ҡ��DOWN�� */
	bsp_DetectButton(&s_BtnLeft);		/* ҡ��LEFT�� */
	bsp_DetectButton(&s_BtnRight);		/* ҡ��RIGHT�� */
}

static void key_thread_entry(void *parameter)
{
    struct rtgui_event_kbd kbd_event;
	uint8_t rtKeyCode;	/* �������� */
	uint8_t rtTemp;
      

    /* init keyboard event */
    RTGUI_EVENT_KBD_INIT(&kbd_event);
    kbd_event.mod  = RTGUI_KMOD_NONE;
    kbd_event.unicode = 0;

    while (1)
    {
		bsp_KeyPro();  /* ����������򣬱���ÿ10ms����1�� */

		rtTemp = bsp_GetKey();
		if (rtTemp > 0)
		{
			rtKeyCode = rtTemp;
			switch(rtKeyCode)
			{
				case KEY_DOWN_JOY_UP:
					kbd_event.key  = RTGUIK_UP;
					kbd_event.type = RTGUI_KEYDOWN;
					break;
				case KEY_DOWN_JOY_DOWN:
					kbd_event.key  = RTGUIK_DOWN;
					kbd_event.type = RTGUI_KEYDOWN;
					break;
				case KEY_DOWN_JOY_LEFT:
					kbd_event.key  = RTGUIK_LEFT;
					kbd_event.type = RTGUI_KEYDOWN;
					break;
				case KEY_DOWN_JOY_RIGHT:
					kbd_event.key  = RTGUIK_RIGHT;
					kbd_event.type = RTGUI_KEYDOWN;
					break;
				default:
					kbd_event.key = RTGUIK_UNKNOWN;
					kbd_event.type = RTGUI_KEYDOWN;
					break;
			}
			/* post down event */
            rtgui_server_post_event(&(kbd_event.parent), sizeof(kbd_event));
		}
		else
		{
			kbd_event.key = RTGUIK_UNKNOWN;
			kbd_event.type = RTGUI_KEYDOWN;
		}		
        /* wait next key press */
        rt_thread_delay(1);
    }
}

static rt_thread_t key_tid;
void rt_hw_key_init(void)
{
	bsp_InitButton();
    key_tid = rt_thread_create("key",
                               &key_thread_entry, RT_NULL,
                               512, 30, 10);

    if (key_tid != RT_NULL) rt_thread_startup(key_tid);
}
