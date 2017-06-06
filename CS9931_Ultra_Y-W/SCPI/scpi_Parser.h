#include <stdint.h>
#ifndef    _SCPI_PARSER_H_
#define    _SCPI_PARSER_H_
#define                         QUEUE_MAX_LEN                          (100)


#if (QUEUE_MAX_LEN < 256)
    
    typedef                     unsigned char                          portQUEUELEN_TYPE; 
#elif (QUEUE_MAX_LEN < 65536)
    
    typedef                     unsigned short int                     portQUEUELEN_TYPE; 
#else
    
    typedef                     unsigned int                           portQUEUELEN_TYPE;  
#endif                  


// #define     		uint8_t    unsigned char                         
// #define         int8_t			signed char                                     
// #define         int8_t		unsigned short int                              
// #define         int16_t		signed short int                                
// #define         uint32_t		unsigned long int                               
// #define        int32_t		signed  long int                                
//��֧�����ָ���Ϊ8��
#define                         INSTRUCTION_MAX_SCALE                  (4)
//ָ�������� ��������֧�ֵ�ָ����������ĿΪ16��
#define                         INSTRUCTION_MAX_PARAM                  (16)


//�޵�λ����
#define                         SIGN_SYMBOL_INDEX_NONE                 (0xff)
#define                         SIGN_SYMBOL_INDEX_NA                   (0x00)
#define                         SIGN_SYMBOL_INDEX_UA                   (0x01)
#define                         SIGN_SYMBOL_INDEX_MA                   (0x02)
#define                         SIGN_SYMBOL_INDEX_A                    (0x03)
#define                         SIGN_SYMBOL_INDEX_V                    (0x04)
#define                         SIGN_SYMBOL_INDEX_KV                   (0x05)
#define                         SIGN_SYMBOL_INDEX_mOHM                 (0x06)
#define                         SIGN_SYMBOL_INDEX_OHM                  (0x07)
#define                         SIGN_SYMBOL_INDEX_KOHM                 (0x08)
#define                         SIGN_SYMBOL_INDEX_MOHM                 (0x09)
#define                         SIGN_SYMBOL_INDEX_GOHM                 (0x0a)
#define                         SIGN_SYMBOL_INDEX_TOHM                 (0x0b)
#define                         SIGN_SYMBOL_INDEX_MS                   (0x0c)
#define                         SIGN_SYMBOL_INDEX_S                    (0x0d)
#define                         SIGN_SYMBOL_INDEX_HZ                   (0x0e)
#define                         SIGN_SYMBOL_INDEX_W                    (0x0f)

/******************************************************************************
 *                           ָ�������Ϣ�궨��
******************************************************************************/ 

//ѹ���������ʱ ������ӷ��� ','
#define                         NO_APPENDED_SIGN                       ((uint32_t)-1)

#define                         INSTRUCTION_ATTR_NONE                  (0x00)                               
#define                         INSTRUCTION_ATTR_EXECUT                (0x01)
#define                         INSTRUCTION_ATTR_QUERY                 (0x02)
#define                         INSTRUCTION_ATTR_OPTIONAL              (0x04)
#define                         INSTRUCTION_ATTR_COMM                  (0x10)

//�꺯�� ����ָ�������е� ָ���  ���ָ���Ϊ8��
#define                         INSTRUCTION_ATTR_SCALES(scales)        (scales << 5)
//�꺯�� ��ȡָ��� ���ָ���Ϊ8��
#define                         INSTRUCTION_SCALES_GET(attr)           (attr >> 5)

//��һ�� ����ִ�е�ַ����Ϊ����ִ�б�ţ� �ڶ��� ��ѯ��������ִ�к���  ������ �������  ����1'\0'����2'\0'����3'\0'
//����������������й��� ���ݸ�ʽ�Դ�˷�ʽ��� ��ʹ����Ӧ�ĺ������л�ȡ ����˴�С�����ڴ���뷽���Ҫ��
typedef uint32_t (FP_pfInstructionExec)(int argc, const uint8_t *argv[]);

struct EXECUTE_VALIDITY_CHK_INFO;
//ִ�кϷ��Լ�麯��ָ��
typedef uint32_t (FP_pfValidityChk)(struct EXECUTE_VALIDITY_CHK_INFO *pexecuteValidityChkInfo);
//������з��ͺ���ָ��
typedef uint32_t (FP_pfOutputQueueSend)(const uint8_t *prscAddr, portQUEUELEN_TYPE len);

typedef struct
{
    uint8_t                                      *m_pdestSplitAddr;
    uint32_t                                      m_rawDecValue;
    uint8_t                                       m_rawConfigLen;
    uint8_t                                       m_rawDotValue;
    uint8_t                                       m_rawSignValue;

}RealParamSplitStruct;

//ʵ�����ݲ���Զ��庯��
typedef uint32_t (FP_pfRealParamSplit)(RealParamSplitStruct *prealParamSplitStruct);

/******************************************************************************
 *                         �ļ��ӿ����ݽṹ������
******************************************************************************/

/******************************************************************************
 *                       ִ�кϷ��Լ����Ϣ�ṹ�嶨��˵��
******************************************************************************/

#define                         VALIDITY_CHK_STEP_RREV_EXECUTE         (0)
#define                         VALIDITY_CHK_STEP_RREV_OUTPUT          (1)

struct EXECUTE_VALIDITY_CHK_INFO   
{
    FP_pfInstructionExec                       *m_pfinstructionExec;            //ִ�к���
    uint8_t                                       m_instructionAttr;              //ִ������                                          
    uint8_t                                       m_step;                         //�׶�
};
// typedef struct TEST_FENTCH_INFO_
// {
// 	uint32_t 		Test_Diaplay_VOL;
// 	uint32_t 		Test_Dispaly_CUR;
// 	uint32_t 		Test_Dispaly_IR;
// 	uint32_t        Test_Status;
// }FentchInfo;
// volatile FentchInfo t_FentchInfo;
// typedef struct _PARAM_INFO_
// {
//     volatile uint32_t	        Voltage_AC;
// 	volatile uint32_t	        Voltage_DC;

// 	volatile int32_t	        Current_AC;
// 	volatile int32_t	        Current_DC;
// 	volatile uint32_t			Current_AC_flag;
// 	volatile uint32_t			Current_DC_flag;
// 	volatile uint32_t			Current_AC_LOW_flag;
// 	volatile uint32_t			Current_DC_LOW_flag;

// 	volatile int32_t	        Current_AC_LOW;
// 	volatile int32_t	        Current_DC_LOW;

// 	volatile int32_t	        Time_AC;
// 	volatile int32_t	        Time_DC;
// 	volatile uint32_t	        Set_time_flag;
// 	volatile uint32_t			Set_time_dc_flag;

// 	volatile uint32_t			Arc_DC;
// 	volatile uint32_t			Arc_DC_Data;

// 	volatile uint32_t			Arc_AC;
// 	volatile uint32_t			Arc_AC_Data;
// 		
// 	volatile uint32_t	        Test_current_mode;
// 	volatile uint32_t	        Test_Mode;

// 	volatile uint32_t			AC_Test_display;
// 	volatile uint32_t         DC_Test_display;
// 	
// }ParamInfo;
// volatile ParamInfo st_ParamInfo;
typedef struct EXECUTE_VALIDITY_CHK_INFO ExecuteValidityChkInfo;

/******************************************************************************
 *                         ָ������ṹ�嶨��˵��
 ******************************************************************************
 *                                                                          
 *  ������ִ��ģ����֧�ֵ�����ָ������ṹ�嶨�����������:
 *  ��ָ��ֻ��һ��ʱ ʹ��SingleScaleParserStruct������ָ������ṹ��
 *  ��ָ�����һ��ʱ ʹ�� HeadScaleParserStruct, MiidleScaleParserStruct, TailScaleParserStruct�������������ṹ��                                                                       
 *                                                                                                              
 *  HeadScaleParserStruct:   �༶ָ���ͷ�����ṹ�� ����ָ���еĵ�һ��ָ�� �磺AA:BB:CC �е�AA��
 *                                                                          
 *  MidleScaleParserStruct:  �༶ָ��������������м�����ṹ�� ����ָ���е��м伶ָ�� �磺AA:BB:CC �е�BB��                                                                       
 *                           ������ ָ��ֻ������ָ��� ����ʹ��MidleScaleParserStruct�ṹ��������
 *                                                                                   
 *  TailScaleParserStruct:   �༶ָ���β�����ṹ�� ����ָ���е����һ��ָ�� �磺AA:BB:CC �е�CC��  
 *                           ��ָ��ֻ������ָ���ʱ ��ڶ���ָ����TailScaleParserStruct������
 *                           ��AA:BB �и��BB��ָ��                                            
 *                                                                          
 *  SingleScaleParserStruct: ���������ṹ�� ��ָ��ֻ����һ��ʱ �ô˽����ṹ                                                                       
 *                           �嶨��ָ��  ��*RSTָ���                                                                                   
 *                                                                                                                                                                          
 *                                                                                                                                                        
******************************************************************************/

/******************************************************************************
 *                           ָ������ṹ������
******************************************************************************/

//������ִ��ģ������֧��256����ָ��   ÿ��ָ�������ֿ����֧��256���Ӽ�ָ��   ����ָ������֧��8��

struct PARSER_CONTAINER;

//ǰ���������ṹ��
struct HEAD_SCALE_PARSER_STRUCT
{
    const uint8_t                                *m_pinstructionStr;                      //ָ���ַ�����ַ
    const struct PARSER_CONTAINER              *m_pnextParserContainer;                 //��һ����������
    uint8_t                                       m_longInstructionHashCode;              //��ָ���ϣ��
    uint8_t                                       m_shortInstructionHashCOde;             //��ָ���ϣ��
    uint8_t                                       m_instructionAttribute;                 //�������� �Ƿ�Ϊ��ѡָ��
    uint8_t                                       m_nextParserContainerCapacity;          //��һ��������������
};
                         
//m_instructionAttribute    ��HeadScaleParserStruct�ﺬ������
//bit   7   6   5   4   3   2   1   0
//bit7 - bit5 :��չλ ���ڹ�����չ
//bit4        :��չλ ���ڹ�����չ
//bit3        :��չλ ���ڹ�����չ
//bit2        :ָʾ����ָ���Ƿ��ѡ
//bit1        :��չλ ���ڹ�����չ
//bit0        :��չλ ���ڹ�����չ

typedef struct HEAD_SCALE_PARSER_STRUCT    HeadScaleParserStruct;

//m_instructionAttribute    ��MidleScaleParserStruct�ﺬ������
//bit   7   6   5   4   3   2   1   0
//bit7 - bit5 :��չλ ���ڹ�����չ
//bit4        :��չλ ���ڹ�����չ
//bit3        :��չλ ���ڹ�����չ
//bit2        :ָʾ����ָ���Ƿ��ѡ
//bit1        :��չλ ���ڹ�����չ
//bit0        :��չλ ���ڹ�����չ

typedef struct HEAD_SCALE_PARSER_STRUCT    MidleScaleParserStruct;

struct PARAM_TOTAL_CONTAINER;

//ĩ�������ṹ��
struct TAIL_SCALE_PARSER_STRUCT
{
    const uint8_t                                *m_pinstructionStr;                      //ָ���ַ�����ַ
    FP_pfInstructionExec                       *m_executeFunc;                          //ָ��ִ�к��� 
    const struct PARAM_TOTAL_CONTAINER         *m_pparamTotalContainer;                 //�����������ṹ��ָ��
    uint8_t                                       m_longInstructionHashCode;              //��ָ���ϣ��
    uint8_t                                       m_shortInstructionHashCOde;             //��ָ���ϣ��
    uint8_t                                       m_instructionAttribute;                 //��������
};

typedef struct TAIL_SCALE_PARSER_STRUCT    TailScaleParserStruct;

//m_instructionAttribute    ��TailScaleParserStruct�ﺬ������
//bit   7   6   5   4   3   2   1   0
//bit7 - bit5 :ָʾָ���  ���֧��8��ָ��
//bit4        :ָʾ��ָ���Ƿ�Ϊ�������� INSTRUCTION_ATTR_COMM
//bit3        :��չλ ���ڹ�����չ
//bit2        :��չλ ���ڹ�����չ
//bit1        :ָʾ��ָ���Ƿ�֧�ֲ�ѯ��ʽ
//bit0        :ָʾ��ָ���Ƿ�֧��ִ�и�ʽ

//���������ṹ�� ��ָ��ֻ����һ��ʱ �ô˽����ṹ�嶨��ָ��  ��*RSTָ���
typedef struct TAIL_SCALE_PARSER_STRUCT    SingleScaleParserStruct;

//m_instructionAttribute    ��SingleScaleParserStruct�ﺬ������
//bit   7   6   5   4   3   2   1   0
//bit7 - bit5 :ָʾָ���  ���֧��8��ָ��
//bit4        :ָʾ��ָ���Ƿ�Ϊ�������� INSTRUCTION_ATTR_COMM
//bit3        :��չλ ���ڹ�����չ
//bit2        :��չλ ���ڹ�����չ
//bit1        :ָʾ��ָ���Ƿ�֧�ֲ�ѯ��ʽ
//bit0        :ָʾ��ָ���Ƿ�֧��ִ�и�ʽ

/******************************************************************************
 *                            ָ�������������
******************************************************************************/

//������������ Ϊ�˷����Ժ�HeadScaleParserStruct MiidleScaleParserStruct �� TailScaleParserStruct������ 
//����ʹ�����������з�װ
typedef enum
{
    PARSER_SINGLE_SCALE = 0,
    //����������Я���Ĳ����������в�������ָ���һ����������ָ������ ����һ���װ ����˸�����
    //PCPA = PARAM CONTAINER PTR ARRAY
    PARSER_SINGLE_SCALE_PCPA,
    PARSER_HEAD_SCALE,
    PARSER_MIDDLE_SCALE,
    PARSER_TAIL_SCALE,
    //����������Я���Ĳ����������в�������ָ���һ����������ָ������ ����һ���װ ����˸�����
    PARSER_TAIL_SCALE_PCPA,                                                            

}ContainerInfo;

struct PARSER_CONTAINER
{
    void                                       *m_pparserStructAddr;
    ContainerInfo                               m_parserStructType;
};

typedef struct PARSER_CONTAINER    ParserContainer;

/******************************************************************************
 *                             ����������������
******************************************************************************/

//�����������ַ����� �ڱ������л���Ϊһ������ ������˵������˵�� �����û����������
//������໮��15�� ÿ�ֲ����п�ϸ��Ϊ15�� ���е�16�ֲ���Ϊ��Ч���� 
typedef enum
{
    //-----------------------
    //��������
    PARAM_TYPE_INTEGER          = 0x00,
    PARAM_TYPE_UINT8,
    PARAM_TYPE_INT8,
    PARAM_TYPE_UINT16,
    PARAM_TYPE_INT16,
    PARAM_TYPE_UINT32,
    PARAM_TYPE_INT32,
    //-----------------------
    //�ַ�������
    PARAM_TYPE_CHARACTER        = 0x10, 
    //-----------------------
    //��������
    PARAM_TYPE_STRING           = 0x20,
    //-----------------------
    //ʵ������
    PARAM_TYPE_REAL             = 0x30,
    PARAM_TYPE_REAL_UINT16,
    PARAM_TYPE_REAL_UINT32,
    //-----------------------
    //����������
    PARAM_TYPE_FLOAT            = 0x40,
    //-----------------------
    //��Ч�Ĳ�������
    PARAM_TYPE_NONE             = 0xf0,                                                           

}ParamTypeInfo;

//�������ͻ�ȡ
#define                         PARAM_TYPE_CLASS_GET(param)            (ParamTypeInfo)(param & 0xf0)

/******************************************************************************
 *                           ����������Ϣ����������
******************************************************************************/

//----------------------------�ܲ����������Ժ궨��----------------------------

//�������������ԣ�ִ��ָ�����
#define                         CONTAINER_ATTR_EXECUTE                 (0x01)
//�������������ԣ���ѯָ�����
#define                         CONTAINER_ATTR_QUERY                   (0x02)
//�������������ԣ�֧��ִ�С���ѯ����
#define                         CONTAINER_ATTR_DOUBLE                  (0x04)
//�������������ԣ�֧��ִ�С���ѯ�����е�һ��
#define                         CONTAINER_ATTR_SINGLE                  (0x00)

//--------------------------�ܲ�������������Ϣ�궨��---------------------------

//�ܲ���������Ϣ
#define                         TOTAL_PARAM_INFO_TOT_CNTS(addr)        (GET_ARRAY_COUNT(addr) & (INSTRUCTION_MAX_PARAM-1))
#define                         TOTAL_PARAM_INFO_TOT_CNTS_GET(info)    (info & 0x0f)
//��ѡ����������Ϣ  ����ڸ�16λ
#define                         TOTAL_PARAM_INFO_OPT_CNTS(cnts)        ((cnts & (INSTRUCTION_MAX_PARAM-1)) << 4)
#define                         TOTAL_PARAM_INFO_OPT_CNTS_GET(info)    ((info & 0xf0) >> 4)

//---------------------------��������������Ϣ�궨��----------------------------

#define                         PARAM_INFO_NONE                        (0x00)
//����֧��DEF����  �ڷ���DEF��DEFaultʱ �������
#define                         PARAM_INFO_DEF                         (0x80)
//��������ʮ�������������������������ֵ���䵵λֵ ������������и��������9.000_Mohm ��֮�����9000_1
#define                         PARAM_INFO_DEC_OUT                     (0x40)
//�����Թ����� ���ԶԴ˲����ĺϷ����ж� ֱ��д�뵽��������
#define                         PARAM_INFO_SKIP                        (0x20)

//ʵ������������Ϣ�궨��
//ʵ�����������ø�����Ϣ Ŀǰ���֧�ֵ�16�� ����һ���Ӧ�������Ѿ��㹻
#define                         REAL_PARAM_INFO_SECTION_CNTS(cnts)     ((GET_ARRAY_COUNT(cnts)) & 0x0f)
#define                         REAL_PARAM_INFO_SECTION_CNTS_GET(info) (info & 0x0f)
//�������磺2482 V�����ַ���ʱ �Ƿ��Զ�ת���� 2.482 kV��
#define                         REAL_PARAM_INFO_NO_AUTO_SIGN           (0x10)

/******************************************************************************
 *                                ������������
******************************************************************************/

struct PARAM_CONTAINER;
//������
struct PARAM_TOTAL_CONTAINER
{
    struct PARAM_CONTAINER                     *m_pparamContainer;
    //������Ϣ ָʾ����ִ�в������� ���� ��ѯ�������� ��ָ��ͬʱ֧��ִ�в������ѯ����ʱ ���ܲ���������
    //����������Ϣ ��ֵΪCONTAINER_ATTR_SINGLE��CONTAINER_ATTR_EXECUTE��CONTAINER_ATTR_DOUBLE��CONTAINER_ATTR_QUERY
    //�����
    uint8_t                                       m_containerAttr;
    //������Ϣ ��Ϊ����λ�����λ ����֧��16λ����
    //����λ����ѡ��������
    //����λ���ܲ�������
    uint8_t                                       m_paramInfo;

};
//������Ϊһ������������ԭ���Ľṹ������ ���е�һ����Ӧִ�в���������  �ڶ�����Ӧ��ѯ����������
typedef struct PARAM_TOTAL_CONTAINER ParamTotalContainer;

struct PARAM_CONTAINER
{
    void                                       *m_pparamStructAddr;
    //����������Ϣ ����ȡֵ�ο�ParamTypeInfo����
    ParamTypeInfo                               m_paramStructType;
    //���ڲ�ͬ�Ĳ����ṹ������ ���в�ͬ�ĺ���
    //����PARAM_TYPE_REAL_UINT16��PARAM_TYPE_REAL_UINT32����
    //����λ
    uint8_t                                       m_paramStructInfo; 
};

typedef struct PARAM_CONTAINER    ParamContainer;

typedef struct 
{
    uint8_t                                       m_upperLimit;
    uint8_t                                       m_lowerLimit;

}ParamUint8Struct;

typedef struct 
{
     int8_t                                       m_upperLimit;
     int8_t                                       m_lowerLimit;

}ParamInt8Struct;

typedef struct 
{
    uint16_t                                      m_upperLimit;
    uint16_t                                      m_lowerLimit;

}ParamUint16Struct;

typedef struct 
{
     int16_t                                      m_upperLimit;
     int16_t                                      m_lowerLimit;

}ParamInt16Struct;

typedef struct 
{
    uint32_t                                      m_upperLimit;
    uint32_t                                      m_lowerLimit;

}ParamUint32Struct;

typedef struct 
{
     int32_t                                      m_upperLimit;
     int32_t                                      m_lowerLimit;

}ParamInt32Struct;

//�ַ���Ԫ
typedef struct
{
    uint8_t                                      *m_plabelStr;
    uint8_t                                       m_value;

}CharacterUnit;

//�ַ������ṹ��
typedef struct 
{
     const CharacterUnit                       *m_pcharacterTable;
     uint8_t                                      m_characterTableNumbs;

}ParamCharacterStruct;

//�����������ַ����͵�һ���Ӽ�
typedef     ParamCharacterStruct    ParamBooleanStruct;

//�����ͽṹ��
typedef struct 
{
     uint8_t                                     *const m_pinvalidStr;

}ParamStringStruct;

/******************************************************************************
 *                              ��λ������������
******************************************************************************/

//������Ϣ�궨��
//ʵ��������Ϣ -> С����λ��
//��С����
#define                         REAL_CFG_INFO_NO_DOT                   (0x00)
#define                         REAL_CFG_INFO_DOT_POS(dotPos)          (dotPos & 0x0f)
#define                         REAL_CFG_INFO_DOT_POS_GET(configInfo)  (configInfo & 0x0f)
//ʵ��������Ϣ -> ���峤�� ���ж�
#define                         REAL_CFG_INFO_STR_LEN(len)             (len << 4)
#define                         REAL_CFG_INFO_STR_LEN_GET(configInfo)  (configInfo >> 4)


//ʵ�����ͽṹ�� ���ֵΪ65535 ��Сֵ�����ֵ���� С����λ�ö���ͬ
typedef struct
{
    uint16_t                                      m_lowerLimit;
    uint16_t                                      m_upperLimit;
    //������Ϣ   ��4λΪ�ַ�����ȥС����֮��ĳ���  ��4λΪС�������ַ����ڵ�ƫ��
    uint8_t                                       m_configInfo;
    //������Ϣ                                  
    uint8_t                                       m_signSymbolIndex;

}ParamRealUint16Struct;

//ʵ�����ͽṹ�� ���ֵΪuint32���� ��Сֵ�����ֵ���� С����λ�ö���ͬ
typedef struct
{
    uint32_t                                      m_lowerLimit;
    uint32_t                                      m_upperLimit;
    //������Ϣ   ��4λΪ�ַ�����ȥС����֮��ĳ���  ��4λΪС�������ַ����ڵ�ƫ��
    uint8_t                                       m_configInfo;
    //������Ϣ                                  
    uint8_t                                       m_signSymbolIndex;

}ParamRealUint32Struct;

/******************************************************************************
 *                         ����ִ������ṹ������
******************************************************************************/

typedef enum
{
    STATUS_NO_ERROR  = 0,
    STATUS_SYNTAX_ERROR,
    STATUS_EXECUTE_NOT_ALLOWED,
    STATUS_PARAM_NOT_ALLOWED,
    STATUS_MISSING_PARAM,
    STATUS_UNDEFINED_HEADER,
    STATUS_PARAM_TYPE_ERROR,
    STATUS_PARAM_LENGTH_ERROR,
    STATUS_INVALID_STRING_DATA,
    STATUS_EXECUTE_TIME_OUT,
    STATUS_DATA_OUT_OF_RANGE,
    STATUS_OUTPUT_QUEUE_FULL,
    STATUS_SUM_CHECK_ERROR,

}ParserErrCode;



extern const ParserContainer                    t_ParserContainer_Root_CS5051[7];
/******************************************************************************
 *                             �ļ��ӿ���������
******************************************************************************/ 

extern const uint8_t *const API_PEM_pParserExeModulerErrCodeArray[];

/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/

//������������ַ������������������������ַ��������������Ӧ�����
extern void API_PEMInit(ParserContainer *ptrootParserContainer, uint8_t prootParserContainerCapacity,
                        uint8_t endCodeIndex, uint8_t responsionCtrl, FP_pfOutputQueueSend *pfOutputQueueSend);
                                 
extern void API_PEMExecute(void);

extern void int_API_PEMRead(uint8_t recvMsg);

extern uint32_t API_PEMOutputQueueErrMsgPush(ParserErrCode parserErrCode, uint32_t appendSign);

extern uint32_t API_PEMExecuteErrCodeSet(ParserErrCode executeErrCode);

extern uint32_t API_PEMOutputQueueStrPush(const uint8_t *prscStr, uint32_t appendSign);

extern uint32_t API_PEMOutputQueueStrnPush(const uint8_t *prscStr, uint32_t len, uint32_t appendSign);

extern uint32_t API_PEMOutputQueueIntegerPush(uint32_t rscData, uint32_t len, uint32_t appendSign);

extern uint32_t API_PEMOutputQueueCharPush(uint32_t appendChar, uint32_t appendSign);

extern void API_PEMParamQueueIntegerWrite(uint32_t rscValue, uint8_t len);

extern void API_PEMParamQueueCharWrite(uint8_t rscChar);

extern void API_PEMParamQueueSplitSignWrite(uint8_t rscChar);

extern void API_PEMParamQueuePosAdjust(uint16_t len);

extern uint8_t *API_PEMParamQueueCurrentPtrGet(void);

extern uint8_t API_PEMItegerParamReadUint8(const uint8_t *rscAddr);

extern int8_t API_PEMItegerParamReadInt8(const uint8_t *rscAddr);

extern uint16_t API_PEMItegerParamReadUint16(const uint8_t *rscAddr);

extern int16_t API_PEMItegerParamReadInt16(const uint8_t *rscAddr);

extern uint32_t API_PEMItegerParamReadUint32(const uint8_t *rscAddr);

extern int32_t API_PEMItegerParamReadInt32(const uint8_t *rscAddr);

extern void API_PEMPrevValidityChkFuncInstall(FP_pfValidityChk *pfprevExecuteValidityChk);

extern void API_PEMPrevValidityChkFuncUninstall(void);

extern void API_PEMRealParamSplitFuncInstall(FP_pfRealParamSplit *pfRealParamSplit);

extern void API_PEMRealParamSplitFuncUninstall(void);

extern void _API_PEMInputQueueFlush(void);
 
extern void _API_PEMOutputQueueFlush(void);


//������з��ͺ��� �˺����������ⲿʵ�� 
extern void port_API_PEMOutputQueueSend(void);

#endif

