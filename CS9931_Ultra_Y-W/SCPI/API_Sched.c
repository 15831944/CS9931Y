/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include "stm32f4xx.h"
// #include <lpc17xx.h>
// #include <system_lpc17xx.h>
// #include <core_cm3.h>
#include <stdint.h> 
#include    "api_sched.h"
#include    "Scpi_Parser.h"


/******************************************************************************
 *                            ���ļ��ڲ��궨��
******************************************************************************/

#define                     PANEL_INSTALL_MSG                         (0x01)
#define                     SCHED_INSTALL_MSG                         (0x02)

#define NULL 0  
#define    FALSE                                            (0)
#define    TRUE                                            (1)
#define    portQUEUELEN_TYPE	uint32_t  
#define    portCPSR_TYPE	uint32_t
#define     INLINE                                          __inline

/******************************************************************************
 *                       ���ļ�������ľ�̬����
******************************************************************************/ 

volatile portuBASE_TYPE         s_SchedulerRunStatus            = SCHEDULER_INIT;           //ϵͳ����״̬  
static volatile PanelUnit      *s_ptCurrentPanelUnit            = NULL;                     //��ǰ���ָ��
static volatile PanelUnit      *s_ptPrevPanelUnit          		= NULL;                     //ǰһ���ָ��
static volatile PanelUnit      *s_ptCurrentPanelUnitTmp         = NULL; 	     		    //��ǰ���ָ���ݴ�ֵ
static volatile uint32_t  s_PanelInstallServMsgID;                                    //��尲װ������ϢID
static  Fp_pfPanelCallBackFunc *s_pfPanelCallBackFunc           = NULL;                     //���ص�����ֵ
static  uint32_t          s_SchedCommPanelMsgID           = 0;                        //�������������干ͬ��ϢID 
static  Fp_pfTestSchedCBF      *s_PfTestSchedCBF                = NULL;				        //���Իص�����ָ��
static  Fp_pfSchedlerDriveMsgSampleCallBackFunc 
					           *s_pfSchedlerDriveMsgSampleCallBackFunc	             
                                                                = NULL;				        //������������Ϣ�ɼ��ص�����ָ��
static  Fp_pfSchedCBF          *s_PfSchedCBF                    = NULL;                     //�������ص�����ָ�� 
static volatile SchedMsg        st_SchedMsg;                           		                //��������Ϣ  

/******************************************************************************
 *                       ���ļ��������ȫ�ֽṹ��
******************************************************************************/

volatile PanelDriveMsg          t_PanelDriveMsg                      = {0};                 //�����Ϣ
volatile PanelDriveMsg  		t_PanelDriveMsgSnapshot;                                    //���������Ϣ����

static portuBASE_TYPE _API_SchedSendMessage(const PanelNoticeMsg *pmessage)
{
    portuBASE_TYPE   rt 	        		                    = TRUE;
    
    rt                                                          = s_pfPanelCallBackFunc((const PanelNoticeMsg *)pmessage);
    #if (DEBUG_VERSION > 0)
    {
        t_AppDebugWatch.m_uint8_1                               = rt;
    }
    #endif
    
    return rt;
}
static portuBASE_TYPE _API_SchedPanelDraw(PanelWindow *ptpanelWindow, portuBASE_TYPE drawInfo)
{
    if (NULL != ptpanelWindow)
    {
        //
    }

    return TRUE;
}

static portuBASE_TYPE _API_SchedSendMessage(const PanelNoticeMsg *pmessage);
static portuBASE_TYPE _API_SchedPanelDraw(PanelWindow *ptpanelWindow, portuBASE_TYPE drawInfo);


portCPSR_TYPE BSP_InterruptDisable(void)
{
    return TRUE;
}
#define portDISABLE_INTERRUPTS()					        BSP_InterruptDisable()
void BSP_InterruptEnable(portCPSR_TYPE level)
{
    
}
#define portENABLE_INTERRUPTS(level)			            BSP_InterruptEnable(level)
/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���һ��������Ϣ                                                              
 *                                                                           
 *  ��ڲ��� : msgHandleStyle��������Ϣ���� ��ΪԶ�ء����⡢��ʵ��������
 *			   msgInstallCode: ��Ϣ��װ��   �����ж�����Ƿ�װ�˴���Ϣ
 *             msgID         : ��ϢID       ��Ϣֵ                                                  
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : �����Ϣʱ ��Ҫ�ٽ����ı���                                                               
 *             							                                                              
 *                                                                            
******************************************************************************/ 

static portuBASE_TYPE _int_API_RSDriveMsgUnitPut(DriveMsgQueue *pdriveMsgQueue, portuBASE_TYPE installCodeIndex, 
                                                portuBASE_TYPE value,
                                                portuBASE_TYPE status)
{
    DriveMsgUnit  *pdriveMsgUnit                                = pdriveMsgQueue->m_queue;
    
    if (pdriveMsgQueue->m_writeIndex < MSG_QUEUE_DEPTH)
    {
        pdriveMsgUnit                                          += pdriveMsgQueue->m_writeIndex;
        pdriveMsgUnit->m_installCodeIndex                       = installCodeIndex;
        pdriveMsgUnit->m_value                                  = value;
        pdriveMsgUnit->m_status                                 = status;
        pdriveMsgQueue->m_writeIndex++;

        return TRUE;
    }
    //��Ϣ���ʧ��
    return FALSE;
}

//��ȡʱ�̶���t_PanelDriveMsgSnapshot��ȡ �������Ƿ�����Ϣ�ɶ�  ����Ϣ�ɶ� ����TRUE 
//����Ϣ�ɶ� ����FALSE
static portuBASE_TYPE _int_API_RSDriveMsgUnitGet(PanelNoticeMsg *ppanelNoticeMsg, 
                                                 DriveMsgQueue *pdriveMsgQueue,
                                                 uint8 *installCodeIndex)
{
    DriveMsgUnit  *pdriveMsgUnit                                = pdriveMsgQueue->m_queue;
    
    pdriveMsgUnit                                              += pdriveMsgQueue->m_readIndex;
    ppanelNoticeMsg->m_value                                    = pdriveMsgUnit->m_value;
    ppanelNoticeMsg->m_status                                   = pdriveMsgUnit->m_status;
    //������Ϣ��װ������
    *installCodeIndex                                           = (pdriveMsgUnit->m_installCodeIndex);
    pdriveMsgQueue->m_readIndex++;
    if (pdriveMsgQueue->m_readIndex == pdriveMsgQueue->m_writeIndex)
    {
        return FALSE;
    }
    return TRUE;
}

/*
static portuBASE_TYPE _API_RSDriveMsgNullChk(DriveMsgQueue *pdriveMsgQueue)
{
    portuBASE_TYPE  rt                                          = FALSE;
    portCPSR_TYPE   level                                       = portDISABLE_INTERRUPTS();

    if (pdriveMsgQueue->m_readIndex == pdriveMsgQueue->m_writeIndex)
    {
        rt                                                      = TRUE;
    }
    portENABLE_INTERRUPTS(level);
    return rt;
}
*/

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���һ��������Ϣ                                                              
 *                                                                           
 *  ��ڲ��� : msgHandleStyle��������Ϣ���� ��ΪԶ�ء����⡢��ʵ��������
 *			   msgInstallCode: ��Ϣ��װ��   �����ж�����Ƿ�װ�˴���Ϣ
 *             msgID         : ��ϢID       ��Ϣֵ                                                  
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : �����Ϣʱ ��Ҫ�ٽ����ı���                                                               
 *             							                                                              
 *                                                                            
******************************************************************************/ 

static portuBASE_TYPE _int_API_SchedDriveMsgAdd(PanelDriveMsg *ptpanelDriveMsg, portuBASE_TYPE msg, 
								                portuBASE_TYPE installCodeIndex, 
                                                portuBASE_TYPE value,
                                                portuBASE_TYPE status)
{
	portuBASE_TYPE     rt                                       = FALSE;
    DriveMsgQueue     *pdriveMsgQueue                           = &ptpanelDriveMsg->m_keyQueue;
    
    //���Ƿ�Ϊ��Ч��Ϣ   ���������˳�
	if (NONE_DRIVE_MSG_ID_INDEX != installCodeIndex)
    {
		//������Ϣ����
		ptpanelDriveMsg->m_msg                                 |= msg; 
		if (INSTALL_MSG_KEY == msg)
		{
		    pdriveMsgQueue                                      = &ptpanelDriveMsg->m_keyQueue;	
		}
		else if (INSTALL_MSG_SCHED == msg)
		{
		    pdriveMsgQueue                                      = &ptpanelDriveMsg->m_schedQueue;
		}
        else if (INSTALL_MSG_COMM == msg)
        {
            //
        }
        rt = _int_API_RSDriveMsgUnitPut(pdriveMsgQueue, installCodeIndex, value, status);
    }

    return rt;
}

static portuBASE_TYPE _API_SchedDriveMsgAdd(PanelDriveMsg *ptpanelDriveMsg, portuBASE_TYPE msg, 
								            portuBASE_TYPE installCodeIndex, 
                                            portuBASE_TYPE value,
                                            portuBASE_TYPE status)
{
    portuBASE_TYPE  rt                                          = FALSE;
    portCPSR_TYPE   level                                       = portDISABLE_INTERRUPTS();

	rt = _int_API_SchedDriveMsgAdd(ptpanelDriveMsg, msg, installCodeIndex, value, status);
	portENABLE_INTERRUPTS(level);

    return rt;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ���һ��������Ϣ                                                              
 *                                                                           
 *  ��ڲ��� : msgHandleStyle��������Ϣ���� ��ΪԶ�ء����⡢��ʵ��������
 *			   msgInstallCodeIndex: ��Ϣ��װ��   �����ж�����Ƿ�װ�˴���Ϣ
 *             msgID         : ��ϢID       ��Ϣֵ                                                  
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : �����Ϣʱ ����Ҫ�ٽ����ı���                                                               
 *             							                                                              
 *                                                                            
******************************************************************************/ 

portuBASE_TYPE API_SchedDriveMsgAdd(portuBASE_TYPE msgHandleStyle, portuBASE_TYPE msgInstallCodeIndex, portuBASE_TYPE msg,
                                    portuBASE_TYPE msgStatus)
{
	return _API_SchedDriveMsgAdd((PanelDriveMsg *)&t_PanelDriveMsg, msgHandleStyle, msgInstallCodeIndex, msg, msgStatus);
}

portuBASE_TYPE API_SchedInnerDriveMsgAdd(portuBASE_TYPE msgHandleStyle, portuBASE_TYPE msgInstallCodeIndex, portuBASE_TYPE msg,
                                     portuBASE_TYPE msgStatus)
{
	return _API_SchedDriveMsgAdd((PanelDriveMsg *)&t_PanelDriveMsgSnapshot, msgHandleStyle, msgInstallCodeIndex, msg, msgStatus);
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��װ��尲װ������ID��(����֧����Ӧ����ϢID��IDֵ����AppConfig.h                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

void API_SchedPanelMsgIDInstall(portMSGID_TYPE panelMsgID)
{
    s_PanelInstallServMsgID                                    |= panelMsgID;    
}

void API_SchedPanelMsgIDUninstall(portMSGID_TYPE panelMsgID)
{
    s_PanelInstallServMsgID                                    &= (~panelMsgID);    
}

void API_SchedPanelMsgIDClr(void)
{
    s_PanelInstallServMsgID                                     = 0;   
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : �����尲װ������ID��(����֧����Ӧ����ϢID��IDֵ����AppConfig.h                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

static void _API_SchedEnvironmentInit(void)
{
//     #if (COMPILE_ENV == AVR_GCC)
//     {
//         s_PanelInstallServMsgID                                 = PGM_STRUCT_MEMBER_WORD_READ(s_ptCurrentPanelUnit, PanelUnit, m_panelInstallServMsgID);
//         s_pfPanelCallBackFunc                                   = (volatile Fp_pfPanelCallBackFunc *)(PGM_STRUCT_MEMBER_WORD_READ(s_ptCurrentPanelUnit, PanelUnit, m_pfPanelCallBackFunc));
//     }
//     #elif (COMPILE_ENV == ARM_KEIL)
//     {
        s_PanelInstallServMsgID                                 = s_ptCurrentPanelUnit->m_panelInstallServMsgID;                
        s_pfPanelCallBackFunc                                   = s_ptCurrentPanelUnit->m_pfPanelCallBackFunc;
//     }
//     #endif
 }

static void _API_SchedPanelSwitchExecCore(PanelNoticeMsg *ptpanelNoticeMsg)
{
    portuBASE_TYPE  rt                                          = TRUE;
    
    //������Ϣ���Ϊ��ʼ�������Ϣ
    ptpanelNoticeMsg->m_msg                                     = MSG_CLOSE;
    //�Ե�ǰ��巢�����ر���Ϣ
    rt  = _API_SchedSendMessage((const PanelNoticeMsg *)ptpanelNoticeMsg);
    if (TRUE == rt)
    {
        s_ptPrevPanelUnit                                       = s_ptCurrentPanelUnit;
        s_ptCurrentPanelUnit                                    = s_ptCurrentPanelUnitTmp;
        //��ʼ���������Ȼ���
	    _API_SchedEnvironmentInit();
        ptpanelNoticeMsg->m_msg                                 = MSG_INITIALIZING;
        _API_SchedSendMessage((const PanelNoticeMsg *)ptpanelNoticeMsg);
        //������
        _API_SchedPanelDraw(s_ptCurrentPanelUnit->m_pwindow, PANEL_DRAW_FLG_NORMAL);
        ptpanelNoticeMsg->m_msg                                 = MSG_INIT_OVER;
        _API_SchedSendMessage((const PanelNoticeMsg *)ptpanelNoticeMsg);
    }
}

static portuBASE_TYPE _API_SchedPanelSwitchExec(PanelNoticeMsg *ptpanelNoticeMsg)
{
    portuBASE_TYPE  rt                                          = FALSE;
    
    //���ص�����ִ�гɹ��� �ж��Ƿ����������ʼ����Ϣ
    if (s_ptCurrentPanelUnit != s_ptCurrentPanelUnitTmp)
    {
        _API_SchedPanelSwitchExecCore(ptpanelNoticeMsg); 
        rt                                                      = TRUE;       
    }

    return rt;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

static INLINE void _int_API_SchedDriveMsgCpyAndClr(void)
{
    //���������Ϣ
	memcpy((void *)&t_PanelDriveMsgSnapshot, (const void *)&t_PanelDriveMsg, sizeof(PanelDriveMsg));
	memset((void *)&t_PanelDriveMsg, 0, sizeof(PanelDriveMsg)); 
}

// static portuBASE_TYPE _API_SchedSendMessage(const PanelNoticeMsg *pmessage)
// {
//     portuBASE_TYPE   rt 	        		                    = TRUE;
//     
//     rt                                                          = s_pfPanelCallBackFunc((const PanelNoticeMsg *)pmessage);
//     #if (DEBUG_VERSION > 0)
//     {
//         t_AppDebugWatch.m_uint8_1                               = rt;
//     }
//     #endif
//     
//     return rt;
// }

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

void API_ShcedPanelDisplay(const PanelUnit *ptpanel)
{
    s_ptCurrentPanelUnitTmp                                     = (PanelUnit *)ptpanel;
}
void API_ShcedPanelDisplayExec(const PanelUnit *ptpanel)
{
    PanelNoticeMsg  t_panelNoticeMsg         	                = {0};      
    
    API_ShcedPanelDisplay(ptpanel);
    _API_SchedPanelSwitchExecCore(&t_panelNoticeMsg);
}

const PanelUnit * API_ShcedCurrentPanelGet(void)
{
    return (const PanelUnit *)s_ptCurrentPanelUnit;
}

const PanelUnit * API_ShcedPrevPanelGet(void)
{
    return (const PanelUnit *)s_ptPrevPanelUnit;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��尲װ��Ϣ���                                                         
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                               
 *                                                                            
******************************************************************************/

portuBASE_TYPE API_RSPanelInstallKeyChk(portuBASE_TYPE msgMaskCodeIndex)
{
    portuBASE_TYPE  rt 	        		                        = FALSE;
	portMSGID_TYPE  msgInstallMaskCode	                        = (portMSGID_TYPE)((portMSGID_TYPE)1 << msgMaskCodeIndex);
	
    //ͨ�����������ж��Ƿ�װ�˴�������Ϣ
    if (msgInstallMaskCode == (s_PanelInstallServMsgID & msgInstallMaskCode))
    {
        rt                    			                        = PANEL_INSTALL_MSG;    
    }
	#if (SCHED_COMMON_MSG_SUPPORT > 0)
	{
		if (msgInstallMaskCode == (s_SchedCommPanelMsgID & msgInstallMaskCode))
		{
			rt                    	                           |= SCHED_INSTALL_MSG;     
		}
	}
	#endif
    
    return rt;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : Ϊ�˼�ʡ���˶Ե������ص������Ƿ���ڵ��ж�                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

void API_SchedExecute(void)
{
    volatile PanelNoticeMsg     t_panelNoticeMsg         	    = {0};                   //�������֪ͨ��Ϣ�ṹ��
	volatile portuBASE_TYPE     drivemsgInstallCodeIndex        = 0;                     //������Ϣ��װ��
    volatile portuBASE_TYPE     rt								= TRUE;
	volatile portuBASE_TYPE     schedMsgHandleCircle            = INSTALL_SCHED_INIT;    //������ѭ��������Ϣ��־
	volatile portuBASE_TYPE     driveMsgInstallResult           = 0;					 //������Ϣ��װ�жϽ��
	volatile portCPSR_TYPE      level;
    Fp_pfSchedCBF              *pfschedCBF;
    Fp_pfTestSchedCBF          *pftestSchedCBF;
    
	switch (s_SchedulerRunStatus)
    {
        //��������ʼ��״̬  �������������Ϣ
        case SCHEDULER_INIT:
		
			t_panelNoticeMsg.m_msg                              = MSG_INITIALIZING;      //������Ϣ���Ϊ��ʼ�������Ϣ
			schedMsgHandleCircle                                = INSTALL_SCHED_INIT;    //��������ʼ����Ϣ
			//��ʼ���������Ȼ���
			_API_SchedEnvironmentInit();
            //���ô����Ļص����� ��������Ϣ
            rt                                                  = _API_SchedSendMessage((const PanelNoticeMsg *)&t_panelNoticeMsg);
            //�����ʾ
            _API_SchedPanelDraw(s_ptCurrentPanelUnit->m_pwindow, PANEL_DRAW_FLG_NORMAL);
            //������Ϣ���Ϊ��ʼ�����������Ϣ
            t_panelNoticeMsg.m_msg                              =  MSG_INIT_OVER;
            rt                                                  = _API_SchedSendMessage((const PanelNoticeMsg *)&t_panelNoticeMsg);
			#if (DEBUG_VERSION > 0)
            {
                t_AppDebugWatch.m_uint8_1                       = rt;
            }
            #endif
            if (TRUE == rt)
			{
				//ϵͳ��������̬
                s_SchedulerRunStatus                            = SCHEDULER_RUNNING; 
                do {

                    //���ص�����ִ�гɹ��� �ж��Ƿ����������ʼ����Ϣ
                    rt  = _API_SchedPanelSwitchExec((PanelNoticeMsg *)&t_panelNoticeMsg);
                }while (TRUE == rt);
			}
			else
			{
				//ϵͳ�������̬
                s_SchedulerRunStatus                            = SCHEDULER_ERROR;
			}
            break;
        
		//����̬
        case SCHEDULER_RUNNING: 
		
			//�ж��Ƿ�װ�˵�����������Ϣ�ɼ�����
			if (NULL != s_pfSchedlerDriveMsgSampleCallBackFunc)
			{
				(*s_pfSchedlerDriveMsgSampleCallBackFunc)();						    //������������Ϣ�ɼ��ص�����
			}
			//������Դ����
            level                                               = portDISABLE_INTERRUPTS();        
            _int_API_SchedDriveMsgCpyAndClr();
            portENABLE_INTERRUPTS(level);
			
			//��ʼ��������������ϢΪ��������������
            schedMsgHandleCircle                                = INSTALL_SCHED_WELL; 
			
			//���������յ�������Ϣ
            if ((MSG_NONE != t_PanelDriveMsgSnapshot.m_msg) && (NULL != s_pfPanelCallBackFunc))
            {
                do{
					//��������Ϣ
                    if (t_PanelDriveMsgSnapshot.m_msg & INSTALL_MSG_KEY)
                    {
                        t_panelNoticeMsg.m_msg                  = MSG_KEY;
                        level   = portDISABLE_INTERRUPTS();
                        //�ж���Ϣ�����ѿ� ���ѿ� �������ʵ����Ϣ
                        if (FALSE == _int_API_RSDriveMsgUnitGet((PanelNoticeMsg *)&t_panelNoticeMsg, 
                                                                (DriveMsgQueue *)&t_PanelDriveMsgSnapshot.m_keyQueue, 
                                                                (uint8 *)&drivemsgInstallCodeIndex))
                        {
                            t_PanelDriveMsgSnapshot.m_msg        &= ~INSTALL_MSG_KEY; 
                        }
                        portENABLE_INTERRUPTS(level);
                    }
					//����������Ϣ
                    else if (t_PanelDriveMsgSnapshot.m_msg & INSTALL_MSG_SCHED)
					{
                        t_panelNoticeMsg.m_msg                  = MSG_SCHED;
                        level   = portDISABLE_INTERRUPTS();
                        //�ж���Ϣ�����ѿ� ���ѿ� �������������Ϣ
                        if (FALSE == _int_API_RSDriveMsgUnitGet((PanelNoticeMsg *)&t_panelNoticeMsg, 
                                                                (DriveMsgQueue *)&t_PanelDriveMsgSnapshot.m_schedQueue, 
                                                                (uint8 *)&drivemsgInstallCodeIndex))
                        {
                            t_PanelDriveMsgSnapshot.m_msg        &= ~INSTALL_MSG_SCHED; 
                        }
                        portENABLE_INTERRUPTS(level);
                    }
                    //ͨѶ����Ϣ
                    else if (t_PanelDriveMsgSnapshot.m_msg & INSTALL_MSG_COMM)
					{
                        t_panelNoticeMsg.m_msg                  = MSG_COMM;
                        level   = portDISABLE_INTERRUPTS();
                        //�ж���Ϣ�����ѿ� ���ѿ� �������������Ϣ
                        if (FALSE == _int_API_RSDriveMsgUnitGet((PanelNoticeMsg *)&t_panelNoticeMsg, 
                                                                (DriveMsgQueue *)&t_PanelDriveMsgSnapshot.m_commQueue, 
                                                                (uint8 *)&drivemsgInstallCodeIndex))
                        {
                            t_PanelDriveMsgSnapshot.m_msg        &= ~INSTALL_MSG_COMM; 
                        }
                        portENABLE_INTERRUPTS(level);
                    }
                    
					//��ȡ������Ƿ�װ�˴�����Ϣ
                    driveMsgInstallResult                       = API_RSPanelInstallKeyChk(drivemsgInstallCodeIndex);
                    //ֻ���ڰ�װ�˰����� ���ܽ������²���
                    if (FALSE != driveMsgInstallResult)     
                    {
                        //�жϴ���ϢID�Ƿ�Ϊ������嶼�й�ͬ��Ӧ��Ϊ(��������干ͬ������Ϣ) ����ʵ����Ϣ�е�EXIT��
                        if (SCHED_INSTALL_MSG == (driveMsgInstallResult & SCHED_INSTALL_MSG))
                        {
                            if (NULL != s_pfPanelCallBackFunc)
							{
								if (TRUE == _API_SchedSendMessage((const PanelNoticeMsg *)&t_panelNoticeMsg))
								{
									//��������Ϣ��������๲ͬ��Ϣ
									st_SchedMsg.m_msg            = SCHED_MSG_COMMON;
									//��ȡ��干ͬ��Ϣ��ŵ�ַ
									st_SchedMsg.m_pMsgAccessAddr = (void *)&t_panelNoticeMsg;
									//���е������ص����� ������Ҳ����Ӧ����
									rt = (*s_PfSchedCBF)((const SchedMsg *)&st_SchedMsg);
								}
							} 
                        }
                        //�����԰�������
                        if (PANEL_INSTALL_MSG == (driveMsgInstallResult & PANEL_INSTALL_MSG))
                        {	
							//���ô����Ļص����� ��������Ϣ ��ȡ�ص�����������
                            if (NULL != s_pfPanelCallBackFunc)
                            {
                                rt                              = _API_SchedSendMessage((const PanelNoticeMsg *)&t_panelNoticeMsg);         
                            }
                        }
                        //���ص�����ִ�гɹ��� �ж��Ƿ����������ʼ����Ϣ
                        if (TRUE == rt)
                        {
							_API_SchedPanelSwitchExec((PanelNoticeMsg *)&t_panelNoticeMsg);
						}
                        //����������Ϣ���������� �˳�ѭ�� 
						else
                        {
                            //��ȡ�������������
                            //�����������������
                            schedMsgHandleCircle               &= ~INSTALL_SCHED_WELL;
                            //��λ����������ʧ��
                            schedMsgHandleCircle               |= INSTALL_SCHED_ERROR;  
                            //ϵͳ�������̬
                            s_SchedulerRunStatus                = SCHEDULER_ERROR;
                            break;                                                        
                        }
                    }
				//ѭ�����������������Ϣ 	
                }while (NONE_MSG_HANDLE != t_PanelDriveMsgSnapshot.m_msg);               
			}

            API_PEMExecute();
			break;
        
        //����̬
        case SCHEDULER_ERROR:
            
            break;
		
		default:
            break;
	}
    
    //----------------------------------------------------------------------------------------------------------------
    //����������ص�������Ϣ      
    //������Դ����
    level                                                       = portDISABLE_INTERRUPTS();        
    pfschedCBF                                                  = s_PfSchedCBF;
    pftestSchedCBF                                              = s_PfTestSchedCBF;
    portENABLE_INTERRUPTS(level);
    if (NULL != pftestSchedCBF)
    {
        pftestSchedCBF();
    }
    if (NULL != pfschedCBF)
    {
        //������������ ��ʵ�ֶԵ����������Ķ����Ϣ��ε���
        do{
            if (schedMsgHandleCircle & INSTALL_SCHED_WELL)
            {
                schedMsgHandleCircle                           &= ~INSTALL_SCHED_WELL;
                st_SchedMsg.m_msg                               = SCHED_MSG_RUN_WELL;
                st_SchedMsg.m_pMsgAccessAddr                    = (void *)&t_panelNoticeMsg; 
            }
            else if (schedMsgHandleCircle & INSTALL_SCHED_ERROR)
            {
                schedMsgHandleCircle                           &= ~INSTALL_SCHED_ERROR;
                st_SchedMsg.m_msg                               = SCHED_MSG_RUN_ERROR;
                st_SchedMsg.m_pMsgAccessAddr                    = (void *)&rt;
            }
            else if (schedMsgHandleCircle & INSTALL_SCHED_INIT)
            {
                schedMsgHandleCircle                           &= ~INSTALL_SCHED_INIT;
                st_SchedMsg.m_msg                               = SCHED_MSG_INIT;
            }
            //���е������ص����� ���͵�������Ϣ
            (*pfschedCBF)((const SchedMsg *)&st_SchedMsg);
            
        }while (((portuBASE_TYPE)0) != schedMsgHandleCircle);
    }
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��װ��������干ͬ��Ϣ �������ϢΪ��ͬ�����������                                                                
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ��������干ͬ��ϢΪ������干�е�������Ϣ �ɵ������Ļص�����������                                                               
 *                                                                            
 *                                                                            
******************************************************************************/
void API_SchedCommPanelMsgIDInstall(const portMSGID_TYPE schedProcPanelMsgID)
{
    s_SchedCommPanelMsgID 					                   |= schedProcPanelMsgID;    
}

void API_SchedCommPanelMsgIDUninstall(const portMSGID_TYPE schedProcPanelMsgID)
{
	s_SchedCommPanelMsgID 					                   &= (~schedProcPanelMsgID);    
}

void API_SchedCommPanelMsgIDClr(void)
{
    s_SchedCommPanelMsgID 					                    = 0;   
}                             

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��װ���ܲ��Իص�����                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : �ڲ��Թ����� ѭ�����ϵ�ִ�д˲��Իص����� ���ϵͳ���Թ���                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

void API_SchedTestSchedCBFInstall(Fp_pfTestSchedCBF *pfTestSchedCBF)
{
	s_PfTestSchedCBF                                            = pfTestSchedCBF;
}

void API_SchedTestSchedCBFUninstall(void)
{
	s_PfTestSchedCBF                                            = NULL;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ��������ʼ��                                                              
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/
void API_SchedExecuteInit(const PanelUnit *ptPanel, 
							const portMSGID_TYPE schedProcPanelMsgID,
							Fp_pfSchedlerDriveMsgSampleCallBackFunc *pfschedlerDriveMsgSampleCallBackFunc, 
							Fp_pfSchedCBF *pfschedCBF, 
							Fp_pfTestSchedCBF *pfTestSchedCBF)
{
	s_ptCurrentPanelUnit           			                    = (PanelUnit *)ptPanel;	//��ǰ���
	s_ptCurrentPanelUnitTmp        			                    = (PanelUnit *)ptPanel;	//��ǰ��屸��
    s_ptPrevPanelUnit              			                    = (PanelUnit *)ptPanel;	//ǰһ���
    s_SchedCommPanelMsgID                                       = schedProcPanelMsgID;	//��������干ͬ��Ϣ
    s_PfTestSchedCBF                                            = pfTestSchedCBF;
	s_pfSchedlerDriveMsgSampleCallBackFunc                      = pfschedlerDriveMsgSampleCallBackFunc;
	s_PfSchedCBF                                                = pfschedCBF;
}

/******************************************************************************
 *  �������� :                                                               
 *                                                                           
 *  �������� : ����������Դ���������������                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                             
 *                                                                        
 *                                                                            
******************************************************************************/
// static portuBASE_TYPE _API_SchedPanelDraw(PanelWindow *ptpanelWindow, portuBASE_TYPE drawInfo)
// {
//     if (NULL != ptpanelWindow)
//     {
//         //
//     }

//     return TRUE;
// }
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
