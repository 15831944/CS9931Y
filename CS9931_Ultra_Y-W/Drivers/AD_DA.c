#include "AD_DA.h"


#if defined(__cplusplus)

    extern "C" {     /* Make sure we have C-declarations in C++ programs */

#endif

/**************************************************************************
 *                           ���������ؼ��ֺ궨�� -- ���������
***************************************************************************/ 

#define     INLINE                                   __inline 



			
static void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ1ms*/
static void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period = 1000 * 1;				  			/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler = (168 - 1);				    /* ʱ��Ԥ��Ƶ�� 168M/168 */
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; 		/* ������Ƶ */
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		  /* �������жϱ�־ */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);																		  /* ����ʱ�� */
}		
	


union{	

	uint16_t   DAC_CH_DATA[2][4];
	uint16_t   DAC_EACH_DATA[8];
	struct{
		uint16_t W_VREF_Value; 
		uint16_t GR_VREF_Value;        
		uint16_t Short_VREF_Value;           
		uint16_t ARC_VREF_Value;          
		uint16_t ARC_4us_VREF_Value;      
		uint16_t ARC_10us_VREF_Value;       
		uint16_t ARC_20us_VREF_Value;      
		uint16_t ARC_40us_VREF_Value;     
	}VREF_Value;
	
}DAC_DATA;

void DAC_SetValue(uint8_t ch,uint16_t value)
{
	if(ch>7)return;
	
	if(value < 4096)
	{
		DAC_DATA.DAC_EACH_DATA[ch] = value;
	}
// 	DAC_DualSoftwareTriggerCmd(DISABLE);
//   DAC_SetDualChannelData(DAC_Align_12b_R, DAC_DATA.DAC_CH_DATA[1][ch], DAC_DATA.DAC_CH_DATA[0][ch]); 
// 	DAC_DualSoftwareTriggerCmd(ENABLE);	
	return;
}


uint16_t DAC_GetValue(uint8_t ch)
{
	if(ch>7)return 0;
	return DAC_DATA.DAC_EACH_DATA[ch];
}
			
			
/*CD4052�������Ŷ���*/
#define  CD4052_EN_PORT           GPIOA
#define  CD4052_EN_PIN            GPIO_Pin_1
#define  CD4052_EN_SET()          ((GPIO_TypeDef *)CD4052_EN_PORT)->BSRRL = CD4052_EN_PIN
#define  CD4052_EN_CLR()          ((GPIO_TypeDef *)CD4052_EN_PORT)->BSRRH = CD4052_EN_PIN

#define  CD4052_A_PORT            GPIOH
#define  CD4052_A_PIN             GPIO_Pin_2
#define  CD4052_A_SET()           ((GPIO_TypeDef *)CD4052_A_PORT)->BSRRL = CD4052_A_PIN
#define  CD4052_A_CLR()           ((GPIO_TypeDef *)CD4052_A_PORT)->BSRRH = CD4052_A_PIN

#define  CD4052_B_PORT            GPIOH
#define  CD4052_B_PIN             GPIO_Pin_3
#define  CD4052_B_SET()           ((GPIO_TypeDef *)CD4052_B_PORT)->BSRRL = CD4052_B_PIN
#define  CD4052_B_CLR()           ((GPIO_TypeDef *)CD4052_B_PORT)->BSRRH = CD4052_B_PIN

#define  CD4052_CONNECT()         CD4052_EN_CLR()
#define  CD4052_DISCONNECT()      CD4052_EN_SET()

// static uint16_t const DAC_CH_DATA[2][4] = {
// 	{W_VREF,       GR_VREF,        Short_VREF,     ARC_VREF},
// 	{ARC_4us_VREF, ARC_10us_VREF,  ARC_20us_VREF,  ARC_40us_VREF},
// };



/*
*********************************************************************************************************
*	�� �� ��: CD4052_GPIOConfig
*	����˵��: DACͨ��һ��PA4��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CD4052_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
    /*  ʹ������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOH, ENABLE);
    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		  /* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	  /* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	/* IO������ٶ� */
	
	
	GPIO_InitStructure.GPIO_Pin = CD4052_EN_PIN;
	GPIO_Init(CD4052_EN_PORT, &GPIO_InitStructure);  	
	
	GPIO_InitStructure.GPIO_Pin = CD4052_A_PIN | CD4052_B_PIN;
	GPIO_Init(GPIOH, &GPIO_InitStructure); 
  
}


/*
*********************************************************************************************************
*	�� �� ��: DAC_GPIOConfig
*	����˵��: DACͨ��һ��PA4��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DAC_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
    /*  ʹ������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    /* DACͨ��1����������ã�PA.4�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
}

/*
*********************************************************************************************************
*	�� �� ��: DAC_Ch1_Ch2_TriangleConfig
*	����˵��: DACͨ��1ͨ��2������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DAC_Ch1_Ch2_TriangleConfig(void)
{
	DAC_InitTypeDef  DAC_InitStructure;
    
	/* DAC ����ʱ��ʹ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    
	/* DACͨ������ */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/*ʹ��DACͨ��1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	
	/*ʹ��DACͨ��2 */
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	/* ����DAC˫ͨ�� DHR12RD �Ĵ��� */
	DAC_SetDualChannelData(DAC_Align_12b_R, 0, 0);
	
	DAC_DualSoftwareTriggerCmd(ENABLE);
	
	/*һֱ��ͨGR_VERF*/
//	CD4054_DAC_OUTPUT(1); 
	
	
}



/*
*********************************************************************************************************
*	�� �� ��: CD4054_DAC_OUTPUT
*	����˵��: ͨ��4054���
*	��    ��: ch:0~3  �����ͨ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CD4054_DAC_OUTPUT(uint8_t ch)
{
	if(ch>3)return;
	CD4052_DISCONNECT(); 
	DAC_DualSoftwareTriggerCmd(DISABLE);
  DAC_SetDualChannelData(DAC_Align_12b_R, DAC_DATA.DAC_CH_DATA[1][ch], DAC_DATA.DAC_CH_DATA[0][ch]); 
	DAC_DualSoftwareTriggerCmd(ENABLE);	
	if(ch & 0x01){CD4052_A_SET();}else{CD4052_A_CLR();}
	if(ch & 0x02){CD4052_B_SET();}else{CD4052_B_CLR();}
	CD4052_CONNECT(); 
}



/*
*********************************************************************************************************
*	�� �� ��: CD4054_DAC_OUTPUT
*	����˵��: ͨ��4054���
*	��    ��: ch:0~3  �����ͨ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DAC_VREF_Refresh(void)
{
	static uint8_t i = 0,j = 0;
	if(j){
		CD4052_DISCONNECT(); 
		j = 0;
		return;
	}
	i++;
	if(i % 2){
		CD4052_DISCONNECT(); 
		DAC_DualSoftwareTriggerCmd(DISABLE);
		DAC_SetDualChannelData(DAC_Align_12b_R, DAC_DATA.DAC_CH_DATA[1][i/2], DAC_DATA.DAC_CH_DATA[0][i/2]); 
		DAC_DualSoftwareTriggerCmd(ENABLE);
		if((i/2) & 0x01){CD4052_A_SET();}else{CD4052_A_CLR();}
		if((i/2) & 0x02){CD4052_B_SET();}else{CD4052_B_CLR();}
	}else{			
		CD4052_CONNECT(); 
		j = 1;
	}
	i = i % 8;
	
}


/* define ---------------------------------------------------------------------*/
#define ADC1_DR_ADDRESS          ((uint32_t)0x4001204C)
#define ADC2_DR_ADDRESS          ((uint32_t)0x4001214C)
#define ADC3_DR_ADDRESS          ((uint32_t)0x4001224C)

#define ADC_FLITER_POOL_SIZE     (8)          //AD�˲����ݳش�С 
#define ADC_FLITER_FACTER        (6)          //AD�˲�����

/* ���� ----------------------------------------------------------------------*/
union{	

	volatile  uint16_t   ADC_CH_VALUE[ADC_FLITER_POOL_SIZE][5];
	
	struct{
		volatile  uint16_t SELF_VREF_Value; 
		volatile  uint16_t W_V_VREF_Value;        
		volatile  uint16_t W_I_VREF_Value;           
		volatile  uint16_t GR_I_VREF_Value;          
		volatile  uint16_t GR_V_VREF_Value;      
	}EACH_CH_Value;
	
}ADC_VALUE;

static  volatile  uint32_t  AD_VALUE_BUFFER_POOL[5] = {0,0,0,0,0};
static  volatile  uint16_t  AD_VALUE_BUFFER[5];

/*
*********************************************************************************************************
*	�� �� ��: ADC_GPIOConfig
*	����˵��: ADCͨ����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ADC_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
    /*  ʹ������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;       //ģ������ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				   //VREF_SELF_AD_IN
	GPIO_Init(GPIOF, &GPIO_InitStructure); 
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
}


/*
*********************************************************************************************************
*	�� �� ��: ADC_DMAConfig
*	����˵��: ADCͨ��DMA����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ADC_DMAConfig(void)
{
  DMA_InitTypeDef       DMA_InitStructure;
	NVIC_InitTypeDef      NVIC_InitStructure;
	/* ʹ������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&AD_VALUE_BUFFER[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 5 * 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	/* DMA2_Stream0 enable */
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	  /* ʹ�ܽ��մ�������ж� */
//	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE); 	
	
	/* ʹ�� DMA Stream �ж�ͨ�� */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

/*
*********************************************************************************************************
*	�� �� ��: ADC_ADC3_Config
*	����˵��: ADCͨ����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ADC_ADC3_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	/****************************************************************************   
	  PCLK2 = HCLK / 2 
	  ����ѡ�����2��Ƶ
	  ADCCLK = PCLK2 /2 = HCLK / 4 = 168 / 4 = 42M
      ADC����Ƶ�ʣ� Sampling Time + Conversion Time = 3 + 12 cycles = 15cyc
                    Conversion Time = 42MHz / 15cyc = 2.8Mbps. 
	*****************************************************************************/
	/* ADC�������ֳ�ʼ��**********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC3 ��ʼ�� ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 5;
	ADC_Init(ADC3, &ADC_InitStructure);


	ADC_RegularChannelConfig(ADC3, ADC_Channel_4 , 1, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 2, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 3, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 4, ADC_SampleTime_480Cycles); 
	ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 5, ADC_SampleTime_480Cycles); 
	
	/* ʹ��DMA����(��ADCģʽ) */
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

	/* ʹ�� ADC3 DMA */
	ADC_DMACmd(ADC3, ENABLE);

	/* ʹ�� ADC3 */
	ADC_Cmd(ADC3, ENABLE);
	
	/* �������ADCת�� */ 
	ADC_SoftwareStartConv(ADC3);
}

/*
*********************************************************************************************************
*	�� �� ��: Read_AD_Value
*	����˵��: ��ȡADת���Ľ��
*	��    ��: Ҫ��ȡ��ͨ��
*	�� �� ֵ: ADֵ
*********************************************************************************************************
*/
uint16_t Read_AD_Value(uint8_t ch)
{
	uint8_t i = 0, j = 0;
	uint32_t ADC_temp = 0;
	uint16_t ADC_DATA[ADC_FLITER_POOL_SIZE];
	if(ch>4) return 0;
    
	for(; i < ADC_FLITER_POOL_SIZE; i++)
    {
        ADC_DATA[i] = ADC_VALUE.ADC_CH_VALUE[i][ch];
    }
    
	for(i=1;i<(ADC_FLITER_POOL_SIZE-1);i++)
    {
		for(j=0;j<(ADC_FLITER_POOL_SIZE-i);j++)
        {
			if(ADC_DATA[j]>ADC_DATA[j+1])
            {
                ADC_temp = ADC_DATA[j];
                ADC_DATA[j] = ADC_DATA[j+1];
                ADC_DATA[j+1] = ADC_temp;
            }
		}
    }
    
	j = 0;
	ADC_temp = 0;
    
	for(i = (ADC_FLITER_POOL_SIZE / 4); i < (ADC_FLITER_POOL_SIZE * 3 / 4); i++)
    {
		ADC_temp += ADC_DATA[i];// + ADC_DATA[i-1] + ADC_DATA[i] + ADC_DATA[i+1];
		j++;
	}
    
    AD_VALUE_BUFFER_POOL[ch] =
                (uint16_t)((float)
                (AD_VALUE_BUFFER_POOL[ch] * (ADC_FLITER_FACTER-1) + ADC_temp / j)
                / (float)ADC_FLITER_FACTER);
    return	AD_VALUE_BUFFER_POOL[ch] = (uint16_t)((float)(AD_VALUE_BUFFER_POOL[ch] * (ADC_FLITER_FACTER-1) + ADC_temp / j)/(float)ADC_FLITER_FACTER);
//  return ADC_temp / j;
//	return ADC_VALUE.ADC_CH_VALUE[0][ch];
}


/*
*********************************************************************************************************
*	�� �� ��: Read_AD_Value_Cal
*	����˵��: У׼ʱ��ȡADת���Ľ��
*	��    ��: Ҫ��ȡ��ͨ��
*	�� �� ֵ: ADֵ
*********************************************************************************************************
*/
uint16_t Read_AD_Value_Cal(uint8_t ch)
{
	uint8_t i = 0, j = 0;
	uint32_t ADC_temp = 0;
	uint16_t ADC_DATA[ADC_FLITER_POOL_SIZE];
	if(ch>4) return 0;
	for(;i<ADC_FLITER_POOL_SIZE;i++) ADC_DATA[i] = ADC_VALUE.ADC_CH_VALUE[i][ch];
	for(i=1;i<(ADC_FLITER_POOL_SIZE-1);i++)
		for(j=0;j<(ADC_FLITER_POOL_SIZE-i);j++){
			if(ADC_DATA[j]>ADC_DATA[j+1]){ADC_temp = ADC_DATA[j];ADC_DATA[j] = ADC_DATA[j+1];ADC_DATA[j+1] = ADC_temp;}
		}
	j = 0;
	ADC_temp = 0;
	for(i=(ADC_FLITER_POOL_SIZE / 4);i<(ADC_FLITER_POOL_SIZE * 3 / 4);i++){
		ADC_temp += ADC_DATA[i];// + ADC_DATA[i-1] + ADC_DATA[i] + ADC_DATA[i+1];
		j++;
	}
  return	AD_VALUE_BUFFER_POOL[ch] = (uint16_t)((float)(AD_VALUE_BUFFER_POOL[ch] * (8-1) + ADC_temp / j)/(float)8);
//  return ADC_temp / j;
//	return ADC_VALUE.ADC_CH_VALUE[0][ch];
}


/*
*********************************************************************************************************
*	�� �� ��: AD_DA_Config
*	����˵��: ADC��ʼ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  AD_DA_Config(void)
{
	CD4052_GPIOConfig();
	DAC_GPIOConfig();
	DAC_DeInit();
	DAC_Ch1_Ch2_TriangleConfig();
	
	ADC_DeInit();
	ADC_DMAConfig();
	ADC_GPIOConfig();
	ADC_ADC3_Config();
	
	TIM2_NVIC_Configuration();
	TIM2_Configuration();
	
	DAC_DATA.DAC_EACH_DATA[0]  = 0;
	DAC_DATA.DAC_EACH_DATA[1]  = 0; 	
	DAC_DATA.DAC_EACH_DATA[2]  = 3000;
	DAC_DATA.DAC_EACH_DATA[3]  = 0;
	DAC_DATA.DAC_EACH_DATA[4]  = 0;
	DAC_DATA.DAC_EACH_DATA[5]  = 0;
	DAC_DATA.DAC_EACH_DATA[6]  = 0;
	DAC_DATA.DAC_EACH_DATA[7]  = 0;
}


/*Ϊ��ʱˢ�»���ֵ����*/
void TIM2_IRQHandler(void)
{
// 	static uint8_t i = 0;
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);   
		DAC_VREF_Refresh();
		{
			static uint8_t counter = 0;
			uint8_t i = 0;
			for(i=0;i<5;i++){
				ADC_VALUE.ADC_CH_VALUE[counter][i] = AD_VALUE_BUFFER[i];
			}
			counter++;
			if(counter >= ADC_FLITER_POOL_SIZE)counter = 0;
		}
// 		DAC_DATA.VREF_Value.GR_VREF_Value = DAC_DATA.VREF_Value.GR_VREF_Value++ % 4096;
//  		CD4054_DAC_OUTPUT(1);
// 		i = (++i) % 4;
	}		 	
}

void DMA2_Stream0_IRQHandler(void)
{
	uint8_t i = 0, j = 0 , ch = 0;
	uint32_t ADC_temp = 0;
	uint16_t ADC_DATA[ADC_FLITER_POOL_SIZE];
	if(DMA2->LISR & DMA_FLAG_TCIF0)
 	{
		DMA2->LIFCR = DMA_FLAG_TCIF0;		
 	
	
		for(ch=0;ch<5;ch++){
			for(;i<ADC_FLITER_POOL_SIZE;i++) ADC_DATA[i] = ADC_VALUE.ADC_CH_VALUE[i][ch];
			for(i=1;i<(ADC_FLITER_POOL_SIZE-1);i++)
				for(j=0;j<(ADC_FLITER_POOL_SIZE-i);j++){
					if(ADC_DATA[j]>ADC_DATA[j+1]){ADC_temp = ADC_DATA[j];ADC_DATA[j] = ADC_DATA[j+1];ADC_DATA[j+1] = ADC_temp;}
				}
			j = 0;
			for(i=(ADC_FLITER_POOL_SIZE / 4);i<(ADC_FLITER_POOL_SIZE * 3 / 4);i++){
				ADC_temp += ADC_DATA[i];// + ADC_DATA[i-1] + ADC_DATA[i] + ADC_DATA[i+1];
				j++;
			}
			AD_VALUE_BUFFER_POOL[ch] = (AD_VALUE_BUFFER_POOL[ch] * (ADC_FLITER_FACTER-1) + ADC_temp / j)/ADC_FLITER_FACTER;
		}
	}
}

//�Լ��׼��ѹ
#include "spi_cpld.h"


static void Delay(volatile uint32_t t)
{
	while(t--){__NOP();}
}


/*
*********************************************************************************************************
*	�� �� ��: VREF_SelfTest_CH_Change
*	����˵��: �ο���ѹ��ͨ��ѡ��
*	��    ��: ch:0~7 ѡ���ͨ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void VREF_SelfTest_CH_Change(uint8_t ch)
{
	if(ch>7)return;
	CPLD_GPIO_Control(Self_CD4051_EN,1);  //��ֹ4051�����
//	Delay(100);
	if(ch & 0x01){CPLD_GPIO_Control(Self_CD4051_A,1);}else{CPLD_GPIO_Control(Self_CD4051_A,0);}
	if(ch & 0x02){CPLD_GPIO_Control(Self_CD4051_B,1);}else{CPLD_GPIO_Control(Self_CD4051_B,0);}
	if(ch & 0x04){CPLD_GPIO_Control(Self_CD4051_C,1);}else{CPLD_GPIO_Control(Self_CD4051_C,0);}
	CPLD_GPIO_Control(Self_CD4051_EN,0);  //ʹ��4051�����
	Delay(35000);
}






