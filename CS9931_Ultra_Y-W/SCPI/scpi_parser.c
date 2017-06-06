/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
 

#include "scpi_parser.h"
#include "ScpiInstructionTable.h"
#include "macro.h"
#include "App_config.h"
#include "stdlib.h"

#include "stm32f4xx.h"


/******************************************************************************
 *                            ���ļ��ڲ��궨��
******************************************************************************/
#define                         _FRAME_CHK_SUPPORT                     (1)

typedef                         unsigned char                          portHASHCODE_TYPE;
#define                         HASH_INIT_CODE                         (13)  //ȡ�����ȽϺ�
#define                         PARSER_START_POS                       (0)

#define                         PARAM_CONTAINER_NORMAL                 (0)
#define                         PARAM_CONTAINER_PTR_ARRAY              (1)

#define                         PARAM_SPECIAL_ATTR_MIN                 (0x01)
#define                         PARAM_SPECIAL_ATTR_MAX                 (0x02)
#define                         PARAM_SPECIAL_ATTR_DEF                 (0x04)
#define                         PARAM_SPECIAL_ATTR_INF                 (0x08)

//д��һ���ַ�
#define                         PARAM_QUEUE_CHAR_WRITE(value)          (*(uint8_t *)(&t_ParserExeModuler.m_pparamQueue[t_ParserExeModuler.m_paramPutPosIndex++]) = value)
#define                         PARAM_QUEUE_CURR_POS_GET()             (&t_ParserExeModuler.m_pparamQueue[t_ParserExeModuler.m_paramPutPosIndex])
#define                         PARAM_QUEUE_CURR_POS_ADJ(value)        (t_ParserExeModuler.m_paramPutPosIndex += (value))
#define                         PARAM_QUEUE_SPLIT_SIGN_WRITE()         (PARAM_QUEUE_CHAR_WRITE('\0'))
                            
//��λ������
#define                         SIGN_SYMBOL_CLASS(sign)                (sign << 3)
#define                         SIGN_SYMBOL_CLASS_GET(sign)            (sign >> 3)
#define                         SIGN_SYMBOL_INDEX_GET(sign)            (sign & 0x07)

#define                         REAL_ATTR_SPECIAL                      (0x01)
#define                         REAL_ATTR_UINT32                       (0x02)

//����֮�������� ��:V -> kV  �������Ϊ3
#define                         SIGN_INTERVAL_BIT                      (3)

//�������ѹ������ -> �ַ���
#define                         _OUTPUT_QUEUE_PUSH_STR                 (0)
//�������ѹ������ -> ����
#define                         _OUTPUT_QUEUE_PUSH_INT                 (1)
//�������ѹ������ -> �ַ�
#define                         _OUTPUT_QUEUE_PUSH_CHAR                (2)
                                
/******************************************************************************
 *                       ���ļ��������ȫ�ֽṹ��
******************************************************************************/  

//������״̬
typedef enum
{
    STATUS_PARSERING = 0,
    STATUS_EXECUTING,
    STATUS_IDLE,                                                          

}ParserStatus;

//����ִ�����
typedef struct 
{
    //---------------------------------------------------------------------------------------------------------------
    //��������
    const ParserContainer                      *m_currParserContainerPtr;               //��������ǰ����ṹ��ָ��
    const ParserContainer                      *m_rootParserContainerPtr;               //�����ṹ�����ַ 
    FP_pfInstructionExec                       *m_executeFunc;                          //ָ��ִ�к��� 
    //ָ�������ַָ��
//    const uint8_t                                *m_pparamStrAddrArray[INSTRUCTION_MAX_PARAM+2]; //�����ַ�����ַָ������
    const uint8_t                               **m_ppparamStrAddrArray;                  //ָ������ַ�����ַָ�������ָ��
    const uint8_t                                *m_pparamQueue;                          //ָ��������е�ָ��

    //---------------------------------------------------------------------------------------------------------------
    
    uint16_t                                      m_instructionExecuteMaxTime;            //ָ��ִ���ʱ��
    portQUEUELEN_TYPE                           m_parserPosIndex;                       //����λ������ ��¼��ǰ�Ľ���λ��   
    portQUEUELEN_TYPE                           m_paramPutPosIndex;                     //��������λ������
    //---------------------------------------------------------------------------------------------------------------

    uint8_t                                       m_parserStatus;                         //������״̬  ������ ִ���� ����
    ParserErrCode                               m_parserErrCode;                        //������������ 
    uint8_t                                       m_endCodeIndex;                         //����������
    uint8_t                                       m_responsionCtrl;                       //Ӧ����ƿ���
    uint8_t                                       m_instructionAttr;                      //ָ������ ִ�С���ѯ������ָ��
    uint8_t                                       m_instructionExecuteTimeOut;            //ָ��ִ�г�ʱ
    uint8_t                                       m_totalInstructionChains;               //���������� ��������
    uint8_t                                       m_singleInstructionScales;              //��������� һ�������ɼ�������
    uint8_t                                       m_singleInstructionParamCnts;           //���������������
    uint8_t                                       m_rootParserStructCnts;                 //�������ṹ�����
    uint8_t                                       m_currParserStructCnts;                 //��ǰ�����ṹ�����
    uint8_t                                       m_longLenPerScale[INSTRUCTION_MAX_SCALE];  //ÿ��ָ��ĳ�ָ���
    uint8_t                                       m_shortLenPerScale[INSTRUCTION_MAX_SCALE]; //ÿ��ָ��Ķ�ָ���

    //---------------------------------------------------------------------------------------------------------------
    
    //����������в���
    portQUEUELEN_TYPE   			            m_inputQueueLen;                        //������г���
    portQUEUELEN_TYPE   			            m_outputQueueLen;                       //������г���
     int8_t    	            				    m_inputQueue[QUEUE_MAX_LEN];            //�������
     int8_t    					                m_outputQueue[QUEUE_MAX_LEN];           //�������

}ParserExeModuler;

ParserExeModuler                t_ParserExeModuler;                 
//������
static uint8_t                    s_EndCode                               = 0;
//Ԥִ�кϷ����жϺ���ָ��
static FP_pfValidityChk        *s_FP_pfPrevValidityChk                  = NULL;
//��������������ָ��
static FP_pfOutputQueueSend    *s_FP_pfOutputQueueSend                  = NULL;
//ʵ�Ͳ�������Զ��庯��ָ��
static FP_pfRealParamSplit     *s_FP_pfRealParamSplit                   = NULL;
/*
const uint8_t *const API_PEM_pParserExeModulerErrCodeArray[] = {

        (const uint8_t *)"+0",                          //"+0, No error"
		(const uint8_t *)"-102",                        //"-102, Syntax error"
		(const uint8_t *)"-105",                        //"-105, Execute not allowed"
		(const uint8_t *)"-108",                        //"-108, Parameter not allowed"
		(const uint8_t *)"-109",                        //"-109, Missing parameter"
		(const uint8_t *)"-113",                        //"-113, Undefined header"
		(const uint8_t *)"-120",                        //"-120, Parameter type error"
		(const uint8_t *)"-121",                        //"-121, Parameter length error"
		(const uint8_t *)"-151",                        //"-151, Invalid string data"
		(const uint8_t *)"-152",                        //"-152, Execute time out"
		(const uint8_t *)"-222",                        //"-222, Data out of range"
        (const uint8_t *)"-252",                        //"-252, Output Queue Full"

    };
*/
const uint8_t *const API_PEM_pParserExeModulerErrCodeArray[] = {

        (const uint8_t *)"+0, No error",
				(const uint8_t *)"-102, Syntax error",
				(const uint8_t *)"-105, Execute not allowed",
				(const uint8_t *)"-108, Parameter not allowed",
				(const uint8_t *)"-109, Missing parameter",
				(const uint8_t *)"-113, Undefined header",
				(const uint8_t *)"-120, Parameter type error",
				(const uint8_t *)"-121, Parameter length error",
				(const uint8_t *)"-151, Invalid string data",
				(const uint8_t *)"-152, Execute time out",
				(const uint8_t *)"-222, Data out of range",
        (const uint8_t *)"-252, Output Queue Full",
        (const uint8_t *)"-262, Sum Check Error",

    };
// const uint8_t *const API_PEM_pParserExeModulerErrCodeArray[] = {

//         (const uint8_t *)"1,00",
// 		(const uint8_t *)"0,Syntax error",
// 		(const uint8_t *)"0,Execute not allowed",
// 		(const uint8_t *)"0,Parameter not allowed",
// 		(const uint8_t *)"0,Missing parameter",
// 		(const uint8_t *)"0,Undefined header",
// 		(const uint8_t *)"0,Parameter type error",
// 		(const uint8_t *)"0,Parameter length error",
// 		(const uint8_t *)"0,Invalid string data",
// 		(const uint8_t *)"0,Execute time out",
// 		(const uint8_t *)"0,Data out of range",
//         (const uint8_t *)"0,Output Queue Full",
//         (const uint8_t *)"0,Sum Check Error",

//     };

typedef struct
{
    uint8_t                                       m_lowerLimitBitLen;
    uint8_t                                       m_upperLimitBitLen;
    //������Ϣ   ��4λΪ�ַ�����ȥС����֮��ĳ���  ��4λΪС�������ַ����ڵ�ƫ��
    uint8_t                                       m_configBitLen;
    uint8_t                                       m_dotBitLen;
    //������Ϣ                                  
    uint8_t                                       m_signBitLen;

}ParamRealSectorInfoStruct;

typedef struct                                
{
    uint8_t                                       *m_psignStr;
    //������� ����ڷ���ϵ����ͷ��ŵ����� ��kv �� v ������Ϊ1  ʵ��ֵΪ1*1000
    uint8_t                                        m_relativeIndex;

}SignSymbolInfo;

static const SignSymbolInfo st_SignSymbolTable[] = {
    
        //nA uA mA A ����ϵ��
        {"nA", SIGN_SYMBOL_CLASS(0)|0}, {"uA", SIGN_SYMBOL_CLASS(0)|1}, {"mA", SIGN_SYMBOL_CLASS(0)|2}, {"A", SIGN_SYMBOL_CLASS(0)|3},
        //V kV ����ϵ��
        {"V", SIGN_SYMBOL_CLASS(1)|0}, {"kV", SIGN_SYMBOL_CLASS(1)|1},
        //mohm ohm kohm Mohm Gohm Tohm ����ϵ��
        {"mohm", SIGN_SYMBOL_CLASS(2)|0}, {"ohm", SIGN_SYMBOL_CLASS(2)|1}, {"kohm", SIGN_SYMBOL_CLASS(2)|2}, 
        {"Mohm", SIGN_SYMBOL_CLASS(2)|3}, {"Gohm", SIGN_SYMBOL_CLASS(2)|4}, {"Tohm", SIGN_SYMBOL_CLASS(2)|5},
        //s ����ϵ��
        {"ms", SIGN_SYMBOL_CLASS(3)|0}, {"s", SIGN_SYMBOL_CLASS(3)|1},
        //Hz ����ϵ��
        {"Hz", SIGN_SYMBOL_CLASS(4)|0},
        //W ����ϵ��
        {"W", SIGN_SYMBOL_CLASS(5)|0},
    };

/******************************************************************************
 *                           ���ļ���̬��������
******************************************************************************/ 

static uint32_t _API_PEMPrevExecuteChk(void);
static uint32_t _API_PEMSyntaxAnalyze(void);
static uint32_t _API_PEMInstructionAnalyze(void);
static uint32_t _API_PEMParameterAnalyze(ParamContainer *pparamContainer, uint8_t paramContainerCapacity, 
                                              uint8_t recvParamCnts, uint32_t paramContainerInfo);
 
static portHASHCODE_TYPE _API_PEMStrHashCodeGet(const uint8_t *prscStr, uint32_t len);
static  ParamTypeInfo _API_PEMParamTyleClassGet(int8_t *paccessAddr, uint32_t paramLen, 
                                                ParamContainer *pparamContainer,
                                                uint32_t *pparamSpecialAttr);
static  uint32_t _API_PEMIntegerParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr);
static  uint32_t _API_PEMCharacterParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr);
static  uint32_t _API_PEMStringParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr);
static  uint32_t _API_PEMRealParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr);
static  uint32_t _API_PEMFloatParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr);
static  void _API_PEMParamQueueStrWrite(const uint8_t *prscStrAddr, uint8_t rscStrLen);
static uint32_t _API_PEMNullValidityChk(struct EXECUTE_VALIDITY_CHK_INFO *pvalidityChkInfo);
static uint32_t _API_PEMParserErrChk(void);
static uint32_t _RealParamSplitServ(RealParamSplitStruct *prealParamSplitStruct);

#if (_FRAME_CHK_SUPPORT > 0)

    static uint8_t _API_PEMFrameChkSumGet(const uint8_t *prscQueue, portQUEUELEN_TYPE len);

#endif    

                  
/******************************************************************************
 *                        ���ļ���̬��������ָ������
******************************************************************************/ 

static uint32_t (*pf_API_PEMParamValidChk[])(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr) = {

        _API_PEMIntegerParamValidChk, _API_PEMCharacterParamValidChk, _API_PEMStringParamValidChk, 
        _API_PEMRealParamValidChk, _API_PEMFloatParamValidChk,
    };

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

void API_PEMInit(ParserContainer *ptrootParserContainer, uint8_t prootParserContainerCapacity,
                 uint8_t endCodeIndex, uint8_t responsionCtrl, FP_pfOutputQueueSend *pfOutputQueueSend)
{
    //��ȡ������������Ϣ
    t_ParserExeModuler.m_rootParserContainerPtr             = ptrootParserContainer;
    t_ParserExeModuler.m_rootParserStructCnts               = prootParserContainerCapacity;
    //��ʼ��������״̬
    t_ParserExeModuler.m_parserStatus                       = STATUS_IDLE;
    t_ParserExeModuler.m_parserErrCode                      = STATUS_NO_ERROR;
    //��ʼ��������ַ����
    t_ParserExeModuler.m_parserPosIndex                     = PARSER_START_POS;
    t_ParserExeModuler.m_currParserContainerPtr             = t_ParserExeModuler.m_rootParserContainerPtr;
    t_ParserExeModuler.m_currParserStructCnts               = t_ParserExeModuler.m_rootParserStructCnts;
    //��ʼ��������
    t_ParserExeModuler.m_endCodeIndex                       = endCodeIndex; 
    //��ʼ��Ԥִ�кϷ��Լ�⺯��ָ��
    s_FP_pfPrevValidityChk                                  = _API_PEMNullValidityChk;
    //��ʼ��������з��ͺ���ָ��
    s_FP_pfOutputQueueSend                                  = pfOutputQueueSend;
    //��ʼ��ʵ�Ͳ�������Զ��庯��ָ��Ϊ��
    //s_FP_pfRealParamSplit                                   = NULL;
    API_PEMRealParamSplitFuncInstall(_RealParamSplitServ);//
//     if ((CR_LF_INDEX == endCodeIndex) || (LF_INDEX == endCodeIndex))
//     {
//         //������ΪLF--����
//         s_EndCode                                           = '\n';
//     }
//     else
//     {
        //������Ϊ#
        s_EndCode                                           = '#';
//     }
    //��ʼ��Ӧ�����
    t_ParserExeModuler.m_responsionCtrl                     = responsionCtrl;
    //����������
    _API_PEMInputQueueFlush();
    //����������
    _API_PEMOutputQueueFlush();
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
int8_t              paramQueue[QUEUE_MAX_LEN];

void API_PEMExecute(void)
{    
    const uint8_t       *pparamStrAddrArray[INSTRUCTION_MAX_PARAM+2];     //�����ַ�����ַָ������
    //int8              paramQueue[QUEUE_MAX_LEN];
// 	s_FP_pfOutputQueueSend((const uint8_t *)t_ParserExeModuler.m_inputQueue, t_ParserExeModuler.m_inputQueueLen);
    //�ж��Ƿ���Ҫִ��
    if (STATUS_PARSERING == t_ParserExeModuler.m_parserStatus)
    {
        ExecuteValidityChkInfo  t_validityChkInfo;

        t_ParserExeModuler.m_ppparamStrAddrArray                = pparamStrAddrArray;
        t_ParserExeModuler.m_pparamQueue                        = (const uint8_t *)paramQueue;
        
        //---------------------------------------------������--------------------------------------------------

        #if (_FRAME_CHK_SUPPORT > 0)
        {
            //����֡У����Ƿ���ȷ ���յ���У����Ƿ���ڼ������У��� ������Ϊ#ʱ û��У���
            //t_ParserExeModuler.m_inputQueueLen����Ϊ�����������ĳ���
            if (CODE_0x23_INDEX != t_ParserExeModuler.m_endCodeIndex)
            {
                if (((uint8_t)t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen-1])
                     != _API_PEMFrameChkSumGet((const uint8_t *)t_ParserExeModuler.m_inputQueue, t_ParserExeModuler.m_inputQueueLen-1))
                {
                    API_PEMExecuteErrCodeSet(STATUS_SUM_CHECK_ERROR);
                    _API_PEMParserErrChk();
                    t_validityChkInfo.m_step                    = VALIDITY_CHK_STEP_RREV_EXECUTE;
                    goto _quit;
                }
                else
                {
                    //�������յ������ݳ��� ��Ϊ�����������ǲ���ҪУ������ݵ�
                    t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen - 1] = ';';
                    t_ParserExeModuler.m_inputQueueLen--;
                }
            }
        }
        #endif

        do {
            //Ԥִ���ж� -> �﷨���� -> ָ����� -> ��������   ָ������� ����ָ��ַ�����������ַ���
            if ((TRUE == _API_PEMPrevExecuteChk()) && (TRUE == _API_PEMSyntaxAnalyze()) && (TRUE == _API_PEMInstructionAnalyze()))
            {
        //---------------------------------------------ִ����--------------------------------------------------
                //����������ִ��״̬
                t_ParserExeModuler.m_parserStatus               = STATUS_EXECUTING;
                //�ж�ִ�к����Ƿ�Ϊ�� ����Ϊ����ִ�д˺���
                if (NULL != t_ParserExeModuler.m_executeFunc)
                {
                    t_validityChkInfo.m_pfinstructionExec       = t_ParserExeModuler.m_executeFunc;
                    t_validityChkInfo.m_instructionAttr         = t_ParserExeModuler.m_instructionAttr; 
                    t_validityChkInfo.m_step                    = VALIDITY_CHK_STEP_RREV_EXECUTE;
                    
                    //��Ԥִ�кϷ��Լ�⺯������ֵ��ΪTRUEʱ ��ִֹ��
                    if (TRUE != s_FP_pfPrevValidityChk(&t_validityChkInfo))
                    {
                        _API_PEMParserErrChk();
                        break;
                    }         
                    //���ڻ״̬ �ҿ��Ʒ�ʽΪԶ��ʱ ����ָ���Ӧ
                    //t_ParserExeModuler.m_singleInstructionParamCnts+2�� ��Ϊ����������ִ�к���������������Ϣ
                    if (TRUE == (*t_ParserExeModuler.m_executeFunc)(t_ParserExeModuler.m_singleInstructionParamCnts+2, 
                            t_ParserExeModuler.m_ppparamStrAddrArray))
                    {
                        //ִ��ָ�� ִ�гɹ�ʱ
                        if ((t_ParserExeModuler.m_instructionAttr & INSTRUCTION_ATTR_EXECUT)
                            && (STATUS_NO_ERROR == t_ParserExeModuler.m_parserErrCode))
                        {
                            //ѹ��ִ�гɹ���Ϣ
                            API_PEMOutputQueueErrMsgPush(t_ParserExeModuler.m_parserErrCode, NO_APPENDED_SIGN);
                        }
                        //���ڶ༶����ָ�� ֻ����ִ�гɹ�ʱ ����ӷָ��  �� ָ�����Ϊִ��ָ��Ͳ�ѯָ��
                        if (t_ParserExeModuler.m_totalInstructionChains > 1)
                        {
                            API_PEMOutputQueueCharPush(';', NO_APPENDED_SIGN);
                        }
                    }
                    //����ִ���з�������
                    else 
                    {
                        //�жϴ�����:t_ParserExeModuler.m_parserErrCode ���� 
                        //��t_ParserExeModuler.m_parserErrCode ��Ϊ STATUS_NO_ERRORʱ ����ѹ���������
                        _API_PEMParserErrChk();
                        break;
                    }
                }                        
            }
            else
            {
                //�м䷢������ ֹͣ��������ָ�� �˳�����ִ�в���
                t_validityChkInfo.m_pfinstructionExec       = NULL;
                t_validityChkInfo.m_instructionAttr         = INSTRUCTION_ATTR_NONE;
                break;
            }

        }while (--t_ParserExeModuler.m_totalInstructionChains > 0);

        //--------------------------------------------�������-------------------------------------------------
_quit:  t_validityChkInfo.m_step                                = VALIDITY_CHK_STEP_RREV_OUTPUT;
        //��Ϊ�㲥ͨѶ״̬���������׼����->ͨ��������г������ж�  �� ����״̬Ϊ��Ϊ�ǻ״̬
        if ((0 != t_ParserExeModuler.m_outputQueueLen) && (TRUE == s_FP_pfPrevValidityChk(&t_validityChkInfo)))
        {
            //��Ϊִ��ָ�� �������Ӧ����ƹر�ʱ ������в������������
            //���������ִ����������������
            if (!((t_ParserExeModuler.m_instructionAttr & INSTRUCTION_ATTR_EXECUT)
                && (CLOSE == t_ParserExeModuler.m_responsionCtrl)))
            {
                #if (_FRAME_CHK_SUPPORT > 0)
                {
                     if (CODE_0x23_INDEX != t_ParserExeModuler.m_endCodeIndex)
                     {
                        //����֡У���
                        t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen] =
                            _API_PEMFrameChkSumGet((const uint8_t *)t_ParserExeModuler.m_outputQueue, t_ParserExeModuler.m_outputQueueLen);
                        t_ParserExeModuler.m_outputQueueLen++;
                     }
                }
                #endif

                //��ӽ����� ��λ��������Ϣ�̶���CR+LFΪ������
 								t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen++] = '#';
//                 t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen++] = '\r';
//                 t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen++] = '\n';
                //ִ��������з��ͺ���
                if (NULL != s_FP_pfOutputQueueSend)
                {
                    s_FP_pfOutputQueueSend((const uint8_t *)t_ParserExeModuler.m_outputQueue, t_ParserExeModuler.m_outputQueueLen);
                }
            }
        }
        //--------------------------------------------��ʼ��PEM-------------------------------------------------
        //��ս��ն���
        _API_PEMInputQueueFlush();
        //��շ��Ͷ���
        _API_PEMOutputQueueFlush();
        //��ʼ��������ַ����
        t_ParserExeModuler.m_parserPosIndex                     = PARSER_START_POS;
        //��ʼ���������ܼ���
        t_ParserExeModuler.m_totalInstructionChains             = 0;
        t_ParserExeModuler.m_parserErrCode                      = STATUS_NO_ERROR;
        //�������ָ�����״̬
        t_ParserExeModuler.m_parserStatus                       = STATUS_IDLE;
    }
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

static uint32_t _API_PEMParserErrChk(void)
{
    if (STATUS_NO_ERROR != t_ParserExeModuler.m_parserErrCode)
    {
        //��������Ϣѹ�����������
        API_PEMOutputQueueErrMsgPush(t_ParserExeModuler.m_parserErrCode, NO_APPENDED_SIGN);
        return FALSE;    
    }
    return TRUE;
}

static uint32_t _API_PEMPrevExecuteChk(void)
{
    //��ʼ��ִ�к�����ִ������
    t_ParserExeModuler.m_executeFunc                    = NULL;
    t_ParserExeModuler.m_instructionAttr                = INSTRUCTION_ATTR_NONE;
    t_ParserExeModuler.m_paramPutPosIndex               = 0;
    return _API_PEMParserErrChk();
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �Խ��յ����ַ������з���  ֻ����һ�����                                                                
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
 *  ��    ע : �ڱ������ڲ����Ĵ��� ȫ��Ϊ�﷨����STATUS_SYNTAX_ERROR                                                              
 *             �﷨���������˫���ŷ�Χ�ڵ��ַ������з���                                                               
 *                                                                            
******************************************************************************/

static uint32_t _API_PEMSyntaxAnalyze(void)
{
     int8_t                   recvMsg                             = 0;
    portQUEUELEN_TYPE       parserStartPosIndex                 = PARSER_START_POS + t_ParserExeModuler.m_parserPosIndex;
    portQUEUELEN_TYPE       i                                   = parserStartPosIndex;
    //�ո���ų���λ��
    portQUEUELEN_TYPE       spaceSignAppearPos                  = 0;
    //�Ǻŷ��ų��ֵĴ���
    portQUEUELEN_TYPE       starSignAppearPos                   = 0;
    //�ڶ���˫���Ŵ� ���������һ���Ƕ��� �����Ҫ�����ж��� ��������¼���Ǵ˿̵�λ��
    portQUEUELEN_TYPE       quoteSignValidChkPos                = 0;
    //�ո���ֵĴ���
    uint32_t           spaceSignAppearCnts                = 0;
    //С������ֵĴ���
    uint32_t           dotAppearCnts                      = 0;
    //���ų��ֵĴ���
    uint32_t           quoteSignAppearCnts                 = 0;
    //�ڶ���˫���Ŵ� ���������һ���Ƕ��� �����Ҫ�����ж���
    uint32_t           quoteSignFormateValid               = TRUE;
    //Сд�ַ��Ƿ����
    uint32_t           lowerCaseCharAppear                 = FALSE;
    //�ʺų��ֵĴ���
    uint32_t           interrogationSignAppearCnts         = 0;
    uint32_t           longLenPerScale                     = 0;
    uint32_t           shortLenPerScale                    = 0;
    uint32_t           currParserContainerCapacity         = t_ParserExeModuler.m_currParserStructCnts;
    const ParserContainer   *currParserContainerPtr              = t_ParserExeModuler.m_currParserContainerPtr;
    
    //��ʼ����־λ
    t_ParserExeModuler.m_instructionAttr                        = INSTRUCTION_ATTR_EXECUT;
    t_ParserExeModuler.m_instructionExecuteTimeOut              = FALSE;
    t_ParserExeModuler.m_singleInstructionScales                = 0;
    t_ParserExeModuler.m_singleInstructionParamCnts             = 0;
    memset((void *)t_ParserExeModuler.m_longLenPerScale, '\0', sizeof(t_ParserExeModuler.m_longLenPerScale));
    memset((void *)t_ParserExeModuler.m_shortLenPerScale, '\0', sizeof(t_ParserExeModuler.m_shortLenPerScale));

    do{
        //��ȡһ�������ַ�
        recvMsg                                             = t_ParserExeModuler.m_inputQueue[i];
        //�Ϸ��ַ� ������ֻ�������ַ� ������A - Z   a - z    0 - 9
        if ((' ' == recvMsg) || (':' == recvMsg) || (',' == recvMsg) || ('"' == recvMsg)  
            || ('*' == recvMsg) || ('?' == recvMsg) || (';' == recvMsg))
        {
            //�Ǻ��ַ����
            if ('*' == recvMsg)
            {
                //��Ϊ��һ�����ֵ��ַ�ʱ
                if (parserStartPosIndex == i) 
                { 
                    //�Ӹ�·����ʼ����  ���ݴ���currParserStructPtr�� �ȵ��﷨��ȷ���ٸ���ֵs 
                    currParserContainerPtr                       = t_ParserExeModuler.m_rootParserContainerPtr;
                    currParserContainerCapacity                  = t_ParserExeModuler.m_rootParserStructCnts;
                    starSignAppearPos++;
                    //��λ��������
                    t_ParserExeModuler.m_instructionAttr      |= INSTRUCTION_ATTR_COMM;
                    //������ַ��������һ
                    t_ParserExeModuler.m_parserPosIndex++;
                }
                //��ʱ�Ǻ��Ѿ����ֳ���һ���� ˫����û�г���ʱ->�﷨����
                else if (0 == quoteSignAppearCnts)
                {
                    break;
                }
            }
            //ð���ַ����  
            else if (':' == recvMsg)
            {
                if (parserStartPosIndex == i) 
                { 
                    //�Ӹ�·����ʼ����  ���ݴ���currParserStructPtr�� �ȵ��﷨��ȷ���ٸ���ֵs 
                    currParserContainerPtr                        = t_ParserExeModuler.m_rootParserContainerPtr;
                    currParserContainerCapacity                   = t_ParserExeModuler.m_rootParserStructCnts;
                    //������ַ��������һ
                    t_ParserExeModuler.m_parserPosIndex++;
                }
                //�ж��Ǻ��Ƿ���ֹ���˫�����Ƿ���ֹ�
                else if (0 == quoteSignAppearCnts)  
                {
                    if (starSignAppearPos > 0)
                    {
                        break;      
                    }
                    //����������������ж�
                    else
                    {
                        //��¼����ָ��εĳ�����ָ���  �˴������ǰ����  ���������һ��
                        t_ParserExeModuler.m_longLenPerScale[t_ParserExeModuler.m_singleInstructionScales]  = longLenPerScale;
                        t_ParserExeModuler.m_shortLenPerScale[t_ParserExeModuler.m_singleInstructionScales] = shortLenPerScale; 
                        t_ParserExeModuler.m_singleInstructionScales++;
                        //����ǰ��ð���滻�ɽ�����'\0'
                        t_ParserExeModuler.m_inputQueue[i]       = '\0'; 
                        //��һ��ָ��ν��� ��ʼ��Сд�ַ����ֹ���ʶ��    
                        lowerCaseCharAppear                      = FALSE;
                        longLenPerScale                          = 0;
                        shortLenPerScale                         = 0;
                    }
                }
            }
            //�ո��ַ���� �ո���һ�������ֻ�ܳ���һ��  ���ھ���˫���Ų����п��Գ��ֶ�� 
            //�ո��Ҳ�ɳ�����ʵ�����ͺ͸���������     
            else if (' ' == recvMsg)
            {
                //����˫���ŷ�Χ��
                if (0 == (quoteSignAppearCnts % 2))
                {
                    spaceSignAppearCnts++;
                }
                //��¼�¿ո��һ�γ���ʱ��λ��
                if (1 == spaceSignAppearCnts)
                {
                    //�жϿո����ʱ ��ǰ����̳����Ƿ�Ϊ�� ��Ϊ�� �﷨���� ������ð�ź���ֱ�Ӹ��ո�Ĵ���
                    if (0 == longLenPerScale)
                    {
                        break;
                    }
                    spaceSignAppearPos                          = i;
                    //����ǰ�Ŀո��滻�ɽ�����'\0'
                    t_ParserExeModuler.m_inputQueue[i]          = '\0';
                    //�жϿո��ַ�����һ���ַ��Ƿ������ ; ���������﷨����
                    if (';' == t_ParserExeModuler.m_inputQueue[i+1])
                    {
                        break;
                    }
                    else
                    {
                        //�ڴ˼�������в��� ����������һ
                        t_ParserExeModuler.m_singleInstructionParamCnts++;
                    }
                }
            }
            //˫�����ַ���� ˫�����ַ�����ɶԳ���
            else if ('"' == recvMsg)
            {
                quoteSignAppearCnts++; 
                if (0 == (quoteSignAppearCnts % 2))
                {
                    //�����ж�β����ַ��Ƿ�Ϊ���� �ȼ���Ϊ���Ϸ�
                    quoteSignFormateValid                       = FALSE;
                    //��¼��ǰ��λ��
                    quoteSignValidChkPos                        = i;
                }  
            }
            //�����ַ���� ÿ����һ��
            else if (',' == recvMsg)
            {
                //���ű����ڿո�֮�����  �ո�Ͷ���֮���λ�ü���������1
                if ((0 == spaceSignAppearCnts) || ((i - spaceSignAppearPos) <= 1))
                {
                    break;
                }
                //���ڶ���˫���ź�β����Ƕ����ַ� ������quoteSignFormateValid = TRUE
                if (FALSE == quoteSignFormateValid)
                {
                    quoteSignFormateValid                       = TRUE;
                }
                t_ParserExeModuler.m_singleInstructionParamCnts++;
                //��˫���ų��ִ���Ϊ�ɶԳ��ֺ� �������滻Ϊ������'\0'
                if (0 == (quoteSignAppearCnts % 2))
                {
                    //����ǰ��ð���滻�ɽ�����'\0'
                    t_ParserExeModuler.m_inputQueue[i]          = '\0';
                }
            }
            //�ʺ��ַ����
            else if ('?' == recvMsg)
            {
                //����˫���ŷ�Χ��
                if (0 == (quoteSignAppearCnts % 2))
                {
                    interrogationSignAppearCnts++;
                }
                //˫����û�г��� ����ȴ�����������ʺŵ������ -> �﷨����
                if (1 == interrogationSignAppearCnts)
                {
                    //ȡ��ִ������    ����һ�����յ���ָ������ ��ѯ���Ժ�ִ�������ǻ����
                    t_ParserExeModuler.m_instructionAttr &= (~INSTRUCTION_ATTR_EXECUT);
                    //��λ��ѯ����
                    t_ParserExeModuler.m_instructionAttr |= INSTRUCTION_ATTR_QUERY;
                }
                else
                {
                    break;
                }
            }
        }
        //�ж��ַ��Ƿ���A - Z��Χ�� 
        else if ((recvMsg >= 'A') && (recvMsg <= 'Z'))
        {
            //��ָ������׶� ����Сд�ַ���������˴�д�ַ� ���﷨����
            if (0 == spaceSignAppearCnts)
            {
                if (TRUE == lowerCaseCharAppear)
                {
                    break;
                }
                //��ָ��ȼ�һ
                shortLenPerScale++;
                //��ָ��ȼ�һ
                longLenPerScale++;
            }
        }
        //�ж��ַ��Ƿ���a - z    0 - 9 + - . ��Χ�� 
        else if (((recvMsg >= 'a') && (recvMsg <= 'z'))
            || ((recvMsg >= '0') && (recvMsg <= '9')) || ('+' == recvMsg) || ('-' == recvMsg) || ('.' == recvMsg))
        {
            //�ո��δ����ʱ ����ָ���
            if (spaceSignAppearCnts == 0)
            {
                //��ָ��ȼ�һ
                longLenPerScale++;
                lowerCaseCharAppear                                = TRUE;
            }

            //˫��������С�����������һ �﷨����
            if ('.' == recvMsg)
            {
                if (0 == (quoteSignAppearCnts % 2))
                {
                    dotAppearCnts++;
                }

                if (dotAppearCnts > 1)
                {
                    break;
                }
            }
        }
        else
        {
            //�Ƿ��ַ�
            break;
        }

        //�Եڶ���˫���ź���β����ַ��Ƿ�Ϊ���Ž����ж� �Ҳ�Ϊ';'ʱ
        //�ڶ����ֺź������ֱ��Ϊ';'
        if ((FALSE == quoteSignFormateValid) && ((i - quoteSignValidChkPos) == 1)
            && (recvMsg != ';'))
        {
            //û�б�β��Ķ�����λΪ�Ϸ� -> �﷨����
            break;    
        }
        //������һ���ַ�
        i++;

    //; �ֺ���Ϊָ����ָ��֮��ķָ���
    }while (recvMsg != ';');

    //�жϽ��������Ƿ���е����һ���ַ� ��û��������м�һ���������﷨����
    if (recvMsg != ';')
    {
        t_ParserExeModuler.m_parserErrCode                      = STATUS_SYNTAX_ERROR;
    }    
    else
    {
        //��˫���ų��ֵ���������ж� ˫�����ַ�����ɶԳ���
        //�жϽ�����֮ǰ���ַ��Ƿ�Ϊ���� ��Ϊ���� ���϶�Ϊ�﷨����  �����Ѿ�������Ľ��������滻Ϊ'\0'
        if ((0 != (quoteSignAppearCnts % 2)) || ('\0' == t_ParserExeModuler.m_inputQueue[i-2])
            || (0 == longLenPerScale))
        {
            t_ParserExeModuler.m_parserErrCode                  = STATUS_SYNTAX_ERROR;
        }
        else
        {
            //��������';'�滻Ϊ'\0'
            t_ParserExeModuler.m_inputQueue[i-1]                = '\0';
            //��¼����ָ��εĳ�����ָ���  �˴���������һ��
            t_ParserExeModuler.m_longLenPerScale[t_ParserExeModuler.m_singleInstructionScales]  = longLenPerScale;
            t_ParserExeModuler.m_shortLenPerScale[t_ParserExeModuler.m_singleInstructionScales] = shortLenPerScale;
        }
    }
    //��������Ϣ
    if (TRUE != _API_PEMParserErrChk())
    {
        return FALSE;
    }   
    //��ʱ���µ�ǰ�����ṹ��ָ��ֵ
    t_ParserExeModuler.m_currParserContainerPtr                    = currParserContainerPtr;
    //��ʱ���µ�ǰ�����ṹ������
    t_ParserExeModuler.m_currParserStructCnts                      = currParserContainerCapacity;
    //��ǰָ����Լ�һ
    t_ParserExeModuler.m_singleInstructionScales++;

    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ָ����� ���Ҷ�Ӧ��ָ���Ƿ���� ��������������ݽ��з���                                                              
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

static uint32_t _API_PEMInstructionAnalyze(void)
{
    HeadScaleParserStruct           *ptprevScaleParserStruct;
    TailScaleParserStruct           *ptlastScaleParserStruct;
    const ParserContainer           *ptparserContainer;
    const ParamTotalContainer       *ptparamTotalContainer;
    uint8_t                           *currParserAddr;
    const uint8_t                     *prscStrAddr;
    void                            *pvoid;
    uint32_t                   segmentLongHashCode, segmentShortHashCode, segmentLongLen, segmentShortLen, i;
    uint32_t                   longInstructionHashCode, shortInstructionHashCode, instructionAttribute;
    uint32_t                   rscStrCmpTrig, parserStructType, instructionMatchChkTrig = TRUE;
    uint32_t                   singleInstructionScales     = 0;
    uint32_t                   paramContainerInfo          = PARAM_CONTAINER_NORMAL;
    uint32_t                   paramTotalContainerAttr;
    //ʹ��uint8_t���� �����������������������������Ϊ256
    uint8_t                            nextParserContainerCapacity, totalParamCnts, recvParamCnts, optionalParamCnts;

    do {                                        
        //��ǰ��������ָ��
        ptparserContainer                                       = t_ParserExeModuler.m_currParserContainerPtr;
        //���µ�ǰ������ַ
        currParserAddr                                          = (uint8_t *)&t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_parserPosIndex];
        //��ȡָ����г�ָ���
        segmentLongLen                                          = t_ParserExeModuler.m_longLenPerScale[singleInstructionScales];
        //��ȡָ����ж�ָ���
        segmentShortLen                                         = t_ParserExeModuler.m_shortLenPerScale[singleInstructionScales];
        //��ȡָ���HASH�� ��ָ��ȵ��ڶ�ָ���ʱ ��ʱ���յ���ָ��Ϊ��ָ�� ��ȡ��ָ����
        if (segmentLongLen == segmentShortLen)
        {
            //��ȡָ����ж�ָ��HASH��
            segmentShortHashCode                                = _API_PEMStrHashCodeGet(currParserAddr, segmentShortLen);
        }
        else
        {
            //��ȡָ����г�ָ��HASH��
            segmentLongHashCode                                 = _API_PEMStrHashCodeGet(currParserAddr, segmentLongLen);
        }
        
        //������������ ����ָ��ptparserContainer������
        for (i = 0; i < t_ParserExeModuler.m_currParserStructCnts; i++, ptparserContainer++)
        {
            //����Դ�ַ����Ƚϴ�����־λ
            rscStrCmpTrig                                       = FALSE;
            instructionMatchChkTrig                             = TRUE;
            parserStructType                                    = (ptparserContainer->m_parserStructType);
            //ͷ�������ṹ�� �� �м伶�����ṹ��
            if ((PARSER_HEAD_SCALE == parserStructType) || (PARSER_MIDDLE_SCALE == parserStructType))
            {
                ptprevScaleParserStruct                         = (HeadScaleParserStruct *)(ptparserContainer->m_pparserStructAddr);
                //�˴�û�в���ֱ�ӵ�ַָ��ת���ķ�������Ϊ��ͬ��ƽ̨�䳤��Ҳ��һ��
                prscStrAddr                                     = ptprevScaleParserStruct->m_pinstructionStr;
                pvoid                                           = (void *)ptprevScaleParserStruct->m_pnextParserContainer;
                longInstructionHashCode                         = ptprevScaleParserStruct->m_longInstructionHashCode;
                shortInstructionHashCode                        = ptprevScaleParserStruct->m_shortInstructionHashCOde;
                instructionAttribute                            = ptprevScaleParserStruct->m_instructionAttribute;
                //�˴�Ϊ��һ��������������
                nextParserContainerCapacity                     = ptprevScaleParserStruct->m_nextParserContainerCapacity;
            }
            //ĩ�������ṹ�� �� ���������ṹ�� �� ���ǵ������ʽ �����������е�����ָ���Ƿ�ָ��ָ������
            else
            {
                ptlastScaleParserStruct                         = (TailScaleParserStruct *)(ptparserContainer->m_pparserStructAddr);
                prscStrAddr                                     = ptlastScaleParserStruct->m_pinstructionStr;
                pvoid                                           = (void *)ptlastScaleParserStruct->m_executeFunc;
                longInstructionHashCode                         = ptlastScaleParserStruct->m_longInstructionHashCode;
                shortInstructionHashCode                        = ptlastScaleParserStruct->m_shortInstructionHashCOde;
                instructionAttribute                            = ptlastScaleParserStruct->m_instructionAttribute;
            }
            //�жϱ���ָ���Ƿ��ָ��  ��ָ���³�ָ���==��ָ���
            if (segmentLongLen == segmentShortLen)
            {
                //�Ƚ϶�ָ��HASH��
                if (shortInstructionHashCode == segmentShortHashCode)
                {
                    rscStrCmpTrig                               = TRUE;
                }
            }
            //��ָ�� �Ƚϳ�ָ��HASH��
            else if (longInstructionHashCode == segmentLongHashCode)
            {
                rscStrCmpTrig                                   = TRUE;
            }
            //�ж�Դ�ַ����Ƚ��Ƿ�ɹ�
            if ((TRUE == rscStrCmpTrig) && (0 == strncmp((void *)currParserAddr, (void *)prscStrAddr, segmentLongLen)))
            {
                //�ж��Ƿ�Ϊǰ��ָ���  ����ͷ�����м伶
                if ((PARSER_HEAD_SCALE == parserStructType) || (PARSER_MIDDLE_SCALE == parserStructType))
                {
                    //ƥ��ɹ� ����ָ����һ��ƥ��
                    t_ParserExeModuler.m_parserPosIndex        += segmentLongLen+1;
                    //�ƶ�����һ������
                    t_ParserExeModuler.m_currParserContainerPtr = (ParserContainer *)pvoid;
                    //������������
                    t_ParserExeModuler.m_currParserStructCnts   = nextParserContainerCapacity;
                    //�Ѿ�ƥ��ɹ��Ͳ���Ҫ����ָ���Ƿ�ƥ��ɹ����ж��� ����жϷ����������� ��Ϊ������
                    //t_ParserExeModuler.m_currParserStructCnts��i�Ƚϵ� ������ǰ�߱�����Ϊ�¼�ָ��ṹ�������
                    //��ʵ����Ҫ�Ƚϵ��Ǹ���ǰ��ֵ
                    instructionMatchChkTrig                     = FALSE;
                }
                //ĩ��ָ��� �� ����ָ��� 
                //�ж�ָ����֧�ֵļ����������յ��ļ����Ƿ���ͬ ��������ж�--ָ������������
                //else if (INSTRUCTION_SCALES_GET(instructionAttribute) != t_ParserExeModuler.m_singleInstructionScales)
                //{
                    //δ�����ͷ  ��δ�����ָ��
                //    t_ParserExeModuler.m_parserErrCode          = STATUS_UNDEFINED_HEADER;
                //}
                else if (!((t_ParserExeModuler.m_instructionAttr & instructionAttribute) & ~INSTRUCTION_ATTR_COMM))
                {
                    //δ�����ͷ  ��δ�����ָ��
                    t_ParserExeModuler.m_parserErrCode          = STATUS_UNDEFINED_HEADER;
                }
                else if (singleInstructionScales != (t_ParserExeModuler.m_singleInstructionScales - 1))
                {
                    //δ�����ͷ  ��δ�����ָ��
                    t_ParserExeModuler.m_parserErrCode          = STATUS_UNDEFINED_HEADER;
                }
                //ƥ��ɹ� -> ���������������
                else
                {
                    //��ȡ����������
                    ptparamTotalContainer                       = ptlastScaleParserStruct->m_pparamTotalContainer;
                    //�ܲ�������ָ�벻Ϊ��ʱ
                    if (NULL != ptparamTotalContainer)
                    {
                        //��ȡ�ܲ�����������
                        paramTotalContainerAttr                 = ptparamTotalContainer->m_containerAttr;
                        //֧��ִ�кͲ�ѯ��������
                        if (CONTAINER_ATTR_DOUBLE == (CONTAINER_ATTR_DOUBLE & paramTotalContainerAttr))
                        {
                            //���յ���ѯָ��
                            if (INSTRUCTION_ATTR_QUERY & t_ParserExeModuler.m_instructionAttr)
                            {
                                if (!(CONTAINER_ATTR_QUERY & paramTotalContainerAttr))
                                {
                                    //��������ָ�����
                                    ptparamTotalContainer++;
                                }
                            }
                            //���յ�ִ��ָ��
                            else
                            {
                                if (!(CONTAINER_ATTR_EXECUTE & paramTotalContainerAttr))
                                {
                                    //��������ָ�����
                                    ptparamTotalContainer++;
                                }
                            }
                        }
                        //ֻ֧��ִ�л��ѯ��������
                        else
                        {
                            //���յ���ѯָ��
                            if (INSTRUCTION_ATTR_QUERY & t_ParserExeModuler.m_instructionAttr)
                            {
                                if (!(CONTAINER_ATTR_QUERY & paramTotalContainerAttr))
                                {
                                    //δ�ҵ��ܲ������� ��˸�ֵΪ��
                                    ptparamTotalContainer       = NULL;
                                }
                            }
                            //���յ�ִ��ָ��
                            else
                            {
                                if (!(CONTAINER_ATTR_EXECUTE & paramTotalContainerAttr))
                                {
                                    //δ�ҵ��ܲ������� ��˸�ֵΪ��
                                    ptparamTotalContainer       = NULL;
                                }
                            }
                        }
                    }
                    //�����յ��Ĳ������� ʹ��REGISTER ʹִ���ٶȱ��
                    recvParamCnts                               = t_ParserExeModuler.m_singleInstructionParamCnts;
                    //�жϱ�ָ���Ƿ���в�����Ϣ
                    if (NULL == ptparamTotalContainer)
                    {
                        //�����в�����Ϣ
                        totalParamCnts                          = 0;
                        optionalParamCnts                       = 0;
                    }
                    //��ָ����в�����Ϣ
                    else 
                    {
                        //��ȡ�ܲ��������Ϳ�ѡ������Ϣ����ֵ
                        totalParamCnts                          = TOTAL_PARAM_INFO_TOT_CNTS_GET(ptparamTotalContainer->m_paramInfo);
                        optionalParamCnts                       = TOTAL_PARAM_INFO_OPT_CNTS_GET(ptparamTotalContainer->m_paramInfo);
                    }                
                    //�ж�ָ����֧�ֵĲ����������յ��Ĳ��������Ƿ���ͬ
                    if (totalParamCnts != recvParamCnts)
                    {
                        //ָ����֧�ֲ������������յ��Ĳ������� �� ���յ��Ĳ������ڵ���(totalParamCnts - optionalParamCnts)ʱ
                        //��ָ�����Я���Ĳ������� Ҳ��Ϊ��ȷ
                        //����֮�� �϶�ָ��������ȴ���
                        if (!((totalParamCnts > recvParamCnts) 
                            && (recvParamCnts >= (totalParamCnts - optionalParamCnts))))
                        {
                            //����֧�ֵĲ�������Ϊ�� ����Ϊ�ǲ������������
                            if (0 == totalParamCnts)
                            {
                                t_ParserExeModuler.m_parserErrCode  = STATUS_PARAM_NOT_ALLOWED;
                            }
                            //ָ��������ȴ��� ����������
                            else
                            {
                                t_ParserExeModuler.m_parserErrCode  = STATUS_PARAM_LENGTH_ERROR;
                            }
                            break;
                        }
                    }
                    //�ж��Ƿ�Ϊ��������
                    if (INSTRUCTION_ATTR_COMM & t_ParserExeModuler.m_instructionAttr)
                    {
                        //�жϱ�ָ���Ƿ�Ϊ��������
                        if (!(instructionAttribute & INSTRUCTION_ATTR_COMM))
                        {
                            //δ�����ͷ  ��δ�����ָ��
                            t_ParserExeModuler.m_parserErrCode      = STATUS_UNDEFINED_HEADER;
                            break;
                        }
                    }
                    //��ѯָ���β������Ҫ�����ʺŵĳ���
                    if (INSTRUCTION_ATTR_QUERY & t_ParserExeModuler.m_instructionAttr)
                    {
                        segmentLongLen++;
                    }                 
                    //��ʼ��������ŵ�ַ����
                    t_ParserExeModuler.m_paramPutPosIndex       = 0;
                    //ƥ��ɹ� ����ָ���������  ָ�������֮��ͨ��һ���ո��ַ��ָ�
                    t_ParserExeModuler.m_parserPosIndex        += segmentLongLen + 1; 
                    //��һ�������¼ִ�к�����ַ��Ϣ
                    t_ParserExeModuler.m_ppparamStrAddrArray[0] = (const uint8_t *)pvoid;
                    //�ڶ��������¼ָ������:��ѯ���� or ִ������
                    t_ParserExeModuler.m_ppparamStrAddrArray[1] = (const uint8_t *)(t_ParserExeModuler.m_instructionAttr&(INSTRUCTION_ATTR_QUERY|INSTRUCTION_ATTR_EXECUT));
                    //��¼ִ�к�����ַ
                    t_ParserExeModuler.m_executeFunc            = (FP_pfInstructionExec *)pvoid;
                }
                break;
            }
        }
        //�ж�ָ���Ƿ�ƥ��ɹ�  �����ߵ�ֵ���ʱ ����ƥ��ʧ��
        if ((TRUE == instructionMatchChkTrig) && (t_ParserExeModuler.m_currParserStructCnts == i))
        {
            //δ�����ͷ  ��δ�����ָ��
            t_ParserExeModuler.m_parserErrCode                  = STATUS_UNDEFINED_HEADER;
            break;
        }          
                              
    }while (++singleInstructionScales < t_ParserExeModuler.m_singleInstructionScales);

    //���Ϊ����������֮һ �����û�н��յ�����ָ�� ����Ҳ������
    if ((PARSER_HEAD_SCALE == parserStructType) || (PARSER_MIDDLE_SCALE == parserStructType))
    {
        t_ParserExeModuler.m_parserErrCode                      = STATUS_UNDEFINED_HEADER;
    }
    //�ж�ָ������Ƿ�������
    if (t_ParserExeModuler.m_parserErrCode != STATUS_NO_ERROR)
    {
        //��������Ϣѹ�����������
        API_PEMOutputQueueErrMsgPush(t_ParserExeModuler.m_parserErrCode, NO_APPENDED_SIGN);
        return FALSE;
    }
    //���в������� ������������Ϊ��ʱ���������Ϊ���ҿ�ѡ����������ʱ 
    else if ((0 != recvParamCnts) || (optionalParamCnts > 0))
    {
        if ((PARSER_SINGLE_SCALE_PCPA == parserStructType) 
            || (PARSER_TAIL_SCALE_PCPA == parserStructType))
        {
            paramContainerInfo                                  = PARAM_CONTAINER_PTR_ARRAY;
        }
        return (_API_PEMParameterAnalyze((ptparamTotalContainer->m_pparamContainer), totalParamCnts,
                                         recvParamCnts, paramContainerInfo));    
    } 
    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� : ���������׵�ַ                                                               
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
 *  ��    ע : ����������ʼ��ַ:                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

static uint32_t _API_PEMParameterAnalyze(ParamContainer *pparamContainer, uint8_t paramContainerCapacity, 
                                              uint8_t recvParamCnts, uint32_t paramContainerInfo)
{
    uint32_t       i                                       = 0;
    //�̶���������Ϊ2�ĵ�ַ�����  ������0����ִ�к���ָ��ֵ  ������1����ִ�в�ѯ����
    uint32_t       paramPutPosIndex                        = 2;
    uint32_t       paramLen, paramSpecialAttr;
    int8_t               *paccessAddr;
    uint32_t       paramTypeClass;
    ParamContainer    **ppparamContainer;
    
    //����������ָ��ָ�����ָ���������� ���pparamContainerֵ��������
    if (PARAM_CONTAINER_PTR_ARRAY == paramContainerInfo)
    {
        ppparamContainer                                        = (ParamContainer **)pparamContainer;
        pparamContainer                                         = *ppparamContainer++;
    }

    for (; i < paramContainerCapacity; i++, paramPutPosIndex++)
    {
        //�������յ��������н���
        if (i < recvParamCnts)
        {
            paccessAddr                                         = &t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_parserPosIndex];
            paramLen                                            = strlen((void *)paccessAddr);
            //��ȡ��������
            paramTypeClass                                      = _API_PEMParamTyleClassGet(paccessAddr, paramLen, pparamContainer, &paramSpecialAttr);
            //���������޷�ʶ�� ���ش���
            if (PARAM_TYPE_NONE == paramTypeClass)
            {
                break;
            }
            //������ַ�����ƶ�����һ��λ�ô� ��1����Ϊ���������֮���зָ���','
            t_ParserExeModuler.m_parserPosIndex                    += paramLen + 1;
        }
        //�Կ�ʡ�Բ������д��� �����յ���ʡ�Բ��� �����Ĳ���ȫ��ʡ�� ��AA:BB [param_1, [param_2, [param_3]]]
        else
        {
            //���ʵ�ַ����Ϊ��
            paccessAddr                                         = NULL;
            //�������ȹ̶���ֵΪ��
            paramLen                                            = 0;
            //������������
            paramSpecialAttr                                    = PARAM_SPECIAL_ATTR_DEF;
            //��ȡ��������
            paramTypeClass                                      = PARAM_TYPE_CLASS_GET(pparamContainer->m_paramStructType);
        }
        //��¼������ŵ�ַ  t_ParserExeModuler.m_paramPutPosIndex��ֵ���������ڸ����Ͳ����Ϸ����жϺ�����
        t_ParserExeModuler.m_ppparamStrAddrArray[paramPutPosIndex] = (const uint8_t *)PARAM_QUEUE_CURR_POS_GET();
        
        //�����Ϸ��Լ��  ���ɲ�������ת��Ϊ��������������
        paramTypeClass                                        >>= 4;
        if (FALSE == (*pf_API_PEMParamValidChk[paramTypeClass])((uint8_t *)paccessAddr, pparamContainer, paramSpecialAttr))
        {
            break;
        }
        //��������ָ������ƶ�
        if (PARAM_CONTAINER_NORMAL == paramContainerInfo)
        {
            pparamContainer++;
        }
        else
        {
            pparamContainer                                     = *ppparamContainer++;
        }
    }
    //��������Ϣ
    if (TRUE != _API_PEMParserErrChk())
    {
        return FALSE;
    }
    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �������յ��Ĳ�������                                                              
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
 *  ��    ע : ������ֻ�Բ������ͽ��з��� ���������������ж�                                                           
 *                                                                            
 *                                                                            
******************************************************************************/

static  ParamTypeInfo _API_PEMParamTyleClassGet(int8_t *paccessAddr, uint32_t paramLen, 
                                                ParamContainer *pparamContainer,
                                                uint32_t *pparamSpecialAttr)
{
    uint32_t       i                                       = 0;
    //�����ṹ������
    ParamTypeInfo       paramStructTypeClass                    = PARAM_TYPE_CLASS_GET(pparamContainer->m_paramStructType);
    //���ų��ֵĴ���
    uint32_t       quoteSignAppearCnts                     = 0;
    //С������ֵĴ���
    uint32_t       dotSignAppearCnts                       = 0;
    //��ǰ���ڷ������ַ�
    uint32_t       analyseChar                             = 0;
    //�������ų��ִ���
    //uint32_t       signAppearCnts                          = 0;    
    //���ֳ��ִ���
    uint32_t       numbAppearCnts                          = 0; 
    //�ַ����ִ���
    uint32_t       charAppearCnts                          = 0; 
    //�ո�����ִ���
    uint32_t       spaceAppearCnts                         = 0; 
    //�ַ�e���ִ���
    uint32_t       charAppearCnts_e                        = 0; 
    //�ַ�E���ִ���
    uint32_t       charAppearCnts_E                        = 0;
    //�ո������λ�� �ַ�������ʼλ�� ���֡�С���������ֹλ��
    uint32_t       spaceStartAppearPos = 0, charStartAppearPos = 0, numbDotEndAppearPos = 0;
    //���ų���λ��
    //uint32_t       signStartAppearPos                      = 0;
    ParamTypeInfo        paramTypeClass                          = PARAM_TYPE_NONE, paramTypeClassTmp;
    
    //for (; i < paramLen; i++, paccessAddr++)
    for (; i < paramLen; i++)
    {
        analyseChar                                             = (*(paccessAddr + i));
        //�����ַ�
        if ('"' == analyseChar)
        {
            quoteSignAppearCnts++;
        }
        //С����                        
        else if ('.' == analyseChar)
        {
            dotSignAppearCnts++; 
            numbDotEndAppearPos                                 = i;
        }
        /* SCPI����Է��Ų�����֧��
        //������                     
        else if (('+' == analyseChar) || ('-' == analyseChar))
        {
            signAppearCnts++;
            numbDotEndAppearPos                                 = i;
            //���ų���λ��
            signStartAppearPos                                  = i;
        }
        */
        //����
        else if ((analyseChar >= '0') && (analyseChar <= '9'))
        {
            numbAppearCnts++;
            numbDotEndAppearPos                                 = i;
        }
        //�ո��
        else if (' ' == analyseChar)
        {
            spaceAppearCnts++;
            //��¼��ʼλ��
            if (1 == spaceAppearCnts)
            {
                spaceStartAppearPos                             = i;
            }
        }
        //������� ��Ϊ���ַ�
        else
        {
            if ('e' == analyseChar)
            {
                charAppearCnts_e++;
            }
            else if ('E' == analyseChar)
            {
                charAppearCnts_E++;
            }
            charAppearCnts++;
            //��¼��ʼλ��
            if (1 == charAppearCnts)
            {
                charStartAppearPos                              = i;
            }
        }
    }
    //��˫�����ַ����� ��Ϊ������  �������� �涨˫�����ַ��ڴ˶β����� ֻ�ܳ�������
    if (quoteSignAppearCnts > 0)
    {
        if (2 == quoteSignAppearCnts)
        {
            paramTypeClass                                      = PARAM_TYPE_STRING;
        }
    }
    //������ ʵ�����϶������Ƿ����С�����ո�����϶��� Ŀǰ��֧�ָ�����
    //1 kV  Ҳ�ɱ�ʶ��Ϊʵ��
    else if ((dotSignAppearCnts > 0) || (spaceAppearCnts > 0))
    {
        //�ж�С����ĸ����Ƿ�Ϊ1 �� Сд�ַ�e���ִ��������� �� ��дE���ִ�������1 ���������ֳ���
        if ((charAppearCnts_e > 0) || (charAppearCnts_E > 1) || (0 == numbAppearCnts) || (spaceAppearCnts > 1))
        {
            //��ʽ����
        }
        //�������� E �� e �����Ϊ������
        else if ((1 == charAppearCnts_E) || (1 == charAppearCnts_e))
        {
            //Ŀǰ��֧�ָ���������
            //paramTypeClass                                    = PARAM_TYPE_FLOAT;
        }
        //����Ϊʵ��
        else
        {
            //��С���� ���� �ո�� �ַ���λ�ý����ж� 
            if (((charStartAppearPos > spaceStartAppearPos) && (numbDotEndAppearPos < spaceStartAppearPos)
            //��ʽ -> 0.123 kV  1 kV
                    && ((1 == spaceAppearCnts) && (charAppearCnts > 0)))
            //��ʽ -> 0.123 
                || ((spaceAppearCnts == 0) && (charAppearCnts == 0) && (1 == dotSignAppearCnts)))
            {
                paramTypeClass                                  = PARAM_TYPE_REAL;
            }
            //��֧�ֵ�����: 0x123kV   0.123���пո�
        }
        /*
        //�ж�С����ĸ����Ƿ�Ϊ1 �� Сд�ַ�e���ִ��������� �� ��дE���ִ�������1 ���������ֳ��� С����������
        if ((charAppearCnts_e > 0) || (charAppearCnts_E > 1) || (0 == numbAppearCnts) || (spaceAppearCnts > 1)
            || (1 != dotSignAppearCnts))
        {
            //��ʽ����
        }
        //�������� E  �����Ϊ������
        else if (1 == charAppearCnts_E)
        {
            //Ŀǰ��֧�ָ���������
            //paramTypeClass                                    = PARAM_TYPE_FLOAT;
        }
        //����Ϊʵ��
        else
        {
            //��С���� ���� �ո�� �ַ���λ�ý����ж�  ��ʽ����̫���� ��Ȼ�����ӽ�������ĸ��ӳ̶�
            //�з������ ��ʽ -> 0.123 kV  
            if (1 == spaceAppearCnts)
            {
                if ((charStartAppearPos > spaceStartAppearPos) && (numbDotEndAppearPos < spaceStartAppearPos)
                    && (charAppearCnts > 0))
                {
                    paramTypeClass                              = PARAM_TYPE_REAL;
                }
            }
            //�޷������ ��ʽ -> 0.123 
            else if ((charAppearCnts == 0))
            {
                paramTypeClass                                  = PARAM_TYPE_REAL;
            }
            //��֧�ֵ�����: 0x123kV   0.123���пո� 1 kV
        }
        */
    }
    //�ַ������ݡ��������Ͳ��ܰ���С���㡢�ո��    ��ΪӲ�Թ涨
    //�ַ��͵��϶��ǿ����յ����ַ������Ƿ�Ϊ���������ж��� ������涨 �ַ������Ͳ����������� +��-
    //�ַ������ݱ�������ַ� 
    else if (charAppearCnts > 0)
    {
        paramTypeClass                                          = PARAM_TYPE_CHARACTER;
    }
    //��������  ����PARAM_TYPE_INTEGER���� ���Ծ������������������
    else
    {
        paramTypeClass                                          = PARAM_TYPE_INTEGER;
    }

    //��ʼ��������������
    (*pparamSpecialAttr)                                        = 0;
    //�жϷ��������Ĳ��������Ƿ������������ָ���Ĳ���������ƥ��  �˴����жϲ��������Ƿ�һ��
    if (paramStructTypeClass != paramTypeClass)
    {
        paramTypeClassTmp                                       = paramTypeClass;
        paramTypeClass                                          = PARAM_TYPE_NONE;
        //�ٴ��ж��²��������Ƿ�Ϊ�ַ��� �� ��������������Ϊ���͡�ʵ�͡�������
        if ((PARAM_TYPE_CHARACTER == paramTypeClassTmp) && ((PARAM_TYPE_INTEGER == paramStructTypeClass) 
            || (PARAM_TYPE_REAL == paramStructTypeClass) || (PARAM_TYPE_FLOAT == paramStructTypeClass)))
        {                                                           
            //�жϽ��յ����ַ����Ƿ�Ϊ�����������Ե��ַ���
            //Ŀǰ��������ֻ֧�� ���ֵ ��Сֵ Ĭ��ֵ ����  ����һ������Ѿ��㹻��
            if ((0 == strcmp((void *)"MINimum", (void *)paccessAddr)) || (0 == strcmp((void *)"MIN", (void *)paccessAddr)))
            {
                (*pparamSpecialAttr)                           |= PARAM_SPECIAL_ATTR_MIN;
            }
            else if ((0 == strcmp((void *)"MAXimum", (void *)paccessAddr)) || (0 == strcmp((void *)"MAX", (void *)paccessAddr)))
            {
                (*pparamSpecialAttr)                           |= PARAM_SPECIAL_ATTR_MAX;
            }
            else if ((0 == strcmp((void *)"DEFault", (void *)paccessAddr)) || (0 == strcmp((void *)"DEF", (void *)paccessAddr)))
            {
                (*pparamSpecialAttr)                           |= PARAM_SPECIAL_ATTR_DEF;
            }
            //���¶���������� ��ָ����������
            paramTypeClass                                      = paramStructTypeClass;
        }
        //������������Ϊ���� ����������������Ϊ�ַ���
        else if ((PARAM_TYPE_INTEGER == paramTypeClassTmp) && (PARAM_TYPE_CHARACTER == paramStructTypeClass))
        {
            //���¶���������� ��ָ����������
            paramTypeClass                                      = paramStructTypeClass;
        }
        //������������Ϊ���� ����������������Ϊʵ����
        else if ((PARAM_TYPE_INTEGER == paramTypeClassTmp) && (PARAM_TYPE_REAL == paramStructTypeClass))
        {
            //���¶���������� ��ָ����������
            paramTypeClass                                      = paramStructTypeClass;
        }
    }
    
    //�ж�����������Ϸ��Ĳ�������
    if (PARAM_TYPE_NONE == paramTypeClass)
    {
        t_ParserExeModuler.m_parserErrCode                      = STATUS_PARAM_TYPE_ERROR;
    }
    return paramTypeClass;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ���β����Ϸ����ж�                                                            
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

uint32_t _API_PEMParamDefSpecialChk(uint32_t paramSpecialAttr)
{
    //�жϲ����������ɺ�����������->Ĭ��ֵ
    if (paramSpecialAttr & PARAM_SPECIAL_ATTR_DEF)
    {
        //�����������д��"DEF"
        _API_PEMParamQueueStrWrite((const uint8_t *)"DEF", strlen((void *)"DEF"));
        return TRUE;
    }
    return FALSE;
}

static  uint32_t _API_PEMIntegerParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr)
{
    uint32_t              upperLimit, lowerLimit;
    uint32_t       paramStructType;
    void               *pparamStructAddr;
    int32_t               recvValue;
    uint8_t               paramStr[11] = {0};

    //�жϲ����������ɺ�����������->Ĭ��ֵ
    if (TRUE == _API_PEMParamDefSpecialChk(paramSpecialAttr))
    {
        return TRUE;
    }
    paramStructType                                             = pparamContainer->m_paramStructType;
    pparamStructAddr                                            = pparamContainer->m_pparamStructAddr;
    switch (paramStructType)
    {
        case PARAM_TYPE_UINT8:
        case PARAM_TYPE_INT8:
                
            upperLimit                                          = ((ParamUint8Struct *)pparamStructAddr)->m_upperLimit;
            lowerLimit                                          = ((ParamUint8Struct *)pparamStructAddr)->m_lowerLimit;
            break;

        case PARAM_TYPE_UINT16:
        case PARAM_TYPE_INT16:
            
            upperLimit                                          = ((ParamUint16Struct *)pparamStructAddr)->m_upperLimit;
            lowerLimit                                          = ((ParamUint16Struct *)pparamStructAddr)->m_lowerLimit;
            break;

        case PARAM_TYPE_UINT32:
        case PARAM_TYPE_INT32:
            
            upperLimit                                          = ((ParamUint32Struct *)pparamStructAddr)->m_upperLimit;
            lowerLimit                                          = ((ParamUint32Struct *)pparamStructAddr)->m_lowerLimit;
            break;

        default:
            break;
    }
    //�жϲ����������ɺ�����������->���ֵ
    if (paramSpecialAttr & PARAM_SPECIAL_ATTR_MAX)
    {
        recvValue                                               = upperLimit;    
    }
    //�жϲ����������ɺ�����������->��Сֵ
    else if (paramSpecialAttr & PARAM_SPECIAL_ATTR_MIN)
    {
        recvValue                                               = lowerLimit; 
    }
    //��������������         
    else
    {
        recvValue                                               = atoi((void *)rscAddr);
        //�޷����ж�
        if ((PARAM_TYPE_UINT8 == paramStructType) || (PARAM_TYPE_UINT16 == paramStructType)
            || (PARAM_TYPE_UINT32 == paramStructType))
        {
            if ((recvValue > upperLimit) || (recvValue < lowerLimit))
            {
                t_ParserExeModuler.m_parserErrCode              = STATUS_DATA_OUT_OF_RANGE;
            }    
        }
        //�з����ж�
        else
        {
            if (((int32_t)recvValue > (int32_t)upperLimit) || ((int32_t)recvValue < (int32_t)lowerLimit))
            {
                t_ParserExeModuler.m_parserErrCode              = STATUS_DATA_OUT_OF_RANGE;
            }
        }
    
        if (STATUS_NO_ERROR != t_ParserExeModuler.m_parserErrCode)
        {
            return FALSE;    
        }
    }
    LIB_ConvertNmubToString(recvValue, LIB_DataBitLenGet(recvValue), paramStr);
    _API_PEMParamQueueStrWrite(paramStr, strlen((void *)paramStr));
    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �ַ��Ͳ����Ϸ����ж�                                                            
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
 *  ��    ע : �ַ��Ͳ������п�ʡ������ ��������PARAM_SPECIAL_ATTR_MAX PARAM_SPECIAL_ATTR_MIN����                                                          
 *                                                                            
 *                                                                            
******************************************************************************/

static  uint32_t _API_PEMCharacterParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr)
{
    uint32_t           i                                   = 0;
    ParamCharacterStruct   *pparamStructAddr;
    const CharacterUnit    *pcharacterUnitAddr;
    uint32_t           characterTableNumbs;

    //�жϲ����������ɺ�����������->Ĭ��ֵ
    if (TRUE == _API_PEMParamDefSpecialChk(paramSpecialAttr))
    {
        return TRUE;
    }
    pparamStructAddr                                            = (ParamCharacterStruct *)pparamContainer->m_pparamStructAddr;
    pcharacterUnitAddr                                          = pparamStructAddr->m_pcharacterTable;
    characterTableNumbs                                         = pparamStructAddr->m_characterTableNumbs;
    //�Խ��յ����ַ����Ϸ����ж�
    for (; i < characterTableNumbs; i++, pcharacterUnitAddr++)
    {
        if (0 == strcmp((void *)rscAddr, (void *)(pcharacterUnitAddr->m_plabelStr)))
        {
            break;
        }
    }
    if (characterTableNumbs == i)
    {
        //���յ��ַ����Ϸ�  �������Ͷ���Ϊ����������
        t_ParserExeModuler.m_parserErrCode                      = STATUS_PARAM_NOT_ALLOWED;
    }
    else
    {
        //��ȡ�ַ�����Ӧ��ֵ��д����������� ֵΪuint8_t����
        PARAM_QUEUE_CHAR_WRITE(pcharacterUnitAddr->m_value);
        //д��ָ���'\0'
        PARAM_QUEUE_SPLIT_SIGN_WRITE();
    }

    if (STATUS_NO_ERROR != t_ParserExeModuler.m_parserErrCode)
    {
        return FALSE;    
    }
    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ���Ͳ����Ϸ����ж�                                                            
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
 *  ��    ע : ��������ָ������˫����Ϊ��ʼ�ͽ�����ʶ������ ��"ABC"                                                           
 *                                                                            
 *                                                                            
******************************************************************************/

static  uint32_t _API_PEMStringParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr)
{
    uint32_t           i                                   = 0;
    //��ȡ�����ַ������� ���� �ܳ���-1 ȥ��β��˫����
    uint32_t           recvStrLen                          = 0;
    const uint8_t            *pnoValidStr;
    uint32_t           noValidStrLen;

    //�жϲ����������ɺ�����������->Ĭ��ֵ
    if (TRUE == _API_PEMParamDefSpecialChk(paramSpecialAttr))
    {
        return TRUE;
    }
    pnoValidStr                                                 = ((ParamStringStruct *)pparamContainer->m_pparamStructAddr)->m_pinvalidStr;
    noValidStrLen                                               = strlen((void *)pnoValidStr);
    //�Խ��յ��Ĵ����Ϸ����ж�
    //ָ��ֵ���µ��� �Թ���ʼ˫����
    rscAddr++;
    recvStrLen                                                  = strlen((void *)rscAddr) - 1;
    //ɨ�������Ƿ��ַ��� �ж������յ��Ĳ������Ƿ���ڷǷ��ַ�
    for (; i < noValidStrLen; i++)
    {
        if (-1 != LIB_Strnpos(rscAddr, pnoValidStr[i], recvStrLen))
        {
            //�ҵ��Ƿ�ֵ
            t_ParserExeModuler.m_parserErrCode                  = STATUS_INVALID_STRING_DATA;
            break;
        }
    }

    if (STATUS_NO_ERROR != t_ParserExeModuler.m_parserErrCode)
    {
        return FALSE;    
    }
    //�����������д�봮����ֵ 
    _API_PEMParamQueueStrWrite((const uint8_t *)rscAddr, recvStrLen);

    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ʵ�Ͳ����Ϸ����ж�                                                            
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
 *  ��    ע : ����ʵ������ ����������Ӳ�Թ涨���������С���� ���յ��ĳ��ȱ��������õĳ�����ƥ��                                                          
 *                                                                            
 *                                                                             
******************************************************************************/

static void _API_PEMRealParamConfigInfoGet(void *pparamStructAddr, ParamRealUint32Struct *pparamConfigStruct, 
                                           ParamTypeInfo paramStructType, uint32_t index)
{
    ParamRealUint16Struct   *pparamRealUint16Struct;
    ParamRealUint32Struct   *pparamRealUint32Struct;
    
    if (PARAM_TYPE_REAL_UINT16 == paramStructType)
    {
        pparamRealUint16Struct                      = (ParamRealUint16Struct *)pparamStructAddr;
        pparamRealUint16Struct                     += index;
        pparamConfigStruct->m_upperLimit            = pparamRealUint16Struct->m_upperLimit;
        pparamConfigStruct->m_lowerLimit            = pparamRealUint16Struct->m_lowerLimit;
        pparamConfigStruct->m_configInfo			= pparamRealUint16Struct->m_configInfo;
        pparamConfigStruct->m_signSymbolIndex		= pparamRealUint16Struct->m_signSymbolIndex;
    }
    else if (PARAM_TYPE_REAL_UINT32 == paramStructType)
    {
        pparamRealUint32Struct                      = (ParamRealUint32Struct *)pparamStructAddr;
        pparamRealUint32Struct                     += index;
        pparamConfigStruct->m_upperLimit            = pparamRealUint32Struct->m_upperLimit;
        pparamConfigStruct->m_lowerLimit            = pparamRealUint32Struct->m_lowerLimit;
        pparamConfigStruct->m_configInfo			= pparamRealUint32Struct->m_configInfo;
        pparamConfigStruct->m_signSymbolIndex		= pparamRealUint32Struct->m_signSymbolIndex;
    }
}

// static uint32_t _API_PEMRealSignBitLenGet(uint32_t signSymbolIndex)
// {
//     //ͬһ���������λ����֮����3
//     return SIGN_INTERVAL_BIT*SIGN_SYMBOL_INDEX_GET((st_SignSymbolTable[signSymbolIndex]).m_relativeIndex);
// }

static uint32_t _API_PEMRealSignBitLenGet(uint32_t signSymbolIndex)
{
	if(signSymbolIndex!=SIGN_SYMBOL_INDEX_NONE)
    //ͬһ���������λ����֮����3
    return SIGN_INTERVAL_BIT*SIGN_SYMBOL_INDEX_GET((st_SignSymbolTable[signSymbolIndex]).m_relativeIndex);
	else
		return SIGN_SYMBOL_INDEX_NONE;
}

static uint32_t _API_PEMRealParamSectorBitLenInfoGet(ParamRealUint32Struct *pparamSectorStruct, 
                                                           ParamRealSectorInfoStruct *pparamRealSectorInfoStruct)
{
    uint32_t      dotBitLenValue                  = REAL_CFG_INFO_DOT_POS_GET(pparamSectorStruct->m_configInfo);
    uint32_t      configBitLenValue	            = REAL_CFG_INFO_STR_LEN_GET(pparamSectorStruct->m_configInfo);
    uint32_t      signBitLenValue	                = _API_PEMRealSignBitLenGet(pparamSectorStruct->m_signSymbolIndex);
    
    if (NULL != pparamRealSectorInfoStruct)
    {
        //��С��������Ϊ�� ��С��������λ�����������ַ���������ͬ
        if (REAL_CFG_INFO_NO_DOT == dotBitLenValue)
        {
            dotBitLenValue                              = configBitLenValue;
        }
        //С��������λ����ֵ
        pparamRealSectorInfoStruct->m_dotBitLen         = dotBitLenValue;
        if (SIGN_SYMBOL_INDEX_NONE == signBitLenValue)
        {
            signBitLenValue                             = 0;  
        }
        //��λ��������λ����ֵ
        pparamRealSectorInfoStruct->m_signBitLen        = signBitLenValue;
        //�����ַ�����������λ����ֵ
        pparamRealSectorInfoStruct->m_configBitLen      = configBitLenValue;
        //��������λ����ֵ
        pparamRealSectorInfoStruct->m_lowerLimitBitLen  = LIB_DataBitLenGet(pparamSectorStruct->m_lowerLimit) 
                                                          + dotBitLenValue + signBitLenValue;
        //��������λ����ֵ
        pparamRealSectorInfoStruct->m_upperLimitBitLen  = LIB_DataBitLenGet(pparamSectorStruct->m_upperLimit) 
                                                          + dotBitLenValue + signBitLenValue;
    }
    return TRUE;
}

static  uint32_t _API_PEMRealParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr)
{
    ParamRealUint32Struct t_paramRealStruct;
    ParamRealSectorInfoStruct t_paramRealSectorInfoStruct;
    //�����ṹ������
    ParamTypeInfo         paramStructType                       = pparamContainer->m_paramStructType;
    void                 *pparamStructAddr                      = pparamContainer->m_pparamStructAddr;  
    uint8_t                *pparamQueuePos                        = (uint8_t *)(PARAM_QUEUE_CURR_POS_GET());
    //��ȡʵ�����������ø�����Ϣ
    uint32_t        realParamInfoSectionCnts              = REAL_PARAM_INFO_SECTION_CNTS_GET(pparamContainer->m_paramStructInfo);
    uint32_t        recvDataBitLen, configDataBitLen, signSymbolIndex;
    uint32_t        recvValidStrLen, recvSignBitLen, recvTotalBitLen, recvNeedModifyBitLen;
    uint32_t        sectorDotSignBitLen;
    uint32_t        i, j;
    //�ո�������ŵ�λ��С�������λ�� ��ʼ��Ϊδ�ҵ�
    int8_t         spaceSignPos, unitSignPos, recvDotBitLen;
    //�Ŵ����Сλ����
    int8_t         zoomOutInBitLen;
    int32_t                 recvValue;

    //�жϲ��������Ƿ�����������->Ĭ��ֵ
    if (TRUE == _API_PEMParamDefSpecialChk(paramSpecialAttr))
    {
        return TRUE;
    }
    //��ȡ�������ö�������ֵ��С���㡢�����������ַ�������λ����Ϣ �˴�Ϊ����ȷ�Ļ���ַ�������λ����ֵ
    _API_PEMRealParamConfigInfoGet(pparamStructAddr, &t_paramRealStruct, paramStructType, 0);
    //����涨 ʵ���Ķ�����ö�֮����ַ������ȱ�����ͬ ����Ӧ������ һ������������� 
    configDataBitLen                                            = REAL_CFG_INFO_STR_LEN_GET(t_paramRealStruct.m_configInfo);;
    //��ʼ�������ַ����з���λ����Ϊ��
    recvSignBitLen                                              = 0;
    //---------------------------------------------------------------------------------------------------
    
    //�жϲ��������Ƿ�����������->���ֵ
    if (paramSpecialAttr & PARAM_SPECIAL_ATTR_MAX)      
    {
        //��ȡ�������ö�������ֵ��С���㡢�����������ַ�������λ����Ϣ
        _API_PEMRealParamConfigInfoGet(pparamStructAddr, &t_paramRealStruct, paramStructType, realParamInfoSectionCnts - 1);
        _API_PEMRealParamSectorBitLenInfoGet(&t_paramRealStruct, &t_paramRealSectorInfoStruct);
        recvValue                                               = t_paramRealStruct.m_upperLimit;                                     
    }
    //�жϲ��������Ƿ�����������->��Сֵ
    else if (paramSpecialAttr & PARAM_SPECIAL_ATTR_MIN)
    {
        _API_PEMRealParamSectorBitLenInfoGet(&t_paramRealStruct, &t_paramRealSectorInfoStruct);
        recvValue                                               = t_paramRealStruct.m_lowerLimit;
    }
    //������������������ �������������ж�
    else
    {
        //----------------------��һ��  ���źϷ����ж�  ֻ�����Ѿ����õķ���-----------------------------

        //��÷���������֮��ķָ��� -> �ո�� �����ҵ���������λ��
        spaceSignPos                                            = LIB_Strpos((void *)rscAddr, ' ');
        //�����Ƿ���յ��ո�� ������Ӧ�ж�
        
        //�ж��Ƿ��ҵ��ո�� ���ҵ� ��������һ���з����ַ� -> ���з����ж�
        if (-1 != spaceSignPos)
        {
            //�ո�д�������
            rscAddr[spaceSignPos]                               = '\0';
            unitSignPos                                         = spaceSignPos + 1;
        }
        //�������ö�������Ϣ
        for (i = 0; i < realParamInfoSectionCnts; i++)
        {
            _API_PEMRealParamConfigInfoGet(pparamStructAddr, &t_paramRealStruct, paramStructType, i);
            //���յ��ո�� ��������һ���з����ַ� -> ���з����ж�
            if (-1 != spaceSignPos)
            {
                signSymbolIndex                                 = t_paramRealStruct.m_signSymbolIndex;
                //�ӵ�λ���ű�����ƥ��ĵ�λ���� �ҵ�������Ϊ��signSymbolIndex
                if (0 == strcmp((void *)(st_SignSymbolTable[signSymbolIndex]).m_psignStr, (void *)&rscAddr[unitSignPos]))
                {
                    //��¼�����ַ�����λ������ֵ                                   
                    recvSignBitLen                              = _API_PEMRealSignBitLenGet(signSymbolIndex);
                    break;
                }
            }
            //δ���յ��ո�� -> �ж��������Ƿ��з�������
            else
            {
                if (t_paramRealStruct.m_signSymbolIndex != SIGN_SYMBOL_INDEX_NONE)
                {
                    break;
                }
            }
        }
        //(���յ��ո��ʱ �� �Ҳ���ƥ��ĵ�λ����) �� (δ���յ��ո��ʱ �� ���ö������������˷���)
        if ((-1 != spaceSignPos) && (i == realParamInfoSectionCnts)
            || ((-1 == spaceSignPos) && (i != realParamInfoSectionCnts)))
        {
            //������Я���ĵ�λ���Ŵ���
            t_ParserExeModuler.m_parserErrCode              = STATUS_PARAM_TYPE_ERROR;
            return FALSE;
        }
        //----------------------�ڶ���  ȥ�������ַ���ǰ�����ַ�-----------------------------------------
        //���磺 000999.9 M -> 999.9 M 
        //ע���ڴ˲��� һ�������ý��յ����ַ�����չ ��Ȼ�п��ܻ��ٵ�������յĲ�������
        //ͨ������rscAddrָ�����ﵽȥ��ǰ�����Ч��
        
        //�˲���������֮��Ľ��Ϊ 000000.9 -> .9
        while ('0' == (*rscAddr))
        {
            rscAddr++;
        }
        //----------------------������  �ض��ַ���-------------------------------------------------------
        //ע���ڴ˲��� һ�������ý��յ����ַ�����չ ��Ȼ�п��ܻ��ٵ�������յĲ�������
        //�˴�����������Ϣ�еĳ���

        //��ȡ�˿̵��ַ���С����λ�� �� �ַ�������
        recvDotBitLen                                           = LIB_Strpos((void *)rscAddr, '.');
        recvValidStrLen                                         = strlen((void *)rscAddr);
        j                                                       = configDataBitLen;
        //����С����λ�ö���Ч�ַ������Ƚ������� С���������λ�ò�Ϊ��ʱ
        //����С������С���㲻����λʱ
        if (recvDotBitLen > 0)
        {
            //����С������С�����������ַ����������� ���ַ�����Ч���ȼ�һ ����С���㳤��
            if (recvDotBitLen < j)
            {
                j++;
            }
            //С�����������ַ����������� ����Ϊ��С����û����; ʡ�Ե�
            else
            {
                recvDotBitLen                                   = -1;
            }
        }
        //�ж��Ƿ���Ҫ���ַ������нض�
        if (recvValidStrLen > j)
        {
            rscAddr[j]                                          = '\0';
            //�����ַ����ĳ�������Ϊ���ó���
            recvValidStrLen                                     = j;
        }
        //----------------------���Ĳ�  ������յ����ַ�����������λ����ֵ-------------------------------
        //������λ����=С��������λ���� + ����ֵ����λ���� + ��λ����λ���� + ��Ҫ�޲�����λ����
        if (-1 != recvDotBitLen)  
        {
            if (recvDotBitLen < j)
            {
                //��С�������ַ���������
                LIB_StringLsl((uint8_t *)&rscAddr[recvDotBitLen], recvValidStrLen-recvDotBitLen, 1);
                //����С���� �򳤶�ֵ��һ �˳���Ϊ��ȥС����֮��ĳ���
                recvValidStrLen--;
            }
        }
        else
        {
            //δ���յ�С���� ��С���㳤��Ϊ�ַ�������
            recvDotBitLen                                       = recvValidStrLen;
        }
        //����ȥ��С����֮���ַ���������ֵ
        recvValue                                               = atoi((void *)rscAddr);
        //������յ��ַ�����ֵλ���� ��λ����Ϊ���յ��ַ�������Ч���� �磺.01 λ����Ϊ�� .001 λ����Ϊ��
        recvDataBitLen                                          = LIB_DataBitLenGet(recvValue);
        //������Ҫ������λ����                                                           
        recvNeedModifyBitLen                                    = configDataBitLen - recvValidStrLen;
        //������յ��ַ�����������ֵ
        recvTotalBitLen                                         = recvDotBitLen + recvDataBitLen
                                                                  + recvSignBitLen + recvNeedModifyBitLen;
        //----------------------���岽  �����ַ�����������λ��������һ��ƥ��Ķ���������-----------------
        //ͨ���ӵ����ϵı���������һ��ƥ�����������
        //������������������
        for (i = 0; i < realParamInfoSectionCnts; i++)
        {
            _API_PEMRealParamConfigInfoGet(pparamStructAddr, &t_paramRealStruct, paramStructType, i);
            //��������������λ���ȷ�Χ
            _API_PEMRealParamSectorBitLenInfoGet(&t_paramRealStruct, &t_paramRealSectorInfoStruct);
            //�����յ�ֵΪ�� ����Ϊ��һ������ ֱ�Ӷ�λ�����ö������е���Ͷ�
            if (0 == recvValue)
            {
                break;
            }
            //����Ͷ����������� �ж��Ƿ�������� ���������� ����ʾ���ݷ�Χ����
            if ((recvTotalBitLen < t_paramRealSectorInfoStruct.m_lowerLimitBitLen) && (0 == i))
            {
                t_ParserExeModuler.m_parserErrCode  = STATUS_DATA_OUT_OF_RANGE;
                return FALSE;
            }
            if (recvTotalBitLen <= t_paramRealSectorInfoStruct.m_upperLimitBitLen)
            {
                break;
            }
        }
        //�ж��Ƿ���ҵ����ʵĶ���������
        if (i == realParamInfoSectionCnts)
        {
            //δ�ҵ� ����ʾ���ݷ�Χ����
            t_ParserExeModuler.m_parserErrCode                  = STATUS_DATA_OUT_OF_RANGE;
            return FALSE;
        }
        //----------------------������  �����ҵ��Ķ��������� �Խ��յ����ַ���ֵ���зŴ����С------------
        //���յ���ֵ�Ŵ����С�� Ҫ���ݵõ���ֵ�����ҵĶ�����������������ж� �鿴�Ƿ��м����Ϸ��Ŀ���    
        //����Ŵ����Сλ���� = ��Ҫ������λ���� + ��λ����λ���� + С��������λ���� -
        //                       (Ŀ�Ķ���������λ����λ���� + Ŀ�Ķ���������С��������λ����)
        zoomOutInBitLen  = recvNeedModifyBitLen + recvSignBitLen + recvDotBitLen
                           - (t_paramRealSectorInfoStruct.m_signBitLen + t_paramRealSectorInfoStruct.m_dotBitLen);
        if (zoomOutInBitLen < 0)
        {
            //�������ֵ
            recvValue                                          /= LIB_Get10nData(abs(zoomOutInBitLen));
        }
        else
        {
            //�������ֵ
            recvValue                                          *= LIB_Get10nData(zoomOutInBitLen);
        }
        //���յ���ֵӦ�ñ����ҵ������ö������е�����ֵҪ��
        if (recvValue < t_paramRealStruct.m_lowerLimit)
        {
            //δ�ҵ� ����ʾ���ݷ�Χ����
            t_ParserExeModuler.m_parserErrCode                  = STATUS_DATA_OUT_OF_RANGE;
            return FALSE;
        }
        //�ж������Ϸ�����
        for (; i < realParamInfoSectionCnts; i++)
        {
            if (recvValue > t_paramRealStruct.m_upperLimit)
            {
                // �����ֵ���󣬳���Χ��2012.05.24
				if (i == (realParamInfoSectionCnts - 1))
		        {
		            //δ�ҵ� ����ʾ���ݷ�Χ����
		            t_ParserExeModuler.m_parserErrCode          = STATUS_DATA_OUT_OF_RANGE;
		            return FALSE;
		        }

				//���㵱ǰ�����������еķ�������λ����+С��������λ����
                sectorDotSignBitLen                             = t_paramRealSectorInfoStruct.m_signBitLen 
                                                                  + t_paramRealSectorInfoStruct.m_dotBitLen;
                //��ȡ�Ϸ���Ķ�����������Ϣ
                _API_PEMRealParamConfigInfoGet(pparamStructAddr, &t_paramRealStruct, paramStructType, i+1);
                //��ȡ�Ϸ���Ķ���������λ���ȷ�Χ
                _API_PEMRealParamSectorBitLenInfoGet(&t_paramRealStruct, &t_paramRealSectorInfoStruct);
                //������Ҫ��С��ֵ
                zoomOutInBitLen                                 = t_paramRealSectorInfoStruct.m_signBitLen 
                                                                  + t_paramRealSectorInfoStruct.m_dotBitLen
                                                                  - sectorDotSignBitLen;
                //�����µ��ַ�������ֵ
                recvValue                                      /= LIB_Get10nData(zoomOutInBitLen);
            }
            else
            {
                break;
            }
        }
        //�ж��Ƿ���ҵ����ʵĶ���������
        if (i == realParamInfoSectionCnts)
        {
            //δ�ҵ� ����ʾ���ݷ�Χ����
            t_ParserExeModuler.m_parserErrCode                  = STATUS_DATA_OUT_OF_RANGE;
            return FALSE;
        }
    }
    //----------------------���߲�  �����ҵ��Ķ��������� �Խ��յ����ַ���ֵ���и�ʽ��--------------------
    //�ж�ʵ�Ͳ�������Զ��庯��ָ���Ƿ�Ϊ��
    if (NULL == s_FP_pfRealParamSplit)
    {
        //��׼�Ĳ�ֹ��� �����ݲ��Ϊ�ַ�����ʽ
        LIB_ConvertNmubToString(recvValue, t_paramRealSectorInfoStruct.m_configBitLen, pparamQueuePos);
        //�ж��Ƿ���Ҫ����С����
        if (t_paramRealSectorInfoStruct.m_configBitLen != t_paramRealSectorInfoStruct.m_dotBitLen)
        {
            LIB_StrInsert(pparamQueuePos, t_paramRealSectorInfoStruct.m_dotBitLen, '.');
            t_paramRealSectorInfoStruct.m_configBitLen++;
        }
        //���²������λ������
        PARAM_QUEUE_CURR_POS_ADJ(t_paramRealSectorInfoStruct.m_configBitLen);
        //ѹ�뵥λ������Ϣ
        if (-1 != spaceSignPos)
        {
            pparamQueuePos  = (uint8_t *)((st_SignSymbolTable[t_paramRealStruct.m_signSymbolIndex]).m_psignStr);
            //д��ո��
            PARAM_QUEUE_CHAR_WRITE(' ');
            _API_PEMParamQueueStrWrite(pparamQueuePos, strlen((void *)pparamQueuePos));
        }
        else
        {
            //д��ָ���'\0'
            PARAM_QUEUE_SPLIT_SIGN_WRITE();
        }
    }        
    //�����Զ����ʵ�Ͳ�����ֺ���
    else
    {
        RealParamSplitStruct    t_RealParamSplitStruct;

        t_RealParamSplitStruct.m_pdestSplitAddr                 = pparamQueuePos;
        t_RealParamSplitStruct.m_rawDecValue                    = recvValue;
        t_RealParamSplitStruct.m_rawConfigLen                   = t_paramRealSectorInfoStruct.m_configBitLen;
        t_RealParamSplitStruct.m_rawDotValue                    = t_paramRealSectorInfoStruct.m_dotBitLen;
        t_RealParamSplitStruct.m_rawSignValue                   = st_SignSymbolTable[t_paramRealStruct.m_signSymbolIndex].m_relativeIndex;
        if (FALSE == s_FP_pfRealParamSplit(&t_RealParamSplitStruct))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �����Ͳ����Ϸ����ж�                                                            
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

static  uint32_t _API_PEMFloatParamValidChk(uint8_t *rscAddr, ParamContainer *pparamContainer, uint32_t paramSpecialAttr)
{
    
    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ��������д���ַ���                                                              
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

static void _API_PEMParamQueueStrWrite(const uint8_t *prscStrAddr, uint8_t rscStrLen)
{
    //д�봮����ֵ 
    strncpy((void *)(PARAM_QUEUE_CURR_POS_GET()), (void *)prscStrAddr, rscStrLen);
    //���²������λ������
    PARAM_QUEUE_CURR_POS_ADJ(rscStrLen);
    //д��ָ���'\0'
    PARAM_QUEUE_SPLIT_SIGN_WRITE();
}

void API_PEMParamQueueIntegerWrite(uint32_t rscValue, uint8_t len)
{
    LIB_ConvertNmubToString(rscValue, len, (uint8_t *)(PARAM_QUEUE_CURR_POS_GET()));
    //���²������λ������
    PARAM_QUEUE_CURR_POS_ADJ(len);
}

void API_PEMParamQueueCharWrite(uint8_t rscChar)
{
    PARAM_QUEUE_CHAR_WRITE(rscChar);
}

void API_PEMParamQueueSplitSignWrite(uint8_t rscChar)
{
    //д��ָ���'\0'
    PARAM_QUEUE_SPLIT_SIGN_WRITE();
}

void API_PEMParamQueuePosAdjust(uint16_t len)
{
    //���²������λ������
    PARAM_QUEUE_CURR_POS_ADJ(len);
}

uint8_t *API_PEMParamQueueCurrentPtrGet(void)
{
    return (uint8_t *)(PARAM_QUEUE_CURR_POS_GET());
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

void _API_PEMInputQueueFlush(void)
{
    //��������� ; �ĳ���
    memset(t_ParserExeModuler.m_inputQueue, '\0', t_ParserExeModuler.m_inputQueueLen+1);
    t_ParserExeModuler.m_inputQueueLen                      = PARSER_START_POS;  
}

void _API_PEMOutputQueueFlush(void)
{
    //��������� ; �ĳ���
    memset(t_ParserExeModuler.m_outputQueue, '\0', t_ParserExeModuler.m_outputQueueLen+1);
    t_ParserExeModuler.m_outputQueueLen                     = 0;  
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

static uint32_t _API_PEMOutputQueuePush(void *pvoid, portQUEUELEN_TYPE len, 
                                             uint32_t appendSign, uint32_t style)
{
    uint32_t   i                                       = (appendSign == NO_APPENDED_SIGN)?(0):(1);
    
    //�жϷ�Χ
    if (((t_ParserExeModuler.m_outputQueueLen + len + i) > (QUEUE_MAX_LEN-1))
        && (!((2 == style) && ((uint32_t)pvoid == STATUS_OUTPUT_QUEUE_FULL))))
    {
        t_ParserExeModuler.m_parserErrCode                   = STATUS_OUTPUT_QUEUE_FULL;
        //��Ϣ���ʧ��
        return  FALSE;
    }
    //���ַ������в���
    if (_OUTPUT_QUEUE_PUSH_STR == style)
    {
        memcpy((void *)&t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen], (void *)pvoid, len);
    }
    //������������
    else if (_OUTPUT_QUEUE_PUSH_INT == style)
    {
        LIB_ConvertNmubToString((uint32_t)pvoid, len, (void *)&t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen]);
    }
    //�ַ��Ͳ���
    else
    {
        t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen] = (uint32_t)pvoid;
    }
    t_ParserExeModuler.m_outputQueueLen                    += len;
    if (NO_APPENDED_SIGN != appendSign)
    {
        t_ParserExeModuler.m_outputQueue[t_ParserExeModuler.m_outputQueueLen++] = (int8_t)appendSign;
    }
    return FALSE;
}

uint32_t API_PEMOutputQueueErrMsgPush(ParserErrCode parserErrCode, uint32_t appendSign)
{
    return _API_PEMOutputQueuePush((void *)API_PEM_pParserExeModulerErrCodeArray[parserErrCode], 
        strlen((void *)API_PEM_pParserExeModulerErrCodeArray[parserErrCode]), appendSign, _OUTPUT_QUEUE_PUSH_STR);
}

uint32_t API_PEMExecuteErrCodeSet(ParserErrCode executeErrCode)
{
    if ((STATUS_EXECUTE_NOT_ALLOWED == executeErrCode) || (STATUS_PARAM_NOT_ALLOWED == executeErrCode)
        || (STATUS_EXECUTE_TIME_OUT == executeErrCode) || (STATUS_DATA_OUT_OF_RANGE == executeErrCode)
        || (STATUS_SUM_CHECK_ERROR  == executeErrCode))
    {
        t_ParserExeModuler.m_parserErrCode                      = executeErrCode;
        return TRUE;
    }
    return FALSE;
}

uint32_t API_PEMOutputQueueStrPush(const uint8_t *prscStr, uint32_t appendSign)
{
    return _API_PEMOutputQueuePush((void *)prscStr, strlen((void *)prscStr), appendSign, _OUTPUT_QUEUE_PUSH_STR);
}

uint32_t API_PEMOutputQueueStrnPush(const uint8_t *prscStr, uint32_t len, uint32_t appendSign)
{
    return _API_PEMOutputQueuePush((void *)prscStr, len, appendSign, _OUTPUT_QUEUE_PUSH_STR);
}

uint32_t API_PEMOutputQueueIntegerPush(uint32_t rscData, uint32_t len, uint32_t appendSign)
{
    return _API_PEMOutputQueuePush((void *)rscData, len, appendSign, _OUTPUT_QUEUE_PUSH_INT);
}  

uint32_t API_PEMOutputQueueCharPush(uint32_t appendChar, uint32_t appendSign)
{
    return _API_PEMOutputQueuePush((void *)appendChar, 1, appendSign, _OUTPUT_QUEUE_PUSH_CHAR);
}  

void int_API_PEMRead(uint8_t recvMsg)
{
	//����������ʱ ��ʼ����
    if (STATUS_IDLE == t_ParserExeModuler.m_parserStatus)
    {
        //ϵͳ���������� ���յ���������� ��������ʼ����������� 
        //�������������
        if (recvMsg == s_EndCode)
        {
    	    //��������ΪCR+LFʱ Ҫ�ж���һ�ַ��Ƿ�ΪCR ����ΪCR ��LF��Ч
            if (CR_LF_INDEX == t_ParserExeModuler.m_endCodeIndex)
            {
                //�ж����޽��յ��س���CR
                if ('\r' == t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen - 1])
                {
                    //��������ȷ ��CR��λ�ô�д��LF
                    t_ParserExeModuler.m_inputQueueLen--;
                }
                else
                {
                    //���������� ������յ����ַ�����
                    t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen] = recvMsg;
                    // ������ܻ�����
                    _API_PEMInputQueueFlush();
                    return;
                }
            }
            //����ַ���������
    	    t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen] = ';';
    	    //ϵͳָ�����ִ��״̬ ������
    	    t_ParserExeModuler.m_parserStatus               = STATUS_PARSERING;
            //ָ����Ĭ�ϴӸ�ָ�����������ʼ����
            t_ParserExeModuler.m_currParserContainerPtr     = t_ParserExeModuler.m_rootParserContainerPtr;
            t_ParserExeModuler.m_currParserStructCnts       = t_ParserExeModuler.m_rootParserStructCnts;
            //����������һ
            t_ParserExeModuler.m_totalInstructionChains++; 
        }
        //���������ַ����Լ�����
        else
        {
    	    t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen++] = recvMsg;
            //���������������ж�
            if (';' == recvMsg)
            {
                //����������һ
                t_ParserExeModuler.m_totalInstructionChains++;  
            }
    	    if (t_ParserExeModuler.m_inputQueueLen >= QUEUE_MAX_LEN)
    	    {
    		    //��һ  ʹ֮��t_ParserExeModuler.m_inputQueue��Χ��
                t_ParserExeModuler.m_inputQueueLen--;
                //�ڽ��ջ����������һ���ֽڴ������ַ���������
    		    t_ParserExeModuler.m_inputQueue[t_ParserExeModuler.m_inputQueueLen] = ';';
    		    //ϵͳָ�����ִ��״̬ ������
    	        t_ParserExeModuler.m_parserStatus           = STATUS_PARSERING;
    	    }
    	}
    }
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

static portHASHCODE_TYPE _API_PEMStrHashCodeGet(const uint8_t *prscStr, uint32_t len)
{
    uint32_t              hashCode                            = HASH_INIT_CODE;
    portHASHCODE_TYPE   maxHashCode                         = (portHASHCODE_TYPE)-1;
	uint8_t               rscChar;
   
    while (len--)
	{
        rscChar											    = (*prscStr++);
        hashCode                                            = hashCode<<7;
		hashCode                                           += rscChar;
        hashCode                                           %= maxHashCode;
    }
    return (portHASHCODE_TYPE)hashCode;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ���ݻ�ȡ������                                                              
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
 *  ��    ע : �����Դ�˷�ʽ��֯                                                               
 *                                                                            
 *                                                                            
******************************************************************************/

uint8_t API_PEMItegerParamReaduint8_t(const uint8_t *rscAddr)
{
    return (uint8_t)atoi((void *)rscAddr);
}

int8_t API_PEMItegerParamReadInt8(const uint8_t *rscAddr)
{
    return (int8_t)atoi((void *)rscAddr);
}

uint16_t API_PEMItegerParamReadUint16(const uint8_t *rscAddr)
{
    return (uint16_t)atoi((void *)rscAddr);
}

int16_t API_PEMItegerParamReadInt16(const uint8_t *rscAddr)
{
    return (int16_t)(API_PEMItegerParamReadUint16(rscAddr));
}

uint32_t API_PEMItegerParamReadUint32(const uint8_t *rscAddr)
{
    return (uint32_t)atoi((void *)rscAddr);
}

int32_t API_PEMItegerParamReadInt32(const uint8_t *rscAddr)
{
    return atoi((void *)rscAddr);
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :  Ԥִ�кϷ��Լ�⺯����װ ж�غ���                                                             
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

static uint32_t _API_PEMNullValidityChk(struct EXECUTE_VALIDITY_CHK_INFO *pvalidityChkInfo)
{
    return TRUE;
} 

void API_PEMPrevValidityChkFuncInstall(FP_pfValidityChk *pfprevExecuteValidityChk)
{
    s_FP_pfPrevValidityChk                                      = pfprevExecuteValidityChk;
}

void API_PEMPrevValidityChkFuncUninstall(void)
{
    s_FP_pfPrevValidityChk                                      = NULL;
}

void API_PEMRealParamSplitFuncInstall(FP_pfRealParamSplit *pfRealParamSplit)
{
    s_FP_pfRealParamSplit                                       = pfRealParamSplit;
}

void API_PEMRealParamSplitFuncUninstall(void)
{
    s_FP_pfRealParamSplit                                       = NULL;
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
/*
static uint32_t _API_PEMSegmentShortLenGet(uint32_t longLen)
{
    uint32_t   i                                       = t_ParserExeModuler.m_parserPosIndex;

    longLen                                                += i;                                    
    for (;i < longLen; i++)
    {
        if ((t_ParserExeModuler.m_inputQueue[i] >= 'A') && (t_ParserExeModuler.m_inputQueue[i] <= 'Z'))
        {
            continue;
        }
        else if ('*' == t_ParserExeModuler.m_inputQueue[i])
        {
            continue;
        }
        else
        {
            break;
        }
    }

    return i - t_ParserExeModuler.m_parserPosIndex;
} 

static uint32_t _API_PEMSegmentLongLenGet(void)
{
    uint32_t   i                                       = t_ParserExeModuler.m_parserPosIndex;

    //��ͨѶָ����������� '\0' �� ';' ��������Ϊָ������� 
    while (('\0' != t_ParserExeModuler.m_inputQueue[i]) && (';' != t_ParserExeModuler.m_inputQueue[i]))
    {
        i++;
    }

    return i - t_ParserExeModuler.m_parserPosIndex;
}
*/

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ʵ�����ݲ�ֺ���                                                             
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
static uint32_t _RealParamSplitServ(RealParamSplitStruct *prealParamSplitStruct)
{
    uint8_t               revParamMappingVal  = 0;
    uint8_t               dotMappingVal       = prealParamSplitStruct->m_rawDotValue;

    LIB_ConvertNmubToString(prealParamSplitStruct->m_rawDecValue, prealParamSplitStruct->m_rawConfigLen, (uint8_t *)(PARAM_QUEUE_CURR_POS_GET()));
    //���²������λ������
    PARAM_QUEUE_CURR_POS_ADJ(prealParamSplitStruct->m_rawConfigLen);
    //API_PEMParamQueueIntegerWrite(prealParamSplitStruct->m_rawDecValue, prealParamSplitStruct->m_rawConfigLen);
    API_PEMParamQueueCharWrite(' ');
    if (REAL_CFG_INFO_NO_DOT == dotMappingVal)
    {
        dotMappingVal                       = prealParamSplitStruct->m_rawConfigLen;
    }
    revParamMappingVal                      =   prealParamSplitStruct->m_rawConfigLen + dotMappingVal
                                              + (prealParamSplitStruct->m_rawSignValue & 0x07)*3;
    API_PEMParamQueueCharWrite(revParamMappingVal/10 + '0');
    API_PEMParamQueueCharWrite(revParamMappingVal%10 + '0');
    PARAM_QUEUE_CHAR_WRITE('\0');

    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : ͨѶ֡У��ͻ�ȡ����                                                              
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
 *  ��    ע : ָ֡��������ַ��ۼӺ�|0x80                                                               
 *                                                                            
 *                                                                            
******************************************************************************/
#if (_FRAME_CHK_SUPPORT > 0)

static uint8_t _API_PEMFrameChkSumGet(const uint8_t *prscQueue, portQUEUELEN_TYPE len)
{
    portQUEUELEN_TYPE   i                                       = 0;
    uint8_t               chkSum                                = 0;
    
    for (; i < len; i++)
    {
        chkSum                                                 += prscQueue[i];
    }    
    return chkSum|0x80;
}
#endif
