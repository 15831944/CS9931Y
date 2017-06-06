/******************************************************************************
 * �ļ���Ϣ :  
 *
 * �� �� �� :  ����
 *
 * �������� :  2007.09.29
 * 
 * ԭʼ�汾 : 
 *     
 * �޸İ汾 :  
 *    
 * �޸����� : 
 *
 * �޸����� :
 * 
 * �� �� �� :
 *
 * ��    ע :
 *
 * ��    �� :   Դ����
 *
 * ��    Ȩ :   �Ͼ���ʢ�������޹�˾ , Copyright Reserved
 * 
******************************************************************************/

/******************************************************************************
 *                              ͷ�ļ���ʿ
******************************************************************************/ 
 
#ifndef    _SET_PANEL_H
#define    _SET_PANEL_H

#if defined(__cplusplus)

    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
			
#ifndef    _API_SCHED_H

    #include    "Api_sched.h"     
#endif

		

#ifndef    _SCPI_PARSER_H_

    #include    "scpi_parser.h"     
#endif
			
/******************************************************************************
 *                             �����ļ�����
******************************************************************************/ 

/******************************************************************************
 *                           �ļ��ӿ���Ϣ�궨��
******************************************************************************/
			
#define uint8		uint8_t
extern uint32_t Lock_key_counter;//��һ�ν������ý����������Ӧ����Ӧ����ִ�в���
extern uint32_t test_mode_flag;
extern uint16_t ac_dc_flag;
extern uint32_t test_display_flag;
extern uint32_t Set_Resistor_time_flag;
extern uint32_t time_dc_flag;
extern uint32_t Set_Current_flag;
extern uint32_t Set_Test_mode;
extern uint16_t Set_Mode_mark;
// extern uint32_t time_flag;								//����ACʱ��ʱ�ж��Ƿ���С����ı�־Ϊ0��ʱ����С����Ϊ1��ʱ��û��С����
/******************************************************************************
 *                         �ļ��ӿ����ݽṹ������
******************************************************************************/
// �����ͺź궨�� ���ӻ�����ͺ�ʱҪ�޸� Model_ReadSysModel()�� MEM_InitAllMemParam()����
#define             CS9931YS                 	(0)
           

#define             MAX_MODEL_NUMB              (1)    // �ͺŸ���


typedef struct
{
    uint8_t                       *m_InstrumentModel[MAX_MODEL_NUMB];     // �����ͺ�
    uint8_t                       *m_InstrumentID;                        // ����ID��
    uint8_t                       *m_InstrumentSoftwareVersion;           // ��������汾��

}InstrumentAttrConfig;

// typedef struct _ENV_PARAM_
// {
//     uint8_t                    m_sysModel;      // ����

//     uint8_t                    m_keySound;      // ��������
//     //uint8                    m_keyLock;       // ������

// }EnvParam;

// extern EnvParam t_EnvParam;    

extern InstrumentAttrConfig t_InstrumentAttrConfig;      

extern const PanelUnit  t_SetPanel;

// extern void LIB_Set_Test_Mode(void);

extern void LIB_Set_Time(uint32_t time);

extern void LIB_Set_Current(uint32_t current); //д��8G,9G

extern void LIB_WriteAdd(uint16_t mode, uint32_t voltage);//д��G1

extern void LIB_Set_Auto_Manual(uint32_t flag);

// extern void _Set_current_enter_control(void);//�������ʱ����õĺ���
/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/ 



/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#if defined(__cplusplus)

    }
#endif 

#endif
