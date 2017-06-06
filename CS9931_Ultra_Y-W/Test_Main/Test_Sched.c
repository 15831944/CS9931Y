#include  "Test_Sched.h"
#include  "ACW_Routine.h"
#include  "DCW_Routine.h"
#include  "GR_Routine.h"
#include  "LC_Routine.h"
#include  "CS99xx.h"
#include  "memorymanagement.h"
#include  "PLC.h"
#include  "sui_window.h"
#include "STM32_GPIO_Config.h"
#include "ext.h" 


/*ȫ�ֱ�������*/

TEST_SCHED_PARAM    Test_Sched_Param;           //���Ȳ��Բ���
static  rt_timer_t          Test_Sched_Timer;           //���Ⱥ����Ķ�ʱ��ָ��
rt_timer_t          Test_Sampling_Timer;
//static  rt_thread_t         Test_Sched_thread;          //�����߳�

struct  rt_mailbox test_sever_mb;
/* ���ڷ��ʼ����ڴ��*/
static  char test_sever_mb_pool[4];

static  char test_loop_flag;

struct  rt_mailbox test_sever_clock_mb;

static  char test_sever_clock_mb_pool[4];


/*��������*/

void Test_Sched_Main(void *);                     //�����Ժ���
extern void ui_teststr_darw(struct font_info_t *font, struct rect_type *rect, char *str);
extern   uint8_t  Cal_Facter_Recover(void);
// extern   uint8_t mapanflag;


static void Test_Sched_Sever(void *parameter);
static void Test_Sched_Clock_Sever(void *parameter);
static void Test_Sampling_Clock_Sever(void *parameter);
struct rt_thread thread_Test_Sched_Sever;
static char thread_Test_Sched_Sever_stack[4096];

//�ܲ��Ե��Ȼ�����ʼ������
/*******************************
��������  Test_Sched_Environment_Init
��  ����  ��

����ֵ��  ��
********************************/
void Test_Sched_Environment_Init(void)
{
	//1���ָ���У׼ϵ��
	Cal_Facter_Recover();
	
	//2����ʼ�����Բ���
	Test_Sched_Param.Test_Sched_State     = TEST_SCHED_STATE_INIT;          //���Ե�����״̬Ϊ��ʼ��״̬
	Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
	Test_Sched_Param.Stop_Flag            = 0;                              //���STOP��־λ
	Test_Sched_Param.Short_Flag           = 0;                              //���Short_int��־λ
	Test_Sched_Param.Pass_Flag            = 1;                              //��λ���Ժϸ��־λ
	Test_Sched_Param.Warning_Flag         = 0;                              //���Warning_Flag��־λ
	Test_Sched_Param.Test_Status          = TEST_STATE_WAIT;                //����״̬Ϊ�ȴ�����
	Test_Sched_Param.Offset_Get_Flag      = 0;                              //���offset��ȡ��־λ
	Test_Sched_Param.Offset_Is_Flag       = 0;                              //���offsetʹ�ܱ�־λ
	Test_Sched_Timer = rt_timer_create( "Test_Sched_Timer",                 /* ��ʱ������*/
	                                    Test_Sched_Clock_Sever, /* ��ʱʱ�ص��Ĵ����� */
	                                    RT_NULL, /* ��ʱ��������ڲ��� */
	                                    TEST_SCHED_PERIODIC, /* ��ʱ���ȣ���OS TickΪ��λ����timeout_value��OS Tick */
	                                    RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
   
    #if LC_SAMPLING_DEBUG
    Test_Sampling_Timer = rt_timer_create( "Test_Sampling_Timer",/* ��ʱ������*/
                                    Test_Sampling_Clock_Sever, /* ��ʱʱ�ص��Ĵ����� */
                                    RT_NULL, /* ��ʱ��������ڲ��� */
                                    TEST_SAMPLE_PERIODIC, /* ��ʱ���ȣ���OS TickΪ��λ����timeout_value��OS Tick */
                                    RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
	#endif
    if (Test_Sched_Timer != RT_NULL)
	{
		/*������*/
	}
	
	//�������Է����߳�
	rt_mb_init(&test_sever_clock_mb , "test_sever_clock_mb",  test_sever_clock_mb_pool , sizeof(test_sever_clock_mb_pool) / 4 , RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ�*/
	
	//	Test_Sched_thread = rt_thread_create("test_sched_serve",
	//																		 Test_Sched_Sever, RT_NULL,
	//																		 4096, 6, 20);
	rt_thread_init(&thread_Test_Sched_Sever,
	               "Test_Sched_Sever",
	               Test_Sched_Sever,
	               RT_NULL,
	               &thread_Test_Sched_Sever_stack[0],
	               4096, 6, 20);
	rt_thread_startup(&thread_Test_Sched_Sever);
	
	//	if (Test_Sched_thread != RT_NULL)
	//			rt_thread_startup(Test_Sched_thread);
	
	/*��ֹ���ŵ�һ����������*/
	AC_SetVoltage(0, 60);
	DC_SetVoltage(0);
	AC_SetVoltage(1, 60);
	DC_SetVoltage(1);
	AC_SetVoltage(0, 60);
	DC_SetVoltage(0);
	
	//��ʼ�����е�ָʾ�ƺ�PLC״̬
	bsp_display(LED_PASS, 0);
	bsp_display(LED_FAIL, 0);
	bsp_display(LED_TEST, 0);
	
	PLC_Testing_Out(0);
	PLC_Pass_Out(0);
	PLC_Fail_Out(0);
	
	AC_Output_Disable();
	DC_Output_Disable();
	GR_Output_Disable();
	LC_Assit_Output_Disable();
	LC_Main_Output_Disable();
	
}

void Reset_LED_PLC(void)
{
	//��ʼ�����е�ָʾ�ƺ�PLC״̬
	bsp_display(LED_PASS, 0);
	bsp_display(LED_FAIL, 0);
	bsp_display(LED_TEST, 0);
	
	PLC_Testing_Out(0);
	PLC_Pass_Out(0);
	PLC_Fail_Out(0);
}

uint8_t Get_Test_Warning_State(void)
{
	return Test_Sched_Param.Warning_Flag;
}

typedef struct
{
	rt_uint16_t   routine_test_year;  //�����������
	rt_uint16_t   routine_test_month; //���������·�
	rt_uint16_t   routine_test_day;   //������������
	rt_uint16_t   start_count;        //��������
	rt_uint32_t   test_count;         //���Լ���
	rt_uint32_t   single_runtime;     //��������ʱ��
	rt_uint32_t   total_runtime;      //�ۼ�����ʱ��
} system_run_info;

#define  SYS_RUN_INF   (*((system_run_info *)0x40024000))
extern u8 usb_host_flag;
extern u8 bar_code_flag;
//���Ե��ȿ�ʼ
/*******************************
��������  Test_Sched_Start
��  ����  ��ȡ���Բ����Ļص�ָ��

����ֵ��  ��
********************************/
void Test_Sched_Start(CALLBACK_GET_TESTPARAM   *p_Get_Test_Param)
{
    if(current_step_num == 1 && bar_code_flag == 0)
    {
        test_flag.bar_code_sw = START_SCAN_BAR_CODE;
        update_bar_code(NULL, "");
        clear_scan_code_buf();
    }
    
	if(usb_host_flag == 1)//���������ɨ��ǹ�������ɨ����������һ�β���
	{
		if(bar_code_flag == 1) //�Ѿ�ɨ����
		{
            test_flag.bar_code_sw = STOP_SCAN_BAR_CODE;
            bar_code_flag = 0;
		}
		else
		{
			struct panel_type *win;
			struct rect_type rect = {190, 140, 200, 300};
			struct font_info_t font = {0, 0X4208, 0x53fa, 1, 1, 16};
			
			win = sui_window_create(T_STR("���棡����","Warning! ! ! "), &rect);
			font.panel = win;
			rect.x = 0;
			rect.y = 80;
			rect.h = 8;
			rect.w = 300;
            
            if(language == 0)
            {
                ui_text_draw_alpha(&font, &rect, "�Ѿ�����ɨ��ǹ������ǰ��ɨ�룡");
			}
            else
            {
                ui_text_draw_alpha(&font, &rect, "Sweep gun inserted.");
                rect.y = 80 + 3 * rect.h;
                ui_text_draw_alpha(&font, &rect, "Sweep code before test.");
            }
			
			sui_window_update(win);
			return;
		}
	}
	
    
    
	// 	if(Test_Sched_Param.Pass_Flag == 0)                              return;
	if(Test_Sched_Param.Warning_Flag)                                return;
	
	if(Test_Sched_Param.Test_Sched_State == TEST_SCHED_STATE_RUNNING  &&
	        Test_Sched_Param.Pause_Flag       == 0)
		return;  //�����ǰ���ڲ��ԣ�ֱ���˳�
		
	Test_Sched_Param.Test_Sched_State     = TEST_SCHED_STATE_RUNNING;         //���Ե�����״̬Ϊ���ڲ���
	Test_Sched_Param.p_Get_Test_Param_Fun = p_Get_Test_Param;
	Test_Sched_Param.Pass_Flag            = 1;                                //��λ���Ժϸ��־λ
	Test_Sched_Param.Stop_Flag            = 0;                              //���STOP��־λ
	Test_Sched_Param.Short_Flag           = 0;                              //���Short_int��־λ
	Test_Sched_Param.gfi_Flag             = 0;                              //���arc_int��־λ
	Test_Sched_Param.arc_Flag             = 0;                              //���arc_int��־λ
	Test_Sched_Param.arc_Isable           = 0;                              //���arc_intʹ��
	Test_Sched_Param.Continue_Flag        = 0;                              //������������־
	
	if(Test_Sched_Param.Pause_Flag)                                         //�ж��ǲ����ɲ���PAUSE������
	{
		Test_Sched_Param.Pause_Flag         = 0;                              //�������Pause��־
		Test_Sched_Param.Continue_Flag      = 1;                              //��λ��־λ���Ա����һ����ʼ����
	}
	
	else
	{
		Test_Sched_Param.Continue_Flag      = 0;
	}
	
    #if LC_SAMPLING_DEBUG
    rt_timer_start(Test_Sampling_Timer);/* ����������ʱ�� */
    #endif
	rt_timer_start(Test_Sched_Timer); /* ������ʱ�� */
	test_loop_flag = 1;
	rt_mb_send(&test_sever_mb, 1);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource15);
	// 	mapanflag = 2;
	bsp_display(LED_PASS, 0);
	ui_key_updata(0xfc);
	SYS_RUN_INF.test_count++;
}

//���Ե��ȹر�
/*******************************
��������  Test_Sched_Close
��  ����  ��

����ֵ��  ��
********************************/
extern  void  Reset_Home (void);
void Test_Sched_Close(void)
{
    
	/* ����ֹͣ����������� */
	switch(Test_Sched_Param.p_Test_Param->com.mode)
	{
	//������ѹ
	case ACW:
	
		AC_Output_Disable();
		
		break;
		
	//ֱ����ѹ
	case DCW:
	
		DC_Output_Disable();
		
		break;
		
	//��Ե����
	case IR:
	
		break;
		
	//�ӵص���
	case GR:
		GR_Output_Disable();
		break;
		
	//й©
	case LC:
		LC_Assit_Output_Disable();
		LC_Main_Output_Disable();
		break;
		
	//PW
	case PW:
		LC_Main_Output_Disable();
		break;
		
	//ACW_GR
	case ACW_GR:
		AC_Output_Disable();
		GR_Output_Disable();
		break;
		
	//DCW_GR
	case DCW_GR:
		DC_Output_Disable();
		GR_Output_Disable();
		break;
		
	default:
	
		break;
	}
	
	
	bsp_display(LED_PASS, 0);
	
	if(Test_Sched_Param.Warning_Flag == 0)ui_key_updata(0);                                  //�ָ���������
	
	if(Test_Sched_Param.Stop_Flag)
	{
		bsp_display(LED_FAIL, 0);
		bsp_display(FMQ, 0);
		Test_Sched_Param.Warning_Flag         = 0;
		Test_Sched_Param.Short_Flag           = 0;
		Test_Sched_Param.arc_Flag             = 0;
		Test_Sched_Param.gfi_Flag             = 0;
		Reset_Home();
		
    test_flag.bar_code_sw = START_SCAN_BAR_CODE;
    update_bar_code(NULL, "");
    
		if(Test_Sched_Param.Test_Sched_State == TEST_SCHED_STATE_RUNNING)
		{
			char buf[20] = "";
			struct font_info_t font = {&panel_home, 0xffff, 0x0, 1, 1, 32};
			
			struct rect_type rect = {440, 72, 38, 200};
			strcpy(buf, T_STR("ֹͣ����","STOP"));
			ui_teststr_darw(&font, &rect, buf);
			Test_Sched_Param.Test_Sched_State     = TEST_SCHED_STATE_STOP;         //���Ե�����״̬Ϊֹͣ����
			Test_Sched_Param.Test_Status          = TEST_STATE_WAIT;
		}
	}
	
	else
	{
	
		Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
		Test_Sched_Param.Pass_Flag            = 0;
		
	}
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
	
	//��stop����������
	{
		rt_mb_send(&key_mb, KEY_EXIT | KEY_UP);
		rt_mb_send(&key_mb, KEY_EXIT | KEY_UP);
		rt_mb_send(&key_mb, KEY_EXIT | KEY_DOWN);
		rt_mb_send(&key_mb, KEY_EXIT | KEY_DOWN);
		panel_flag = 0;
	}
	//	ui_key_updata(0);
}


/*��·�ж�*/
void short_int(void)
{



	if(Test_Sched_Param.Stop_Flag == 1)return;
	
	if(Test_Sched_Param.Test_Sched_State != TEST_SCHED_STATE_RUNNING)return;
	
	Test_Sched_Close();
	
	bsp_display(LED_TEST, 0);
	bsp_display(LED_FAIL, 1);
	bsp_display(FMQ, 1);
	
	
	// 	Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
	Test_Sched_Param.Short_Flag           = 1;
	Test_Sched_Param.Pass_Flag            = 0;
	
	Test_Sched_Param.Test_Status = TEST_STATE_SHORT;
}


/*ARC�ж�*/
void arc_int(void)
{



	if(Test_Sched_Param.arc_Isable == 0)return;
	
	if(Test_Sched_Param.Stop_Flag == 1)return;
	
	if(Test_Sched_Param.Test_Sched_State != TEST_SCHED_STATE_RUNNING)return;
	
	Test_Sched_Close();
	
	bsp_display(LED_TEST, 0);
	bsp_display(LED_FAIL, 1);
	bsp_display(FMQ, 1);
	
	
	// 	Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
	Test_Sched_Param.arc_Flag             = 1;
	Test_Sched_Param.Pass_Flag            = 0;
	
	Test_Sched_Param.Test_Status = TEST_STATE_ARC_ALARM;
}

void gfi_cycle_clear_count(void)
{
    static uint32_t c = 0;
    
    /* 80������ 90���� ��Ϊ100 */
    if(++c > GFI_CLEAR_COUNT_CYCLE)
    {
        test_flag.gfi_delay_count = 0;
        c = 0;
    }
}

/*GFI�ж�*/
void GFI_int(void)
{
	if(0 == test_flag.gfi_en)
	{
		return;
	}
	
    if(++test_flag.gfi_delay_count < GFI_DELAY_COUNT_MAX)
    {
        return;
    }



	if(Test_Sched_Param.Stop_Flag == 1)return;
	
	if(Test_Sched_Param.Test_Sched_State != TEST_SCHED_STATE_RUNNING)return;
	
	Test_Sched_Close();
	
	bsp_display(LED_TEST, 0);
	bsp_display(LED_FAIL, 1);
	bsp_display(FMQ, 1);
	
	
	// 	Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
	Test_Sched_Param.gfi_Flag             = 1;
	Test_Sched_Param.Pass_Flag            = 0;
	
	Test_Sched_Param.Test_Status = TEST_STATE_GFI_ALARM;
}

extern void buzzer_test(u8 timer);

extern void ACW_GR_Mode_Test(struct step_acw_gr_t *acw_gr_test_para);
extern void DCW_GR_Mode_Test(struct step_dcw_gr_t *dcw_gr_test_para);
extern void PW_Mode_Test(struct step_pw_t *pw_test_para);
extern void IR_Mode_Test(struct step_ir_t *IR_test_para);

//�ܲ��Ե��Ⱥ���
/*******************************
��������  Test_Sched_Main
��  ����  ��

����ֵ��  ��
********************************/
void Test_Sched_Main(void *p_Param)
{
	uint8_t Is_Fail_Continue = 0;
	// 	static  uint8_t   delay_count = 0;
	// 	uint8_t test_step_state_bk;
	p_Param = p_Param;
	
	
	if(Test_Sched_Param.Test_Step_State == TEST_STEP_OUT)  //���ڲ���״̬
	{
	
		if(current_step_num > 1 && Test_Sched_Param.Continue_Flag == 0)
		{
			switch(Test_Sched_Param.p_Test_Param->com.mode)
			{
			//������ѹ
			case ACW:
			
				if(Test_Sched_Param.p_Test_Param->acw.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->acw.failstop)Is_Fail_Continue = 1;
				
				break;
				
			//ֱ����ѹ
			case DCW:
			
				if(Test_Sched_Param.p_Test_Param->dcw.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->dcw.failstop)Is_Fail_Continue = 1;
				
				break;
				
			//��Ե����
			case IR:
			
				break;
				
			//�ӵص���
			case GR:
			
				if(Test_Sched_Param.p_Test_Param->gr.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->gr.failstop)Is_Fail_Continue = 1;
				
				break;
				
			//й©
			case LC:
			
				if(Test_Sched_Param.p_Test_Param->lc.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->lc.failstop)Is_Fail_Continue = 1;
				
				break;
				
			//PW
			case PW:
			
				if(Test_Sched_Param.p_Test_Param->pw.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->pw.failstop)Is_Fail_Continue = 1;
				
				break;
				
			//ACW_GR
			case ACW_GR:
			
				if(Test_Sched_Param.p_Test_Param->acw_gr.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->acw_gr.failstop)Is_Fail_Continue = 1;
				
				break;
				
			//DCW_GR
			case DCW_GR:
			
				if(Test_Sched_Param.p_Test_Param->dcw_gr.stepcontinuation)
				{
				
				}
				else
				{
					Test_Sched_Param.Pause_Flag = 1;
				}
				
				if(Test_Sched_Param.p_Test_Param->dcw_gr.failstop)Is_Fail_Continue = 1;
				
				break;
				
			default:
			
				break;
			}
			
			
		}
		
		
		
		if(Test_Sched_Param.Pause_Flag && Test_Sched_Param.Continue_Flag == 0 &&
		        current_step_num <= file_info[flash_info.current_file].totalstep   &&
		        Test_Sched_Param.Stop_Flag == 0
		  )
		{
			rt_timer_stop(Test_Sched_Timer);
			test_loop_flag = 0;
			Test_Sched_Param.Stop_Flag = 1;
			return;
		}
		
		Test_Sched_Param.Continue_Flag = 0;
		
		
		
		if(Test_Sched_Param.Warning_Flag)
		{
			if(Is_Fail_Continue)
			{
				Test_Sched_Param.Stop_Flag    = 0;
				Test_Sched_Param.Warning_Flag = 0;
				bsp_display(LED_FAIL, 0);
				bsp_display(FMQ, 0);
				Test_Sched_Param.Short_Flag   = 0;
				Test_Sched_Param.arc_Flag     = 0;
				Test_Sched_Param.gfi_Flag     = 0;
			}
		}
		
		else
		{
			if(Test_Sched_Param.Stop_Flag)rt_timer_stop(Test_Sched_Timer);
			
			if(Test_Sched_Param.Stop_Flag)test_loop_flag = 0;
		}
		
		//ȡ��һ��������ָ��
		if(Test_Sched_Param.p_Get_Test_Param_Fun != NULL)
			Test_Sched_Param.p_Test_Param = Test_Sched_Param.p_Get_Test_Param_Fun();
			
		if(Test_Sched_Param.p_Test_Param == RT_NULL)
		{
		
			/*�������*/
			if(Test_Sched_Param.Pass_Flag)                     //���Ժϸ�
			{
				buzzer_test(file_info[flash_info.current_file].buzzertime * 10);   //����������
				bsp_display(LED_PASS, 1);                        //����PASS��
				bsp_display(LED_FAIL, 0);                        //�ر�Fail��
				AC_Output_Disable();                             //�ر�AC���
				DC_Output_Disable();                             //�ر�DC���
				GR_Output_Disable();                             //�ر�GR���
				Test_Sched_Param.Stop_Flag    = 1;               //��ֹͣ��־��λ��ֹͣ����
				Test_Sched_Param.Test_Status = TEST_STATE_PASS;  //���Ժϸ�
				
				if(Test_Sched_Param.Offset_Get_Flag)             //����ǿ�����ƫ�ƣ���¼ƫ��ֵ
				{
					Test_Sched_Param.Offset_Get_Flag = 0;          //�����¼offset�ı���λ
					save_steps_to_flash(flash_info.current_file);  //�������в���offsetֵ
				}
				
			}
			
			else 																						  //���Բ��ϸ�
			{
				if(Test_Sched_Param.Stop_Flag == 0)              //�ж��Ƿ����ⲿ������ֹ
				{
					bsp_display(LED_FAIL, 1);                      //����Fail��
					bsp_display(FMQ, 1);                           //��������
					Test_Sched_Param.Stop_Flag    = 1;             //��ֹͣ��־��λ��ֹͣ����
					Test_Sched_Param.Warning_Flag = 1;             //���Դ����־��λ
				}
			}
			
			bsp_display(LED_TEST, 0);//�رղ���ָʾ��
			
			Test_Sched_Param.Test_Sched_State = TEST_SCHED_STATE_STOP;         //���Ե�����״̬Ϊֹͣ����
			
        #if LC_SAMPLING_DEBUG
            rt_timer_stop(Test_Sampling_Timer);//ֹͣ������ʱ��
        #endif
			rt_timer_stop(Test_Sched_Timer);                   //ֹͣ�����ö�ʱ��
			test_loop_flag = 0;                                //�˳�����ѭ�����ó�CPU��Դ
			
			SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);   //�ó��жϸ�����
		}
		
		
	}
	
	switch(Test_Sched_Param.p_Test_Param->com.mode)
	{
	//������ѹ
	case ACW:
	
		ACW_Mode_Test(&Test_Sched_Param.p_Test_Param->acw);
		
		break;
		
	//ֱ����ѹ
	case DCW:
	
		DCW_Mode_Test(&Test_Sched_Param.p_Test_Param->dcw);
		
		break;
		
	//��Ե����
	case IR:
		IR_Mode_Test(&Test_Sched_Param.p_Test_Param->ir);
		break;
		
	//�ӵص���
	case GR:
	
		GR_Mode_Test(&Test_Sched_Param.p_Test_Param->gr);
		
		break;
		
	//й©
	case LC:
	
		LC_Mode_Test(&Test_Sched_Param.p_Test_Param->lc);
		
		break;
		
	//PW
	case PW:
	
		PW_Mode_Test(&Test_Sched_Param.p_Test_Param->pw);
		
		break;
		
	//ACW_GR
	case ACW_GR:
	
		ACW_GR_Mode_Test(&Test_Sched_Param.p_Test_Param->acw_gr);
		
		break;
		
	//DCW_GR
	case DCW_GR:
	
		DCW_GR_Mode_Test(&Test_Sched_Param.p_Test_Param->dcw_gr);
		
		break;
		
	default:
	
		break;
	}
	
	
	
	if(Test_Sched_Param.Test_Step_State == TEST_STEP_OUT)
	{
		/*����ֹͣ*/
		
	}
	
	
	
	
	
}



static void Test_Sched_Sever(void *parameter)
{
	u8 msg;
	rt_mb_init(&test_sever_mb , "test_sever_mb",  test_sever_mb_pool , sizeof(test_sever_mb_pool) / 4 , RT_IPC_FLAG_FIFO); /* ����FIFO��ʽ�����̵߳ȴ�*/
	
	while(1)
	{
		if (rt_mb_recv(&test_sever_mb, (rt_uint32_t *)&msg, RT_TICK_PER_SECOND / 10) == RT_EOK)
		{
			while(test_loop_flag)
			{
				if (rt_mb_recv(&test_sever_clock_mb, (rt_uint32_t *)&msg, RT_TICK_PER_SECOND / 100) == RT_EOK)
					Test_Sched_Main(NULL);
					
				/*�˴����GFI�ļ��*/
				{
					static uint8_t gfi_count = 0;
					
					if(GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_8) == 0)
					{
						gfi_count++;
					}
					
					else
					{
						gfi_count = 0;
					}
					
					if(gfi_count > 2)GFI_int();
				}
			}
		}
	}
	
	
}

static void Test_Sched_Clock_Sever(void *parameter)
{
	rt_mb_send(&test_sever_clock_mb, 1);
}

uint16_t cur_sample_buf[2][1000];
// uint16_t cur_max;
// uint16_t cur_min;
// uint16_t vol_max;
// uint16_t vol_min;
uint16_t cur_ave;
uint16_t vol_ave;
volatile int sampling_mutex_flag;//����ʹ�õĻ�����

void count_max_min_value(uint16_t *buf, uint16_t n, uint16_t *max, uint16_t *min)
{
    int i = 0;
    uint16_t t_max;
    uint16_t t_min;
    
    t_max = buf[0];
    t_min = buf[0];
    
    for(i = 0; i < n; i++)
    {
        if(t_max < buf[i])
        {
            t_max = buf[i];
        }
        
        if(t_min > buf[i])
        {
            t_min = buf[i];
        }
    }
    
    *max = t_max;
    *min = t_min;
}



/********************************************************************/
#define DEEP_SLOW		500
#define DEEP_FAST		5
#define DEEP_MID		80
#define SLITHER_DEEP    DEEP_SLOW
#define SLITHER_DEEP_	DEEP_SLOW
typedef struct{
    uint16_t max;
    uint16_t min;
    uint32_t deep;
    uint32_t index;
    uint64_t sum;
    uint32_t c;
    uint16_t buf[SLITHER_DEEP_];/* �˲����� */
}SAMBLE_INFO;

SAMBLE_INFO lc_sample_buf[2];
#define     LC_FS_CUR  0
#define     LC_FS_VOL  1


void clear_samble_buf(SAMBLE_INFO *inf)
{
    memset(((uint8_t*)inf) + 8 , 0, sizeof(SAMBLE_INFO) - 8);
}

uint16_t ave_sample(SAMBLE_INFO *info, uint16_t value)
{
    uint16_t res = 0;
	uint16_t deep = info->deep;
    
    info->index = (info->index + 1) % deep;
    
    info->sum -= info->buf[info->index];
    
    info->buf[info->index] = value;
    
    info->sum += info->buf[info->index];
    
    if(info->c < deep)
    {
        info->c++;
        count_max_min_value(&info->buf[1], info->c, &info->max, &info->min);
    }
    else
    {
        info->c = deep;
        count_max_min_value(info->buf, info->c, &info->max, &info->min);
    }
    
    if(value <= 10)
    {
        if(info->index > 0)
        {
            if(info->buf[info->index - 1] <= 10)
            {
                info->max = 0;
            }
        }
        else
        {
            if(info->buf[deep - 1] <= 10)
            {
                info->max = 0;
            }
        }
    }
    
    if(info->index > 3)
    {
        #define CHANGE_GREAD    3
        uint16_t t1 = info->buf[info->index - 2];
        uint16_t t2 = info->buf[info->index - 1] + CHANGE_GREAD;
        uint16_t t3 = info->buf[info->index - 1];
        uint16_t t4 = info->buf[info->index] + CHANGE_GREAD;
        uint16_t t5 = info->buf[info->index - 2] + CHANGE_GREAD;
        uint16_t t6 = info->buf[info->index - 1];
        uint16_t t7 = info->buf[info->index - 1] + CHANGE_GREAD;
        uint16_t t8 = info->buf[info->index];
        
        if(t1 > t2 || t3 > t4)
        {
            info->max = info->min = value;
            clear_samble_buf(info);
        }
        else if(t5 < t6 || t7 < t8)
        {
            info->max = info->min = value;
            clear_samble_buf(info);
        }
    }
    
    if(value + 50 < info->max)
    {
        info->max = value;
        clear_samble_buf(info);
    }
    
    if(info->min + 50 < info->max)
    {
        info->min = info->max;
    }
    
    res = info->sum / info->c;
    
    return res;
}

static void Test_Sampling_Clock_Sever(void *parameter)
{
    static int i = 0;
    static int size = 100;
    uint16_t t_cur;
    uint16_t t_vol;
    
    lc_sample_buf[LC_FS_CUR].deep = SLITHER_DEEP;
    lc_sample_buf[LC_FS_VOL].deep = 20;
    i = (i + 1) % size;
    
    t_cur = D3_Mcp3202_Read(0);
    
    if(0xffff == t_cur)
    {
        return;
    }
//     t_vol = D16_Mcp3202_Read(0);
    ave_sample(&lc_sample_buf[LC_FS_CUR], t_cur);
//     vol_ave = ave_sample(&lc_sample_buf[LC_FS_VOL], t_vol);
//     cur_sample_buf[1][i] = D16_Mcp3202_Read(0);
    
//     count_max_min_value(cur_sample_buf[0], size, &cur_max, &cur_min);
//     count_max_min_value(cur_sample_buf[1], size, &vol_max, &vol_min);
    
    cur_ave = (lc_sample_buf[LC_FS_CUR].max + lc_sample_buf[LC_FS_CUR].min) / 2;
//     vol_ave = (lc_sample_buf[LC_FS_VOL].max + lc_sample_buf[LC_FS_VOL].min) / 2;
}





