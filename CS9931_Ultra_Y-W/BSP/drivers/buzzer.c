/*
 * Copyright(c) 2013,
 * All rights reserved
 * �ļ����ƣ�buzzer.c
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0�������ɱ�д
 * �޸ļ�¼��
 * V1.0, 2014.07.26, �˰汾����Թ����CS99xxZ(7����)ϵ��������ƣ�Ŀǰ������CS99xxZ(7����)ϵ��
 *
 */

#include "buzzer.h"

#define		BUZZER_SETB()			(GPIO_SetBits(GPIOG,GPIO_Pin_6))
#define		BUZZER_CLR()			(GPIO_ResetBits(GPIOG,GPIO_Pin_6))

/*
 * ��������buzzer_GPIO_Config
 * ����  ����������
 * ����  ����
 * ���  ����
 */
static void buzzer_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
			
	/* ��1������GPIOA GPIOC GPIOD GPIOF GPIOG��ʱ��
	   ע�⣺����ط�����һ����ȫ��
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}


/*
 * ��������buzzer_init
 * ����  ��buzzer��ʼ��
 * ����  ����
 * ���  ����
 */
void buzzer_init(void)
{
	buzzer_gpio_config();
	
}
