/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�CS99xx.C
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 */

/******************* �����ļ� *********************/
#include "CS99xx.h"
#include "driver.h"
#include  "CH376_USB_UI.H"
#include "CS9931_Config.h"
#include "scpi_parser.h"
#include "ScpiInstructionTable.h"
#include "api_sched.h"
#include "library.h"
// #include "mb.h"
#include "Communication.h"
#include "Usart1.h"

TEST_FLAG test_flag;

/* ϵͳ������������ */
struct system_parameter system_parameter_t;

rt_uint8_t panel_flag = 0; // ��ǰ����
rt_uint8_t startup_flag = 0;

/* �ļ���Ϣ�������� */
rt_uint8_t current_file_num; // ��ǰ�ļ����
struct file_info_t file_info[FILE_NUM]; // �����洢�ļ���Ϣ�Ľṹ��

/* ������Ϣ�������� */
rt_uint8_t current_step_num = 1; // ��ǰ������


char version_name[2][40] = {"CS99xx�ۺϲ�����","CS99xx test"};
const char *mode_name[] = {
	"ACW",
	"ACW+GR",
	"DCW",
	"DCW+GR",
	"IR",
	"IR+GR",
	"GR",
	"LC",
	"LC+PW",
	"PW",
	"LR",
	"OSC",
	"DLA",
};
const char *curgear_name[] = {    //douyijun
	"2uA",
	"20uA",
	"200uA",
	"2mA",
	"20mA",
	"100mA",
};

const char *curgear_name_LC[] = {    //douyijun
	"2uA",
	"20uA",
	"200uA",
	"2mA",
	"20mA",
	"100mA",
};

const char *curgear_name_DCW[] = {    //douyijun
	"2uA",
	"20uA",
	"200uA",
	"2mA",
	"10mA",
	"100mA",
};
const char* boolean_name[2][2]=
{
	{"�ر�","����"},
	{"OFF","ON"}
};

const char* single_boolean_name[2][2]=
{
	{"�ر�","����"},
	{"OFF","ON"}
};
const char* sw_status_name[2][2]=
{
	{"�Ͽ�","�պ�"},
	{"OFF","ON"}
};
const char* sw_status_name_static[2][2]=
{
	{"����","����"},
	{"OFF","ON"}
};
const char* lc_phase_name[2][2]={
	{"L ��","N ��"},
	{"L phase","N phase"}
};
const char* lc_detection_name[]={
	"AC","AC+DC","PEAK","DC"
};
const char* lc_MDnetwork_name[]={
	"MD-A","MD-B","MD-C","MD-D","MD-E","MD-F","MD-G","MD-H"
};

const char *workmode_name[]={"N","G"};
const char *error_name[2][10] = {
	{
		"�޴���",
		"���ޱ���",
		"���ޱ���",
		"��·����",
		"��·����",
		"�绡����",
		"GFI����",
		"���ű���",
		"��ѹ�쳣",
	},
	{
		"NO ERR.",
		"HIGH",
		"LOW",
		"OPEN",
		"SHORT",
		"ARC",
		"GFI",
		"AMP",
		"VOL.ERR.",
	},
};
#include "Communication.h"
extern struct system_parameter system_parameter_t;
extern void memorymanagement_init(void);
extern void Test_Sched_Environment_Init(void);
extern void USART2_DISPOSE_TASK(void);
void cs99xx_init(void)
{
	
	uint32_t  usartbaud;
	memorymanagement_init();
	
// 	rt_enter_critical();
	USB_Device_Chg(USB_1);
	USB_DEVICE_INIT();
	USB_Device_Chg(USB_2);
	USB_DEVICE_INIT();
	
	switch(system_parameter_t.com.baud){
		case 0:
			usartbaud = 4800;
		break;
		
		case 1:
			usartbaud = 9600;
		break;
		
		case 2:
			usartbaud = 19200;
		break;
		
		default:
			usartbaud = 9600;
		break;
		
		
	}
	
	
	{
		usart2_init(usartbaud);
// 		eMBInit(MB_RTU, 0x01, 2, usartbaud,  MB_PAR_NONE);
// 		eMBEnable();
	}
	
	{
		extern  uint32_t rt_uart_write(const void* buffer, uint32_t size);
		ParserContainer    *ptrootParserContainer;
		uint8_t             prootParserContainerCapacity;
		//��ȡ�������ṹ����Ϣ
    APP_RootParserContainerInfoGet((ParserContainer **)&ptrootParserContainer, &prootParserContainerCapacity);
    //��������ʼ��
    API_PEMInit(ptrootParserContainer, prootParserContainerCapacity, 
                2, '1', (FP_pfOutputQueueSend *)rt_uart_write);
    APP_CommProtocolLogicInit(system_parameter_t.com.address);
		
		rt_thread_idle_sethook(USART2_DISPOSE_TASK);
	}
// 	rt_exit_critical();
	
	Test_Sched_Environment_Init();
}


/*
 * ��������CS99xx_GPIO_Config
 * ����  ������IO����
 * ����  ����
 * ���  ����
 */
void CS99xx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ�� GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD
						 | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH
						 | RCC_AHB1Periph_GPIOI, ENABLE);
	/* ʹ��SYSCFGʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* A0,A1,A2,A3,A4,A5,A6,A7,A8,A9 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	                            GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	/* A10,A11,A12,A13,A14,A15 */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	/* A16,A17,A18 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* A19,A20 */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* D0,D1,D2,D3 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* D4,D5,D6,D7,D8,D9,D10,D11,D12 */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* D13,D14,D15 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	
	/* NE1 (CS) */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* NE2 (CS) */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* NE3 (CS) */
// 	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
// 	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	/* NE4 (CS) */
// 	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
// 	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	/* NOE (RD), NWE (WR) */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/***************** ���ӻ����� *******************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	//У׼����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/******************************************************************************************/
	/*	�Ӱ�����״̬�Ƶ� MC4094 �������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	/*******************************************************************************************/
	
	
	/* �����ж����� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//L
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//R
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource3);
	
	/*��·�ж�*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//SHORT_INT
	GPIO_Init(GPIOG, &GPIO_InitStructure);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//AMP_INT
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource7);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//ARC_INT
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource6);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource10);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//GFI_INT
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource8);
	
	/* ��������ֹͣ������ */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;//S , E
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource8);
	
	/******************************************************************************************/
	/* ����̵����� MC4094 �������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	/******************************************************************************************/
	/* SD �� �������� */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);

	/* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PD.02 CMD line */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure PC.12 pin: CLK pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*****************************************************************************************/
	/* SPI FLASH �������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	/* IO������ٶ� */
	/* SPI CS1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* SPI CS2, CS3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/* SPI CS4 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	/* SPI CLK, DO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* SPI WP */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	/* SPI DI */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  /* ��������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
	GPIO_SetBits(GPIOH,GPIO_Pin_5);
}

/*
 * ��������CS99xx_Peripheral_Config
 * ����  ��������������
 * ����  ����
 * ���  ����
 */
void CS99xx_Peripheral_Config(void)
{
	FSMC_NORSRAMInitTypeDef  init;
	FSMC_NORSRAMTimingInitTypeDef  timingWrite;
	FSMC_NORSRAMTimingInitTypeDef  timingRead;

	/******************************************************************************************/
	/* LCD FSMC �������� */
	/* ʹ��FSMCʱ�� */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	
	timingWrite.FSMC_AddressSetupTime = 0;
	timingWrite.FSMC_AddressHoldTime = 0;
	timingWrite.FSMC_DataSetupTime = 0;
	timingWrite.FSMC_BusTurnAroundDuration = 0;
	timingWrite.FSMC_CLKDivision = 0;
	timingWrite.FSMC_DataLatency = 0;
	timingWrite.FSMC_AccessMode = FSMC_AccessMode_A;
	
	timingRead.FSMC_AddressSetupTime = 4;
	timingRead.FSMC_AddressHoldTime = 0;
	timingRead.FSMC_DataSetupTime = 8;
	timingRead.FSMC_BusTurnAroundDuration = 1;
	timingRead.FSMC_CLKDivision = 0;
	timingRead.FSMC_DataLatency = 0;
	timingRead.FSMC_AccessMode = FSMC_AccessMode_A;

	/*
	 LCD configured as follow:
	    - Data/Address MUX = Disable
	    - Memory Type = SRAM
	    - Data Width = 16bit
	    - Write Operation = Enable
	    - Extended Mode = Enable
	    - Asynchronous Wait = Disable
	*/
	init.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	/* ע��ɿ��������Ա */
	init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	init.FSMC_WrapMode = FSMC_WrapMode_Disable;
	init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	init.FSMC_ReadWriteTimingStruct = &timingRead;
	init.FSMC_WriteTimingStruct = &timingWrite;

	FSMC_NORSRAMInit(&init);

	/* - BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
	
	
	/******************************************************************************************/
	/* �ⲿ SRAM FSMC �������� */
	timingWrite.FSMC_AddressSetupTime = 3;		/* ����Ϊ2�����; 3���� */
	timingWrite.FSMC_AddressHoldTime = 0;
	timingWrite.FSMC_DataSetupTime = 2;			/* ����Ϊ1����2���� */
	timingWrite.FSMC_BusTurnAroundDuration = 1;
	timingWrite.FSMC_CLKDivision = 0;
	timingWrite.FSMC_DataLatency = 0;
	timingWrite.FSMC_AccessMode = FSMC_AccessMode_A;

	init.FSMC_Bank = FSMC_Bank1_NORSRAM2;
	init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
	init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	/* ע��ɿ��������Ա */
	init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	init.FSMC_WrapMode = FSMC_WrapMode_Disable;
	init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	init.FSMC_ReadWriteTimingStruct = &timingWrite;
	init.FSMC_WriteTimingStruct = &timingWrite;

	FSMC_NORSRAMInit(&init);

	/* - BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);
	
	
	/* Enable the SDIO APB2 Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
	/* Enable the DMA2 Clock */
	RCC_AHB1PeriphClockCmd(SD_SDIO_DMA_CLK, ENABLE);
	
	/* �ⲿ�ж� ���� */
	{
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
		/* Enable the EXTI0 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource8);
		
		/* Configure  EXTI  */
		EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line8 | EXTI_Line15 | EXTI_Line7 | EXTI_Line6 | EXTI_Line10;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//Falling�½��� Rising����
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_ClearITPendingBit(EXTI_Line3); /* ����жϱ�־λ */
		EXTI_ClearITPendingBit(EXTI_Line8);
		EXTI_ClearITPendingBit(EXTI_Line15);
		EXTI_ClearITPendingBit(EXTI_Line7);
		EXTI_ClearITPendingBit(EXTI_Line6);
		EXTI_ClearITPendingBit(EXTI_Line10);
		EXTI_Init(&EXTI_InitStructure);
	}
}

/********************************************************************************************/
