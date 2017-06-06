#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "stm32f4xx.h"
#include "CS99xx.h"
#include "rtservice.h"
#include "rtdef.h"
#include "stdio.h"	/* ���� NULL ���� */
#include "stdlib.h"
#include "spi_flash.h"

// #define FILE_MAX	120	/* �ļ��������Ŀ */
// #define TOTALSTEP_NUM	70	/* �ļ�������������� */

/********************** Flash���� **************************/
#define F_PAGE_SIZE (1024*4)	/* Flash һҳ�Ĵ�С */

#define	FILE_BASE		(1024*64)	/* �ļ���Ϣ�洢��ַ �ռ�64k ��Ŵ�1400���ļ� */
#define FILE_OFFSET		(sizeof(struct file_info_t))	/* ���44���ֽ� */
#define PER_P_FILES		(F_PAGE_SIZE/FILE_OFFSET)	/* ÿ4k�ܷŵ��ļ���Ŀ ��� 93���ļ� */

#define GROUP_BASE		(1024*128)	/* �������ַ 128k */
#define GROUP_OFFSET 	(1024*16)	/* 16K */

#define STEP_OFFSET		sizeof(UN_STR)	/* ���48���ֽ� */
#define PER_P_STEPS		(F_PAGE_SIZE/STEP_OFFSET)	/* ÿ4k�ܴ�ŵĲ�����Ŀ ���85�� */

/***********************************************************/

struct step_com{
	rt_uint8_t step;
	rt_uint8_t mode;
};

typedef union{
	struct step_com com;
	struct step_acw_t acw;
	struct step_dcw_t dcw;
	struct step_ir_t ir;
	struct step_gr_t gr;
	struct step_lc_t lc;
	struct step_acw_gr_t acw_gr;
	struct step_dcw_gr_t dcw_gr;
	struct step_ir_gr_t ir_gr;
	struct step_pw_t pw;
}UN_STR;

/* �ڵ㶨�� */
typedef struct{
	UN_STR un;
	struct rt_list_node list;	/* �Ѿ����õ����� */
}STEP_NODE;

/* ������ */
typedef struct{
	rt_uint8_t size;/* ������ */
	struct rt_list_node *head;/* ����ͷ */
	struct rt_list_node *tail;/* ����β */
}CS99XX_LIST;

extern struct rt_list_node *head;/* ����ͷ */
extern struct rt_list_node *tail;/* ����β */
// struct rt_list_node *g_free_list;/* ָ������� */

extern STEP_NODE step_pool[];/* �����������еĲ� */
extern struct file_info_t file_info[];
extern CS99XX_LIST cs99xx_list;/* ������ */

extern struct rt_list_node *g_cur_step;/* ָ��ǰ�� */

void init_list(void);
void init_acw_step(UN_STR *un);
void init_dcw_step(UN_STR *un);
void init_ir_step(UN_STR *un);
void init_gr_step(UN_STR *un);
void init_lc_step(UN_STR *un);
void init_ir_step(UN_STR *un);
void init_pw_step(UN_STR *un);
void init_acw_gr_step(UN_STR *un);
void init_dcw_gr_step(UN_STR *un);

rt_int32_t read_flash_to_files(void);
rt_int32_t save_files_to_flash(void);
rt_int32_t save_file_to_flash(rt_uint8_t n);
rt_int8_t save_steps_to_flash(const rt_uint8_t n);
rt_int8_t read_flash_to_list(const rt_uint8_t n);

rt_int8_t insert_after(rt_uint8_t step);
struct rt_list_node *position_step(rt_uint8_t pos);
STEP_NODE *remove_step(rt_uint8_t step);
rt_int8_t swap_step(rt_uint8_t one, rt_uint8_t two);

#endif//__MEMORY_H__
