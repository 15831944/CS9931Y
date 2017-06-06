
#include "stm32f4xx.h"
#include "bsp_button.h"
#include <rtthread.h>
#include "CS99xx.h"
#include "memorymanagement.h"
#include "Test_Sched.h"
#include "PLC.h"
// #include <rtgui/event.h>
// #include <rtgui/rtgui_server.h>

	/* �����ڶ�Ӧ��RCCʱ�� */
	#define RCC_ALL_KEY 	(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOH)

	#define GPIO_PORT_EXT1  	GPIOA
	#define GPIO_PIN_EXT1	    GPIO_Pin_8

	#define GPIO_PORT_EXT2  	GPIOA
	#define GPIO_PIN_EXT2	    GPIO_Pin_9

	#define GPIO_PORT_EXT3  	GPIOA
	#define GPIO_PIN_EXT3	    GPIO_Pin_10

	#define GPIO_PORT_IN1    	GPIOA
	#define GPIO_PIN_IN1   	 	GPIO_Pin_12

	#define GPIO_PORT_IN2    	GPIOH
	#define GPIO_PIN_IN2   	 	GPIO_Pin_13
	
	#define GPIO_PORT_IN3    	GPIOH
	#define GPIO_PIN_IN3   	 	GPIO_Pin_14
	
	#define GPIO_PORT_IN4    	GPIOH
	#define GPIO_PIN_IN4   	 	GPIO_Pin_15



	#define	KEY_PORT_ST			GPIO_PORT_EXT2
	#define	KEY_PIN_ST			GPIO_PIN_EXT2
	
	#define KEY_PORT_CLK		GPIO_PORT_EXT1
	#define	KEY_PIN_CLK			GPIO_PIN_EXT1
	
	#define	KEY_PORT_DATA		GPIO_PORT_EXT3
	#define	KEY_PIN_DATA		GPIO_PIN_EXT3
	

#define		KEY_DLY				0

static BUTTON_T s_Btn;

static KEY_FIFO_T s_Key;		/* ����FIFO����,�ṹ�� */

static void bsp_InitButtonVar(void);
// static void bsp_InitButtonHard(void);
static void bsp_DetectButton(BUTTON_T *_pBtn);

extern uint8_t Get_Test_Warning_State(void);

rt_uint8_t keym_disable;
u16 KEY_BUFFER=0;
u32 LED_SINGLE=0;
/* ������ƿ�*/
struct rt_mailbox key_mb;
/* ���ڷ��ʼ����ڴ��*/
static char key_mb_pool[64];

static void key_delay(u32 t)
{
	while(t--);
}
/*
 * ��������key_write
 * ����  ��д
 * ����  ����
 * ���  ����
 */
static void key_write(void)
{
	unsigned char i;
	unsigned int pos=0x8000;
	
	register rt_base_t level;

  /* disable interrupt */
  level = rt_hw_interrupt_disable();
	
//	rt_enter_critical();
	GPIO_ResetBits(KEY_PORT_ST,KEY_PIN_ST);
	for(i=0;i<16;i++)
	{	
		GPIO_ResetBits(KEY_PORT_CLK,KEY_PIN_CLK);
		if(KEY_BUFFER & pos)
		{
			GPIO_SetBits(KEY_PORT_DATA,KEY_PIN_DATA);
		}
		else
		{
			GPIO_ResetBits(KEY_PORT_DATA,KEY_PIN_DATA);
		}
		key_delay(KEY_DLY);
		GPIO_SetBits(KEY_PORT_CLK,KEY_PIN_CLK);	
		key_delay(KEY_DLY);
		pos=pos>>1;
	}
	GPIO_SetBits(KEY_PORT_ST,KEY_PIN_ST);
//	rt_exit_critical();
	rt_hw_interrupt_enable(level);
}

void bsp_display(u32 cmd,u8 status)
{
	u32 cmd_bk   = cmd;
	u8 status_bk = status;
	switch(status)
	{
		case 2:
			if(cmd & 0xffff0000)
			{
				LED_SINGLE ^= cmd;
				if(LED_SINGLE & LED_TEST)				
					GPIO_SetBits(GPIOI,GPIO_Pin_2);
				else
					GPIO_ResetBits(GPIOI,GPIO_Pin_2);
			}
			cmd &= 0xffff;
			if(cmd)
			{
				KEY_BUFFER ^= cmd;
				key_write();
			}
			break;
		default:
			if(cmd & 0xffff0000)
			{
				LED_SINGLE = status!=0 ? (LED_SINGLE | cmd):(LED_SINGLE & ~cmd);
				if(LED_SINGLE & LED_TEST)				
					GPIO_SetBits(GPIOI,GPIO_Pin_2);
				else
					GPIO_ResetBits(GPIOI,GPIO_Pin_2);
			}
			cmd &= 0xffff;
			if(cmd)
			{
				KEY_BUFFER = status!=0 ? (KEY_BUFFER | cmd):(KEY_BUFFER & ~cmd);
				key_write();
			}
			break;
	}
	//�ڴ˴����PLC����ز���
	{
		switch(cmd_bk){
		
			case LED_TEST:
			{
				switch(status_bk){
					case 0:
						PLC_Testing_Out(0);
					break;
					
					case 1:
						//û�в������Ѿ��ڿ�ʼʱ�����
					break;
					case 2:
//						PLC_Testing_Out(1);
					break;
					default:
						
					break;
				}
			}
			break;
			
			case LED_PASS:
			{
				switch(status_bk){
					case 0:
						PLC_Pass_Out(0);
					break;
					
					case 1:
						PLC_Pass_Out(1);
					break;
					default:
						
					break;
				}
			}
			break;
			
			case LED_FAIL:
			{
				switch(status_bk){
					case 0:
						PLC_Fail_Out(0);
					break;
					
					case 1:
						PLC_Fail_Out(1);
					break;
					default:
						
					break;
				}
			}
			break;
			default:
			
			break;
		}
	}
}
/*
 * ��������key_scan_read
 * ����  ��д
 * ����  ����
 * ���  ����
 */
static u8 key_scan_read(void)
{
	unsigned char key=0;
	key|=(GPIO_ReadInputDataBit(GPIO_PORT_IN1,GPIO_PIN_IN1)!=0?0x01:0);
	key|=(GPIO_ReadInputDataBit(GPIO_PORT_IN2,GPIO_PIN_IN2)!=0?0x02:0);
	key|=(GPIO_ReadInputDataBit(GPIO_PORT_IN3,GPIO_PIN_IN3)!=0?0x04:0);
	key|=(GPIO_ReadInputDataBit(GPIO_PORT_IN4,GPIO_PIN_IN4)!=0?0x08:0);
	return key;
}

/*
 * ��������key_scan
 * ����  ��д
 * ����  ����
 * ���  ����
 */

static u32 key_scan(void)
{
	unsigned char i;
	unsigned char pos=0x80;
	u32 key=0;
	
	for(i=0;i<8;i++)
	{
		KEY_BUFFER &= 0xff00;
		KEY_BUFFER |= ((~pos)&0x00ff);
		key_write();
		key |= (key_scan_read() << (i*4));
		pos >>= 1;
	}
	
	switch(key)
	{
		case 0xefffffff:
			return 1;
		
		case 0xfeffffff:
			return 2;
		case 0xffefffff:
			return 3;
		case 0xfffeffff:
			return 4;
		case 0xffffefff:
			return 5;
		case 0xfffffeff:
			return 6;
		case 0xffffffef:
			return 7;
		
		case 0xfffffffe:
			return 8;
		case 0xdfffffff:
			return 9;
		case 0xfdffffff:
			return 10;
		case 0xffdfffff:
			return 11;
		
		case 0xfffdffff://0
			return 12;
		case 0xffffdfff://1
			return 13;
		case 0xbfffffff://2
			return 14;
		case 0xffffbfff://3
			return 15;	
		case 0xfffffdff://4
			return 16;
		case 0xfbffffff://5
			return 17;
		case 0xfffffbff://6
			return 18;
		case 0xffffffdf://7
			return 19;
		case 0xffbfffff://8
			return 20;
		case 0xffffffbf://9
			return 21;
		
		case 0xfffffffd://pos
			return 22;
		case 0xfffbffff://shift
			return 23;
		
		
		
		case 0xfffffffb://left
			return 24;
		case 0x7fffffff://up
			return 25;
		case 0xf7ffffff://down
			return 26;
		case 0xff7fffff://right
			return 27;
		
		default:
			return 0;
	}
}

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
// 	bsp_InitButtonHard();		/* ��ʼ������Ӳ�� */
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
// 	bsp_PutKey(KEY_DOWN_EXIT);
}
	

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonHard
*	����˵��: ��ʼ������Ӳ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// static void bsp_InitButtonHard(void)
// {
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 			
// 	/* ��1������GPIOA GPIOC GPIOD GPIOF GPIOG��ʱ��
// 	   ע�⣺����ط�����һ����ȫ��
// 	*/
// 	RCC_AHB1PeriphClockCmd(RCC_ALL_KEY, ENABLE);
// 	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
// 	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* ��Ϊ����ģʽ */
// 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �������������� */
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_IN1;
// 	GPIO_Init(GPIO_PORT_IN1, &GPIO_InitStructure);
// 	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_IN2;
// 	GPIO_Init(GPIO_PORT_IN2, &GPIO_InitStructure);
// 	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_IN3;
// 	GPIO_Init(GPIO_PORT_IN3, &GPIO_InitStructure);
// 	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_IN4;
// 	GPIO_Init(GPIO_PORT_IN4, &GPIO_InitStructure);
// 	
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
// 	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
// 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_EXT1;
// 	GPIO_Init(GPIO_PORT_EXT1, &GPIO_InitStructure);

// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_EXT2;
// 	GPIO_Init(GPIO_PORT_EXT2, &GPIO_InitStructure);
// 	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_EXT3;
// 	GPIO_Init(GPIO_PORT_EXT3, &GPIO_InitStructure);
// }
	
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
	s_Btn.IsKeyDownFunc		= key_scan;					/* �жϰ������µĺ��� */
	s_Btn.FilterTime		= BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_Btn.LongTime			= 400;			             /* ����ʱ�� */
	s_Btn.Count				= s_Btn.FilterTime / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
	s_Btn.State				= KEY_NONE;					/* ����ȱʡ״̬��0Ϊδ���� */
	s_Btn.KeyCodeDown		= KEY_DOWN;					/* �������µļ�ֵ���� */
	s_Btn.KeyCodeUp			= KEY_UP;					/* ��������ļ�ֵ���� */
	s_Btn.KeyCodeLong		= KEY_LONG;					/* �������������µļ�ֵ���� */
	s_Btn.RepeatSpeed		= 0;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_Btn.RepeatCount		= 0;						/* ���������� */		

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
	u8	key;
	static u32 key_mem;
	/* ���û�г�ʼ�������������򱨴� */
// 	if (_pBtn->IsKeyDownFunc == 0)
// 	{
// 		return;//"Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine";
// 	}

	key = _pBtn->IsKeyDownFunc();
	if(key == 0xff)return;
	if (key != 0)
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
					bsp_PutKey(_pBtn->KeyCodeDown | key);
					key_mem = key;
					
					buzzer(5);
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
						bsp_PutKey(_pBtn->KeyCodeLong | key);						
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
							bsp_PutKey(_pBtn->KeyCodeDown | key);														
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
					bsp_PutKey(_pBtn->KeyCodeUp | key_mem);			
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
// 	KEY_TEMP = key_scan();
	if(Test_Sched_Param.Test_Sched_State == TEST_SCHED_STATE_RUNNING)return;
	bsp_DetectButton(&s_Btn);	
}

















u8 keyf_down_flag=0;

/* language , panel(panel_flag) , Fn */
const char * keyf_name[2][10][10]={
	{
/*   ��ҳ  -> */		{"��~��"	,"��~��"	,"ϵ~ͳ"	,"��~��"	,"��~��"	,"��~��"},
/*   �ļ�  -> */		{"��~��"	,"��~��"	,"��~ȡ"	,"��~��"	,"ɾ~��"	,"��~��"},
/*   ����  -> */		{"��~ϸ"	,"��~��"	,"ɾ~��"	,"ǰ~��"	,"��~��"	,"��~��"},
/*   ϵͳ  -> */		{"~"		,"~"		,"~"		,"~"		,"~"		,"��~��"},
/*   ���  -> */		{"��~��"	,"ɾ~��"	,"ͳ~��"	,"��~��"	,"��~ת"	,"��~��"},
/*   ����  -> */		{"~"		,"~"		,"~"		,"~"		,"~"		,"��~��"},
/*   ����  -> */		{"~"		,"~"		,"~"		,"~"		,"~"		,"��~��"},
/* �����ļ�-> */		{"��~��"	,"��~��"	,"~"	,"~"	,"~"	  ,"��~��"},
                    {"~"		,"~"		,"~"		,"~"		,"~"		,"��~��"},
/* �Զ�У׼-> */    {"~"		,"~"		,"~"		,"~"		,"~"		,"��~��"},
	},
	{
					{"File",	"Step",	"System",	"Result",	"Help",	"About"},
					{"File",	"New",	"Read",		"Edit",	"Delete",	"More"},
					{"Detail",	"New",	"Delete",	"Prev",	"Next",	"Back"},
					{"~",		"~",	"~",		"~",	"~",	"Back"},
					{"Detail","Delete","Stat  ","Export","Jump  ","Back  "},
					{"~",		"~",	"~",		"~",	"~",	"Back"},
					{"~",		"~",	"~",		"~",	"~",	"Back"},
					{"Import",		"Export",	"~",		"~",	"~",	"Back"},
					{"~"		,"~"		,"~"		,"~"		,"~"		,"Back"},
					{"~"		,"~"		,"~"		,"~"		,"~"		,"Back"},
	}
};

u8		buzzer_timer = 10;
static void key_thread_entry(void *parameter)
{

	uint8_t rtKeyCode;	/* �������� */
	uint8_t rtTemp;
	struct font_info_t font={&panel_key,BUTTON_BACKCOLOR,0x0,1,0,24};
		
	/* ��ʼ��һ��mailbox */
	rt_mb_init(&key_mb,"key_mb", key_mb_pool, sizeof(key_mb_pool)/4, RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ�*/
	
    while (1)
    {
		if(system_parameter_t.Com_lock == 0 && Get_Test_Warning_State() == 0)
		{
		bsp_KeyPro();  /* ����������򣬱���ÿ10ms����1�� */
		
		rtTemp = bsp_GetKey();
		if (rtTemp > 0)
		{
			rtKeyCode = rtTemp & 0x1f;
			
			if(rtTemp == (KEY_LOCK | KEY_DOWN))
			{
				system_parameter_t.key_lock ^= 1;
				rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_KEYLOCK_EVENT | (system_parameter_t.key_lock));
				if(system_parameter_t.psd.keylockmem_en !=0)
				{
					memory_systems_save();
				}
			}
			if(system_parameter_t.key_lock==0)
			{
				if(rtKeyCode >= KEY_F1 && rtKeyCode <= KEY_F6)
				{
	// 				rt_enter_critical();
	// 				font_info_set(&panel_key,BUTTON_BACKCOLOR,0x0,1,24);
					if((rtTemp & KEY_DOWN)&&(keyf_down_flag==0)&&(keym_disable&(0x80>>(rtKeyCode-KEY_F1)))==0)
					{
						keyf_down_flag = 1;
						rt_memcpy((void *)((u8 *)panel_key.data+(rtKeyCode-KEY_F1)*ExternSramKeySize),(void *)ExternSramKeyDownAddr,ExternSramKeySize);
						font_draw((120-rt_strlen(keyf_name[language][panel_flag][rtKeyCode-KEY_F1])*12)/2,
							23+(rtKeyCode-KEY_F1)*67,&font,
							keyf_name[language][panel_flag][rtKeyCode-KEY_F1]);
						rt_mb_send(&screen_mb, UPDATE_KEY);
						rt_mb_send(&key_mb, rtTemp);
					}
					else if((keyf_down_flag == 1) &&(keym_disable&(0x80>>(rtKeyCode-KEY_F1)))==0)
					{
						keyf_down_flag = 0;
						rt_memcpy((void *)((u8 *)panel_key.data+(rtKeyCode-KEY_F1)*ExternSramKeySize),(void *)ExternSramKeyUpAddr,ExternSramKeySize);
						font_draw((120-rt_strlen(keyf_name[language][panel_flag][rtKeyCode-KEY_F1])*12)/2,
							21+(rtKeyCode-KEY_F1)*67,&font,
							keyf_name[language][panel_flag][rtKeyCode-KEY_F1]);
						rt_mb_send(&screen_mb, UPDATE_KEY);
						rt_mb_send(&key_mb, rtTemp);
					}
	// 				rt_exit_critical();
				}else{
					rt_mb_send(&key_mb, rtTemp);
				}
				
			}
		}	
		}
		/* ���������ð�����10msʱ��Ƭ */
		if(buzzer_timer > 0)
			if(--buzzer_timer == 0)bsp_display(FMQ,0);
		/* wait next key press */
		rt_thread_delay(RT_TICK_PER_SECOND/100);
		key_write();
    }
}

void buzzer(u8 timer)
{
	if(Test_Sched_Param.Test_Sched_State == TEST_SCHED_STATE_RUNNING)return;
	bsp_display(FMQ,1);
	buzzer_timer = timer;
}

void buzzer_test(u8 timer)
{
	if(Test_Sched_Param.Test_Sched_State != TEST_SCHED_STATE_RUNNING)return;
	if(timer == 0)
	{
		bsp_display(FMQ,0);
		buzzer_timer = 0;
		return;
	}
	bsp_display(FMQ,1);
	buzzer_timer = timer;
}


void ui_key_updata(rt_uint8_t key_disable)
{
	u8 i;
	struct font_info_t font={&panel_key,BUTTON_BACKCOLOR,0x0,1,0,24};

	keym_disable = key_disable;
	for(i=0;i<6;i++)
		rt_memcpy((void *)(ExternSramKeyAddr+ExternSramKeySize*i),(void *)ExternSramKeyUpAddr,ExternSramKeySize);
// 	font_info_set(&panel_key,BUTTON_BACKCOLOR,0x0,1,24);
	
// 	rt_enter_critical();
	for(i=0;i<6;i++)
	{
		if(key_disable & (0x80>>i))font.fontcolor = 0XA514;
		else font.fontcolor = 0xffff;
		font_draw((120-rt_strlen(keyf_name[language][panel_flag][i])*12)/2,
						21+(i)*67,&font,
						keyf_name[language][panel_flag][i]);
	}
// 	rt_exit_critical();
	rt_mb_send(&screen_mb, UPDATE_KEY);
}

static rt_thread_t key_tid;
void rt_hw_key_init(void)
{
	bsp_InitButton();
    key_tid = rt_thread_create("key",
                               &key_thread_entry, RT_NULL,
                               512, 10, 10);

    if (key_tid != RT_NULL) rt_thread_startup(key_tid);
}
