#ifndef    _API_SCHED_H
#define    _API_SCHED_H

#if defined(__cplusplus)

    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/******************************************************************************
 *                             �����ļ�����
******************************************************************************/ 



/******************************************************************************
 *                           �ļ��ӿ���Ϣ�궨��
******************************************************************************/ 

#define	portuBASE_TYPE	uint32_t
#define	uint8	uint8_t			
			
#define           NO_SHECD_PROC_PANEL_MSG_ID                0x70000000
typedef		      unsigned long long int					portMSGID_TYPE;

#define           NONE_MSG_HANDLE               	        (0x00)
//��Ϣ�������
#define           MSG_QUEUE_DEPTH                           (5)
typedef           portuBASE_TYPE                            portMSG_TYPE;

#define           MSG_ID_GET(msgIdIndex)                    ((portMSGID_TYPE)1UL << msgIdIndex)

/*******************************************************************************
 *                           �����Ʊ�־�궨��
********************************************************************************/

#define           PANEL_DRAW_FLG_NORMAL                     (1)

/*******************************************************************************
 *                            ��Ϣ��װ���Ͷ���
********************************************************************************/

#define  		  INSTALL_MSG_KEY                           (2)
#define  		  INSTALL_MSG_SCHED                         (4)
#define  		  INSTALL_MSG_COMM                          (8)
#define 		  INSTALL_MSG_PUSHBUTTON					(16)
/*******************************************************************************
 *              ��Ϣ���Ͷ��� --- ÿһ����Ϣ���Ͷ���Ӧ��һ����ϢID
********************************************************************************/

#define           MSG_STATUS_NONE                           (0)
#define           MSG_STATUS_PRESS                          (1)
#define           MSG_STATUS_RELEASE                        (2)

#define  	      MSG_NONE                                  (0)
#define  		  MSG_INITIALIZING                          (MSG_NONE+1)
#define  		  MSG_INIT_OVER                             (MSG_INITIALIZING+1)
#define  		  MSG_CLOSE				                    (MSG_INIT_OVER+1)

#define  		  MSG_PREV_PAGE                             (MSG_CLOSE+1)
#define  		  MSG_NEXT_PAGE                             (MSG_PREV_PAGE+1)
#define  		  MSG_PREV_PAGE_INIT                        (MSG_NEXT_PAGE+1)
#define  		  MSG_NEXT_PAGE_INIT                        (MSG_PREV_PAGE_INIT+1)

#define  		  MSG_KEY                                   (MSG_NEXT_PAGE_INIT+1)
#define  		  MSG_SCHED                                 (MSG_KEY+1)
#define  		  MSG_COMM                                  (MSG_SCHED+1)
//#define  		  MSG_COMM                                  (MSG_PUSHBUTTON+1)

#define			  MSG_PUSHBUTTON							(MSG_COMM+1)
/*******************************************************************************
 *                            ϵͳԤ������ϢID�궨��
********************************************************************************/

#define  		  INVALID_DRIVE_MSG_ID_INDEX                (62)
#define  		  NONE_DRIVE_MSG_ID_INDEX                   (63)
#define  		  MSG_KEY_ID_INDEX                          (0) 
#define  		  MSG_SCHED_ID_INDEX                        (MSG_KEY_ID_INDEX+1) 
#define  		  MSG_COMM_ID_INDEX                         (MSG_SCHED_ID_INDEX+1)

//��Ŀ��������Զ�����Ϣ�� MSG_USER_ID_INDEX ֮�����
#define           MSG_USER_ID_INDEX                         (MSG_COMM_ID_INDEX+1)


#define  		  NONE_DRIVE_MSG_ID                         MSG_ID_GET(NONE_DRIVE_MSG_ID_INDEX) 
#define  		  MSG_KEY_ID                                MSG_ID_GET(MSG_KEY_ID_INDEX) 
#define  		  MSG_SCHED_ID                              MSG_ID_GET(MSG_SCHED_ID_INDEX) 
#define  		  MSG_COMM_ID                               MSG_ID_GET(MSG_COMM_ID_INDEX)
#define           MSG_USER_ID                               MSG_ID_GET(MSG_USER_ID_INDEX) 


#define           NONE_DRIVE_MSG                            (0xff)

/*******************************************************************************
 *                              ��������Ϣ�궨�� 
********************************************************************************/

#define  		  INSTALL_SCHED_INIT                        (0x01)
#define  		  INSTALL_SCHED_WELL                        (0x02)
#define  		  INSTALL_SCHED_ERROR                       (0x04)
#define  		  INSTALL_SCHED_PANEL                       (0x08)

#define  		  SCHED_MSG_RUN_WELL                        (0x01)
#define  		  SCHED_MSG_RUN_ERROR                       (0x02)
#define  		  SCHED_MSG_COMMON                          (0x03)                  //����๲ͬ��Ϣ����
#define  		  SCHED_MSG_COMMU                           (0x04)                  //ͨѶ��Ϣ����
#define  		  SCHED_MSG_INIT                            (0x05)                  //��ʼ����Ϣ

/*******************************************************************************
 *                              ������״̬�궨�� 
********************************************************************************/

#define  		  SCHEDULER_INIT                            (1)
#define  		  SCHEDULER_RUNNING                         (2)
#define  		  SCHEDULER_ERROR                           (3)

/******************************************************************************
 *                         �ļ��ӿ����ݽṹ������
******************************************************************************/ 

/******************************************************************************
 *                           ������Ϣ�ṹ������
******************************************************************************/ 

typedef struct
{
    uint8                                                       m_installCodeIndex;
    uint8                                                       m_value;
    //��Ϣ״̬ ���� �ͷ�
    uint8                                                       m_status;
                                                        
}DriveMsgUnit;

typedef struct
{
    DriveMsgUnit                                                m_queue[MSG_QUEUE_DEPTH];
    uint8                                                       m_writeIndex;
    uint8                                                       m_readIndex;

}DriveMsgQueue;


typedef struct _PANEL_DRIVE_MSG_
{
    portMSG_TYPE                                                m_msg;                                            		
    DriveMsgQueue                                               m_keyQueue;
    DriveMsgQueue                                               m_schedQueue;
    DriveMsgQueue                                               m_commQueue;

	DriveMsgQueue												m_pushbuttonQueue;

}PanelDriveMsg;

extern volatile PanelDriveMsg t_PanelDriveMsg;


typedef struct _PANEL_NOTICE_MSG_
{
	portMSG_TYPE                                                m_msg;                                            	
    
	uint8                                                       m_value;                          		                      
    uint8                                                       m_status;                                               
    
}PanelNoticeMsg;

typedef struct _SCHED_MSG_
{
    portMSG_TYPE                                                m_msg; 
    //��������Ϣ��ȡ��ַָ��                  
    void                                                       *m_pMsgAccessAddr;                                         		

}SchedMsg;

/******************************************************************************
 *                           �ص�����ָ������
******************************************************************************/ 

typedef portuBASE_TYPE  (Fp_pfPanelCallBackFunc)(const PanelNoticeMsg *ptpanelNoticeMsg);    //���ص�����ָ��
typedef portuBASE_TYPE  (Fp_pfTestSchedCBF)(void); 							                 //���Իص�����ָ��
typedef portuBASE_TYPE  (Fp_pfSchedlerDriveMsgSampleCallBackFunc)(void); 					 //������������Ϣ�ɼ��ص�����ָ��
typedef portuBASE_TYPE  (Fp_pfSchedCBF)(const SchedMsg *ptschedMsg);                         //�������ص�����ָ��

/******************************************************************************
 *                           ��嵥Ԫ�ṹ������
******************************************************************************/ 

typedef struct _PANEL_UNIT_
{
    uint8              				   				            m_panelID;                   //���ID�� 
    Fp_pfPanelCallBackFunc 		 	  			               *m_pfPanelCallBackFunc;       //���ص�����ָ�뿽��ֵ
	portMSGID_TYPE                       			            m_panelInstallServMsgID;     //��尲װ���������Ϳ���ֵ
    struct _PANEL_WINDOW_                                      *m_pwindow;                   //��崰��ָ��    

}PanelUnit;

typedef struct _PANEL_WINDOW_
{
    void                                                       *m_void;

}PanelWindow;

/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/ 

extern portuBASE_TYPE API_SchedDriveMsgAdd(portuBASE_TYPE msgHandleStyle, portuBASE_TYPE msgInstallCodeIndex, portuBASE_TYPE msg,
                                           portuBASE_TYPE msgStatus);

extern void API_SchedExecute(void);

extern void API_SchedPanelMsgIDInstall(portMSGID_TYPE panelMsgID);

extern void API_SchedPanelMsgIDUninstall(portMSGID_TYPE panelMsgID);

extern void API_SchedPanelMsgIDClr(void);

extern void API_SchedExecuteInit(const PanelUnit *ptPanel,                
									const portMSGID_TYPE schedProcPanelMsgID,
									Fp_pfSchedlerDriveMsgSampleCallBackFunc *pfschedlerDriveMsgSampleCallBackFunc, 
									Fp_pfSchedCBF *pfschedCallBackFunc, 
									Fp_pfTestSchedCBF *pfTestSchedCBF);

extern void API_SchedCommPanelMsgIDInstall(const portMSGID_TYPE schedProcPanelMsgID);

extern void API_SchedCommPanelMsgIDUninstall(const portMSGID_TYPE schedProcPanelMsgID);

extern void API_SchedCommPanelMsgIDClr(void);

extern void API_SchedTestSchedCBFInstall(Fp_pfTestSchedCBF *pfTestSchedCBF);

extern void API_SchedTestSchedCBFUninstall(void);

extern void API_ShcedPanelDisplay(const PanelUnit *ptpanel);


/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#if defined(__cplusplus)

    }
#endif 

#endif
