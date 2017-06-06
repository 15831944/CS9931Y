/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�spi_cpld.C
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 */

/******************* �����ļ� *********************/
#include "spi_cpld.h"


#define			SPI_CS_EN()				GPIO_ResetBits(GPIOF,GPIO_Pin_9)
#define			SPI_CS_DIS()			GPIO_SetBits(GPIOF,GPIO_Pin_9)
#define			SPI_MOSI_HIGH()		GPIO_SetBits(GPIOF,GPIO_Pin_8)
#define			SPI_MOSI_LOW()		GPIO_ResetBits(GPIOF,GPIO_Pin_8)
#define			SPI_CLK_HIGH()		GPIO_SetBits(GPIOF,GPIO_Pin_7)
#define			SPI_CLK_LOW()			GPIO_ResetBits(GPIOF,GPIO_Pin_7)
#define			SPI_MISO_DI()			GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11)


/*
 * ��������spi_gpio_init
 * ����  ����ʼ���ܽ�
 * ����  ����
 * ���  ����
 */
static void spi_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE| RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */

	
	/* CPLD_CLK,CPLD_DI(ʵ��ΪCPLD����������),CPLD_CS */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	
	
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		/* ������ʹ�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOI, &GPIO_InitStructure);

//PG��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	/* IO������ٶ� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);




	/* CPLD_INT1 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource10);
	{
		EXTI_InitTypeDef EXTI_InitStructure;

    /* Configure  EXTI  */
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//Falling�½��� Rising����
    /* enable */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line10);
	}
}

/*
 * ��������NVIC_Configuration
 * ����  ����ʼ���ж�������
 * ����  ����
 * ���  ����
 */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the EXTI0 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������spi_cpld_init
 * ����  ����ʼ��cpld
 * ����  ����
 * ���  ����
 */
void spi_cpld_init(void)
{
	spi_gpio_init();
	NVIC_Configuration();
	
}

/*
 * ��������cpld_write
 * ����  ����CPLDд���������һ�ֽ�
 * ����  ������
 * ���  ������ֵ
 */
void cpld_write(u32 SendData)
{
	u8  ShiftCounter;
	u32 DataBuffer;
	
	SPI_CS_EN();																				//CS=0;ƬѡCPLD

	for(ShiftCounter=0;ShiftCounter<26;ShiftCounter++)
	{
		DataBuffer 		 = SendData;
		DataBuffer     = DataBuffer & 0x00000001;				//ȡ�������λ
	  SendData       = SendData >>1;									//��������һλ 
	  SPI_CLK_HIGH(); 																//ʱ���ø�

	  if(DataBuffer == 0x00000000)
	  {
		  SPI_MOSI_LOW();
	  }
	  else
	  {
		  SPI_MOSI_HIGH();
	  }

	  SPI_CLK_LOW();																	//����д��CPLD
		
   }
	 SPI_CS_DIS();																		//CS=1;ȡ��Ƭѡ

}

//��CPLD�ж�����
u16 ReadDataFromCPLD(u32 SendData)
{
	u8  ShiftCounter;
	u16 DataBuffer=0;
	u32 SendDataBuffer;
	
	SendDataBuffer   = SendData;	
	cpld_write(SendDataBuffer);																//��CPLD���Ͷ�����
	
	SPI_CS_EN();																							//Ƭѡ
	
	
	for(ShiftCounter=0;ShiftCounter<16;ShiftCounter++)
	{

		DataBuffer = DataBuffer << 1;													//����һλ

    SPI_CLK_HIGH();																				//ʱ���ø�
	 
	  SPI_CLK_LOW();
    
		
	  if(SPI_MISO_DI() == 1)
  	{
	    DataBuffer = DataBuffer | 0x0001;  									 	//��λ��1
 	  }
    else
    {
	    DataBuffer = DataBuffer & 0xfffe; 										//��λ��0
	  }		
 
  }	
	SPI_CS_DIS();
	
	return(DataBuffer);
}






/*
 * ��������cpld_int1_irq
 * ����  ��cpld�жϴ�����
 * ����  ����
 * ���  ����
 */
void cpld_int1_irq(void)
{
	
}
/********************************************************************************************/
