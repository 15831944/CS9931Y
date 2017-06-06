/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
 
#include    "ScpiInstructionTable.h"
#include    "Communication.h"
#include    "CS9931CInstructionTable.h"


#define	        GET_ARRAY_COUNT(addr)					        (sizeof(addr)/sizeof(addr[0]))
/******************************************************************************
 *                           ָ�Ϊ���νṹ
 *                                ��ָ�
 *                        AA                      BB    
 *                   CC   DD   EE             FF  GG  HH
 *              ZZ   MM   NN                          UU II OO PP
 *
******************************************************************************/

/******************************************************************************
 *                       ���ļ�������ľ�̬���ݽṹ
******************************************************************************/

static const HeadScaleParserStruct   st_InstructionStruct_Step_9931; 


/******************************************************************************
 *                           ��ָ������������
******************************************************************************/ 
//CS2676C��ָ������
const ParserContainer t_ParserContainer_Root_CS9931[7] = {
    
        {(void *)&t_InstructionStruct_RST           		, PARSER_SINGLE_SCALE},
        {(void *)&t_InstructionStruct_IDN           		, PARSER_SINGLE_SCALE},
        {(void *)&t_InstructionStruct_Source        		, PARSER_HEAD_SCALE  },
        {(void *)&st_InstructionStruct_Step_9931        , PARSER_HEAD_SCALE  },
        {(void *)&t_InstructionStruct_System        		, PARSER_HEAD_SCALE  },
        {(void *)&t_InstructionStruct_Communication 		, PARSER_HEAD_SCALE  },
				{(void *)&__t_InstructionStruct_File          	, PARSER_HEAD_SCALE  },
    };

/******************************************************************************
 *                           ���ָ������������
******************************************************************************/

//���ָ��ڶ�������     
static const ParserContainer st_ParserContainer_Result[] = {
    
        //{(void *)&t_InstructionStruct_ResuCapacity  , PARSER_MIDDLE_SCALE},
        //{(void *)&t_InstructionStruct_ResuClr       , PARSER_TAIL_SCALE  },
        {(void *)&t_InstructionStruct_ResuFetch     , PARSER_MIDDLE_SCALE},
        //{(void *)&t_InstructionStruct_ResuPpercent  , PARSER_TAIL_SCALE  },
        //{(void *)&t_InstructionStruct_ResuStatistics, PARSER_TAIL_SCALE  },
    }; 

//���ָ���һ����������
const HeadScaleParserStruct   t_InstructionStruct_Result = {
   
       "RESult", st_ParserContainer_Result, 141, 44, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_Result),
   };

/******************************************************************************
 *                           Դָ������������
******************************************************************************/
//Դ->�б�->�ļ�������ָ��
// const TailScaleParserStruct   t_InstructionStruct_SourceListFIndex = {
//     
//         "FINDex", InstructionExec_SourceListFIndex, 0, 1, 87, 
//         INSTRUCTION_ATTR_SCALES(3)|INSTRUCTION_ATTR_QUERY,
//     };

// //Դ-�б�ָ�����������
// static const ParserContainer st_ParserContainer_SrcList[] = {
//     
//         //{(void *)&t_InstructionStruct_SrcListMode    , PARSER_TAIL_SCALE},
// 		{(void *)&t_InstructionStruct_SourceListFIndex, PARSER_TAIL_SCALE},
//         {(void *)&t_InstructionStruct_SrcListSindex  , PARSER_TAIL_SCALE},
//         {(void *)&t_InstructionStruct_SrcListSmessage, PARSER_TAIL_SCALE},
//     };

// static const MidleScaleParserStruct   st_InstructionStruct_SrcList = {
//     
//         "LIST", st_ParserContainer_SrcList, 170, 170, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_SrcList),
//     };

// //Դָ��"SOURce"�ڶ�������
// static const ParserContainer st_ParserContainer_Src[] = {
//     
//         {(void *)&t_InstructionStruct_SrcTest , PARSER_MIDDLE_SCALE},
//         //{(void *)&t_InstructionStruct_SrcLoad , PARSER_MIDDLE_SCALE},
//         {(void *)&st_InstructionStruct_SrcList, PARSER_MIDDLE_SCALE},
//     };

//Դָ���һ��(����)
//const HeadScaleParserStruct   t_InstructionStruct_Source = {
//    
//        "SOURce", st_ParserContainer_Src, 26, 12, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_Src),
//    };

/******************************************************************************
 *                           ϵͳָ������������
******************************************************************************/

// //ϵͳ->����ָ�����
// static const ParserContainer st_ParserContainer_SystemKey[] = {
//     
//         {(void *)&t_InstructionStruct_SystemKeyVolume, PARSER_TAIL_SCALE  },
//         //{(void *)&st_InstructionStruct_SystemKeyKlock , PARSER_TAIL_SCALE  }, 
//     };

// //ϵͳ->����ָ�
// static const MidleScaleParserStruct   st_InstructionStruct_SystemKey = {
//     
//         "KEY", st_ParserContainer_SystemKey, 112, 112, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_SystemKey),
//     };
// //-----------------------------------------------------------------------------

// //ϵͳָ�����
// static const ParserContainer st_ParserContainer_System[] = {
//     
//         {(void *)&st_InstructionStruct_SystemKey     , PARSER_MIDDLE_SCALE   },
// 					{(void *)&st_InstructionStruct_SystemOALArm     , PARSER_MIDDLE_SCALE },
        //{(void *)&st_InstructionStruct_SystemPassword, PARSER_MIDDLE_SCALE   },
        //{(void *)&st_InstructionStruct_SystemRhint   , PARSER_TAIL_SCALE     },
        //{(void *)&st_InstructionStruct_SystemRsave   , PARSER_TAIL_SCALE     },
        //{(void *)&st_InstructionStruct_SystemOcover  , PARSER_TAIL_SCALE     },
        //{(void *)&st_InstructionStruct_SystemLanguage, PARSER_TAIL_SCALE     },
        //{(void *)&st_InstructionStruct_SystemTime    , PARSER_TAIL_SCALE_PCPA},
        //{(void *)&st_InstructionStruct_SystemDate    , PARSER_TAIL_SCALE_PCPA},
//     }; 


//ͨѶָ��ڶ�������
static const ParserContainer t_ParserContainer_System[] = {
    
        {(void *)&t_InstructionStruct_SystemOALArm     , PARSER_TAIL_SCALE_PCPA },
    }; 

//ͨѶָ���һ��(����)
const HeadScaleParserStruct  t_InstructionStruct_System = {
    
        "SYSTem", t_ParserContainer_System, 4, 143, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(t_ParserContainer_System),
    }; 
//-----------------------------------------------------------------------------

//ϵͳָ���һ����������
//const HeadScaleParserStruct   t_InstructionStruct_System = {
//    
//        "SYSTem", st_ParserContainer_System, 4, 143, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_System),
//    };

/******************************************************************************
 *                           ����ָ������������
******************************************************************************/
//STEP:IR������ָ������
static const ParserContainer st_ParserContainer_StepIr_9931[] = {
    
        {(void *)&t_InstructionStruct_StepIrVolt  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepIrHigh  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepIrLow   , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepIrTtime , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepIrRtime , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepIrItime , PARSER_TAIL_SCALE},
//         {(void *)&t_InstructionStruct_StepIrDtime , PARSER_TAIL_SCALE},
//         {(void *)&t_InstructionStruct_StepIrOmode , PARSER_TAIL_SCALE},
    };
//STEP:ACW������ָ������
static const ParserContainer st_ParserContainer_StepAcw_9931[] = {
    
        {(void *)&t_InstructionStruct_StepAcwVolt  	  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepAcwCurHigh  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepAcwCurLow   , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepAcwTtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepAcwRtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepAcwItime 	  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepAcwArc      , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepAcwFREQuency, PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepAcwRange  	, PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepAcwItime  	, PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepAcwFtime  	, PARSER_TAIL_SCALE},
    };
//STEP:DCW������ָ������
static const ParserContainer st_ParserContainer_StepDcw_9931[] = {
    
        {(void *)&t_InstructionStruct_StepDcwVolt  	  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepDcwCurHigh  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepDcwCurLow   , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepDcwTtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepDcwRtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepDcwItime 	  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepDcwArc      , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepDcwRange  	, PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepDcwFtime  	, PARSER_TAIL_SCALE},

    };
//STEP:GR������ָ������
static const ParserContainer st_ParserContainer_StepGr_9931[] = {
				{(void *)&t_InstructionStruct_StepGrCurr  	  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepGrHigh      , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepGrLow       , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepGrTtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepGrItime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepGrFREQuency, PARSER_TAIL_SCALE},
    };

//STEP:LC������ָ������
static const ParserContainer st_ParserContainer_StepLC_9931[] = {
				{(void *)&t_InstructionStruct_StepLcVolt  	  , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepLcCurHigh   , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepLcCurLow    , PARSER_TAIL_SCALE},
        {(void *)&t_InstructionStruct_StepLcTtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcRtime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcItime 	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcFREQuency , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcRange  	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcItime  	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcPHASe  	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcMDNEt  	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcMDVOl  	  , PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcASSVOl  	, PARSER_TAIL_SCALE},
				{(void *)&t_InstructionStruct_StepLcMDCOm  	  , PARSER_TAIL_SCALE},

    };



//"STEP"�ڶ���ָ��ṹ��



static const ParamUint8Struct st_ParamStruct_StepRange[]        = {

        {4, 0},
    };
const ParamContainer st_ParamStruct_StepInsertMode[]       = {
    
        {(void *)st_ParamStruct_StepRange, PARAM_TYPE_UINT8, PARAM_INFO_NONE},
    };
//�ܲ���������Ϣ
const ParamTotalContainer t_ParamTotalContainer_StepInsertMode[] = {
    
        {                                        
            (void *)st_ParamStruct_StepInsertMode, CONTAINER_ATTR_DOUBLE|CONTAINER_ATTR_QUERY|CONTAINER_ATTR_EXECUTE, 
            TOTAL_PARAM_INFO_OPT_CNTS(0)|TOTAL_PARAM_INFO_TOT_CNTS(st_ParamStruct_StepInsertMode)
        },                   
    };
//����->ģʽָ��
static const TailScaleParserStruct   st_InstructionStruct_StepMode = {
    
        "MODE", InstructionExec_StepMode, t_ParamTotalContainer_StepInsertMode , 181, 181, 
        INSTRUCTION_ATTR_SCALES(2)|INSTRUCTION_ATTR_EXECUT
    };
static const MidleScaleParserStruct   st_InstructionStruct_StepIr_9931 = {
    
        "IR"  , st_ParserContainer_StepIr_9931, 58, 58, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_StepIr_9931),
    };

static const MidleScaleParserStruct   st_InstructionStruct_StepAcw_9931 = {
    
        "ACW"  , st_ParserContainer_StepAcw_9931, 234, 234, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_StepAcw_9931),
    };

static const MidleScaleParserStruct   st_InstructionStruct_StepDcw_9931 = {
    
        "DCW"  , st_ParserContainer_StepDcw_9931, 171, 171, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_StepDcw_9931),
    };

static const MidleScaleParserStruct   st_InstructionStruct_StepGr_9931 = {
    
        "GR"  , st_ParserContainer_StepGr_9931, 57, 57, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_StepGr_9931),
    };

static const MidleScaleParserStruct   st_InstructionStruct_StepLc_9931 = {
    
        "LC"  , st_ParserContainer_StepLC_9931, 172, 172, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_StepLC_9931),
    };

//-----------------------------------------------------------------------------

//"STEP"�ڶ���ָ������
static const ParserContainer st_ParserContainer_Step_9931[] = {
				{(void *)&st_InstructionStruct_StepMode      , PARSER_TAIL_SCALE},
				{(void *)&st_InstructionStruct_StepIr_9931   , PARSER_MIDDLE_SCALE},
				{(void *)&st_InstructionStruct_StepAcw_9931  , PARSER_MIDDLE_SCALE},
				{(void *)&st_InstructionStruct_StepDcw_9931  , PARSER_MIDDLE_SCALE},
				{(void *)&st_InstructionStruct_StepGr_9931   , PARSER_MIDDLE_SCALE},
				{(void *)&st_InstructionStruct_StepLc_9931   , PARSER_MIDDLE_SCALE},
    };

//"STEP"ָ���һ��(����)
static const HeadScaleParserStruct   st_InstructionStruct_Step_9931 = {
    
        "STEP", st_ParserContainer_Step_9931, 67, 67, INSTRUCTION_ATTR_NONE, GET_ARRAY_COUNT(st_ParserContainer_Step_9931),
    };
