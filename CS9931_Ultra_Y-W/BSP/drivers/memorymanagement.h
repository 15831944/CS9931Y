/*
 * Copyright(c) 2013,
 * All rights reserved
 * �ļ����ƣ�memorymanagement.h
 * ժ  Ҫ  ��ͷ�ļ�
 * ��ǰ�汾��V1.0�������ɱ�д
 * �޸ļ�¼��
 * V1.0, 2014.07.26, �˰汾����Թ����CS99xxZ(7����)ϵ��������ƣ�Ŀǰ������CS99xxZ(7����)ϵ��
 *
 */
#ifndef __MEMORYMANAGEMENT_H
#define __MEMORYMANAGEMENT_H


#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
			
/******************* <Include> ********************/
#include <stm32f4xx.h>
#include "CS99xx.h"
		
/******************* <define> *********************/
#define				FLASH_FILE_START		(0X00001000)	//4k
#define				FLASH_FILE_END			(0x00002000)

#define				FLASH_BKP_ADDR			(0x00000000)
#define				FLASH_OFFSET			(0x00001000)
		
#define				FLASH_BKP_VALUE			(0xA5A5A5A5)

struct flash_info_t
{
	uint32_t	bkp;
	uint32_t	totalsize;
	uint32_t	yongsize;
	uint32_t	offset;
	uint32_t	current_file;
};

struct result_headinfo_t
{
	/* ͳ����Ϣ */
	uint32_t	test_count;
	uint32_t	pass_count;
	/* ������Ϣ */
	uint32_t	total_size;
	uint32_t	single_size;
	/* ��ʼ��ַ */
	uint32_t	start_addr;
	/* ��ǰ��� */
	uint32_t	current_numb;
	/* ���ݼ�¼ */
	uint32_t	bkp;
};

struct result_info_t
{
	/* ������Ϣ */
	char name[20];
	uint16_t	s_voltage;
	uint16_t	s_current;
	uint16_t	s_hightlimit;
	uint16_t	s_lowlimit;
	uint16_t	s_arc;
	uint16_t	s_outfreq;
	uint16_t	s_testtime;
	uint16_t	s_gear;
	uint16_t  s_powerhigh;
	uint16_t  s_powerlow;
	uint16_t  s_factorhigh;
	uint16_t  s_factorlow;
	/* ������Ϣ */
	uint8_t	current_step;
	uint8_t	total_step;
	/* ģʽ */
	enum test_mode_type		mode;
	/* ������Ϣ */
	enum error_type			error;
	/* ʱ����Ϣ */
	struct rtc_time_type	time;
	
	/* �����Ϣ */
	uint16_t	voltage;
	uint16_t	current;
	uint16_t	currents;
	uint32_t	resister;
	uint16_t	testtime;
	uint16_t  power;
	uint16_t  factor;
	char _bar_code[40];
};

/******************* �������� *********************/
void memorymanagement_init(void);
void load_filetosram(void);
void load_sramtofile(void);

uint32_t memory_result_delete(void);
struct result_info_t *memory_result_read(uint32_t numb);
uint32_t memory_result_write(struct result_info_t *temp);
void result_save(enum test_mode_type mode,enum error_type	error,void *p_Parm,
								uint16_t	voltage,
								uint16_t	current,
								uint16_t	currents,
								uint32_t	resister,
								uint16_t	testtime);

void result_save_cw_gr(enum test_mode_type mode,enum error_type	error,void *p_Parm,
								uint16_t	voltage,
								uint16_t	current,
								uint16_t	currents,
								uint16_t	resister,
								uint16_t	testtime);

void result_save_pw(enum test_mode_type mode,enum error_type	error,void *p_Parm,
								uint16_t	voltage,
								uint16_t	current,
								uint16_t	power,
								uint32_t	factor,
								uint16_t	testtime);

void memory_systems_init(void);
void memory_systems_open(void);
void memory_systems_save(void);
void memory_systems_defa(void);

extern struct flash_info_t			flash_info;
extern struct result_headinfo_t		result_headinfo;

/**************************************************/
#if defined(__cplusplus)
    }
#endif 
/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif

