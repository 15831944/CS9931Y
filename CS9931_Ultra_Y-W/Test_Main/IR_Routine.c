/*������ͷ�ļ�*/
#include "driver.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "DCW_Routine.h"
#include "Test_Sched.h"
#include "memorymanagement.h"
#include "Cal.h"
#include "PLC.h"
#include "IR.h"

#define  UPPER_LIMIT          (0)
#define  DOWN_LIMIT           (1)
#define  TEST_PASS            (2)
#define  SHORT_WARNING        (3)
#define  GFI_WARNING          (4)
#define  ARC_WARNING          (5)
#define  VOL_ERROR            (6)


#define  GEAR_10M             (1)
#define  GEAR_100M            (2)
#define  GEAR_1G              (3)
#define  GEAR_10G             (4)

extern  char       mem_str_vol[],mem_str_cur[],mem_str_res[],mem_str_time[];


void IR_Test_Inerface(uint8_t type,void *value);
static uint8_t Auto_Change_Gear(void);

typedef struct{
	uint8_t   Test_Keep_Flag : 1;                     //�������Ա�־
	uint8_t   Gear           : 4;                     //��λ
	uint16_t  Wait_Time;                              //�����ѹǰ�ĵȴ�ʱ��
	uint16_t  Test_Time;                              //����ʱ��
	uint16_t  Down_Time;                              //�½�ʱ��
	uint16_t  Pause_Time;                             //����ʱ��
	uint16_t  Voltage_Rise_Number;                    //��ѹ�����Ĵ���
	uint16_t  Voltage_Down_Number;                    //��ѹ�½��Ĵ���
	uint32_t  Voltage_Rise_Interval;                  //��ѹ�����ļ��
	uint32_t  Voltage_Down_Interval;                  //��ѹ�½��ļ��
	uint16_t  Voltage_Start_Value;                    //��ʼ�����ѹ
	uint16_t  Voltage_Process_Value;                  //���������ѹ
	uint16_t  Voltage_Final_Value;                    //���������ѹ
	uint16_t  Voltage_Out_Freq;                       //�����ѹƵ��
	uint16_t  Voltage_Wait_Count;                     //���Եȴ�����                 
	uint16_t  Voltage_Rise_Count;                     //��ѹ��������
	uint16_t  Testing_Count;                          //����ʱ�����
	uint16_t  Voltage_Down_Count;                     //��ѹ�½�����
	uint16_t  Pause_Count;                            //����������
	uint32_t  current_value;
	uint32_t  Voltage_value; 
	uint32_t  IR_Res_Value;
}TEST_PARAM;






static struct step_ir_t IR_Mode_Param,*p_IR_Mode_Param;
static TEST_PARAM IR_Test_Param;




static void IRModeTestEnvironmentEnter(struct step_ir_t *ir_test_para)
{
	IR_Mode_Param = *ir_test_para;
		
	if(IR_Mode_Param.waittime < 1)IR_Test_Param.Wait_Time = 1;             //�ȴ�ʱ������
 	else IR_Test_Param.Wait_Time = IR_Mode_Param.waittime;                 //ȷ��������0.1s
	
	if(IR_Mode_Param.ramptime==0){
		IR_Test_Param.Voltage_Rise_Number = 1;
	}else{
		IR_Test_Param.Voltage_Rise_Number = IR_Mode_Param.ramptime;
	}
	
	
	IR_Test_Param.Test_Keep_Flag        = 0;
	IR_Test_Param.Test_Time             = IR_Mode_Param.testtime;
	IR_Test_Param.Pause_Time            = IR_Mode_Param.pausetime;
	
	if(IR_Mode_Param.testtime == 0)IR_Test_Param.Test_Keep_Flag = 1;
	
	IR_Test_Param.Voltage_Start_Value   = 0;
	
	IR_Test_Param.Voltage_Process_Value = 0;
	
	IR_Test_Param.Voltage_Final_Value   = IR_Mode_Param.outvol;
	
	IR_Test_Param.Voltage_Wait_Count    = 0;
	
	IR_Test_Param.Voltage_Rise_Count    = 0;
	
	IR_Test_Param.Testing_Count         = 0;
	
	IR_Test_Param.current_value         = 0;
	
	IR_Test_Param.Pause_Count           = 0;
	
	IR_Test_Param.Gear                  = IR_Mode_Param.autogear;
	
	IR_Test_Param.Voltage_Rise_Interval = (IR_Mode_Param.outvol - 0) / IR_Test_Param.Voltage_Rise_Number;


	IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_TEST_WAIT);
	if(Test_Sched_Param.Stop_Flag == 0)DC_Output_Enable();  //ʹ��DC���
	
	LC_Relay_Control(LC_NY,0,1);
	
	{
		EXTI_InitTypeDef EXTI_InitStructure;

		/* Configure  EXTI  */
		EXTI_InitStructure.EXTI_Line = EXTI_Line15 | EXTI_Line7 | EXTI_Line10;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//Falling�½��� Rising����
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

		EXTI_Init(&EXTI_InitStructure);
	}
	
	
	switch(IR_Mode_Param.autogear)
	{
		case 0:  //AUTO_GEAR
			IR_Test_Param.Gear = GEAR_10M;
			Sampling_Relay_State_CHange(DC_2mA);
		break;
		
		case 1:  //10M��
			Sampling_Relay_State_CHange(DC_2mA);
		break;
		
		case 2:  //100M��
			Sampling_Relay_State_CHange(DC_200uA);
		break;
		
		case 3:  //1G��
			Sampling_Relay_State_CHange(DC_20uA);
		break;
		
		case 4:  //10G��
			Sampling_Relay_State_CHange(DC_2uA);
		break;
		
		case 5:  //100G��
			IR_Test_Param.Gear = GEAR_10G;
			Sampling_Relay_State_CHange(DC_2uA);
		break;
		
		default:
			Sampling_Relay_State_CHange(DC_2mA);
		
		break;
		
	}
		
	
	
	
	
	Relay_ON(ACW_DCW_IR);
	ctrl_relay_EXT_DRIVE_O4_O5(RELAY_OFF);///<2017.5.11 wangxin

}


void IRModeTestEnvironmentExit(void)
{
	DC_SetVoltage(0);
	DC_Output_Disable();
	Relay_OFF(ACW_DCW_IR);
}


void IR_Range_Check(uint8_t type)
{
	if(IR_Test_Param.IR_Res_Value == 0xFFFFFFFF)return;
	switch(type){
		case UPPER_LIMIT:
			if(IR_Mode_Param.reshigh == 0)return;
			if(IR_Test_Param.IR_Res_Value > IR_Mode_Param.reshigh){
				/*��������*/
				Test_Sched_Param.Test_Status          = TEST_STATE_HIGH;
				IR_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)UPPER_LIMIT);
				Test_Sched_Param.Pass_Flag = 0;  //������Ժϸ��־λ
				bsp_display(LED_TEST,0);
				IRModeTestEnvironmentExit();
				Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
				Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
				Test_Sched_Param.Warning_Flag         = 1;
				result_save(IR,HIGH,(void *)&IR_Mode_Param,IR_Test_Param.Voltage_value,0,0,IR_Test_Param.IR_Res_Value,IR_Test_Param.Testing_Count);
			}
		break;
		
		case DOWN_LIMIT:
			if(IR_Mode_Param.reslow == 0)return;
			if(IR_Test_Param.IR_Res_Value < IR_Mode_Param.reslow){
				/*��������*/
				Test_Sched_Param.Test_Status          = TEST_STATE_LOW;
				IR_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)DOWN_LIMIT);
				Test_Sched_Param.Pass_Flag = 0;  //������Ժϸ��־λ
				bsp_display(LED_TEST,0);
				IRModeTestEnvironmentExit();
				Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
				Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
				Test_Sched_Param.Warning_Flag         = 1;
				result_save(IR,LOW,(void *)&IR_Mode_Param,IR_Test_Param.Voltage_value,0,0,IR_Test_Param.IR_Res_Value,IR_Test_Param.Testing_Count);
			}
		break;
			
		case VOL_ERROR:    //�жϵ�ѹ�Ƿ��쳣
			if(IR_Test_Param.Voltage_value  < (IR_Test_Param.Voltage_Final_Value / 2)){
				/*��������*/
				Test_Sched_Param.Test_Status          = TEST_STATE_VOL_ABNORMAL;
				IR_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)VOL_ERROR);
				IR_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(IR_Test_Param.Voltage_value));		  //����ѹֵˢ�µ���Ļ
				Test_Sched_Param.Pass_Flag = 0;  //������Ժϸ��־λ
				bsp_display(LED_TEST,0);
				IRModeTestEnvironmentExit();
				Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
				Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
				Test_Sched_Param.Warning_Flag         = 1;
				result_save(IR,VOL_ABNORMAL,(void *)&IR_Mode_Param,IR_Test_Param.Voltage_value,0,0,IR_Test_Param.IR_Res_Value,IR_Test_Param.Testing_Count);
			}
			
		break;
		
		default:
			
		break;
	}
}


void IR_Mode_Test(struct step_ir_t *IR_test_para)
{
	if(Test_Sched_Param.Short_Flag)
	{
		Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
		Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
		Test_Sched_Param.Warning_Flag         = 1;
		IR_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)SHORT_WARNING);

		result_save(IR,SHORT,(void *)&IR_Mode_Param,IR_Test_Param.Voltage_value,0,0,IR_Test_Param.IR_Res_Value,IR_Test_Param.Testing_Count);
		return;
	}
	
	
	if(Test_Sched_Param.gfi_Flag)
	{
		Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
		Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
		Test_Sched_Param.Warning_Flag         = 1;
		IR_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)GFI_WARNING);
		result_save(IR,GFI,(void *)&IR_Mode_Param,IR_Test_Param.Voltage_value,0,0,IR_Test_Param.IR_Res_Value,IR_Test_Param.Testing_Count);
		return;
	}
	
	if(Test_Sched_Param.Stop_Flag){                            //Stop�������£�����
		DC_Output_Disable();
		Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
		IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_STOP);
	}
	if(Test_Sched_Param.Test_Step_State == TEST_STEP_OUT){     //���ڲ���״̬
		IRModeTestEnvironmentEnter(IR_test_para);              //��ʼ��
		Test_Sched_Param.Test_Step_State = TEST_STEP_TEST_WAIT;  //�������״̬
		PLC_Testing_Out(1);
	}
	switch(Test_Sched_Param.Test_Step_State){
				
		//�ȴ�����״̬
		case TEST_STEP_TEST_WAIT:
			if(--IR_Test_Param.Wait_Time == 0){
				if(IR_Test_Param.Voltage_Rise_Number == 1){           //�������ʱ��Ϊ0
					Test_Sched_Param.Test_Step_State = TEST_STEP_TESTING;
					IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_TESTING);
					IR_Test_Param.Voltage_Process_Value = IR_Test_Param.Voltage_Final_Value;
					DC_SetVoltage(IR_Test_Param.Voltage_Process_Value);
				}else{
					Test_Sched_Param.Test_Step_State = TEST_STEP_VOL_RISE;
					IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_VOL_RISE);
					DC_SetVoltage(IR_Test_Param.Voltage_Start_Value);
				}
				
			}
		
			IR_Test_Param.Voltage_Wait_Count++;
			IR_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)IR_Test_Param.Voltage_Wait_Count);
		break;
			
		
		//��ѹ����״̬
		case TEST_STEP_VOL_RISE:
			
			IR_Test_Param.Voltage_value=DC_GetVoltage();
			IR_Test_Param.current_value=DC_GetCurrent();
			
			IR_Test_Param.IR_Res_Value  = IR_Get_RS(IR_Test_Param.Gear);
		
			if(IR_Mode_Param.autogear == 0)  //��Ҫ�Զ�����
			{
				if(Auto_Change_Gear())
				{
					
				}
			}
			
			if(--IR_Test_Param.Voltage_Rise_Number == 0){   //��ѹ��������Ŀ���ѹ
				Test_Sched_Param.Test_Step_State = TEST_STEP_TESTING;					
				IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_TESTING);
				DC_SetVoltage(IR_Test_Param.Voltage_Final_Value);					
			}else{
				IR_Test_Param.Voltage_Process_Value += IR_Test_Param.Voltage_Rise_Interval;
				DC_SetVoltage(IR_Test_Param.Voltage_Process_Value);	
			}
			
			if(Test_Sched_Param.Offset_Is_Flag){
				IR_Test_Param.current_value = IR_Test_Param.current_value>(*p_IR_Mode_Param).offsetvalue? (IR_Test_Param.current_value - (*p_IR_Mode_Param).offsetvalue) : 0;
			}
			
			/*�ж�����*/
			
			
			IR_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(IR_Test_Param.Voltage_value));
			IR_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(IR_Test_Param.current_value));
			IR_Test_Param.Voltage_Rise_Count++;             //����ʱ���1
			
			IR_Test_Inerface(TEST_RESISTER_REFRESH_EVENT,(void *)(IR_Test_Param.IR_Res_Value));
			IR_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)IR_Test_Param.Voltage_Rise_Count);
			IR_Range_Check(UPPER_LIMIT);
		
		break;
		//����״̬ 
		case TEST_STEP_TESTING:
	
			IR_Test_Param.Voltage_value = DC_GetVoltage();
			IR_Test_Param.current_value = DC_GetCurrent();
			
			IR_Test_Param.IR_Res_Value  = IR_Get_RS(IR_Test_Param.Gear);
		
			if(IR_Mode_Param.autogear == 0)  //��Ҫ�Զ�����
			{
				if(Auto_Change_Gear())
				{
					
				}
			}
		
			if(IR_Test_Param.Test_Keep_Flag){              //����ʱ��Ϊ0��һֱ����
				/*�ж�����*/
				if(IR_Test_Param.Testing_Count >= 10000)IR_Test_Param.Testing_Count = 0;
			}else{
				if(--IR_Test_Param.Test_Time == 0){
					if(1){           //����½�ʱ��Ϊ0
						if(IR_Test_Param.Pause_Time > 0 && current_step_num <= file_info[flash_info.current_file].totalstep){                   //��������Ϊ0
							Test_Sched_Param.Test_Step_State = TEST_STEP_PAUSE;
							IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_PAUSE);
						}else{                                               //������Ϊ0�����β��Խ���
							Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
						}
						IR_Range_Check(VOL_ERROR);
						DC_SetVoltage(0);
						DC_Output_Disable();

					}else{
						Test_Sched_Param.Test_Step_State = TEST_STEP_VOL_DOWN;		
						IR_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_VOL_DOWN);		
					}
				}
			}
			
			IR_Test_Inerface(TEST_RESISTER_REFRESH_EVENT,(void *)(IR_Test_Param.IR_Res_Value));
			
			IR_Test_Param.Testing_Count++;             //����ʱ���1			
			IR_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)IR_Test_Param.Testing_Count);
			
			if(Test_Sched_Param.Test_Step_State != TEST_STEP_TESTING)
			{
				break;
			}
			
			if(Test_Sched_Param.Offset_Is_Flag){
				IR_Test_Param.current_value = IR_Test_Param.IR_Res_Value>(*p_IR_Mode_Param).offsetvalue? (IR_Test_Param.IR_Res_Value - (*p_IR_Mode_Param).offsetvalue) : 0;
			}
			
			
			IR_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(IR_Test_Param.Voltage_value));	
			IR_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(IR_Test_Param.current_value));
			IR_Range_Check(UPPER_LIMIT);
			if(IR_Test_Param.Testing_Count > 4)IR_Range_Check(DOWN_LIMIT);
			
			if(Test_Sched_Param.Offset_Get_Flag){
				(*p_IR_Mode_Param).offsetvalue = IR_Test_Param.IR_Res_Value;
				
			}
			
		break;
		//��ѹ�½�״̬
		case TEST_STEP_VOL_DOWN:
			
		
		break;
		//����ȴ�
		case TEST_STEP_PAUSE:
			if(IR_Mode_Param.steppass)
			{
				PLC_Pass_Out(1);
				PLC_Testing_Out(0);
			}
			
			IR_Range_Check(UPPER_LIMIT);
			IR_Test_Param.Pause_Count++;
			IR_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)IR_Test_Param.Pause_Count);

			IR_Range_Check(UPPER_LIMIT);
			if(--IR_Test_Param.Pause_Time == 0){
				Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
				PLC_Pass_Out(0);
			}
		break;
		//����ֹͣ
		case TEST_STEP_STOP:	
			
			
			IRModeTestEnvironmentExit();
			Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
			bsp_display(LED_TEST,0);
			
			result_save(IR,PASS,(void *)&IR_Mode_Param,IR_Test_Param.Voltage_value,0,0,IR_Test_Param.IR_Res_Value,IR_Test_Param.Testing_Count);
			Test_Sched_Main((void *)0);
		break;
		//����״̬
		default:
			//����Ӧ�����˴�
		break;
		
		
		
	}
	
}



extern void ui_teststr_darw(struct font_info_t *font,struct rect_type *rect,char *str);
void IR_Test_Inerface(uint8_t type,void *value)
{
	char buf[20] = "";
	struct font_info_t font={&panel_home,0xffff,0x0,1,1,32};
	switch(type){
		
		case TEST_STATE_REFRESH_EVENT:          //����״̬�¼�
		{
			u32 temp = (u32)value;
			struct rect_type rect={440,72,38,200};
			switch(temp)
			{
				case TEST_STEP_TEST_WAIT:
					Test_Sched_Param.Test_Status = TEST_STATE_WAIT;
					strcpy(buf,T_STR("�ȴ�ʱ��","WAIT"));
					break;
				case TEST_STEP_VOL_RISE:
					Test_Sched_Param.Test_Status = TEST_STATE_VOL_RISE;
					strcpy(buf,T_STR("����ʱ��","RiseTime"));
					break;
				case TEST_STEP_TESTING:
					Test_Sched_Param.Test_Status = TEST_STATE_TEST;
					strcpy(buf,T_STR("����ʱ��","TestTime"));
					break;
				case TEST_STEP_VOL_DOWN:
					Test_Sched_Param.Test_Status = TEST_STATE_VOL_DOWN;
						strcpy(buf,T_STR("�½�ʱ��","FallTime"));
					break;
				case TEST_STEP_PAUSE:
					Test_Sched_Param.Test_Status = TEST_STATE_PAUSE;
					strcpy(buf,T_STR("���ʱ��","Interval"));
					break;
				case TEST_STEP_STOP:
					Test_Sched_Param.Test_Status = TEST_STATE_STOP;
					strcpy(buf,T_STR("ֹͣ����","STOP"));
					break;
			}
			ui_teststr_darw(&font,&rect,buf);
		}
		break;
		
		case TEST_VOLTAGE_REFRESH_EVENT:        //���Ե�ѹ�¼�
//		if(Test_Sched_Param.Test_Step_State == TEST_STEP_STOP)return;
		{
			u32 voltage = (u32)value;
			struct rect_type rect={242,140,38,398};
			
			rt_sprintf(buf,"%d.%03dkV", voltage/1000,voltage%1000);
			ui_teststr_darw(&font,&rect,buf);
			strcpy(mem_str_vol,"                   ");
			strncpy(mem_str_vol,buf,strlen(buf));
		}
// 		bsp_display(LED_TEST,2);
		break;
		
		case TEST_CURRENT_REFRESH_EVENT:        //���Ե����¼�
		if(Test_Sched_Param.Test_Step_State == TEST_STEP_STOP)return;
		strcpy(mem_str_cur,"                   ");

		break;
		
		case TEST_RESISTER_REFRESH_EVENT:       //���Ե����¼�
			if(Test_Sched_Param.Test_Step_State == TEST_STEP_STOP)return;
			{
				u32 res = (u32)value;
				
				struct rect_type rect={242,208,38,398};
				if(res == 0xFFFFFFFF){
					rt_sprintf(buf,"-.---M��");
				}else{
					if(res<10000)//0~100M
					{
						rt_sprintf(buf,"%d.%02dM��", res/100,res%100);
					}else if(res<100000)//100M~1G
					{
						res = res / 10;											
						rt_sprintf(buf,"%d.%01dM��", res/10,res%10);
					}else if(res<1000000)//1G~10G
					{
						res = res / 100;											
						rt_sprintf(buf,"%d.%03dG��", res/1000,res%1000);
					}
					else if(res<10000000)//10G~100G
					{
						res = res / 1000;											
						rt_sprintf(buf,"%d.%02dG��", res/100,res%100);
					}
					else 
					{
						res = res / 10000;											
						rt_sprintf(buf,"%d.%01dG��", res/10,res%10);
					}
				}
				

				ui_teststr_darw(&font,&rect,buf);
				
				strcpy(mem_str_res,"                   ");
				strncpy(mem_str_res,buf,strlen(buf));
			}
		break;
		 
		case TEST_WARNING_REFRESH_EVENT:        //���Ա����¼�
		{
			u32 temp = (u32)value;
			struct rect_type rect={440,72,38,200};
			switch(temp)
			{
				case UPPER_LIMIT:
					strcpy(buf,T_STR("���ޱ���","High Fail"));
					bsp_display(LED_FAIL,1);
					bsp_display(FMQ,1);
				break;
				case DOWN_LIMIT:
					strcpy(buf,T_STR("���ޱ���","Low Fail"));
					bsp_display(LED_FAIL,1);
					bsp_display(FMQ,1);
				break;
				case SHORT_WARNING:
					strcpy(buf,T_STR("��·����","Short Fail"));
					bsp_display(LED_FAIL,1);
					bsp_display(FMQ,1);
				break;
				case GFI_WARNING:
					strcpy(buf,T_STR("GFI����","GFI Fail"));
					bsp_display(LED_FAIL,1);
					bsp_display(FMQ,1);
				break;
				case ARC_WARNING:
					strcpy(buf,T_STR("ARC����","ARC Fail"));
					bsp_display(LED_FAIL,1);
					bsp_display(FMQ,1);
				break;
				case VOL_ERROR:
					strcpy(buf,T_STR("��ѹ����","Vol.Fail"));
					bsp_display(LED_FAIL,1);
					bsp_display(FMQ,1);
				break;
			}
			ui_teststr_darw(&font,&rect,buf);
			}
		break;
		
		case TEST_TIME_COUNT_REFRESH_EVENT:     //����ʱ���¼�
		{
			u32 time = (u32)value;
			struct rect_type rect={242,344,38,398};
			
			rt_sprintf(buf,"%d.%ds", time/10,time%10);
			ui_teststr_darw(&font,&rect,buf);
			strcpy(mem_str_time,"                   ");
			strncpy(mem_str_time,buf,strlen(buf)-1);
		}
		if(Test_Sched_Param.Stop_Flag == 0)bsp_display(LED_TEST,2);
		break;
		
		default:
			
		break;
		
	}
	
}

static uint8_t Auto_Change_Gear(void)
{
	uint8_t Is_need_Change = 0;
	
	switch(IR_Test_Param.Gear)
	{
		case GEAR_10M:
			if(IR_Test_Param.IR_Res_Value>=1100)
			{
				IR_Test_Param.Gear = GEAR_100M;
				Is_need_Change = 1;
			}
		break;
		
		case GEAR_100M:
			if(IR_Test_Param.IR_Res_Value>=11000)
			{
				IR_Test_Param.Gear = GEAR_1G;
				Is_need_Change = 1;
			}
			if(IR_Test_Param.IR_Res_Value<=950)
			{
				IR_Test_Param.Gear = GEAR_10M;
				Is_need_Change = 1;
			}
		break;
		
		case GEAR_1G:
			if(IR_Test_Param.IR_Res_Value>=105000)
			{
				IR_Test_Param.Gear = GEAR_10G;
				Is_need_Change = 1;
			}
			if(IR_Test_Param.IR_Res_Value<=9500)
			{
				IR_Test_Param.Gear = GEAR_100M;
				Is_need_Change = 1;
			}
		break;
		
		case GEAR_10G:
			if(IR_Test_Param.IR_Res_Value<=95000)
			{
				IR_Test_Param.Gear = GEAR_1G;
				Is_need_Change = 1;
			}
		break;
		
		default:
			
		break;
	
	}
	
	if(Is_need_Change)
	{
		switch(IR_Test_Param.Gear)
		{
							
			case GEAR_10M:  //10M��
				Sampling_Relay_State_CHange(DC_2mA);
			break;
			
			case GEAR_100M:  //100M��
				Sampling_Relay_State_CHange(DC_200uA);
			break;
			
			case GEAR_1G:  //1G��
				Sampling_Relay_State_CHange(DC_20uA);
			break;
			
			case GEAR_10G:  //10G��
				Sampling_Relay_State_CHange(DC_2uA);
			break;
			
			case 5:  //100G��
				Sampling_Relay_State_CHange(DC_2uA);
			break;
			
			default:
				Sampling_Relay_State_CHange(DC_2mA);
			
			break;
			
		}
	}
	
	return Is_need_Change;
}
