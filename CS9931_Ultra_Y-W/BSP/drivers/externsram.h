/*
 * Copyright(c) 2013,
 * All rights reserved
 * �ļ����ƣ�externsram.h
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0�������ɱ�д
 * �޸ļ�¼��
 * V1.0, 2014.07.26, �˰汾����Թ����CS99xxZ(7����)ϵ��������ƣ�Ŀǰ������CS99xxZ(7����)ϵ��
 *
 */
#ifndef __EXTERNSRAM_H
#define __EXTERNSRAM_H


#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
			
/******************* <Include> ********************/
#include <stm32f4xx.h>
#include "CS99xx.h"		
		
/******************* <define> *********************/
#define		EXTERNSRAMADDR		(__IO uint8_t *)((uint32_t)(0x64000000))

#define		SRAM_FILE_START			(0X00000000)
#define		SRAM_FILE_END			(0x00001000)
		
	
struct file_info_sram_t
{
	struct file_info_t	t[FILE_NUM];
};
		
#define		file_info_sram			((struct file_info_sram_t *)(EXTERNSRAMADDR + SRAM_FILE_START))


/******************* �������� *********************/
void externsram_init(void);

/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif

