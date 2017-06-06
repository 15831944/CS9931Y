/*
 * FreeModbus Libary: STM32 Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"
#include "rtthread.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
/* ----------------------- Start implementation -----------------------------*/

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if (xRxEnable)
	{
		SLAVE_RS485_RECEIVE_MODE;
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
	else
	{
		SLAVE_RS485_SEND_MODE;
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}
	if (xTxEnable)
	{
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
}

void vMBPortClose(void)
{
	USART_ITConfig(USART2, USART_IT_TXE | USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, DISABLE);
}
//Ĭ��һ���ӻ� ����1 �����ʿ�����  ��ż���������
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* ʹ��USARTʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* ���� USART ���ŵ� AF7 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	

	/* ����GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//======================���ڳ�ʼ��=======================================
	USART_InitStructure.USART_BaudRate = ulBaudRate;
	//����У��ģʽ
	switch (eParity)
	{
	case MB_PAR_NONE: //��У��
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		break;
	case MB_PAR_ODD: //��У��
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case MB_PAR_EVEN: //żУ��
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	default:
		return FALSE;
	}

	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	if (ucPORT != 2)
		return FALSE;

	ENTER_CRITICAL_SECTION(); //��ȫ���ж�

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	USART_Cmd(USART2, ENABLE);

	//=====================�жϳ�ʼ��======================================
	//����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXIT_CRITICAL_SECTION(); //��ȫ���ж�

	return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	USART_SendData(USART2, ucByte);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = USART_ReceiveData(USART2);
	return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
	pxMBFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
	pxMBFrameCBByteReceived();
}
/*******************************************************************************
 * Function Name  : USART2_IRQHandler
 * Description    : This function handles USART1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
extern struct rt_mailbox modbus_mb;


void USART2_IRQHandler_Unused(void)
{
	
	rt_interrupt_enter();

	
	
	//rt_interrupt_enter();
	//�����ж�
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		prvvUARTRxISR();
	}
	//�����ж�
	if (USART_GetITStatus(USART2, USART_IT_TXE) == SET)
	{
		prvvUARTTxReadyISR();
	}

	rt_mb_send(&modbus_mb, 1);
	/* leave interrupt */
	rt_interrupt_leave();
	
	
	
	//rt_interrupt_leave();
}
