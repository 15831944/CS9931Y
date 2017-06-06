/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
 #include "stm32f4xx.h"




#include "Communication.h"
#include "macro.h"
#include "ScpiInstructionTable.h"
#include "CS9931CInstructionTable.h"





/******************************************************************************
 *                            ���ļ��ڲ��궨��
******************************************************************************/

/******************************************************************************
 *                       ���ļ�������ľ�̬���ݽṹ
******************************************************************************/

/******************************************************************************
 *                       ���ļ��������ȫ�ֽṹ��
******************************************************************************/

/******************************************************************************
 *                           ָ��ȫ�ֽṹ��
******************************************************************************/

/******************************************************************************
 *                           ����ȫ�ֽṹ��
******************************************************************************/

static const ParamBooleanStruct         st_BooleanParamStruct;
static const ParamContainer             st_ParamContainer_Boolean[];
static const ParamTotalContainer        st_TotalContainer_Boolean[];

/******************************************************************************
 *                           ָ�Ϊ���νṹ
 *                                ��ָ�
 *                        AA                      BB
 *                   CC   DD   EE             FF  GG  HH
 *              ZZ   MM   NN                          UU II OO PP
 *
******************************************************************************/ 

/******************************************************************************
 *                           ����ָ�����
******************************************************************************/ 

//*RST��������   *��ֻ��һ����������ı�ʶ�� ��������������һ����  ��������Ҳ��Ϊ��һ��ָ���ָ��
const SingleScaleParserStruct   t_InstructionStruct_RST = {
    
        "RST", InstructionExec_RST, NULL, 52, 52, INSTRUCTION_ATTR_SCALES(1)|INSTRUCTION_ATTR_COMM|INSTRUCTION_ATTR_EXECUT,
    };

//*IDN��������
const SingleScaleParserStruct   t_InstructionStruct_IDN = {
    
        "IDN", InstructionExec_IDN, NULL, 100, 100, INSTRUCTION_ATTR_SCALES(1)|INSTRUCTION_ATTR_COMM|INSTRUCTION_ATTR_QUERY,
    };
// static const ParserContainer st_ParserContainer_Src[] = {
//     
//         {(void *)&t_InstructionStruct_SrcTest , PARSER_MIDDLE_SCALE},
//         //{(void *)&t_InstructionStruct_SrcLoad , PARSER_MIDDLE_SCALE},
//         //{(void *)&st_InstructionStruct_SrcList, PARSER_MIDDLE_SCALE},
//     };
/******************************************************************************
 *                           Դָ������������
******************************************************************************/
//Դ->�б�->�ļ�������ָ��
const TailScaleParserStruct   t_InstructionStruct_SourceListFIndex = {
    
        "FINDex", InstructionExec_SourceListFIndex, 0, 1, 87, 
        INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_SrcListMode = {
    
        "MODE", InstructionExec_SourceListMode, 0, 181, 181, 
        INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

//Դ-�б�ָ�����������
static const ParserContainer st_ParserContainer_SrcList[] = {
    
        {(void *)&t_InstructionStruct_SrcListMode    , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_SourceListFIndex, PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_SrcListSindex  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_SrcListSmessage, PARSER_TAIL_SCALE},
    };

static const MidleScaleParserStruct   st_InstructionStruct_SrcList = {
    
        "LIST", st_ParserContainer_SrcList, 170, 170, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_SrcList),
    };

//Դָ��"SOURce"�ڶ�������
static const ParserContainer st_ParserContainer_Src[] = {
    
        {(void *)&t_InstructionStruct_SrcTest , PARSER_MIDDLE_SCALE},
        {(void *)&t_InstructionStruct_SrcLoad , PARSER_MIDDLE_SCALE},
        {(void *)&st_InstructionStruct_SrcList, PARSER_MIDDLE_SCALE},
    };
const HeadScaleParserStruct   t_InstructionStruct_Source = {
    
        "SOURce", st_ParserContainer_Src, 26, 12, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_Src),
    };




/******************************************************************************
 *                             Դָ�������
******************************************************************************/
//���ز��Բ�ָ���������
static const ParamUint8Struct st_ParamUint8Struct_SrcLoadStep = {50, 1};

static const ParamContainer st_ParamContainer_SrcLoadStep[] = {
    
        (void *)&st_ParamUint8Struct_SrcLoadStep, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    }; 

static const ParamTotalContainer st_TotalContainer_SrcLoadStep[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_SrcLoadStep, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_SrcLoadStep),
    };

static const CharacterUnit t_ParamReal_Test_Mode_5601[] = {
    
        {"ACW"   	  , '0'},
        {"DCW"   	  , '1'},
				{"IR"   	  , '2'},
				{"ACWTOIR"	, '3'},
				{"DCWTOIR"	, '4'},
				{"IRTOACW"	, '5'},
				{"IRTODCW"	, '6'},
		};

static const CharacterUnit t_ParamReal_Test_Mode_9931[] = {
    
        {"ACW"   	  , '0'},
				{"DCW"   	  , '1'},
				{"IR"	      , '2'},
				{"GR"	      , '3'},
				{"LC"	      , '4'},
    };

static const CharacterUnit t_ParamReal_Test_Mode_5050[] = {
    
        {"ACW"   	, '0'},
    };

static const CharacterUnit t_ParamReal_Test_Mode_5051[] = {
    
        {"ACW"   	, '0'},
				{"DCW"   	, '1'},
    };
static  ParamBooleanStruct st_ParamRealUint16Struct_Test_Mode = {
    
        (void *)t_ParamReal_Test_Mode_5601, GET_ARRAY_COUNT(t_ParamReal_Test_Mode_5601),

    };

static const ParamContainer st_ParamContainer_Test_Mode[] = {
    
        (void *)&st_ParamRealUint16Struct_Test_Mode, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };

static const ParamTotalContainer st_TotalContainer_Test_Mode[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Test_Mode, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Test_Mode),
    };                                                           
//-----------------------------------------------------------------------------

//Դ-���Ե�����ָ��ṹ��
static const TailScaleParserStruct   t_InstructionStruct_SrcTestStart = {
    
        "STARt" , InstructionExec_SrcTestStart, NULL, 22, 67, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT,
    };

static const TailScaleParserStruct   t_InstructionStruct_SrcTestStop = {
    
        "STOP"  , InstructionExec_SrcTestStop, NULL, 72, 72, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT,
    };

static const TailScaleParserStruct   t_InstructionStruct_SrcTestStatus = {
    
        "STATus", InstructionExec_SrcTestStatus, NULL, 127, 69, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_SrcTestFetch = {
    
        "FETCh" , InstructionExec_SrcTestFetch, NULL, 148, 88, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_SrcTestMode = {
    
        "MODE"  , InstructionExec_SrcTestMode, st_TotalContainer_Test_Mode, 181, 181, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//Դ-���Ե�����ָ������
const ParserContainer t_ParserContainer_SrcTest[] = {
    
        {(void *)&t_InstructionStruct_SrcTestStart , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_SrcTestStop  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_SrcTestStatus, PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_SrcTestFetch , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_SrcTestMode  , PARSER_TAIL_SCALE},
    };
extern uint32_t usart_cur_step_set(int argc, const uint8_t *argv[]);
//Դ-���ص�����ָ��ṹ��
static const TailScaleParserStruct   t_InstructionStruct_SrcLoadStep = {
    
        "STEP" , usart_cur_step_set, st_TotalContainer_SrcLoadStep, 67, 67, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT,
    };
extern uint32_t usart_total_step_set(int argc, const uint8_t *argv[]);
static const TailScaleParserStruct   t_InstructionStruct_SrcLoadTotalStep = {
    
        "TOTS" , usart_total_step_set, st_TotalContainer_SrcLoadStep, 172, 172, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT,
    };
//Դ-����ָ�����������
const ParserContainer t_ParserContainer_SrcLoad[] = {
    
			{(void *)&t_InstructionStruct_SrcLoadStep , PARSER_TAIL_SCALE},
			{(void *)&t_InstructionStruct_SrcLoadTotalStep , PARSER_TAIL_SCALE},
    };


//Դ-�б������ָ��ṹ��

const TailScaleParserStruct   t_InstructionStruct_SrcListSindex = {
    
        "SINDex"  , NULL, NULL, 105, 248, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };  

extern uint32_t usart_cur_step_message_get(int argc, const uint8_t *argv[]);
const TailScaleParserStruct   t_InstructionStruct_SrcListSmessage = {
    
        "SMESsage", usart_cur_step_message_get, NULL, 40, 132, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };
     
//Դָ��ڶ���ָ��ṹ��
const MidleScaleParserStruct   t_InstructionStruct_SrcTest = {
    
        "TEST", t_ParserContainer_SrcTest, 170, 170, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_SrcTest),
    };

const MidleScaleParserStruct   t_InstructionStruct_SrcLoad = {
    
        "LOAD", t_ParserContainer_SrcLoad, 19 , 19 , INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_SrcLoad),
    };

/******************************************************************************
 *                            ����ָ�������
******************************************************************************/

//LCģʽ������������ ����
static const ParamRealUint16Struct st_ParamRealUint16Struct_CurrHigh[] = {
    
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(1), SIGN_SYMBOL_INDEX_NA},	//1.00-9.99nA
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(2), SIGN_SYMBOL_INDEX_NA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_NO_DOT    , SIGN_SYMBOL_INDEX_NA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(1), SIGN_SYMBOL_INDEX_UA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(2), SIGN_SYMBOL_INDEX_UA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_NO_DOT    , SIGN_SYMBOL_INDEX_UA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(1), SIGN_SYMBOL_INDEX_MA},
        {100, 200, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(2), SIGN_SYMBOL_INDEX_MA},	//10.0-20.0mA
    };

static const ParamContainer st_ParamContainer_CurrHigh[] = {
    
        (void *)&st_ParamRealUint16Struct_CurrHigh, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_CurrHigh), //PARAM_ATTR_DECIMAL_OUTPUT,
    };

static const ParamTotalContainer st_TotalContainer_CurrHigh[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_CurrHigh, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_CurrHigh),
    };

//LCģʽ������������ ����
static const ParamRealUint16Struct st_ParamRealUint16Struct_CurrLow[] = {
    
        {0  , 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(1), SIGN_SYMBOL_INDEX_NA},	//0.00-9.99nA
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(2), SIGN_SYMBOL_INDEX_NA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_NO_DOT    , SIGN_SYMBOL_INDEX_NA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(1), SIGN_SYMBOL_INDEX_UA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(2), SIGN_SYMBOL_INDEX_UA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_NO_DOT    , SIGN_SYMBOL_INDEX_UA},
        {100, 999, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(1), SIGN_SYMBOL_INDEX_MA},
        {100, 200, REAL_CFG_INFO_STR_LEN(3)|REAL_CFG_INFO_DOT_POS(2), SIGN_SYMBOL_INDEX_MA},	//10.0-20.0mA
    };

static const ParamContainer st_ParamContainer_CurrLow[] = {
    
        (void *)&st_ParamRealUint16Struct_CurrLow, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_CurrLow), //PARAM_ATTR_DECIMAL_OUTPUT,
    };



//-----------------------------------------------------------------------------

//LCģʽ��������������

static const ParamRealUint16Struct st_ParamRealUint16Struct_CCurr[] = {
    
        //{100, 999, REAL_CFG_STR_LEN(3)|2, SIGN_SYMBOL_INDEX_MA},    //10.0-99.9mA
        {10, 500, REAL_CFG_INFO_STR_LEN(3)|0, SIGN_SYMBOL_INDEX_MA},    //10-500mA
    };

static const ParamContainer st_ParamContainer_CCurr[] = {
    
        (void *)&st_ParamRealUint16Struct_CCurr, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_CCurr), //PARAM_ATTR_DECIMAL_OUTPUT,
    };

static const ParamTotalContainer st_TotalContainer_CCurr[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_CCurr, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_CCurr),
    };

//-----------------------------------------------------------------------------

//���ģʽ����������
static const CharacterUnit   t_CharacterParamUnit[] = {
    
        {"AUTO"  , '0'},
        {"MANUAL", '1'},
    };

static const ParamBooleanStruct st_ParamCharacterStruct_OutMode = {
    
        (void *)t_CharacterParamUnit, GET_ARRAY_COUNT(t_CharacterParamUnit),
    };

static const ParamContainer st_ParamContainer_OutMode[] = {
    
        (void *)&st_ParamCharacterStruct_OutMode, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    }; 

static const ParamTotalContainer st_TotalContainer_OutMode[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_OutMode, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_OutMode),
    };

//���ģʽ����������
static const CharacterUnit   t_CharacterParamUnitLcFContinue[] = {
    
        {"Y", '1'},
        {"N", '0'},
        {"1", '1'},
        {"0", '0'},
    };

static const ParamBooleanStruct st_ParamCharacterStruct_LcFContinue = {
    
        (void *)t_CharacterParamUnitLcFContinue, GET_ARRAY_COUNT(t_CharacterParamUnitLcFContinue),
    };

static const ParamContainer st_ParamContainer_LcFContinue[] = {
    
        (void *)&st_ParamCharacterStruct_LcFContinue, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    }; 

static const ParamTotalContainer st_TotalContainer_LcFContinue[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_LcFContinue, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_OutMode),
    };

//-----------------------------------------------------------------------------   

//��ѹ����������

//��Ե���Ե�ѹ����
static const CharacterUnit t_ParamReal_Voltage[] = {
    
//         {"100V", '0'},
        {"250V", '1'},
        {"500V", '2'},
        {"1000V",'3'},
    };

static const CharacterUnit t_ParamReal_Voltage_2676N[] = {
    
        {"500V", '2'},
        {"1000V",'3'},
    };
//ֱ����ѹ����
// static const CharacterUnit t_ParamReal_Voltage_DC[] = {
//     
//         {"3000V", '0'},
//         {"5000V", '1'},
//     };
static const ParamRealUint16Struct t_ParamReal_Voltage_AC[] = {
    
        {50  , 5000, REAL_CFG_INFO_STR_LEN(4)|1, SIGN_SYMBOL_INDEX_NONE},	//0.050-5.000kV
    };	

static const ParamRealUint16Struct t_ParamReal_Voltage_DC[] = {
    
        {50  , 6000, REAL_CFG_INFO_STR_LEN(4)|1, SIGN_SYMBOL_INDEX_NONE},	//0.050-6.000kV
    };	

static const ParamRealUint16Struct t_ParamReal_Voltage_LC[] = {
    
        {0  , 3000, REAL_CFG_INFO_STR_LEN(4)|3, SIGN_SYMBOL_INDEX_NONE},	//0.0-300.0V
    };

static const ParamRealUint16Struct t_ParamReal_AcwCurRange[] = {
    
        {0  , 3,    REAL_CFG_INFO_STR_LEN(1), SIGN_SYMBOL_INDEX_NONE},	//0-3
    };

static const ParamRealUint16Struct t_ParamReal_DcwCurRange[] = {
    
        {0  , 5,    REAL_CFG_INFO_STR_LEN(1), SIGN_SYMBOL_INDEX_NONE},	//0-3
    };

static const ParamRealUint16Struct t_ParamReal_LcCurRange[] = {
    
        {0  , 2,    REAL_CFG_INFO_STR_LEN(1), SIGN_SYMBOL_INDEX_NONE},	//0-3
    };

static const ParamRealUint16Struct t_ParamReal_Current_GR[] = {
    
        {30  , 320, REAL_CFG_INFO_STR_LEN(3)|2, SIGN_SYMBOL_INDEX_NONE},	//0.20-10.00kV
    };
static const CharacterUnit t_ParamReal_Voltage_DC_5101[] = {
    
        {"5000V", '1'},
				{"10000V", '2'},
    };
//ֱ��ARC�����ȼ����ò���	
static const CharacterUnit t_ParamReal_Arc_DC[] = {
    
				{"0", '0'},
				{"1", '1'},
				{"2", '2'},
				{"3", '3'},
				{"4", '4'},
				{"5", '5'},
				{"6", '6'},
				{"7", '7'},
				{"8", '8'},
				{"9", '9'},
    };




/**************************************************************************************************************/
static const ParamBooleanStruct st_ParamRealUint16Struct_Voltage = {
    
        (void *)t_ParamReal_Voltage, GET_ARRAY_COUNT(t_ParamReal_Voltage),
    };

static const ParamBooleanStruct st_ParamRealUint16Struct_Voltage_2676N = {
    
        (void *)t_ParamReal_Voltage_2676N, GET_ARRAY_COUNT(t_ParamReal_Voltage_2676N),
    };

// static const ParamBooleanStruct st_ParamRealUint16Struct_Voltage_DC = {
//     
//         (void *)t_ParamReal_Voltage_DC, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
//     };

static const ParamBooleanStruct st_ParamRealUint16Struct_Voltage_DC_5101 = {
    
        (void *)t_ParamReal_Voltage_DC_5101, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC_5101),
    };

static const ParamBooleanStruct st_ParamRealUint16Struct_Arc_DC = {
    
        (void *)t_ParamReal_Arc_DC, GET_ARRAY_COUNT(t_ParamReal_Arc_DC),
    };

// static const ParamBooleanStruct st_ParamRealUint16Struct_FREQuency = {
//     
//         (void *)t_ParamReal_FREQuency, GET_ARRAY_COUNT(t_ParamReal_FREQuency),
//     };


/**************************************************************************************************************/
static const ParamContainer st_ParamContainer_Voltage[] = {
    
        (void *)&st_ParamRealUint16Struct_Voltage, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };

static const ParamContainer st_ParamContainer_Voltage_2676N[] = {
    
        (void *)&st_ParamRealUint16Struct_Voltage_2676N, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };

static const ParamContainer st_ParamContainer_Voltage_AC[] = {
    
        (void *)&t_ParamReal_Voltage_AC, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
    };

static const ParamContainer st_ParamContainer_Voltage_DC[] = {
    
        (void *)&t_ParamReal_Voltage_DC, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
    };

static const ParamContainer st_ParamContainer_Voltage_LC[] = {
    
        (void *)&t_ParamReal_Voltage_LC, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
    };

static const ParamContainer st_ParamContainer_AcwCurRange[] = {
    
        (void *)&t_ParamReal_AcwCurRange, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
    };

static const ParamContainer st_ParamContainer_DcwCurRange[] = {
    
        (void *)&t_ParamReal_DcwCurRange, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
    };

static const ParamContainer st_ParamContainer_LcCurRange[] = {
    
        (void *)&t_ParamReal_LcCurRange, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Voltage_DC),
    };

static const ParamContainer st_ParamContainer_Current_GR[] = {
    
        (void *)&t_ParamReal_Current_GR, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_Current_GR),
    };

static const ParamContainer st_ParamContainer_Voltage_DC_5101[] = {
    
        (void *)&st_ParamRealUint16Struct_Voltage_DC_5101, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };

static const ParamContainer st_ParamContainer_Arc_DC[] = {
    
        (void *)&st_ParamRealUint16Struct_Arc_DC, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };




/**************************************************************************************************************/
static const ParamTotalContainer st_TotalContainer_Voltage[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Voltage, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Voltage),
    };

static const ParamTotalContainer st_TotalContainer_Voltage_2676N[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Voltage_2676N, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Voltage_2676N),
    };

static const ParamTotalContainer st_TotalContainer_Voltage_AC[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Voltage_AC, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Voltage_AC),
    };

static const ParamTotalContainer st_TotalContainer_Voltage_DC[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Voltage_DC, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Voltage_DC),
    };

static const ParamTotalContainer st_TotalContainer_Voltage_LC[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Voltage_LC, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Voltage_LC),
    };

static const ParamTotalContainer st_TotalContainer_AcwCurRange[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_AcwCurRange, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_AcwCurRange),
    };

static const ParamTotalContainer st_TotalContainer_DcwCurRange[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_DcwCurRange, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_DcwCurRange),
    };

static const ParamTotalContainer st_TotalContainer_LcCurRange[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_LcCurRange, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_LcCurRange),
    };


static const ParamTotalContainer st_TotalContainer_Current_GR[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Current_GR, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Current_GR),
    };


static const ParamTotalContainer st_TotalContainer_Arc_DC[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Arc_DC, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Arc_DC),
    };


// Ƶ�ʲ���
static const ParamRealUint16Struct st_ParamRealUint16Struct_FREQuency[] = {
    
        {500  , 4000, REAL_CFG_INFO_STR_LEN(4)|3, SIGN_SYMBOL_INDEX_NONE},	//50.0-400.0Hz
    };

static const ParamContainer st_ParamContainer_FREQuency[] = {
    
        (void *)&st_ParamRealUint16Struct_FREQuency, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_FREQuency),
    };

static const ParamTotalContainer st_TotalContainer_FREQuency[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_FREQuency, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_FREQuency),
    };

const CharacterUnit   t_CharacterParamUnit_StepLcPHASe[] = {
    
        {"L", 0},
        {"N", 1},
        {"0", 0},
        {"1", 1},
    };

static const ParamBooleanStruct st_ParamCharacterStruct_StepLcPHASe = {
    
        t_CharacterParamUnit_StepLcPHASe, GET_ARRAY_COUNT(t_CharacterParamUnit_StepLcPHASe),
    };

const const ParamContainer st_ParamContainer_StepLcPHASe[] = {
    
        (void *)&st_ParamCharacterStruct_StepLcPHASe, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };


//-----------------------------------------------------------------------------


//ʱ�����������
static const ParamRealUint16Struct st_ParamRealUint16Struct_Time[] = {
    
        {0  , 9999, REAL_CFG_INFO_STR_LEN(4)|3, SIGN_SYMBOL_INDEX_NONE},	//000.0-999.9s
    };
static const ParamContainer st_ParamContainer_Time[] = {
    
        (void *)&st_ParamRealUint16Struct_Time, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_Time),
    };

static const ParamTotalContainer st_TotalContainer_Time[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Time, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Time),
    };

//-----------------------------------------------------------------------------
/**************************************************************************************************************/
//�������޲���������
static const ParamRealUint16Struct st_ParamRealUint16Struct_ResLow[] = {
    
        {1, 9999, REAL_CFG_INFO_STR_LEN(4)|0, SIGN_SYMBOL_INDEX_NONE},	//100.0-999.9K
    };
// static const ParamRealUint16Struct st_ParamRealUint16Struct_GrLow[] = {
//     
//         {1, 9999, REAL_CFG_INFO_STR_LEN(4)|3, SIGN_SYMBOL_INDEX_mOHM },	//100.0-999.9
//     };
static const ParamRealUint16Struct st_ParamRealUint16Struct_GrLow[] = {
    
        {0, 9999, REAL_CFG_INFO_STR_LEN(4)|4, SIGN_SYMBOL_INDEX_NONE },	//100.0-999.9
    };
static const ParamContainer st_ParamContainer_ResLow[] = {
    
        (void *)&st_ParamRealUint16Struct_ResLow, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_ResLow),
    };
static const ParamContainer st_ParamContainer_GrLow[] = {
    
        (void *)&st_ParamRealUint16Struct_GrLow, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_GrLow),
    };

static const ParamTotalContainer st_TotalContainer_ResLow[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_ResLow, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_ResLow),
    };
static const ParamTotalContainer st_TotalContainer_GrLow[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_GrLow, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_GrLow),
    };
/**************************************************************************************************************/
//�������޲���������
static const ParamRealUint16Struct st_ParamRealUint16Struct_ResHigh[] = {
    
       {0,9999, REAL_CFG_INFO_STR_LEN(4)|0, SIGN_SYMBOL_INDEX_NONE},
    };
// static const ParamRealUint16Struct st_ParamRealUint16Struct_GrHigh[] = {
//     
//        {0,9999, REAL_CFG_INFO_STR_LEN(4)|3, SIGN_SYMBOL_INDEX_mOHM},
//     };
static const ParamRealUint16Struct st_ParamRealUint16Struct_GrHigh[] = {
    
       {0,9999, REAL_CFG_INFO_STR_LEN(4)|4, SIGN_SYMBOL_INDEX_NONE},
    };


static const ParamRealUint32Struct st_ParamRealUint32Struct_DcwCurHigh[] = {
    
//        {1  ,2000, REAL_CFG_INFO_STR_LEN(4)|1, SIGN_SYMBOL_INDEX_MA},
	   {0  ,200000, REAL_CFG_INFO_STR_LEN(6)|4, SIGN_SYMBOL_INDEX_NONE},
    };
static const ParamRealUint32Struct st_ParamRealUint32Struct_AcwCurHigh[] = {
    
       {0  ,200000, REAL_CFG_INFO_STR_LEN(6)|4, SIGN_SYMBOL_INDEX_NONE},
// 	   {201,2000, REAL_CFG_INFO_STR_LEN(4)|2, SIGN_SYMBOL_INDEX_MA},
// 	   {201,1000, REAL_CFG_INFO_STR_LEN(4)|3, SIGN_SYMBOL_INDEX_MA},
    };

static const ParamRealUint16Struct st_ParamRealUint16Struct_DcwCurHigh_2676N[] = {
    
       {1  ,2000, REAL_CFG_INFO_STR_LEN(4)|1, SIGN_SYMBOL_INDEX_MA},
	   {201,1000, REAL_CFG_INFO_STR_LEN(4)|2, SIGN_SYMBOL_INDEX_MA},
    };

static const ParamRealUint16Struct st_ParamRealUint16Struct_AcwCurHigh_2676N[] = {
    
       {1  ,2000, REAL_CFG_INFO_STR_LEN(4)|1, SIGN_SYMBOL_INDEX_NONE},
	   {201,2000, REAL_CFG_INFO_STR_LEN(4)|2, SIGN_SYMBOL_INDEX_MA},
    };

static const ParamRealUint32Struct st_ParamRealUint32Struct_DcwCurLow[] = {
    
       {0  ,200000, REAL_CFG_INFO_STR_LEN(6)|4, SIGN_SYMBOL_INDEX_NONE},
    };
/**************************************************************************************************************/
static const ParamContainer st_ParamContainer_ResHigh[] = {
    
        (void *)&st_ParamRealUint16Struct_ResHigh, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_ResHigh),
    };
static const ParamContainer st_ParamContainer_GrHigh[] = {
    
        (void *)&st_ParamRealUint16Struct_GrHigh, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_GrHigh),
    };

static const ParamContainer st_ParamContainer_DcwCurHigh[] = {
    
        (void *)&st_ParamRealUint32Struct_DcwCurHigh, PARAM_TYPE_REAL_UINT32, GET_ARRAY_COUNT(st_ParamRealUint32Struct_DcwCurHigh),
    };

static const ParamContainer st_ParamContainer_DcwCurHigh_2676N[] = {
    
        (void *)&st_ParamRealUint16Struct_DcwCurHigh_2676N, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_DcwCurHigh_2676N),
    };

static const ParamContainer st_ParamContainer_AcwCurHigh[] = {
    
        (void *)&st_ParamRealUint32Struct_AcwCurHigh, PARAM_TYPE_REAL_UINT32, GET_ARRAY_COUNT(st_ParamRealUint32Struct_AcwCurHigh),
    };
	
static const ParamContainer st_ParamContainer_AcwCurHigh_2676N[] = {
    
        (void *)&st_ParamRealUint16Struct_AcwCurHigh_2676N, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(st_ParamRealUint16Struct_AcwCurHigh_2676N),
    };
/**************************************************************************************************************/
static const ParamContainer st_ParamContainer_DcwCurLow[] = {
    
        (void *)&st_ParamRealUint32Struct_DcwCurLow, PARAM_TYPE_REAL_UINT32, GET_ARRAY_COUNT(st_ParamRealUint32Struct_DcwCurLow),
    };
/**************************************************************************************************************/
static const ParamTotalContainer st_TotalContainer_ResHigh[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_ResHigh, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_ResHigh),
    };
static const ParamTotalContainer st_TotalContainer_GrHigh[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_GrHigh, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_ResHigh),
    };
static const ParamTotalContainer st_TotalContainer_DcwCurHigh[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_DcwCurHigh, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_DcwCurHigh),
    };

static const ParamTotalContainer st_TotalContainer_DcwCurHigh_2676N[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_DcwCurHigh_2676N, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_DcwCurHigh_2676N),
    };


static const ParamTotalContainer st_TotalContainer_AcwCurHigh[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_AcwCurHigh, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_AcwCurHigh),
    };


/**************************************************************************************************************/
static const ParamTotalContainer st_TotalContainer_DcwCurLow[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_DcwCurLow, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_DcwCurLow),
    };

const ParamTotalContainer st_TotalContainer_StepLcPHASe[] = {
    
        {                                        
            (void *)st_ParamContainer_StepLcPHASe, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, 
            TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_StepLcPHASe)
        },                   
    };


//-----------------------------------------------------------------------------
//STEP:DCW������ָ��ṹ��
const TailScaleParserStruct   t_InstructionStruct_StepDcwVolt = {
    
        "VOLTage", InstructionExec_StepDcwVolt, st_TotalContainer_Voltage_DC, 174, 233, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };


const TailScaleParserStruct   t_InstructionStruct_StepDcwRange = {
    
        "RANGe", InstructionExec_StepDcwRange, st_TotalContainer_DcwCurRange, 82, 82, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepDcwCurLow = {
    
        "LOW", InstructionExec_StepDcwCurLow, st_TotalContainer_DcwCurLow, 179, 179, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepDcwCurHigh = {
    
        "HIGH", InstructionExec_StepDcwCurHigh , st_TotalContainer_DcwCurHigh, 24, 24, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepDcwTtime = {
    
        "TTIMe", InstructionExec_StepDcwTtime, st_TotalContainer_Time, 150, 98, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepDcwRtime = {
    
        "RTIMe", InstructionExec_StepDcwRtime, st_TotalContainer_Time, 118, 34, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepDcwItime = {
    
        "ITIMe", InstructionExec_StepDcwItime, st_TotalContainer_Time, 229, 1, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

/* �½�ʱ�� */
const TailScaleParserStruct   t_InstructionStruct_StepDcwFtime = {
    
        "FTIMe", InstructionExec_StepDcwFtime, st_TotalContainer_Time, 181, 181, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepDcwArc = {
    
        "ARC", InstructionExec_StepDcwArc, st_TotalContainer_Arc_DC, 94, 94, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
//STEP:ACW������ָ��ṹ��
const TailScaleParserStruct   t_InstructionStruct_StepAcwVolt = {
    
        "VOLTage", InstructionExec_StepAcwVolt, st_TotalContainer_Voltage_AC, 174, 233, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepAcwRange = {
    
        "RANGe", InstructionExec_StepAcwRange, st_TotalContainer_AcwCurRange, 82, 82, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepAcwCurLow = {
    
        "LOW", InstructionExec_StepAcwCurLow, st_TotalContainer_DcwCurLow, 179, 179, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepAcwCurHigh = {
    
        "HIGH", InstructionExec_StepAcwCurHigh , st_TotalContainer_AcwCurHigh, 24, 24, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };


const TailScaleParserStruct   t_InstructionStruct_StepAcwTtime = {
    
        "TTIMe", InstructionExec_StepAcwTtime, st_TotalContainer_Time, 150, 98, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_StepAcwRtime = {
    
        "RTIMe", InstructionExec_StepAcwRtime, st_TotalContainer_Time, 118, 34, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
/* ���ʱ�� */
const TailScaleParserStruct   t_InstructionStruct_StepAcwItime = {
    
        "ITIMe", InstructionExec_StepAcwItime, st_TotalContainer_Time, 229, 1, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

/* �½�ʱ�� */
const TailScaleParserStruct   t_InstructionStruct_StepAcwFtime = {
    
        "FTIMe", InstructionExec_StepAcwFtime, st_TotalContainer_Time, 181, 181, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepAcwArc = {
    
        "ARC", InstructionExec_StepAcwArc, st_TotalContainer_Arc_DC, 94, 94, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_StepAcwFREQuency = {
    
        "FREQuency", InstructionExec_StepAcwFREQuency, st_TotalContainer_FREQuency, 219, 34,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };




//STEP:GR������ָ��ṹ��

// const TailScaleParserStruct   t_InstructionStruct_StepGrLow = {
//     
//         "LOW", InstructionExec_StepGrLow, st_TotalContainer_ResLow, 179, 179, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
//     };
// const TailScaleParserStruct   t_InstructionStruct_StepGrHigh = {
//     
//         "HIGH", InstructionExec_StepGrHigh , st_TotalContainer_ResHigh, 24, 24, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
//     };

const TailScaleParserStruct   t_InstructionStruct_StepGrCurr = {
    
        "CURRent", InstructionExec_StepGrCurr, st_TotalContainer_Current_GR, 70, 10, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepGrLow = {
    
        "LOW", InstructionExec_StepGrLow, st_TotalContainer_GrLow, 179, 179, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepGrHigh = {
    
        "HIGH", InstructionExec_StepGrHigh , st_TotalContainer_GrHigh, 24, 24, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_StepGrTtime = {
    
        "TTIMe", InstructionExec_StepGrTtime, st_TotalContainer_Time, 150, 98, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_StepGrItime = {
    
        "ITIMe", InstructionExec_StepGrItime, st_TotalContainer_Time, 229, 1, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_StepGrFREQuency = {
    
        "FREQuency", InstructionExec_StepGrFREQuency, st_TotalContainer_FREQuency, 219, 34,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };


//-----------------------------------------------------------------------------

//STEP:IR������ָ��ṹ��
const TailScaleParserStruct   t_InstructionStruct_StepIrVolt = {
    
        "VOLTage", InstructionExec_StepIrVolt, st_TotalContainer_Voltage_DC, 174, 233, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrVolt_2676N = {
    
        "VOLTage", InstructionExec_StepIrVolt, st_TotalContainer_Voltage_2676N, 174, 233, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrArange = {
    
        "ARANge", InstructionExec_StepIrArange, st_TotalContainer_Boolean, 56, 124, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrHigh = {
    
        "HIGH", InstructionExec_StepIrHigh, st_TotalContainer_ResHigh, 24, 24, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrHighMax = {
    
        "HMAXimal", NULL, NULL, 8, 38, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };
    
const TailScaleParserStruct   t_InstructionStruct_StepIrHighMin = {
    
        "HMINimal", NULL, NULL, 167, 32, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };
    
const TailScaleParserStruct   t_InstructionStruct_StepIrLowMax = {
    
        "LMAXimal", NULL, NULL, 16, 166, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };
    
const TailScaleParserStruct   t_InstructionStruct_StepIrLowMin = {
    
        "LMINimal", NULL, NULL, 175, 160, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrLow = {
    
        "LOW", InstructionExec_StepIrLow, st_TotalContainer_ResLow, 179, 179, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrTtime = {
    
        "TTIMe", InstructionExec_StepIrTtime, st_TotalContainer_Time, 150, 98, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrRtime = {
    
        "RTIMe", InstructionExec_StepIrRtime, st_TotalContainer_Time, 118, 34, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrItime = {
    
        "ITIMe", InstructionExec_StepIrItime, st_TotalContainer_Time, 229, 1, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrDtime = {
    
        "DTIMe", InstructionExec_StepIrDtime, st_TotalContainer_Time, 149, 96, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

// const TailScaleParserStruct   t_InstructionStruct_StepIrItime = {
//     
//         "ITIMe", NULL, st_TotalContainer_Time, 229, 1, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
//     };

const TailScaleParserStruct   t_InstructionStruct_StepIrFtime = {
    
        "FTIMe", NULL, st_TotalContainer_Time, 181, 160, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

// const TailScaleParserStruct   t_InstructionStruct_StepIrRtime = {
//     
//         "RTIMe", NULL, st_TotalContainer_Time, 118, 34, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
//     };

const TailScaleParserStruct   t_InstructionStruct_StepIrHrange = {
    
        "HRANge", NULL, st_TotalContainer_Boolean, 112, 93, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepIrOmode = {
    
        "OMODe", InstructionExec_StepIrDmode, st_TotalContainer_OutMode, 98, 249, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//STEP:LC������ָ��ṹ��
const TailScaleParserStruct   t_InstructionStruct_StepLcVolt = {
    
        "VOLTage", InstructionExec_StepLcVolt, st_TotalContainer_Voltage_LC, 174, 233, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcRange = {
    
        "RANGe", InstructionExec_StepLcRange, st_TotalContainer_LcCurRange, 82, 82, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcCurLow = {
    
        "LOW", InstructionExec_StepLcCurLow, st_TotalContainer_DcwCurLow, 179, 179, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcCurHigh = {
    
        "HIGH", InstructionExec_StepLcCurHigh , st_TotalContainer_AcwCurHigh, 24, 24, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };


const TailScaleParserStruct   t_InstructionStruct_StepLcTtime = {
    
        "TTIMe", InstructionExec_StepLcTtime, st_TotalContainer_Time, 150, 98, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
const TailScaleParserStruct   t_InstructionStruct_StepLcRtime = {
    
        "RTIMe", InstructionExec_StepLcRtime, st_TotalContainer_Time, 118, 34, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };
/* ���ʱ�� */
const TailScaleParserStruct   t_InstructionStruct_StepLcItime = {
    
        "ITIMe", InstructionExec_StepLcItime, st_TotalContainer_Time, 229, 1, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };


const TailScaleParserStruct   t_InstructionStruct_StepLcFREQuency = {
    
        "FREQuency", InstructionExec_StepLcFREQuency, st_TotalContainer_FREQuency, 219, 34,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcPHASe = {
    
        "PHASe", InstructionExec_StepLcPHASe, st_TotalContainer_StepLcPHASe, 213, 213,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const CharacterUnit   t_CharacterParamUnit_StepLcMDNEt[] = {
    
        {"E", 4},
        {"F", 5},
        {"G", 6},
    };

static const ParamBooleanStruct st_ParamCharacterStruct_StepLcMDNEt = {
    
        t_CharacterParamUnit_StepLcMDNEt, GET_ARRAY_COUNT(t_CharacterParamUnit_StepLcMDNEt),
    };

const const ParamContainer st_ParamContainer_StepLcMDNEt[] = {
    
        (void *)&st_ParamCharacterStruct_StepLcMDNEt, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    };

const ParamTotalContainer st_TotalContainer_StepLcMDNEt[] = {
    
        {                                        
            (void *)st_ParamContainer_StepLcMDNEt, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, 
            TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_StepLcMDNEt)
        },                   
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcMDNEt = {
    
        "MDNEt", InstructionExec_StepLcMDnetwork, st_TotalContainer_StepLcMDNEt, 112, 112,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcMDVOl = {
    
        "MDVOl", InstructionExec_StepLcMDvol, st_TotalContainer_StepLcPHASe, 111, 111,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

const TailScaleParserStruct   t_InstructionStruct_StepLcASSVOl = {
    
        "ASSVOl", InstructionExec_StepLcAssistvol, st_TotalContainer_Voltage_LC, 135, 135,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

static const ParamRealUint16Struct t_ParamReal_LcMDCOm[] = {
    
        {0  , 0x1FFF,    REAL_CFG_INFO_STR_LEN(4), SIGN_SYMBOL_INDEX_NONE},	//0-0x0FFF
    };

static const ParamContainer st_ParamContainer_LcMDCOm[] = {
    
        (void *)&t_ParamReal_LcMDCOm, PARAM_TYPE_REAL_UINT16, GET_ARRAY_COUNT(t_ParamReal_LcMDCOm),
    };

static const ParamTotalContainer st_TotalContainer_LcMDCOm[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_LcMDCOm, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_LcMDCOm),
    };
const TailScaleParserStruct   t_InstructionStruct_StepLcMDCOm = {
    
        "MDCOm", InstructionExec_StepLcMDCom, st_TotalContainer_LcMDCOm, 171, 171,  INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };






/******************************************************************************
 *                            ϵͳָ�������
******************************************************************************/

//ϵͳ���ڲ���������
static const ParamUint16Struct t_ParamUint16Struct_SysDateYear = {2100, 1900};
static const ParamUint8Struct t_ParamUint8Struct_SysDateMonth = {12, 1};
static const ParamUint8Struct t_ParamUint8Struct_SysDateDay   = {31, 1};

const ParamContainer t_ParamContainer_SysDateYear[] = {
    
        (void *)&t_ParamUint16Struct_SysDateYear, PARAM_TYPE_UINT16, PARAM_INFO_NONE,
    };

const ParamContainer t_ParamContainer_SysDateMonth[] = {
    
        (void *)&t_ParamUint8Struct_SysDateMonth, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    };

const ParamContainer t_ParamContainer_SysDateDay[] = {
    
        (void *)&t_ParamUint8Struct_SysDateDay, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    };

//��������ָ������
const ParamContainer *t_ParamContainer_SysDate[] = {
    
        t_ParamContainer_SysDateYear, t_ParamContainer_SysDateMonth, t_ParamContainer_SysDateDay, 
    };

static const ParamTotalContainer t_ParamTotalContainer_SysDate[] = {
    
        //������ִ��ָ�������Ϣ
        (void *)t_ParamContainer_SysDate, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(t_ParamContainer_SysDate),
    };

//-----------------------------------------------------------------------------

//ϵͳʱ�����������
static const ParamUint8Struct t_ParamUint8Struct_SysTimeHour   = {23, 0};
static const ParamUint8Struct t_ParamUint8Struct_SysTimeMinute = {59, 0};
static const ParamUint8Struct t_ParamUint8Struct_SysTimeSecond = {59, 0};

const ParamContainer t_ParamContainer_SysTimeHour[] = {
    
        (void *)&t_ParamUint8Struct_SysTimeHour, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    };

const ParamContainer t_ParamContainer_SysTimeMinute[] = {
    
        (void *)&t_ParamUint8Struct_SysTimeMinute, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    };

const ParamContainer t_ParamContainer_SysTimeSecond[] = {
    
        (void *)&t_ParamUint8Struct_SysTimeSecond, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    };

//��������ָ������
const ParamContainer *t_ParamContainer_SysTime[] = {
    
        t_ParamContainer_SysTimeHour, t_ParamContainer_SysTimeMinute, t_ParamContainer_SysTimeSecond, 
    };

static const ParamTotalContainer t_ParamTotalContainer_SysTime[] = {
    
        //������ִ��ָ�������Ϣ
        (void *)t_ParamContainer_SysTime, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(t_ParamContainer_SysTime),
    };

//-----------------------------------------------------------------------------

static const ParamStringStruct st_ParamStringStruct_PswNew = {"ABC"};

static const ParamContainer st_ParamContainer_PswNew[] = {
    
        (void *)&st_ParamStringStruct_PswNew, PARAM_TYPE_STRING, PARAM_INFO_NONE,
    }; 

static const ParamTotalContainer st_TotalContainer_PswNew[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_PswNew, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_PswNew),
    };

//-----------------------------------------------------------------------------

//ϵͳ->��·ָ�
const TailScaleParserStruct   t_InstructionStruct_SystemOALArm = {
   
        "OALArm", InstructionStruct_SystemOALArm, st_TotalContainer_Boolean,67, 114, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//ϵͳ���Բ���������
static const CharacterUnit   t_CharacterParamUnit_Language[] = {
    
        {"ENGLISH", '1'},
        {"CHINESE", '0'},
    };

static const ParamBooleanStruct st_ParamCharacterStruct_Language = {
    
        (void *)t_CharacterParamUnit_Language, GET_ARRAY_COUNT(t_CharacterParamUnit_Language),
    };

static const ParamContainer st_ParamContainer_Language[] = {
    
        (void *)&st_ParamCharacterStruct_Language, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    }; 

static const ParamTotalContainer st_TotalContainer_Language[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Language, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Language),
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//ϵͳ->����->�½�ָ��ṹ��
static const TailScaleParserStruct   st_InstructionStruct_SystemPasswordNew = {
    
        "NEW", NULL, st_TotalContainer_PswNew, 47, 47, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT,
    };

//ϵͳ->����->��ǰ����ָ��ṹ��
static const TailScaleParserStruct   st_InstructionStruct_SystemPasswordNow = {
    
        "NOW", NULL, NULL, 52, 52, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

//ϵͳ->����->�½�ָ��ṹ��
static const TailScaleParserStruct   st_InstructionStruct_SystemPasswordStatus = {
    
        "STATus", NULL, st_TotalContainer_Boolean, 127, 69, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//ϵͳ->����ָ�����
static const ParserContainer t_ParserContainer_SystemPassword[] = {
    
        {(void *)&st_InstructionStruct_SystemPasswordNew   , PARSER_TAIL_SCALE  },
        {(void *)&st_InstructionStruct_SystemPasswordNow   , PARSER_TAIL_SCALE  },
        {(void *)&st_InstructionStruct_SystemPasswordStatus, PARSER_TAIL_SCALE  }, 
    };

//ϵͳ->����ָ�
static const MidleScaleParserStruct   st_InstructionStruct_SystemPassword = {
    
        "PASSword", t_ParserContainer_SystemPassword, 234, 40, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_SystemPassword),
    }; 

//-----------------------------------------------------------------------------
//ϵͳ->����->����ָ��ṹ��       
const TailScaleParserStruct   t_InstructionStruct_SystemKeyVolume = {
    
        "VOLume", NULL, st_TotalContainer_Boolean, 222, 43, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//ϵͳ->����->����ָ��ṹ��       
static const TailScaleParserStruct   st_InstructionStruct_SystemKeyKlock = {
    
        "KLOCk", NULL, st_TotalContainer_Boolean, 135, 56, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//-----------------------------------------------------------------------------

//ϵͳ->ʱ��ָ�
static const TailScaleParserStruct   st_InstructionStruct_SystemTime = {
    
        "TIME", NULL, t_ParamTotalContainer_SysTime, 153, 153, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//ϵͳ->����ָ�
static const TailScaleParserStruct   st_InstructionStruct_SystemDate = {
    
        "DATE", NULL, t_ParamTotalContainer_SysDate, 25, 25, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//-----------------------------------------------------------------------------
//ϵͳ->LANGuageָ�
static const TailScaleParserStruct   st_InstructionStruct_SystemLanguage = {
    
        "LANGuage", NULL, st_TotalContainer_Language, 177, 25, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//-----------------------------------------------------------------------------
//ϵͳ->�������ָ�
static const TailScaleParserStruct   st_InstructionStruct_SystemRsave = {
    
        "RSAVe", NULL, st_TotalContainer_Boolean, 216, 230, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//-----------------------------------------------------------------------------

//ϵͳ->������ʾָ�
static const TailScaleParserStruct   st_InstructionStruct_SystemRhint = {
    
        "RHINt", NULL, st_TotalContainer_Boolean, 132, 32, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

//-----------------------------------------------------------------------------
//ϵͳ->�������ָ�
static const TailScaleParserStruct   st_InstructionStruct_SystemOcover = {
    
        "OCOVer", NULL, st_TotalContainer_Boolean, 135, 137, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

/******************************************************************************
 *                            ���ָ�������
******************************************************************************/

//-----------------------------------------------------------------------------
//���->����ָ�                     

static const TailScaleParserStruct   t_InstructionStruct_ResuCapacityUsed = {
    
        "USED", NULL, NULL, 55, 55, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_ResuCapacityFree = {
    
        "FREE", NULL, NULL, 22, 22, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_ResuCapacityAll = {
    
        "ALL", NULL, NULL, 100, 100, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_ResuCapacityPass = {
    
        "PASS", NULL, NULL, 40, 40, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_ResuCapacityFail = {
    
        "FAIL", NULL, NULL, 218, 218, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
    };

//���->����ָ��ڶ�������
static const ParserContainer t_ParserContainer_ResultCapacity[] = {
                                                                      
        {(void *)&t_InstructionStruct_ResuCapacityUsed, PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_ResuCapacityFree, PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_ResuCapacityAll , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_ResuCapacityPass, PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_ResuCapacityFail, PARSER_TAIL_SCALE},
    }; 

//���->����ָ���һ��  
static const MidleScaleParserStruct   t_InstructionStruct_ResuCapacity = {
    
        "CAPacity", t_ParserContainer_ResultCapacity, 171, 99, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_ResultCapacity),
    };
//-----------------------------------------------------------------------------
//���->���ָ�

static const TailScaleParserStruct   t_InstructionStruct_ResuClr = {
    
        "CLEar", NULL, NULL, 154, 221, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT 
    };

//-----------------------------------------------------------------------------

//���->��ȡָ��ṹ��
static const TailScaleParserStruct   t_InstructionStruct_ResuFetchAll = {
    
        "ALL", NULL, NULL, 100, 100, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY, 
    };

//���->��ȡָ��ṹ��
static const TailScaleParserStruct   t_InstructionStruct_ResuFetchSingle = {
    
        "SINGle", NULL, NULL, 154, 251, INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY, 
    };

//���->��ȡָ�����
static const ParserContainer t_ParserContainer_ResuFetch[] = {
                                                                      
        {(void *)&t_InstructionStruct_ResuFetchAll   , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_ResuFetchSingle, PARSER_TAIL_SCALE},
    };

//���->��ȡָ�

const MidleScaleParserStruct   t_InstructionStruct_ResuFetch = {
    
        "FETCh", t_ParserContainer_ResuFetch, 148, 88, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_ResuFetch)
    };

//-----------------------------------------------------------------------------

//���->�ϸ���ָ��ṹ��
static const TailScaleParserStruct   t_InstructionStruct_ResuPpercent = {
    
        "PPERcent", NULL, NULL, 175, 227, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_QUERY, 
    };

//-----------------------------------------------------------------------------

//���->ͳ��ָ��ṹ��
static const TailScaleParserStruct   t_InstructionStruct_ResuStatistics = {
    
        "STATistics", NULL, NULL, 165, 69, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_QUERY, 
    };












//�ļ���Χ���� -> ��ִ�к������в����Ϸ����ж� �ڴ�ֻ�ǹ̶��Ľ�������Ϊ���
static const ParamUint8Struct st_ParamStruct_FileRange[]        = {

        {50, 1},
    };

const ParamContainer __t_ParamContainer_FileRange[]       = {
    
        {(void *)st_ParamStruct_FileRange, PARAM_TYPE_UINT8, PARAM_INFO_NONE},
    };

//�ܲ���������Ϣ
const ParamTotalContainer __t_ParamTotalContainer_FileRange[] = {
    
        {                                        
            (void *)__t_ParamContainer_FileRange, CONTAINER_ATTR_DOUBLE|CONTAINER_ATTR_QUERY|CONTAINER_ATTR_EXECUTE, 
            TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(__t_ParamContainer_FileRange)
        },                   
    }; 


/******************************************************************************
 *                            ͨѶָ�������
******************************************************************************/ 

//ͨѶ��ַ��Χ
static const ParamUint8Struct t_ParamStruct_CommSaddr = {255, 0};
    
const ParamContainer t_ParamContainer_CommSaddr_1[] = {         
    
        (void *)&t_ParamStruct_CommSaddr, PARAM_TYPE_UINT8, PARAM_INFO_NONE,
    };                                     

ParamStringStruct t_Stringhehe = {"ABI"};

const ParamContainer t_ParamContainer_CommStr[] = {
    
        (void *)&t_Stringhehe, PARAM_TYPE_STRING, PARAM_INFO_NONE,
    };
/*
static const ParamContainer t_ParamContainer_CommSaddr[] = {
    
        (void *)&t_ParamStruct_CommSaddr, PARAM_TYPE_UINT8, PARAM_ATTR_NONE,
        (void *)&t_BooleanParamStruct, PARAM_TYPE_STRING, PARAM_ATTR_NONE,
        (void *)&t_BooleanParamStruct, PARAM_TYPE_CHARACTER, PARAM_ATTR_NONE,
    };
*/

//ͨѶ��ַ��������ָ������
const ParamContainer *t_ParamContainer_CommSaddr[] = {
    
        t_ParamContainer_CommSaddr_1, t_ParamContainer_CommStr, st_ParamContainer_Boolean, 
    };

//ͨѶ��ַ����������
static const ParamTotalContainer t_ParamTotalContainer_CommSaddr[] = {
    
        //������ִ��ָ�������Ϣ
        (void *)t_ParamContainer_CommSaddr, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(2)|TOTAL_PARAM_INFO_TOT_CNTS(t_ParamContainer_CommSaddr),
    };                                       

//ͨѶָ�����(β��)�ṹ��               
static const TailScaleParserStruct   t_InstructionStruct_CommSaddr = {
    
        "SADDress", InstructionExec_CommSAddr, t_ParamTotalContainer_CommSaddr, 243, 241, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT|INSTRUCTION_ATTR_QUERY,
    };

static const TailScaleParserStruct   t_InstructionStruct_CommRemote = {
    
        "REMote", InstructionExec_CommRemote, NULL, 64, 38, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT,
    };

static const TailScaleParserStruct   t_InstructionStruct_CommLocal = {
    
        "LOCal", InstructionExec_CommLocal, NULL, 5, 159, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT,
    };

static const TailScaleParserStruct   t_InstructionStruct_CommControl = {
    
        "CONTrol", InstructionExec_CommControl, NULL, 209, 136, INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_QUERY,
    };
//-----------------------------------------------------------------------------
//�ļ�->��ȡָ��
static const TailScaleParserStruct   st_InstructionStruct_FileRead = {
    
        "READ", InstructionExec_FileRead, __t_ParamTotalContainer_FileRange, 81, 81, 
        INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT
    };



//�ļ�->Ŀ¼->����ָ��
static const TailScaleParserStruct   st_InstructionStruct_FileCatalogSingle = {
    
        "SINGle", InstructionExec_FileCatalogSingle, __t_ParamTotalContainer_FileRange, 154, 251, 
        INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY
    };

//�ļ�->Ŀ¼ָ�����
static const ParserContainer t_ParserContainer_FileCatalog[] = {
    
        {(void *)&st_InstructionStruct_FileCatalogSingle  , PARSER_TAIL_SCALE  },
    };

//�ļ�->Ŀ¼ָ�
static const MidleScaleParserStruct   st_InstructionStruct_FileCatalog = {
    
        "CATalog", t_ParserContainer_FileCatalog, 220, 103, INSTRUCTION_ATTR_NONE,
        GET_ARRAY_COUNT(t_ParserContainer_FileCatalog)
    };

//ͨѶָ��ڶ�������
const ParserContainer t_ParserContainer_Communication[] = {
    
        {(void *)&t_InstructionStruct_CommSaddr  , PARSER_TAIL_SCALE_PCPA},
        {(void *)&t_InstructionStruct_CommRemote , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_CommLocal  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_CommControl, PARSER_TAIL_SCALE},
    }; 

//ͨѶָ���һ��(����)
const HeadScaleParserStruct  t_InstructionStruct_Communication = {
    
        "COMMunication", t_ParserContainer_Communication, 184, 1, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_Communication),
    }; 
//�ļ�ָ��ڶ�������      
static const ParserContainer t_ParserContainer_File[] = {
    
//         {(void *)&st_InstructionStruct_FileNew     , PARSER_TAIL_SCALE_PCPA},
//         {(void *)&st_InstructionStruct_FileEdit    , PARSER_TAIL_SCALE_PCPA},
//         {(void *)&st_InstructionStruct_FileDelete  , PARSER_MIDDLE_SCALE},
//         {(void *)&st_InstructionStruct_FileSave    , PARSER_TAIL_SCALE_PCPA},
        {(void *)&st_InstructionStruct_FileRead    , PARSER_TAIL_SCALE},
        {(void *)&st_InstructionStruct_FileCatalog , PARSER_MIDDLE_SCALE},
//         {(void *)&st_InstructionStruct_FileFormat  , PARSER_TAIL_SCALE},
    };
//�ļ�ָ���һ����������
const HeadScaleParserStruct   __t_InstructionStruct_File = {
    
        "FILE", t_ParserContainer_File, 87, 87, INSTRUCTION_ATTR_NONE, 
        GET_ARRAY_COUNT(t_ParserContainer_File)
    };
//---------------------------���ò���������------------------------------------                       
 

/******************************************************************************
 *                          BOOLEAN����������
******************************************************************************/ 

static const CharacterUnit   t_BooleanParamUnit[] = {
    
        {"ON" , '0'},
        {"OFF", '1'},
        {"1"  , '2'},
        {"0"  , '3'},
    };

static const ParamBooleanStruct st_BooleanParamStruct = {
    
        (void *)t_BooleanParamUnit, GET_ARRAY_COUNT(t_BooleanParamUnit),
    };

static const ParamContainer st_ParamContainer_Boolean[] = {
    
        (void *)&st_BooleanParamStruct, PARAM_TYPE_CHARACTER, PARAM_INFO_NONE,
    }; 

static const ParamTotalContainer st_TotalContainer_Boolean[] = {
    
        //��ѡ��������Ϊ�� ����֧�ֿ�ѡ����
        (void *)&st_ParamContainer_Boolean, CONTAINER_ATTR_SINGLE|CONTAINER_ATTR_EXECUTE, TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamContainer_Boolean),
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

uint32_t APP_RootParserContainerInfoGet(ParserContainer **ptrootParserContainer, uint8_t *prootParserContainerCapacity)             
{

		//������������ַ
	    (*ptrootParserContainer)                                 = (ParserContainer *)t_ParserContainer_Root_CS9931;
	    //��������������
	    (*prootParserContainerCapacity)                          = GET_ARRAY_COUNT(t_ParserContainer_Root_CS9931);
		  st_ParamRealUint16Struct_Test_Mode.m_pcharacterTable	   = (void *)t_ParamReal_Test_Mode_9931;			  
		  st_ParamRealUint16Struct_Test_Mode.m_characterTableNumbs = GET_ARRAY_COUNT(t_ParamReal_Test_Mode_9931);	

    
    return TRUE;
}
