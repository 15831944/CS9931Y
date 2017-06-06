/*������ͷ�ļ�*/
#include "driver.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "DCW_Routine.h"
#include "Test_Sched.h"
#include "memorymanagement.h"
#include "Cal.h"
#include "PLC.h"

#define  UPPER_LIMIT          (0)
#define  DOWN_LIMIT           (1)
#define  TEST_PASS            (2)
#define  SHORT_WARNING        (3)
#define  GFI_WARNING          (4)
#define  ARC_WARNING          (5)
#define  VOL_ERROR            (6)

extern  char       mem_str_vol[],mem_str_cur[],mem_str_res[],mem_str_time[];

//�绡����
static  const  float  ARC_Facter_Table[] = 
{
	0.000,
	7.143,
	6.428,
	6.3,
	6.2,
	6.0,
	5.7,
	3.1,
	2.05,
	1.2
};

void DCW_Test_Inerface(uint8_t type,void *value);

typedef struct{
	uint8_t   Test_Keep_Flag : 1;                     //�������Ա�־
	uint8_t   ARC_Grade      : 4;                     //�绡���ȼ�
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
}TEST_PARAM;






static struct step_dcw_t Dcw_Mode_Param,*p_Dcw_Mode_Param;
static TEST_PARAM DCW_Test_Param;




static void DCWModeTestEnvironmentEnter(struct step_dcw_t *dcw_test_para)
{
	Dcw_Mode_Param = *dcw_test_para;
	p_Dcw_Mode_Param = dcw_test_para;	
	
	
	/*********************************************/
	//2016.11.26 wangxin
	update_gif_protect_function();
	/*********************************************/
	
	if(Dcw_Mode_Param.waittime < 1)DCW_Test_Param.Wait_Time = 1;             //�ȴ�ʱ������
 	else DCW_Test_Param.Wait_Time = Dcw_Mode_Param.waittime;                 //ȷ��������0.1s
	
	if(Dcw_Mode_Param.ramptime==0){
		DCW_Test_Param.Voltage_Rise_Number = 1;
	}else{
		DCW_Test_Param.Voltage_Rise_Number = Dcw_Mode_Param.ramptime;
	}
	
	if(Dcw_Mode_Param.downtime==0){
		DCW_Test_Param.Voltage_Down_Number = 1;
	}else{
		DCW_Test_Param.Voltage_Down_Number = Dcw_Mode_Param.downtime;
	}
	
	DCW_Test_Param.Test_Keep_Flag        = 0;
	DCW_Test_Param.Test_Time             = Dcw_Mode_Param.testtime;
	DCW_Test_Param.Pause_Time            = Dcw_Mode_Param.pausetime;
	
	if(Dcw_Mode_Param.testtime == 0)DCW_Test_Param.Test_Keep_Flag = 1;
	
	DCW_Test_Param.Voltage_Start_Value   = Dcw_Mode_Param.startvol;
	
	DCW_Test_Param.Voltage_Process_Value = Dcw_Mode_Param.startvol;
	
	DCW_Test_Param.Voltage_Final_Value   = Dcw_Mode_Param.outvol;
	
//	DCW_Test_Param.Voltage_Out_Freq      = Dcw_Mode_Param.outfreq / 10;
	
	DCW_Test_Param.ARC_Grade             = Dcw_Mode_Param.arc;
	
	DCW_Test_Param.Voltage_Wait_Count    = 0;
	
	DCW_Test_Param.Voltage_Rise_Count    = 0;
	
	DCW_Test_Param.Testing_Count         = 0;
	
	DCW_Test_Param.Voltage_Down_Count    = 0;
	
	DCW_Test_Param.current_value         = 0;
	
	DCW_Test_Param.Pause_Count   = 0;
	
	DCW_Test_Param.Voltage_Rise_Interval = (Dcw_Mode_Param.outvol - Dcw_Mode_Param.startvol) / DCW_Test_Param.Voltage_Rise_Number;

	DCW_Test_Param.Voltage_Down_Interval = Dcw_Mode_Param.outvol / DCW_Test_Param.Voltage_Down_Number;

	switch(Dcw_Mode_Param.curgear){
		case I3uA:
			Sampling_Relay_State_CHange(DC_2uA);
		break;
		case I30uA:
			Sampling_Relay_State_CHange(DC_20uA);
		break;
		case I300uA:
			Sampling_Relay_State_CHange(DC_200uA);
			/*���Ķ�·��׼*/
			DAC_SetValue(Short_VREF,(1.05 / 6.6) * 4096);
		break;
		case I3mA:
			Sampling_Relay_State_CHange(DC_2mA);
			/*���Ķ�·��׼*/
			DAC_SetValue(Short_VREF,(1.05 / 6.6) * 4096);
		break;
		case I30mA:
			Sampling_Relay_State_CHange(DC_20mA);
			/*���Ķ�·��׼*/
			if(Dcw_Mode_Param.curhigh <= 500){
				DAC_SetValue(Short_VREF,(1.30 / 6.6) * 4096);//1.05
			}else if(Dcw_Mode_Param.curhigh <= 800){
				DAC_SetValue(Short_VREF,(1.80 / 6.6) * 4096);//1.68
			}else if(Dcw_Mode_Param.curhigh <= 1000){
				DAC_SetValue(Short_VREF,(2.50 / 6.6) * 4096);//2.10
			}else if(Dcw_Mode_Param.curhigh <= 1500){
				DAC_SetValue(Short_VREF,(3.15 / 6.6) * 4096);
			}else if(Dcw_Mode_Param.curhigh <= 2000){
				DAC_SetValue(Short_VREF,(4.20 / 6.6) * 4096);
			}else{
				DAC_SetValue(Short_VREF,(6.6 / 6.6) * 4096);
			}
		break;
		case I100mA:
			Sampling_Relay_State_CHange(DC_100mA);
			/*���Ķ�·��׼*/
			if(Dcw_Mode_Param.curhigh <= 50){
				DAC_SetValue(Short_VREF,(1.05 / 6.6) * 4096);
			}else if(Dcw_Mode_Param.curhigh <= 80){
				DAC_SetValue(Short_VREF,(1.68 / 6.6) * 4096);
			}else if(Dcw_Mode_Param.curhigh <= 100){
				DAC_SetValue(Short_VREF,(2.10 / 6.6) * 4096);
			}else if(Dcw_Mode_Param.curhigh <= 150){
				DAC_SetValue(Short_VREF,(3.15 / 6.6) * 4096);
			}else if(Dcw_Mode_Param.curhigh <= 200){
				DAC_SetValue(Short_VREF,(4.20 / 6.6) * 4096);
			}else{
				DAC_SetValue(Short_VREF,(6.6 / 6.6) * 4096);
			}
		break;
		default:
			
		break;
	}
	DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_TEST_WAIT);
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
	
	
	/*���Ķ�·��׼*/
// 	if(Dcw_Mode_Param.curhigh < 5000){
// 		DAC_SetValue(Short_VREF,(1.5 / 6.6) * 4096);
// 	}else if(Dcw_Mode_Param.curhigh < 8000){
// 		DAC_SetValue(Short_VREF,(2.4 / 6.6) * 4096);
// 	}else if(Dcw_Mode_Param.curhigh < 10000){
// 		DAC_SetValue(Short_VREF,(3.0 / 6.6) * 4096);
// 	}
	
	
	/*����ARC��׼*/
	
	if(DCW_Test_Param.ARC_Grade)
	{
		Test_Sched_Param.arc_Isable = 1;
		DAC_SetValue(ARC_VREF,560 * ARC_Facter_Table[DCW_Test_Param.ARC_Grade] * ((float)Global_Cal_Facter.ARC_Facter.DCW_ARC_Base) / 2000);
	}	
	else
	{
		Test_Sched_Param.arc_Isable = 0;
	}
	
	Relay_ON(ACW_DCW_IR);
	ctrl_relay_EXT_DRIVE_O4_O5(RELAY_OFF);///<2017.5.11 wangxin

}


void DCWModeTestEnvironmentExit(void)
{
	DC_SetVoltage(0);
	DC_Output_Disable();
	Relay_OFF(ACW_DCW_IR);
}


void DCW_Range_Check(uint8_t type)
{
	uint32_t current;
	switch(Dcw_Mode_Param.curgear){
		case I3uA:
				current = DCW_Test_Param.current_value  ;
			break;
			case I30uA:
				current = DCW_Test_Param.current_value;
			break;
			case I300uA:
				current = DCW_Test_Param.current_value;
			break;
		case I3mA:
		
			current = DCW_Test_Param.current_value / 10;
			
		
		break;
		case I30mA:
		
			current = DCW_Test_Param.current_value;

		
		break;
		case I100mA:
		
			current = DCW_Test_Param.current_value;
			
		break;
		default:
			current = DCW_Test_Param.current_value;
		break;
	}
	switch(type){
		case UPPER_LIMIT:
//			if(Dcw_Mode_Param.curhigh == 0)return;
			if(current > Dcw_Mode_Param.curhigh){
				/*��������*/
				Test_Sched_Param.Test_Status          = TEST_STATE_HIGH;
				DCW_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)UPPER_LIMIT);
				Test_Sched_Param.Pass_Flag = 0;  //������Ժϸ��־λ
				bsp_display(LED_TEST,0);
				
				DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)DCW_Test_Param.current_value);
				
				DCWModeTestEnvironmentExit();
				Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
				Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
				Test_Sched_Param.Warning_Flag         = 1;
				result_save(DCW,HIGH,(void *)&Dcw_Mode_Param,DCW_Test_Param.Voltage_value,current,0,0,DCW_Test_Param.Testing_Count);
			}
		break;
		
		case DOWN_LIMIT:
			if(Dcw_Mode_Param.curlow == 0)return;
			if(current < Dcw_Mode_Param.curlow){
				/*��������*/
				Test_Sched_Param.Test_Status          = TEST_STATE_LOW;
				DCW_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)DOWN_LIMIT);
				Test_Sched_Param.Pass_Flag = 0;  //������Ժϸ��־λ
				bsp_display(LED_TEST,0);
				DCWModeTestEnvironmentExit();
				Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
				Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
				Test_Sched_Param.Warning_Flag         = 1;
				result_save(DCW,LOW,(void *)&Dcw_Mode_Param,DCW_Test_Param.Voltage_value,current,0,0,DCW_Test_Param.Testing_Count);
			}
		break;
			
		case VOL_ERROR:    //�жϵ�ѹ�Ƿ��쳣
			if(DCW_Test_Param.Voltage_value  < (DCW_Test_Param.Voltage_Final_Value / 2)){
				/*��������*/
				Test_Sched_Param.Test_Status          = TEST_STATE_VOL_ABNORMAL;
				DCW_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)VOL_ERROR);
				DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(DCW_Test_Param.Voltage_value));		  //����ѹֵˢ�µ���Ļ
				Test_Sched_Param.Pass_Flag = 0;  //������Ժϸ��־λ
				bsp_display(LED_TEST,0);
				DCWModeTestEnvironmentExit();
				Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
				Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
				Test_Sched_Param.Warning_Flag         = 1;
				result_save(DCW,VOL_ABNORMAL,(void *)&Dcw_Mode_Param,DCW_Test_Param.Voltage_value,current,0,0,DCW_Test_Param.Testing_Count);
			}
			
		break;
		
		default:
			
		break;
	}
}


void DCW_Mode_Test(struct step_dcw_t *dcw_test_para)
{
	if(Test_Sched_Param.Short_Flag)
	{
		Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
		Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
		Test_Sched_Param.Warning_Flag         = 1;
		DCW_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)SHORT_WARNING);
		switch(Dcw_Mode_Param.curgear){
				case I3mA:
					result_save(DCW,SHORT,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value / 10,0,0,DCW_Test_Param.Testing_Count);	
				break;
				case I30mA:
				case I100mA:
				default:
					result_save(DCW,SHORT,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value,0,0,DCW_Test_Param.Testing_Count);

				break;
		}
		return;
	}
	
	if(Test_Sched_Param.arc_Flag)
	{
		Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
		Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
		Test_Sched_Param.Warning_Flag         = 1;
		DCW_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)ARC_WARNING);
		switch(Dcw_Mode_Param.curgear){
				case I3mA:
					result_save(DCW,ARC,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value / 10,0,0,DCW_Test_Param.Testing_Count);	
				break;
				case I30mA:
				case I100mA:
				default:
					result_save(DCW,ARC,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value,0,0,DCW_Test_Param.Testing_Count);

				break;
		}
		return;
	}
	
	if(Test_Sched_Param.gfi_Flag)
	{
		Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
		Test_Sched_Param.Stop_Flag            = 1;                             //��λSTOP��־λ
		Test_Sched_Param.Warning_Flag         = 1;
		DCW_Test_Inerface(TEST_WARNING_REFRESH_EVENT,(void *)GFI_WARNING);
		switch(Dcw_Mode_Param.curgear){
				case I3mA:
					result_save(DCW,GFI,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value / 10,0,0,DCW_Test_Param.Testing_Count);	
				break;
				case I30mA:
				case I100mA:
				default:
					result_save(DCW,GFI,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value,0,0,DCW_Test_Param.Testing_Count);

				break;
		}
		return;
	}
	
	if(Test_Sched_Param.Stop_Flag){                            //Stop�������£�����
		DC_Output_Disable();
		Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
		DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_STOP);
	}
	if(Test_Sched_Param.Test_Step_State == TEST_STEP_OUT){     //���ڲ���״̬
		DCWModeTestEnvironmentEnter(dcw_test_para);              //��ʼ��
		Test_Sched_Param.Test_Step_State = TEST_STEP_TEST_WAIT;  //�������״̬
		PLC_Testing_Out(1);
	}
	switch(Test_Sched_Param.Test_Step_State){
				
		//�ȴ�����״̬
		case TEST_STEP_TEST_WAIT:
			if(--DCW_Test_Param.Wait_Time == 0){
				if(DCW_Test_Param.Voltage_Rise_Number == 1){           //�������ʱ��Ϊ0
					Test_Sched_Param.Test_Step_State = TEST_STEP_TESTING;
					DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_TESTING);
					DCW_Test_Param.Voltage_Process_Value = DCW_Test_Param.Voltage_Final_Value;
					DC_SetVoltage(DCW_Test_Param.Voltage_Process_Value);
				}else{
					Test_Sched_Param.Test_Step_State = TEST_STEP_VOL_RISE;
					DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_VOL_RISE);
					DC_SetVoltage(DCW_Test_Param.Voltage_Start_Value);
				}
				
			}
		
			DCW_Test_Param.Voltage_Wait_Count++;
			DCW_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)DCW_Test_Param.Voltage_Wait_Count);
//			DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)DC_GetVoltage());
		break;
			
		
		//��ѹ����״̬
		case TEST_STEP_VOL_RISE:
			
			DCW_Test_Param.Voltage_value=DC_GetVoltage();
			DCW_Test_Param.current_value=DC_GetCurrent();
			
			if(--DCW_Test_Param.Voltage_Rise_Number == 0){   //��ѹ��������Ŀ���ѹ
				Test_Sched_Param.Test_Step_State = TEST_STEP_TESTING;					
				DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_TESTING);
				DC_SetVoltage(DCW_Test_Param.Voltage_Final_Value);					
			}else{
				DCW_Test_Param.Voltage_Process_Value += DCW_Test_Param.Voltage_Rise_Interval;
				DC_SetVoltage(DCW_Test_Param.Voltage_Process_Value);	
			}
			
			if(Test_Sched_Param.Offset_Is_Flag){
				DCW_Test_Param.current_value = DCW_Test_Param.current_value>(*p_Dcw_Mode_Param).offsetvalue? (DCW_Test_Param.current_value - (*p_Dcw_Mode_Param).offsetvalue) : 0;
			}
			
			/*�ж�����*/
			
			
			DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(DCW_Test_Param.Voltage_value));
			DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(DCW_Test_Param.current_value));
			DCW_Test_Param.Voltage_Rise_Count++;             //����ʱ���1
			
			DCW_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)DCW_Test_Param.Voltage_Rise_Count);
			DCW_Range_Check(UPPER_LIMIT);
		
		break;
		//����״̬ 
		case TEST_STEP_TESTING:
	
			DCW_Test_Param.Voltage_value=DC_GetVoltage();
			DCW_Test_Param.current_value=DC_GetCurrent();
		
			if(DCW_Test_Param.Test_Keep_Flag){              //����ʱ��Ϊ0��һֱ����
				/*�ж�����*/
				if(DCW_Test_Param.Testing_Count >= 10000)DCW_Test_Param.Testing_Count = 0;
			}else{
				if(--DCW_Test_Param.Test_Time == 0){
					if(DCW_Test_Param.Voltage_Down_Number == 1){           //����½�ʱ��Ϊ0
						if(DCW_Test_Param.Pause_Time > 0 && current_step_num-1 <= file_info[flash_info.current_file].totalstep){                   //��������Ϊ0
							Test_Sched_Param.Test_Step_State = TEST_STEP_PAUSE;
							DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_PAUSE);
						}else{                                               //������Ϊ0�����β��Խ���
							Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
						}
						DCW_Range_Check(VOL_ERROR);
						DC_SetVoltage(0);
						DC_Output_Disable();

					}else{
						Test_Sched_Param.Test_Step_State = TEST_STEP_VOL_DOWN;		
						DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_VOL_DOWN);		
					}
				}
			}
			
			DCW_Test_Param.Testing_Count++;             //����ʱ���1			
			DCW_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)DCW_Test_Param.Testing_Count);
			
			if(Test_Sched_Param.Test_Step_State != TEST_STEP_TESTING)
			{
				break;
			}
			
			if(Test_Sched_Param.Offset_Is_Flag){
				DCW_Test_Param.current_value = DCW_Test_Param.current_value>(*p_Dcw_Mode_Param).offsetvalue? (DCW_Test_Param.current_value - (*p_Dcw_Mode_Param).offsetvalue) : 0;
			}
			
			
			DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(DCW_Test_Param.Voltage_value));	
			DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(DCW_Test_Param.current_value));
			DCW_Range_Check(UPPER_LIMIT);
			if(DCW_Test_Param.Testing_Count > 4)DCW_Range_Check(DOWN_LIMIT);
			
			if(Test_Sched_Param.Offset_Get_Flag){
				(*p_Dcw_Mode_Param).offsetvalue = DCW_Test_Param.current_value;
				
			}
			
		break;
		//��ѹ�½�״̬
		case TEST_STEP_VOL_DOWN:
			
			DCW_Test_Param.Voltage_value=DC_GetVoltage();
			DCW_Test_Param.current_value=DC_GetCurrent();
		
			if(--DCW_Test_Param.Voltage_Down_Number == 0){   //��ѹ��������Ŀ���ѹ
				if(DCW_Test_Param.Pause_Time > 0 && current_step_num-1 <= file_info[flash_info.current_file].totalstep){                   //��������Ϊ0
					Test_Sched_Param.Test_Step_State = TEST_STEP_PAUSE;
					DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_PAUSE);
				}else{                                               //������Ϊ0�����β��Խ���
					Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
				}		
//				DCW_Range_Check(VOL_ERROR);
				DC_SetVoltage(0);	
				DC_Output_Disable();
			}else{
				DCW_Test_Param.Voltage_Process_Value -= DCW_Test_Param.Voltage_Down_Interval;
				DC_SetVoltage(DCW_Test_Param.Voltage_Process_Value);
							
			}
			
			if(Test_Sched_Param.Offset_Is_Flag){
				DCW_Test_Param.current_value = DCW_Test_Param.current_value>(*p_Dcw_Mode_Param).offsetvalue? (DCW_Test_Param.current_value - (*p_Dcw_Mode_Param).offsetvalue) : 0;
			}
			
			DCW_Test_Param.Voltage_Down_Count++;             //����ʱ���1
			DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(DCW_Test_Param.Voltage_value));	
			DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(DCW_Test_Param.current_value));
			DCW_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)DCW_Test_Param.Voltage_Down_Count);
			DCW_Range_Check(UPPER_LIMIT);
			if(Test_Sched_Param.Test_Step_State != TEST_STEP_VOL_DOWN){
				DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(0));	
				DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(0));
			}
		break;
		//����ȴ�
		case TEST_STEP_PAUSE:
			if(Dcw_Mode_Param.steppass)
			{
				PLC_Pass_Out(1);
				PLC_Testing_Out(0);
			}
			
			DCW_Range_Check(UPPER_LIMIT);
			DCW_Test_Param.Pause_Count++;
			DCW_Test_Inerface(TEST_TIME_COUNT_REFRESH_EVENT,(void *)DCW_Test_Param.Pause_Count);
// 			DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)(DCW_Test_Param.Voltage_value=DC_GetVoltage()));
// 			DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)(DCW_Test_Param.current_value=DC_GetCurrent()));
			DCW_Range_Check(UPPER_LIMIT);
			if(--DCW_Test_Param.Pause_Time == 0){
				Test_Sched_Param.Test_Step_State = TEST_STEP_STOP;
				PLC_Pass_Out(0);
			}
		break;
		//����ֹͣ
		case TEST_STEP_STOP:	
			
//			DCW_Test_Inerface(TEST_STATE_REFRESH_EVENT,(void *)TEST_STEP_STOP);
// 			DCW_Test_Inerface(TEST_VOLTAGE_REFRESH_EVENT,(void *)0);	
// 			DCW_Test_Inerface(TEST_CURRENT_REFRESH_EVENT,(void *)0);	
			
			DCWModeTestEnvironmentExit();
			Test_Sched_Param.Test_Step_State      = TEST_STEP_OUT;                  //���ڲ���״̬
			bsp_display(LED_TEST,0);
			if(DCW_Test_Param.Test_Time == 0 && dcw_test_para->testtime!=0)
			{
				dis_test_pass();
			}
			switch(Dcw_Mode_Param.curgear){
				case I3mA:
					result_save(DCW,PASS,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value / 10,0,0,DCW_Test_Param.Testing_Count);	
				break;
				case I30mA:
				case I100mA:
				default:
					result_save(DCW,PASS,(void *)dcw_test_para,DCW_Test_Param.Voltage_value,DCW_Test_Param.current_value,0,0,DCW_Test_Param.Testing_Count);

				break;
			}
			Test_Sched_Main((void *)0);
		break;
		//����״̬
		default:
			//����Ӧ�����˴�
		break;
		
		
		
	}
	
}



extern void ui_teststr_darw(struct font_info_t *font,struct rect_type *rect,char *str);
void DCW_Test_Inerface(uint8_t type,void *value)
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
					strcpy(buf,T_STR("���ڲ���","Testing"));
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
		switch(Dcw_Mode_Param.curgear){
			case I3uA:
				{
					u32 current = (u32)value ;
					struct rect_type rect={242,208,38,398};
					
					rt_sprintf(buf,"%d.%03duA", current/1000,current%1000);
					ui_teststr_darw(&font,&rect,buf);
					strncpy(mem_str_cur,buf,strlen(buf));
				}
			break;
			case I30uA:
				{
					u32 current = (u32)value;
					struct rect_type rect={242,208,38,398};
					
					rt_sprintf(buf,"%d.%02duA", current/100,current%100);
					ui_teststr_darw(&font,&rect,buf);
					strncpy(mem_str_cur,buf,strlen(buf));
				}
			break;
			case I300uA:
				{
					u32 current = (u32)value;
					struct rect_type rect={242,208,38,398};
					
					rt_sprintf(buf,"%d.%01duA", current/10,current%10);
					ui_teststr_darw(&font,&rect,buf);
					strncpy(mem_str_cur,buf,strlen(buf));
				}
			break;
			case I3mA:
				{
					u32 current = (u32)value / 10;
					struct rect_type rect={242,208,38,398};
					
					rt_sprintf(buf,"%d.%03dmA", current/1000,current%1000);
					ui_teststr_darw(&font,&rect,buf);
					strncpy(mem_str_cur,buf,strlen(buf));
				}
			break;
			case I30mA:
				{
					u32 current = (u32)value;
					struct rect_type rect={242,208,38,398};
					
					rt_sprintf(buf,"%d.%02dmA", current/100,current%100);
					ui_teststr_darw(&font,&rect,buf);
					strncpy(mem_str_cur,buf,strlen(buf));
				}
			break;
			case I100mA:
				{
					u32 current = (u32)value;
					struct rect_type rect={242,208,38,398};
					
					rt_sprintf(buf,"%d.%01dmA", current/10,current%10);
					ui_teststr_darw(&font,&rect,buf);
					strncpy(mem_str_cur,buf,strlen(buf));
				}
			break;
			default:
				
			break;
		}
		break;
		
		case TEST_RESISTER_REFRESH_EVENT:       //���Ե����¼�

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


