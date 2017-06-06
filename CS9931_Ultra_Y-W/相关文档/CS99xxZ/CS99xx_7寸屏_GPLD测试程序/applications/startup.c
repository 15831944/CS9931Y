/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-08-31     Bernard      first implementation
 * 2011-06-05     Bernard      modify for STM32F107 version
 */

#include "stm32f4xx.h"
#include "board.h"


#define     ADG509_A0_HIGH()  GPIO_SetBits(GPIOG,GPIO_Pin_13)
#define     ADG509_A0_LOW()   GPIO_ResetBits(GPIOG,GPIO_Pin_13)
#define     ADG509_A1_HIGH()  GPIO_SetBits(GPIOG,GPIO_Pin_14)
#define     ADG509_A1_LOW()   GPIO_ResetBits(GPIOG,GPIO_Pin_14)


u16 RecData;



/**
 * @addtogroup STM32
 */

/*@{*/

extern int  rt_application_init(void);
#ifdef RT_USING_FINSH
extern void finsh_system_init(void);
extern void finsh_set_device(const char* device);
#endif

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define STM32_SRAM_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define STM32_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define STM32_SRAM_BEGIN    (&__bss_end)
#endif

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
	
	while (1) ;
}

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
	/* init board */
	rt_hw_board_init();

	

	/* never reach here */
	return ;
}




//������
int main(void)
{
	
	rtthread_startup();
	

	

//cpld���ƹܽ�����ĵ�16λΪ0	
 	cpld_write(0x01000000);
	
//CPLD���ƹܽ�����ĵ�16λΪ1	
	cpld_write(0x0100FFFF);


//cpld���ƹܽ�������λΪ0		
	cpld_write(0x01010000);
//cpld���ƹܽ�������λΪ1		
	cpld_write(0x0101FFFF);
	
//��ѹ����Ƶ����	
	cpld_write(0x01020577);

//�ӵز���Ƶ����	
	cpld_write(0x01030577);

	
//������ѹ�������Ҳ�������ؽӵز������Ҳ�
	cpld_write(0x01040066);	
	
//�����ӵز������Ҳ����������ѹ�������Ҳ�
//	cpld_write(0x0104EE00);	
		
	ADG509_A0_LOW();
	ADG509_A1_LOW();
	
	
	
	ADG509_A0_HIGH();

	
	RecData=ReadDataFromCPLD(0x02000000);
//	RecData=ReadDataFromCPLD(0x02010000);
	
	while(1)
	{
   
		
		RecData=ReadDataFromCPLD(0x02010000);				//����

	}
}

/*@}*/
