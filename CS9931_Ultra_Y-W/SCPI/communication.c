/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/

#include "stm32f4xx.h"
#include "Test_Sched.h"
#include "string.h"
#include "CS99xx.h"
#include "Communication.h"
#include "scpi_parser.h"
#include "macro.h"
#include "set_panel.h"
#include "Api_sched.h"
#include "App_config.h"
#include "memorymanagement.h"
#include "library.h"
/******************************************************************************
 *                            本文件内部宏定义
******************************************************************************/

// 广播地址定义
#define                         BROADCAST_ADDR                         (0)

extern struct system_parameter system_parameter_t;
extern void IntToStr(char *pc, u8 Int_num, u8 Dec_num, u32 value);
extern void stritem(char *str);
extern void Util_MultifunctionalDataSplit(uint32_t rscData, uint8_t length, int8_t *pdestAddr, uint8_t dotPos);
extern void refresh_com(void);
void uart2_send_data_en(void);
void uart2_receive_data_en(void);

typedef struct _INSULATION_INFO_
{
	volatile uint32_t	        Insulation_Voltage;
	volatile int32_t			Resistor_upper;
	volatile int32_t			Resistor_lower;
	volatile uint32_t         Resistor_type;
	volatile int32_t			Testtime;
	volatile uint32_t			Testtime_flag;
	volatile int32_t			Delay_time;
	volatile uint32_t			Delay_time_flag;
	volatile uint32_t			Auto_Manual;

	volatile uint32_t			Depart_Mode_mark;
	
	volatile uint32_t         ChangeWeal_Up_Value;//向上换挡的值
	volatile uint32_t         ChangeWeal_Down_Value;//向下换挡的值	   	
}InsulationInfo;
volatile InsulationInfo st_InsulationInfo;
InstrumentAttrConfig t_InstrumentAttrConfig = {
    
    {"CS9931YS"},
    "xxxxxxxxxx",

    "1.0.08"
}; 
typedef struct _ENV_PARAM_
{
    uint8_t                    m_sysModel;      // 机型

    uint8_t                    m_keySound;      // 按键声音
    //uint8                    m_keyLock;       // 按键锁

}EnvParam;
volatile EnvParam t_EnvParam;

char mem_str_vol[20]="",mem_str_cur[20]="",mem_str_realcur[20]="---mA",mem_str_res[20]="",mem_str_time[20]="";

typedef struct _PARAM_INFO_
{
    volatile uint32_t	        Voltage_AC;
	volatile uint32_t	        Voltage_DC;

	volatile int32_t	        Current_AC;
	volatile int32_t	        Current_DC;
	volatile uint32_t			Current_AC_flag;
	volatile uint32_t			Current_DC_flag;
	volatile uint32_t			Current_AC_LOW_flag;
	volatile uint32_t			Current_DC_LOW_flag;

	volatile int32_t	        Current_AC_LOW;
	volatile int32_t	        Current_DC_LOW;

	volatile int32_t	        Time_AC;
	volatile int32_t	        Time_DC;
	volatile uint32_t	        Set_time_flag;
	volatile uint32_t			Set_time_dc_flag;

	volatile uint32_t			Arc_DC;
	volatile uint32_t			Arc_DC_Data;

	volatile uint32_t			Arc_AC;
	volatile uint32_t			Arc_AC_Data;
		
	volatile uint32_t	        Test_current_mode;
	volatile uint32_t	        Test_Mode;

	volatile uint32_t			AC_Test_display;
	volatile uint32_t         DC_Test_display;
	
}ParamInfo;
volatile ParamInfo st_ParamInfo;
typedef struct TEST_FENTCH_INFO_
{
	uint32_t 		Test_Diaplay_VOL;
	uint32_t 		Test_Dispaly_CUR;
	uint32_t 		Test_Dispaly_IR;
	uint32_t        Test_Status;
}FentchInfo;
volatile FentchInfo t_FentchInfo;

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/
// 测试状态标志
static TestStatus    s_TestStatus                               = TEST_STATE_WAITING; 

// 控制状态
typedef enum
{
    STATUS_LOCAL    = '0',
    STATUS_REMOTE   = '1',  
    STATUS_INACTIVE = '2',                                          

}CtrlStatus;

// 通讯控制状态
static CtrlStatus   s_CommCtrlStatus                            = STATUS_INACTIVE;
// 广播通讯状态
static uint8_t        s_CommBroadcastStatus                       = FALSE;
// 本机地址
static uint8_t        s_CommLocalAddr                             = 0;


/******************************************************************************
 *                           本文件静态函数声明
******************************************************************************/ 

static uint32_t _APP_CommPrevValidityChk(struct EXECUTE_VALIDITY_CHK_INFO *pexecuteValidityChkInfo);

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 通讯协议逻辑控制函数                                                             
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                               
 *                                                                            
 *                                                                            
******************************************************************************/ 

void APP_CommProtocolLogicInit(uint32_t localAddr)
{
    s_CommCtrlStatus                                            = STATUS_INACTIVE;
    s_CommBroadcastStatus                                       = FALSE;
    s_CommLocalAddr                                             = localAddr;
    //设置预执行合法性检测函数指针
    API_PEMPrevValidityChkFuncInstall(_APP_CommPrevValidityChk);
}

static uint32_t _APP_CommPrevValidityChk(struct EXECUTE_VALIDITY_CHK_INFO *pexecuteValidityChkInfo)
{
    FP_pfInstructionExec    *pfinstructionExec                  = pexecuteValidityChkInfo->m_pfinstructionExec;
    uint8_t                    instructionAttr                    = pexecuteValidityChkInfo->m_instructionAttr;
    
    //处于预执行合法性判断阶段
    if (VALIDITY_CHK_STEP_RREV_EXECUTE == (pexecuteValidityChkInfo->m_step))
    {
        //未处于活动状态 
        if (STATUS_INACTIVE == s_CommCtrlStatus)
        {
            //只有COMM:SADD指令能够响应 且 指令属性为执行时
            if ((InstructionExec_CommSAddr != (FP_pfInstructionExec *)pfinstructionExec)
                || (instructionAttr & INSTRUCTION_ATTR_QUERY))
            {
                //其他指令不会执行
                return FALSE;
            }
        }
        //处于本控状态时  只响应COMM:REM COMM:LOC COMM:CONT
        else if (STATUS_REMOTE != s_CommCtrlStatus)
        {
            if ((InstructionExec_CommRemote != (FP_pfInstructionExec *)pfinstructionExec)
                && (InstructionExec_CommLocal != (FP_pfInstructionExec *)pfinstructionExec)
                && (InstructionExec_CommControl != (FP_pfInstructionExec *)pfinstructionExec)
                && (InstructionExec_CommSAddr != (FP_pfInstructionExec *)pfinstructionExec))
            {
                //其他指令不会执行
                API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
                return FALSE;
            }
        }
        //处于远控状态时 响应除通讯地址设置指令外所有指令
        else
        {
            if ((InstructionExec_CommSAddr == (FP_pfInstructionExec *)pfinstructionExec)
                && (instructionAttr & INSTRUCTION_ATTR_EXECUT))
            {
                API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
                return FALSE;
            }

            // 测试状态下 只响应以下指令
            if (TEST_STATE_TESTING == s_TestStatus)
            {
                if ((InstructionExec_SrcTestStop   != (FP_pfInstructionExec *)pfinstructionExec)
                  &&(InstructionExec_SrcTestFetch  != (FP_pfInstructionExec *)pfinstructionExec)
                  &&(InstructionExec_SrcTestStatus != (FP_pfInstructionExec *)pfinstructionExec))
                {
                    API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
                    return FALSE;
                }
            }
            //else if (InstructionExec_SrcTestFetch == (FP_pfInstructionExec *)pfinstructionExec)
            //{
            //    return FALSE;
            //}
        }
    }
    //处于预输出合法性判断阶段
    else
    {
        if ((FALSE == s_CommBroadcastStatus) && (STATUS_INACTIVE != s_CommCtrlStatus))
        {
        }
        else
        {
            return FALSE;
        }
    }
    //指令可以执行
    return TRUE;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 公用指令集执行函数                                                              
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                               
 *                                                                            
 *                                                                            
******************************************************************************/ 
extern void Test_Sched_Close(void);
uint32_t InstructionExec_RST(int argc, const uint8_t *argv[])
{
    Test_Sched_Close();
		Test_Sched_Close();
		Test_Sched_Close();
    return TRUE;
}


uint32_t InstructionExec_IDN(int argc, const uint8_t *argv[])
{
	char buf[10];
    
    //格式为Allwin Technologies,仪器型号,仪器串号,软件版本号 
    //压入第一段字符    生产商
    API_PEMOutputQueueStrPush("Allwin Technologies", ',');
    //压入第二段字符    仪器型号
 		strncpy(buf,(const char *)t_InstrumentAttrConfig.m_InstrumentModel[0],8);buf[8] = 0;
    API_PEMOutputQueueStrPush((const uint8_t *)buf, ',');
    //压入第三段字符    仪器串号
    API_PEMOutputQueueStrPush(t_InstrumentAttrConfig.m_InstrumentID, ',');
    //压入第四段字符    软件版本号
    API_PEMOutputQueueStrPush(t_InstrumentAttrConfig.m_InstrumentSoftwareVersion, NO_APPENDED_SIGN);
    
    return TRUE;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : 通讯指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/ 

uint32_t InstructionExec_CommSAddr(int argc, const uint8_t *argv[])
{
		uint8_t		n_bit;
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        uint32_t   recvCommAddr                           = API_PEMItegerParamReadUint32(argv[2]);
        //判断第三项参数
        //判断是否为广播地址
        if (BROADCAST_ADDR == recvCommAddr)
        {
            //置位处于活动状态
            s_CommCtrlStatus                                    = STATUS_LOCAL;
            s_CommBroadcastStatus                               = TRUE;
        }
        //判断是否为本机通讯地址
        else if (s_CommLocalAddr == recvCommAddr)
        {
            //置位处于活动状态
            s_CommCtrlStatus                                    = STATUS_LOCAL;
            s_CommBroadcastStatus                               = FALSE;
        }
        else
        {
            //置位处于非活动状态
            s_CommCtrlStatus                                    = STATUS_INACTIVE;
        }
    }
    //查询指令 只有处于活动状态时 才响应此指令
    else if (STATUS_INACTIVE != s_CommCtrlStatus)
    {
					if(s_CommLocalAddr>=100)
					{
						n_bit=3;
					}
					else if(s_CommLocalAddr>=10)
					{
						n_bit=2;
					}
					else
					{
						n_bit=1;
					}
        //返回本机地址
        API_PEMOutputQueueIntegerPush(s_CommLocalAddr, n_bit, NO_APPENDED_SIGN);
    }

    return TRUE;
}

extern void LIB_LockKeyFuction(void);
uint32_t InstructionExec_CommRemote(int argc, const uint8_t *argv[])
{
    s_CommCtrlStatus                                            = STATUS_REMOTE;
    LIB_LockKeyFuction();          //屏蔽按键的功能
    return TRUE;
}

extern void LIB_OpenKeyFuction(void);
uint32_t InstructionExec_CommLocal(int argc, const uint8_t *argv[])
{
    s_CommCtrlStatus                                            = STATUS_LOCAL;
		LIB_OpenKeyFuction();          //屏蔽按键的功能
    return TRUE;
}

uint32_t InstructionExec_CommControl(int argc, const uint8_t *argv[])
{
    uint8_t   ctrlStatus[]                                        = "0";
    
    ctrlStatus[0]                                                 = s_CommCtrlStatus;
    //只支持查询指令
    API_PEMOutputQueueStrnPush(ctrlStatus, 1, NO_APPENDED_SIGN);
    return TRUE;
}

uint32_t InstructionExec_FileRead(int argc, const uint8 *argv[])
{
 	uint8_t mem;
 	mem = atoi((void *)argv[2]);
	if(file_info[mem].en != 0)
	{
		flash_info.current_file = mem;
		init_list();
		read_flash_to_list(flash_info.current_file);
		FLASH_CS_SET(1);	// 选择参数flash
		sf_WriteBuffer((uint8_t *)&flash_info,FLASH_BKP_ADDR,sizeof(flash_info));
	}
	return TRUE;
}

uint32_t InstructionExec_FileCatalogSingle(int argc, const uint8 *argv[])
{
	uint8_t mem;
	char temp[50];
	mem = atoi((void *)argv[2]);
	if(file_info[mem].en != 0)
	{
		sprintf(temp,"%d,\"%s\",%d,%s,%d,%d,%d",mem,file_info[mem].name,file_info[mem].totalstep,    \
						file_info[mem].mode == N_WORK? "N":"G",                                              \
						file_info[mem].passtime,file_info[mem].buzzertime,file_info[mem].arc);
		API_PEMOutputQueueStrPush((const uint8_t *)temp, NO_APPENDED_SIGN);
		return TRUE;
	}else{
		API_PEMOutputQueueStrPush("0", NO_APPENDED_SIGN);
		return FALSE;
	}
  
}
/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : 源指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

extern void Test_Sched_Start(CALLBACK_GET_TESTPARAM   *p_Get_Test_Param);
extern UN_STR *dyj_next_step(void);
uint32_t InstructionExec_SrcTestStart(int argc, const uint8_t *argv[])
{
//		if(Test_Sched_Param.Test_Sched_State == TEST_SCHED_STATE_RUNNING && 
//		   Test_Sched_Param.Pause_Flag       == 0)
//			return  FALSE; 
//		if(current_step_num != 1)g_cur_step = g_cur_step->prev;   //补偿
		Test_Sched_Start(dyj_next_step);
    return TRUE;
}



uint32_t InstructionExec_SrcTestStop(int argc, const uint8_t *argv[])
{


	Test_Sched_Close();
	return TRUE;
}
 
uint32_t InstructionExec_SrcTestStatus(int argc, const uint8_t *argv[])
{
    uint8_t    paramSrting[15]                    = {0};
    paramSrting[0]                                = Test_Sched_Param.Test_Status / 10 + 0x30;
    paramSrting[1]                                = Test_Sched_Param.Test_Status % 10 + 0x30;
    API_PEMOutputQueueStrnPush(paramSrting, strlen((void *)paramSrting), NO_APPENDED_SIGN);
    return TRUE;
}

uint32_t InstructionExec_SrcTestFetch(int argc, const uint8_t *argv[])
{
	char buf[100],temp[10];
	static UN_STR *un;
	
	if(g_cur_step != NULL)
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	
	// 测试步骤编号
	IntToStr(buf,2,0,current_step_num==1? current_step_num:current_step_num-1);
	
	switch(un->com.mode)
	{
		case ACW://ACW
			// ACW - 0，DCW - 1，IR - 2，GR - 3
			strcat(buf,",0,");
			// 测试电压
			if(strlen(mem_str_vol)==0)strcpy(mem_str_vol,"--kV");
			strcat(buf,mem_str_vol);
			// 电流档位
			sprintf(temp,",%d,",un->acw.curgear+1);
			strcat(buf,temp);
			// 测试电流
			if(strlen(mem_str_cur)==0)strcpy(mem_str_cur,"--mA");
			strcat(buf,mem_str_cur);
			// 真实电流开关状态
			if(un->acw.rmscur)
				strcat(buf,",1,");
			else
				strcat(buf,",0,");
			// 真实电流
			strcat(buf,mem_str_realcur);
			strcat(buf,",");
			// 测试时间
			if(strlen(mem_str_time)==0)strcpy(mem_str_time,"--s");
			strcat(buf,mem_str_time);
			// 测试状态
			strcat(buf,",");
			IntToStr(buf+strlen(buf),2,0,Test_Sched_Param.Test_Status);
			break;
		case DCW://DCW

			// ACW - 0，DCW - 1，IR - 2，GR - 3
			strcat(buf,",1,");
			// 测试电压
			if(strlen(mem_str_vol)==0)strcpy(mem_str_vol,"--kV");
			strcat(buf,mem_str_vol);
			// 电流档位
			sprintf(temp,",%d,",un->acw.curgear+1);
			strcat(buf,temp);
			// 测试电流
			if(strlen(mem_str_cur)==0)strcpy(mem_str_cur,"--mA");
			strcat(buf,mem_str_cur);
			// 测试时间
			strcat(buf,",");
			if(strlen(mem_str_time)==0)strcpy(mem_str_time,"--s");
			strcat(buf,mem_str_time);
			// 测试状态
			strcat(buf,",");
			IntToStr(buf+strlen(buf),2,0,Test_Sched_Param.Test_Status);
			break;
		case IR://IR

			// ACW - 0，DCW - 1，IR - 2，GR - 3
			strcat(buf,",2,");
			// 测试电压
			if(strlen(mem_str_vol)==0)strcpy(mem_str_vol,"--kV");
			strcat(buf,mem_str_vol);
			// 电流档位
			strcat(buf,",1,");
			// 测试电阻
			if(strlen(mem_str_res)==0){strcpy(mem_str_res,"--G");strcat(buf,mem_str_res);}
			else
			{
				u8 i;
				strcat(buf,mem_str_res);
				for(i=0;i<strlen(mem_str_res);i++)
					if(mem_str_res[i] == 0xf4){buf[strlen(buf)-strlen(mem_str_res)+i]=0;break;}
			}
			
			// 测试时间
			strcat(buf,",");
			if(strlen(mem_str_time)==0)strcpy(mem_str_time,"--s");
			strcat(buf,mem_str_time);
			// 测试状态
			strcat(buf,",");
			IntToStr(buf+strlen(buf),2,0,Test_Sched_Param.Test_Status);
			break;
		case GR://GR

			// ACW - 0，DCW - 1，IR - 2，GR - 3
			strcat(buf,",3,");
			// 测试电流
// 			p = strchr(mem_str_cur,' ');
// 			*p = '\0';
			if(strlen(mem_str_cur)==0)strcpy(mem_str_cur,"--A");
			strcat(buf,mem_str_cur);
			strcat(buf,",");
			// 测试电阻
			if(strlen(mem_str_res)==0){strcpy(mem_str_res,"--m");strcat(buf,mem_str_res);}
			else
			{
				u8 i;
				strcat(buf,mem_str_res);
				for(i=0;i<strlen(mem_str_res);i++)
					if(mem_str_res[i] == 0xf4){buf[strlen(buf)-strlen(mem_str_res)+i]=0;break;}
			}
			strcat(buf,",");
			// 测试时间
			if(strlen(mem_str_time)==0)strcpy(mem_str_time,"--s");
			strcat(buf,mem_str_time);
			// 测试状态
			strcat(buf,",");
			IntToStr(buf+strlen(buf),2,0,Test_Sched_Param.Test_Status);
			break;
			
		case LC://LC
			// ACW - 0，DCW - 1，IR - 2，GR - 3
			strcat(buf,",4,");
			// 测试电压
			if(strlen(mem_str_vol)==0)strcpy(mem_str_vol,"--V");
			strcat(buf,mem_str_vol);
			// 电流档位
			//none
			strcat(buf,",");
			// 测试电流
			if(strlen(mem_str_cur)==0)strcpy(mem_str_cur,"--mA");
			strcat(buf,mem_str_cur);
			// 测试相位
			if(un->lc.NorLphase == 0)  //L项
			{	
				strcat(buf,",L-->G");
			}
			else{                     //N项
				strcat(buf,",N-->G");
			}
			// 测试时间
			strcat(buf,",");
			if(strlen(mem_str_time)==0)strcpy(mem_str_time,"--s");
			strcat(buf,mem_str_time);
			// 测试状态
			strcat(buf,",");
			IntToStr(buf+strlen(buf),2,0,Test_Sched_Param.Test_Status);
			break;
	}
	stritem(buf);
 	API_PEMOutputQueueStrnPush((const uint8_t *)buf, strlen((void *)buf), NO_APPENDED_SIGN);
	

	
	
	


  return TRUE;
}

uint32_t InstructionExec_SourceListFIndex(int argc, const uint8_t *argv[])
{
		char temp[10];
		sprintf(temp,"%d",flash_info.current_file);
		API_PEMOutputQueueStrPush((const uint8_t *)temp, NO_APPENDED_SIGN);
    return TRUE;
}
uint32_t InstructionExec_SourceListMode(int argc, const uint8_t *argv[])
{
	UN_STR *un;
	char  buf[10];
	uint8_t step_num;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	
	switch(un->com.mode)
	{
		case ACW:
			step_num = 0;
			break;
		case DCW:
			step_num = 1;;
			break;
		case IR:
			step_num = 2;
			break;
		case GR:
			step_num = 3;
			break;
		case LC:
			step_num = 4;
			break;
	}
	sprintf(buf,"%d",step_num);
	API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
	return TRUE;
}

uint32_t InstructionExec_StepMode(int argc, const uint8_t *argv[])
{
	uint8_t mode;
	mode = atoi((void *)argv[2]);
	switch(mode){
		case 0:         //ACW
			init_acw_step(&rt_list_entry(g_cur_step, STEP_NODE, list)->un);
		break;
		case 1:         //DCW
			init_dcw_step(&rt_list_entry(g_cur_step, STEP_NODE, list)->un);
		break;
		case 2:         //IR
			
		break;
		case 3:         //GR
			init_gr_step(&rt_list_entry(g_cur_step, STEP_NODE, list)->un);
		break;
		case 4:         //LC
			init_lc_step(&rt_list_entry(g_cur_step, STEP_NODE, list)->un);
		break;
		default:
			
		break;
	}
	save_steps_to_flash(flash_info.current_file);
	refresh_com();
	return TRUE;
}

uint32_t InstructionExec_SrcTestMode(int argc, const uint8_t *argv[])
{

    return TRUE;
}


/******************************************************************************
 *  函数名称 :ACW的step指令                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :2012.5.24                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : ACW_STEP指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

uint32_t InstructionExec_StepAcwVolt(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        voltage;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != ACW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		voltage = atoi((void *)argv[2]);
		
		un->acw.outvol = voltage;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%5.3f",(float)un->acw.outvol/1000);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
      
}


uint32_t InstructionExec_StepAcwRange(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        cur_gear;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != ACW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		cur_gear = atoi((void *)argv[2]);
		switch(cur_gear){
			
			case 0:
				un->acw.curgear = I300uA;
			break;
			case 1:
				un->acw.curgear = I3mA;
			break;
			case 2:
				un->acw.curgear = I30mA;
			break;
			case 3:
				un->acw.curgear = I100mA;
			break;
			default:
				
			break;
		}
		
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%d",un->acw.curgear - 2);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
      
}






uint32_t InstructionExec_StepAcwCurHigh(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        CurHigh;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != ACW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		CurHigh = atoi((void *)argv[2]);
		switch(un->acw.curgear)
		{
			case I300uA:
				CurHigh = CurHigh / 10;
				break;
			case I3mA:
				CurHigh = CurHigh * 10;
				break;
			case I30mA:
				CurHigh = CurHigh * 1;
				break;
			case I100mA:
				CurHigh = CurHigh / 1;
				break;
			default:
				
				break;
		}
		if(CurHigh <= un->acw.curlow){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->acw.curhigh = CurHigh;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		switch(un->acw.curgear)
		{
			case I300uA:
				rt_sprintf(buf,"%d.%d", un->acw.curhigh/10,un->acw.curhigh%10);
				break;
			case I3mA:
				rt_sprintf(buf,"%d.%03d", un->acw.curhigh/1000,un->acw.curhigh%1000);
				break;
			case I30mA:
				rt_sprintf(buf,"%d.%02d", un->acw.curhigh/100,un->acw.curhigh%100);
				break;
			case I100mA:
				rt_sprintf(buf,"%d.%d", un->acw.curhigh/10,un->acw.curhigh%10);
				break;
			default:
				
				break;
		}
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
}


uint32_t InstructionExec_StepAcwCurLow(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        CurLow;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != ACW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		CurLow = atoi((void *)argv[2]);
		switch(un->acw.curgear)
		{
			case I300uA:
				CurLow = CurLow / 10;
				break;
			case I3mA:
				CurLow = CurLow * 10;
				break;
			case I30mA:
				CurLow = CurLow / 1;
				break;
			case I100mA:
				CurLow = CurLow / 1;
				break;
			default:
				
				break;
		}
		if(CurLow >= un->acw.curhigh){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->acw.curlow = CurLow;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		switch(un->acw.curgear)
		{
			case I300uA:
				rt_sprintf(buf,"%d.%d", un->acw.curlow/10,un->acw.curlow%10);
				break;
			case I3mA:
				rt_sprintf(buf,"%d.%03d", un->acw.curlow/1000,un->acw.curlow%1000);
				break;
			case I30mA:
				rt_sprintf(buf,"%d.%02d", un->acw.curlow/100,un->acw.curlow%100);
				break;
			case I100mA:
				rt_sprintf(buf,"%d.%d", un->acw.curlow/10,un->acw.curlow%10);
				break;
			default:
				
				break;
		}
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
}


uint32_t InstructionExec_StepAcwTtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
    uint16_t       test_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != ACW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      test_time                             = atoi((void *)argv[2]);
			un->acw.testtime = test_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->acw.testtime/10,un->acw.testtime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }   
}

/* 上升时间 */

uint32_t InstructionExec_StepAcwRtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       ramp_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != ACW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      ramp_time                             = atoi((void *)argv[2]);
			un->acw.ramptime = ramp_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->acw.ramptime/10,un->acw.ramptime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }    
}

/* 间隔时间 */

uint32_t InstructionExec_StepAcwItime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       pause_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != ACW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      pause_time                             = atoi((void *)argv[2]);
			un->acw.pausetime = pause_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->acw.pausetime/10,un->acw.pausetime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }      
}


/* 下降时间 */

uint32_t InstructionExec_StepAcwFtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       downtime;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != ACW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      downtime                             = atoi((void *)argv[2]);
			un->acw.downtime = downtime;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->acw.downtime/10,un->acw.downtime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }      
}

uint32_t InstructionExec_StepAcwArc(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       arc;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != ACW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      arc                             = atoi((void *)argv[2]);
			un->acw.arc = arc;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d",un->acw.arc);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }      
}


uint32_t InstructionExec_StepAcwFREQuency(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       outfreq;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != ACW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      outfreq                             = atoi((void *)argv[2]);
			un->acw.outfreq = outfreq;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%5.1f",(float)un->acw.outfreq / 10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}

/******************************************************************************
 *  函数名称 :DCW的step指令                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :2012.5.23                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : DCW_STEP指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/
uint32_t InstructionExec_StepDcwVolt(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        voltage;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != DCW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		voltage = atoi((void *)argv[2]);
		
		un->dcw.outvol = voltage;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%5.3f",(float)un->dcw.outvol/1000);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }     
}


uint32_t InstructionExec_StepDcwRange(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        cur_gear;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != DCW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		cur_gear = atoi((void *)argv[2]);
		switch(cur_gear){
			
			case 0:
				un->dcw.curgear = I3uA;
			break;
			case 1:
				un->dcw.curgear = I30uA;
			break;
			case 2:
				un->dcw.curgear = I300uA;
			break;
			case 3:
				un->dcw.curgear = I3mA;
			break;
			case 4:
				un->dcw.curgear = I30mA;
			break;
			case 5:
				un->dcw.curgear = I100mA;
			break;
			default:
				
			break;
		}
		
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%d",un->dcw.curgear);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
      
}

uint32_t InstructionExec_StepDcwCurHigh(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        CurHigh;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != DCW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		CurHigh = atoi((void *)argv[2]);
		switch(un->dcw.curgear)
		{
			
			case I3uA:
				CurHigh = CurHigh * 10;
				break;
			case I30uA:
				CurHigh = CurHigh / 1;
				break;
			case I300uA:
				CurHigh = CurHigh / 10;
				break;
			case I3mA:
				CurHigh = CurHigh * 10;
				break;
			case I30mA:
				CurHigh = CurHigh * 10;
				break;
			case I100mA:
				
				break;
			default:
				
				break;
		}
		if(CurHigh <= un->dcw.curlow){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->dcw.curhigh = CurHigh;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
	else
	{
    //查询指令    
		switch(un->dcw.curgear)
		{
			
			case I3uA:
				rt_sprintf(buf,"%d.%03d", un->dcw.curhigh/1000,un->dcw.curhigh%1000);
				break;
			case I30uA:
				rt_sprintf(buf,"%d.%02d", un->dcw.curhigh/100,un->dcw.curhigh%100);
				break;
			case I300uA:
				rt_sprintf(buf,"%d.%d", un->dcw.curhigh/10,un->dcw.curhigh%10);
				break;
			case I3mA:
				rt_sprintf(buf,"%d.%03d", un->dcw.curhigh/1000,un->dcw.curhigh%1000);
				break;
			case I30mA:
				rt_sprintf(buf,"%d.%02d", un->dcw.curhigh/100,un->dcw.curhigh%100);
				break;
			case I100mA:
				rt_sprintf(buf,"%d.%d", un->dcw.curhigh/10,un->dcw.curhigh%10);
				break;
			default:
				
				break;
		}
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
	}
  
}

uint32_t InstructionExec_StepDcwCurLow(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        Curlow;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != DCW){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		Curlow = atoi((void *)argv[2]);
		switch(un->dcw.curgear)
		{
			
			case I3uA:
				Curlow = Curlow * 10;
				break;
			case I30uA:
				Curlow = Curlow / 1;
				break;
			case I300uA:
				Curlow = Curlow / 10;
				break;
			case I3mA:
				Curlow = Curlow * 10;
				break;
			case I30mA:
				Curlow = Curlow * 10;
				break;
			case I100mA:
				
				break;
			default:
				
				break;
		}
		if(Curlow >= un->dcw.curhigh){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->dcw.curlow = Curlow;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
	else
	{
    //查询指令    
		switch(un->dcw.curgear)
		{
			
			case I3uA:
				rt_sprintf(buf,"%d.%03d", un->dcw.curlow/1000,un->dcw.curlow%1000);
				break;
			case I30uA:
				rt_sprintf(buf,"%d.%02d", un->dcw.curlow/100,un->dcw.curlow%100);
				break;
			case I300uA:
				rt_sprintf(buf,"%d.%d", un->dcw.curlow/10,un->dcw.curlow%10);
				break;
			case I3mA:
				rt_sprintf(buf,"%d.%03d", un->dcw.curlow/1000,un->dcw.curlow%1000);
				break;
			case I30mA:
				rt_sprintf(buf,"%d.%02d", un->dcw.curlow/100,un->dcw.curlow%100);
				break;
			case I100mA:
				rt_sprintf(buf,"%d.%d", un->dcw.curlow/10,un->dcw.curlow%10);
				break;
			default:
				
				break;
		}
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
	}
}





uint32_t InstructionExec_StepDcwTtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       testtime;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != DCW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      testtime                             = atoi((void *)argv[2]);
			un->dcw.testtime = testtime;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->dcw.testtime/10,un->dcw.testtime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }      
}

uint32_t InstructionExec_StepDcwRtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       ramptime;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != DCW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      ramptime                             = atoi((void *)argv[2]);
			un->dcw.ramptime = ramptime;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->dcw.ramptime/10,un->dcw.ramptime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }     
}

uint32_t InstructionExec_StepDcwItime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       pause_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != DCW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      pause_time                             = atoi((void *)argv[2]);
			un->dcw.pausetime = pause_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->dcw.pausetime/10,un->dcw.pausetime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }   
}

/* 下降时间 */

uint32_t InstructionExec_StepDcwFtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       downtime;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != DCW){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      downtime                             = atoi((void *)argv[2]);
			un->dcw.downtime = downtime;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->dcw.downtime/10,un->dcw.downtime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }      
}


uint32_t InstructionExec_StepDcwArc(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
											
    }
    //查询指令
    else
    {

    }
    return TRUE;     
}










uint32_t InstructionExec_StepGrCurr(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        current;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != GR){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		current = atoi((void *)argv[2]);
		
		un->gr.outcur = current;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%3.1f",(float)un->gr.outcur/10);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }         
}

uint32_t InstructionExec_StepGrLow(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        reslow;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != GR){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		reslow = atoi((void *)argv[2]);
		if(reslow >= un->gr.reshigh){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->gr.reslow = reslow;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
	else
	{
    //查询指令    
		rt_sprintf(buf,"%d", un->gr.reslow);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
	}    
}


uint32_t InstructionExec_StepGrHigh(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        reshigh;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != GR){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		reshigh = atoi((void *)argv[2]);
		if(reshigh <= un->gr.reslow){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->gr.reshigh = reshigh;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
	else
	{
    //查询指令    
		rt_sprintf(buf,"%d", un->gr.reshigh);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
	}        
}
uint32_t InstructionExec_StepGrTtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       testtime;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != GR){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      testtime                             = atoi((void *)argv[2]);
			un->gr.testtime = testtime;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->gr.testtime/10,un->gr.testtime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }          
}
uint32_t InstructionExec_StepGrItime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       pause_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != GR){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      pause_time                             = atoi((void *)argv[2]);
			un->gr.pausetime = pause_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->gr.pausetime/10,un->gr.pausetime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }       
}
uint32_t InstructionExec_StepGrFREQuency(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       outfreq;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != GR){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      outfreq                             = atoi((void *)argv[2]);
			un->gr.outfreq = outfreq;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%4.1f",(float)un->gr.outfreq / 10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}



/******************************************************************************
 *  函数名称 :LC的step指令                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :2012.5.24                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : LC_STEP指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

uint32_t InstructionExec_StepLcVolt(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        voltage;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != LC){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		voltage = atoi((void *)argv[2]);
		
		un->lc.outvol = voltage;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%5.1f",(float)un->lc.outvol/10);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
      
}


uint32_t InstructionExec_StepLcRange(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        cur_gear;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != LC){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		cur_gear = atoi((void *)argv[2]);
		switch(cur_gear){
			
			case 0:
				un->lc.curgear = I300uA;
			break;
			case 1:
				un->lc.curgear = I3mA;
			break;
			case 2:
				un->lc.curgear = I30mA;
			break;
			case 3:
				
			break;
			default:
				
			break;
		}
		
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		sprintf(buf,"%d",un->lc.curgear - 2);
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
      
}






uint32_t InstructionExec_StepLcCurHigh(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        CurHigh;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != LC){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		CurHigh = atoi((void *)argv[2]);
		switch(un->lc.curgear)
		{
			case I300uA:
				CurHigh = CurHigh / 10;
				break;
			case I3mA:
				CurHigh = CurHigh * 10;
				break;
			case I30mA:
				CurHigh = CurHigh * 1;
				break;
			case I100mA:
				CurHigh = CurHigh / 1;
				break;
			default:
				
				break;
		}
		if(CurHigh <= un->lc.curlow){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->lc.curhigh = CurHigh;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		switch(un->lc.curgear)
		{
			case I300uA:
				rt_sprintf(buf,"%d.%d", un->lc.curhigh/10,un->lc.curhigh%10);
				break;
			case I3mA:
				rt_sprintf(buf,"%d.%03d", un->lc.curhigh/1000,un->lc.curhigh%1000);
				break;
			case I30mA:
				rt_sprintf(buf,"%d.%02d", un->lc.curhigh/100,un->lc.curhigh%100);
				break;
			case I100mA:
				break;
			default:
				
				break;
		}
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
}


uint32_t InstructionExec_StepLcCurLow(int argc, const uint8_t *argv[])
{
	UN_STR *       un;
	char           buf[10];
  int16_t        CurLow;
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	if(un->com.mode != LC){
		API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
		return FALSE;
	}
	if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
  {
		CurLow = atoi((void *)argv[2]);
		switch(un->lc.curgear)
		{
			case I300uA:
				CurLow = CurLow / 10;
				break;
			case I3mA:
				CurLow = CurLow * 10;
				break;
			case I30mA:
				CurLow = CurLow * 1;
				break;
			case I100mA:
				CurLow = CurLow / 1;
				break;
			default:
				
				break;
		}
		if(CurLow >= un->lc.curhigh){
			API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
			return FALSE;
		}
		un->lc.curlow = CurLow;
		save_steps_to_flash(flash_info.current_file);
		refresh_com();
		return TRUE; 
	}
    //查询指令    
  else
  {
		switch(un->lc.curgear)
		{
			case I300uA:
				rt_sprintf(buf,"%d.%d", un->lc.curlow/10,un->lc.curlow%10);
				break;
			case I3mA:
				rt_sprintf(buf,"%d.%03d", un->lc.curlow/1000,un->lc.curlow%1000);
				break;
			case I30mA:
				rt_sprintf(buf,"%d.%02d", un->lc.curlow/100,un->lc.curlow%100);
				break;
			case I100mA:
				break;
			default:
				
				break;
		}
		API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
		return 0; 
  }
}


uint32_t InstructionExec_StepLcTtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
    uint16_t       test_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      test_time                             = atoi((void *)argv[2]);
			un->lc.testtime = test_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->lc.testtime/10,un->lc.testtime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }   
}

/* 上升时间 */

uint32_t InstructionExec_StepLcRtime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       ramp_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      ramp_time                             = atoi((void *)argv[2]);
			un->lc.ramptime = ramp_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->lc.ramptime/10,un->lc.ramptime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }    
}
/* 间隔时间 */

uint32_t InstructionExec_StepLcItime(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       pause_time;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      pause_time                             = atoi((void *)argv[2]);
			un->lc.pausetime = pause_time;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->lc.pausetime/10,un->lc.pausetime%10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }      
}


uint32_t InstructionExec_StepLcFREQuency(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       outfreq;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      outfreq                             = atoi((void *)argv[2]);
			un->lc.outfreq = outfreq;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%5.1f",(float)un->lc.outfreq / 10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}

//  N、L相设置、查询指令
uint32_t InstructionExec_StepLcPHASe(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       NorLphase;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      NorLphase                             = *(argv[2]);
			un->lc.NorLphase = NorLphase;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%c",un->lc.NorLphase? 'N':'L');
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}


//  人体网络设置、查询指令
uint32_t InstructionExec_StepLcMDnetwork(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       MDnetwork;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      MDnetwork                             = *(argv[2]);
			un->lc.MDnetwork = MDnetwork;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%c",un->lc.MDnetwork + 'A');
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}

//  网络电压设置、查询指令
uint32_t InstructionExec_StepLcMDvol(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       MDvol;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      MDvol                             = *(argv[2]);
			un->lc.MDvol = MDvol;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d",un->lc.MDvol);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}


//医用部分

//  辅助电压设置、查询指令
uint32_t InstructionExec_StepLcAssistvol(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		uint16_t       Assistvol;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      Assistvol                             = atoi((void *)argv[2]);;
			un->lc.assistvol = Assistvol;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			sprintf(buf,"%d.%d",un->lc.assistvol / 10,un->lc.assistvol % 10);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}

//  医用综合设置、查询指令
typedef union
{
	rt_uint16_t Com;
	struct{
		rt_uint16_t	S14            : 1;
		rt_uint16_t	S13            : 1;
		rt_uint16_t	S12            : 1;
		rt_uint16_t	S11            : 1;
		rt_uint16_t	S10            : 1;
		rt_uint16_t	S8             : 1;
		rt_uint16_t	S7             : 1;
		rt_uint16_t	SH             : 1;//开、关
		rt_uint16_t	SL             : 1;//MD接地、浮地
		rt_uint16_t	singlefault    : 1;//单一故障：开、关	
		rt_uint16_t	MDpostion      : 6;//MD位置：MD1、MD2、MD3、MD4	
	}Item;
}LC_MD_COM;//LC 医用 综合设置

uint32_t InstructionExec_StepLcMDCom(int argc, const uint8_t *argv[])
{
		UN_STR *       un;
		LC_MD_COM      LcMDCom;
		char           buf[10];
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		if(un->com.mode != LC){
			API_PEMExecuteErrCodeSet(STATUS_EXECUTE_NOT_ALLOWED);
			return FALSE;
		}
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
      LcMDCom.Com                             = atoi((void *)argv[2]);
			if(LcMDCom.Item.MDpostion < 1 || LcMDCom.Item.MDpostion > 4){
				API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
				return FALSE;
			}
			un->lc.MDpostion    = LcMDCom.Item.MDpostion;
			un->lc.singlefault  = LcMDCom.Item.singlefault;
			un->lc.SL           = LcMDCom.Item.SL;
			un->lc.SH           = LcMDCom.Item.SH;
			un->lc.S7           = LcMDCom.Item.S7;
			un->lc.S8           = LcMDCom.Item.S8;
			un->lc.S10          = LcMDCom.Item.S10;
			un->lc.S11          = LcMDCom.Item.S11;
			un->lc.S12          = LcMDCom.Item.S12;
			un->lc.S13          = LcMDCom.Item.S13;
			un->lc.S14          = LcMDCom.Item.S14;
			save_steps_to_flash(flash_info.current_file);
			refresh_com();	
			return TRUE;			
    }
    //查询指令
    else
    {
			LcMDCom.Item.MDpostion    = un->lc.MDpostion;
			LcMDCom.Item.singlefault  = un->lc.singlefault;
			LcMDCom.Item.SL           = un->lc.SL;
			LcMDCom.Item.SH           = un->lc.SH;
			LcMDCom.Item.S7           = un->lc.S7;
			LcMDCom.Item.S8           = un->lc.S8;
			LcMDCom.Item.S10          = un->lc.S10;
			LcMDCom.Item.S11          = un->lc.S11;
			LcMDCom.Item.S12          = un->lc.S12;
			LcMDCom.Item.S13          = un->lc.S13;
			LcMDCom.Item.S14          = un->lc.S14;
			sprintf(buf,"%d",LcMDCom.Com);
			API_PEMOutputQueueStrPush((const uint8_t *)buf, NO_APPENDED_SIGN);
			return 0; 
    }  
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : IR_STEP指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

uint32_t InstructionExec_StepIrVolt(int argc, const uint8_t *argv[])
{


    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {

 	}
    //查询指令    
    else
    {

    }
    return TRUE;     
}

uint32_t InstructionExec_StepIrArange(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令

    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {

    }
    //查询指令
    else
    {

    }
    return TRUE;     
}

uint32_t InstructionExec_StepIrHigh(int argc, const uint8_t *argv[])
{

	
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {

    }
    //查询指令
    else
    {

    }
    return TRUE;     
}

uint32_t InstructionExec_StepIrLow(int argc, const uint8_t *argv[])
{
//     uint8_t    paramSrting[15] ={0};
//     int16_t     spaceSignIndex                     = 0;

    //第二项参数表示是查询指令 还是执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {

    }
    //查询指令
    else
    {

    }
    return TRUE;     
}

uint32_t InstructionExec_StepIrHighMax(int argc, const uint8_t *argv[])
{

    return TRUE;     
}

uint32_t InstructionExec_StepIrHighMin(int argc, const uint8_t *argv[])
{
    uint8_t    paramSrting[15];


    API_PEMOutputQueueStrnPush(paramSrting, strlen((void *)paramSrting), NO_APPENDED_SIGN);

    return TRUE;     
}

uint32_t InstructionExec_StepIrLowMax(int argc, const uint8_t *argv[])
{
    uint8_t    paramSrting[15];


    API_PEMOutputQueueStrnPush(paramSrting, strlen((void *)paramSrting), NO_APPENDED_SIGN);

    return TRUE;     
}

uint32_t InstructionExec_StepIrTtime(int argc, const uint8_t *argv[])
{


    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
									
    }
    //查询指令
    else
    {

    }
    return TRUE;     
}
uint32_t InstructionExec_StepIrRtime(int argc, const uint8_t *argv[])
{


    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {

					
    }
    //查询指令
    else
    {


    }
    return TRUE;     
}


uint32_t InstructionExec_StepIrItime(int argc, const uint8_t *argv[])
{


    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {

					
    }
    //查询指�
    else
    {


    }
    return TRUE;     
}

uint32_t InstructionExec_StepIrDtime(int argc, const uint8_t *argv[])
{


    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
								
    }
    //查询指令
    else
    {

    }
    return TRUE;     
}

uint32_t InstructionExec_StepIrDmode(int argc, const uint8_t *argv[])
{
//     uint8_t       paramSrting[15]                 = {0};
//     int8_t        modeIndex                  		= 0;
//    Param      *ptVoltParam                     = NULL;
//    Param      *ptCCurrParam                    = NULL;
//    uint16      chgCurrUpperVal                 = 0;

//     //第二项参数标示是 查询指令 还是 执行指令
//     if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
//     {
//         modeIndex = atoi((void *)argv[2]);
// 		switch(modeIndex)
// 		{
// 			case 0:
// 				st_InsulationInfo.Auto_Manual = 2;
// 				break;
// 			case 1:
// 				st_InsulationInfo.Auto_Manual = 1;
// 				break;
// 			default:
// 				break;
// 		}
// 		_Set_current_enter_control();
// 		LIB_Set_Test_Mode();
// 	}
// 	else
// 	{
// 		if(st_InsulationInfo.Auto_Manual == 0 || st_InsulationInfo.Auto_Manual == 2)
// 		{
// 			strcat((void *)paramSrting, "IRMODE: AUTO");
//         	API_PEMOutputQueueStrnPush(paramSrting, strlen((void *)paramSrting), NO_APPENDED_SIGN);
// 		}
// 		else
// 		{
// 			strcat((void *)paramSrting, "IRMODE: MANUAL");
//         	API_PEMOutputQueueStrnPush(paramSrting, strlen((void *)paramSrting), NO_APPENDED_SIGN);
// 		}
// 	}
    return TRUE;     
}

uint32_t InstructionExec_StepLcCCurr(int argc, const uint8_t *argv[])
{
//     uint8_t           paramSrting[10]             = {0};
//     int8_t            spaceSignIndex              = 0;

    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("haitao", strlen("haitao"), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : 系统指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/
uint32_t InstructionStruct_SystemOALArm(int argc, const uint8_t *argv[])
{
// 		uint8_t           paramSrting[15]             = {0};
// 		char ht[8]="1,00,0";
// 		uint8_t				    OALArmVal									=0;
// 	  if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
//     {
//       OALArmVal                             = atoi((void *)argv[2]);
// 			if(OALArmVal==1)//打开开路报警
// 			{
// 				open_test_flag=0;
// 			}
// 			else if(OALArmVal==0)//关闭开路报警
// 			{
// 				
// 				open_test_flag=1;
// 			}
// 				OP[0]=open_test_flag;
// 			 // WRITE_24C256(0xA0,9,OP,1);
// 			WRITE_24C256(0xA0,20,OP,1);
//     }
// 		else
// 		{
// 			if(open_test_flag==1)
// 			{
// 				strcat((void *)ht, "0");
// 				
// 			}
// 			else if(open_test_flag==0)
// 			{
// 				strcat((void *)ht, "1");
// 			}
// 			API_PEMOutputQueueStrnPush(ht, strlen((void*)ht), NO_APPENDED_SIGN);
// 		}
//     
// 			
		
	return TRUE; 
}
uint32_t InstructionExec_SysRhint(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysRsave(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysOcover(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysLanguage(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysTime(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysDate(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysKeyKlock(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

uint32_t InstructionExec_SysPswNew(int argc, const uint8_t *argv[])
{
    return TRUE;     
}

uint32_t InstructionExec_SysPswNow(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_SysPswStatus(int argc, const uint8_t *argv[])
{
    //第二项参数标示是 查询指令 还是 执行指令
    if (INSTRUCTION_ATTR_EXECUT == ((uint32_t)argv[1]))
    {
        ;
    }
    //查询指令
    else
    {
        API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    }
    return TRUE;     
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : 结果指令集执行函数                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

uint32_t InstructionExec_ResuCapacityUsed(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuCapacityFree(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuCapacityAll(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuCapacityPass(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuCapacityFail(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuPpercent(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuStatistics(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

uint32_t InstructionExec_ResuClear(int argc, const uint8_t *argv[])
{
    return TRUE;     
}

uint32_t InstructionExec_ResuFetchAll(int argc, const uint8_t *argv[])
{
    API_PEMOutputQueueStrnPush("Query.....", strlen("Query....."), NO_APPENDED_SIGN);
    return TRUE;     
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :  更新测试状态函数                                                             
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/
void Comm_UpdateTestStatusForComm(TestStatus testStatus)
{
    s_TestStatus                    = testStatus;
}



uint32_t usart_cur_step_set(int argc, const uint8_t *argv[])
{
	uint8_t step;

	
	step = atoi((void *)argv[2]);
	if(step < 1)
	{
		step = 1;
	}
	
	if(step>file_info[flash_info.current_file].totalstep)
	{
		API_PEMExecuteErrCodeSet(STATUS_DATA_OUT_OF_RANGE);
		return FALSE;
	}
	
	current_step_num = step;
	g_cur_step = cs99xx_list.head;//指向步骤文件头
	
	for(step = 1;step<current_step_num;step++)
	{
		g_cur_step = g_cur_step->next;	
	}
	
	refresh_com();
	return TRUE;
}


uint32_t usart_cur_step_message_get(int argc, const uint8_t *argv[])
{
	return 0;
}


uint32_t usart_total_step_set(int argc, const uint8_t *argv[])
{
	return 0;
}

extern uint8_t  Usart_Data_Pretreat(uint8_t data);

#define  USART2_TX_BUF_SIZE   (256)
#define  USART2_RX_BUF_SIZE   (256)
typedef struct{
	uint8_t rd_index;
	uint8_t wr_index;
	volatile uint8_t count;
	uint8_t buf_overflow;
	uint8_t *pData;
}UART_FLAG;

uint8_t Usart2_tx_buf[USART2_TX_BUF_SIZE];
UART_FLAG Usart2_Tx_Pra  = {0, 0, 0, 0, Usart2_tx_buf};
uint8_t Usart2_rx_buf[USART2_RX_BUF_SIZE];
UART_FLAG Usart2_Rx_Flag = {0, 0, 0, 0, Usart2_rx_buf};
extern ITStatus USART_SetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
extern ITStatus USART_ResetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
#include "CS9931_Config.h"
#include "jj98.h"

uint8_t usart2_send_st;
uint8_t usart2_busy_st;

void set_usart2_busy_st(void)
{
	usart2_busy_st = 1;
}
void reset_usart2_busy_st(void)
{
	usart2_busy_st = 0;
}
uint8_t get_usart2_busy_st(void)
{
	return usart2_busy_st;
}
uint8_t get_usart2_send_st(void)
{
	uint8_t st = usart2_send_st;
	
	usart2_send_st = 0;
	
	return st;
}
/* 有源时与上位机通讯SCPI */
#if LC_TEST_MODE==LC_YY
void USART2_IRQHandler(void)
{
	uint8_t  tx_temp;
	
	rt_interrupt_enter();
	
	//接收中断
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		uint8_t dummy;
		
		dummy = USART_ReceiveData(USART2);
		
        jj98_comm_analysis(dummy);
		if(system_parameter_t.com.control && Usart_Data_Pretreat(dummy))
		{
			int_API_PEMRead(dummy);
			API_PEMExecute();//通讯解析
		}
	}
	//发送中断
	if (USART_GetITStatus(USART2, USART_IT_TC) == SET)
	{
		if(Usart2_Tx_Pra.count)
		{
			tx_temp = *(Usart2_Tx_Pra.pData + Usart2_Tx_Pra.rd_index);
			
			if(++Usart2_Tx_Pra.rd_index == USART2_TX_BUF_SIZE)
			{
				Usart2_Tx_Pra.rd_index = 0;
			}
			
			--Usart2_Tx_Pra.count;
			USART_SendData(USART2, tx_temp);
		}
		else
		{
			USART_ClearFlag(USART2, USART_FLAG_TC);
		}
	}
	
	/* leave interrupt */
	rt_interrupt_leave();
}
/* 无源时与外部的变频电源通信 符号变频电源的通讯协议 */
#else
void USART2_IRQHandler(void)
{
	uint8_t  tx_temp;
	
	rt_interrupt_enter();
	
	//接收中断
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		uint8_t dummy;
		
		dummy = USART_ReceiveData(USART2);
	}
	//发送中断
	if (USART_GetITStatus(USART2, USART_IT_TC) == SET)
	{
		if(Usart2_Tx_Pra.count == 0)
		{
			uart2_receive_data_en();
			usart2_send_st = 1;
		}
		else
		{
			USART_SendData(USART2, Usart2_Tx_Pra.pData[Usart2_Tx_Pra.rd_index]);
			--Usart2_Tx_Pra.count;
			++Usart2_Tx_Pra.rd_index;
		}
	}
	rt_interrupt_leave();
}
#endif


/**************************************************************
函数名：Usart2_GetKey
功能：  从串口得到数据
参数：  指向数据的指针
返回值：所得到的结果
***************************************************************/
uint8_t Usart2_GetKey(uint8_t *pdata)
{
	if(Usart2_Rx_Flag.count)
	{
	 	*pdata = *(Usart2_Rx_Flag.pData + Usart2_Rx_Flag.rd_index);
		
		if(++Usart2_Rx_Flag.rd_index == USART2_RX_BUF_SIZE)
		{
			Usart2_Rx_Flag.rd_index = 0;
		}
		
		--Usart2_Rx_Flag.count;
		
		return 1;
	}
	else
	{
		return 0;
	}
}

void USART2_DISPOSE_TASK(void)
{
	uint8_t data;
	
	while(Usart2_GetKey(&data))
	{
		if(system_parameter_t.com.control)
		{
			int_API_PEMRead(data);
			API_PEMExecute();//通讯解析
		}
	}
}

void uart2_send_data_en(void)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
}
void uart2_receive_data_en(void)
{
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
}

uint32_t rt_uart_write(const void* buffer, uint32_t size)
{
	int32_t i = 0;
	
	Usart2_Tx_Pra.rd_index = 0;
	Usart2_Tx_Pra.count = size;
	
	if(Usart2_Tx_Pra.count > USART2_TX_BUF_SIZE)
	{
		Usart2_Tx_Pra.count = USART2_TX_BUF_SIZE;
	}
	
	for(i = 0; i < Usart2_Tx_Pra.count; i++)
	{
		Usart2_Tx_Pra.pData[i] = ((uint8_t*)buffer)[i];
//		USART_SendData(USART2, Usart2_Tx_Pra.pData[i]);
//		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
	}
	
	uart2_send_data_en();
	
	
// 	int i;
// 	char *ptr;
// 	uint8_t  tx_temp;
// 	ptr = (char*)buffer;

// 	for(i=0;i<size;i++)
// 	{
// 		//缓冲未满
// 		if(Usart2_Tx_Pra.count < USART2_TX_BUF_SIZE)
// 		{		
// 			Usart2_Tx_Pra.count++;
// 			*(Usart2_Tx_Pra.pData + Usart2_Tx_Pra.wr_index) = *ptr++;
// 			
// 			if(++Usart2_Tx_Pra.wr_index == USART2_TX_BUF_SIZE)
// 			{
// 				Usart2_Tx_Pra.wr_index = 0;
// 			}
// 		}
// 		//缓冲已满
// 		else
// 		{
// 			Usart2_Tx_Pra.buf_overflow = 1;
// 		}
// 	}
// 	
// 	if(Usart2_Tx_Pra.count)
// 	{
// 		tx_temp = *(Usart2_Tx_Pra.pData + Usart2_Tx_Pra.rd_index);
// 		
// 		if(++Usart2_Tx_Pra.rd_index == USART2_TX_BUF_SIZE)
// 		{
// 			Usart2_Tx_Pra.rd_index = 0;
// 		}
// 		
// 		--Usart2_Tx_Pra.count;
// 		USART_SendData(USART2, tx_temp);
// 	}
// 	
// 	return (uint32_t) ptr - (uint32_t) buffer;
}



/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

