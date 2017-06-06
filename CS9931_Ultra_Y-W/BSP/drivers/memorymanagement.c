/*
 * Copyright(c) 2013,
 * All rights reserved
 * �ļ����ƣ�memorymanagement.c
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 * V1.0, 2014.07.26, �˰汾����Թ����CS99xxZ(7����)ϵ��������ƣ�Ŀǰ������CS99xxZ(7����)ϵ��
 *
 */
#include <rtthread.h>
#include "memorymanagement.h"
#include "externsram.h"
#include "spi_flash.h"
#include "memory.h"
#include "CS99xx.h"
#include <rtthread.h>


extern int dfs_init(void);
extern void rt_hw_sdcard_init(void);

extern u8 bar_code[];
extern u8 usb_host_flag;

struct flash_info_t				flash_info;
struct result_headinfo_t		result_headinfo;

int result_fd;

// void load_filetosram(void)
// {
// 	FLASH_CS_SET(1);
// 	sf_ReadBuffer((uint8_t *)(EXTERNSRAMADDR+SRAM_FILE_START),FLASH_FILE_START,FLASH_FILE_END-FLASH_FILE_START);
// }

// void load_sramtofile(void)
// {
// 	flash_unlock();
// 	FLASH_CS_SET(1);
// 	sf_WriteBuffer((uint8_t *)(EXTERNSRAMADDR+SRAM_FILE_START),FLASH_FILE_START,FLASH_FILE_END-FLASH_FILE_START);
// 	flash_lock();
// }
static void file_default_init(void)
{
	file_info[0].en = 1;
	rt_strncpy(file_info[0].name,"default",8);
	file_info[0].mode		=	N_WORK; // ����ģʽ Nģʽ or Gģʽ
	file_info[0].totalstep	=	1; // �ܲ���
	// ����ʱ�� ��:10000s(2014-06-06 12:00:00)
	{
		rt_device_t device;
	
		device = rt_device_find("rtc");
		if(device != RT_NULL)
		{
			rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &file_info[0].time);						
		}
	}
	file_info[0].arc		=	0; // �绡���ģʽ �ȼ� or ����
	file_info[0].outdelay	=	0; // �����ʱʱ�� ��Χ 0 - 999.9s
	file_info[0].passtime	=	2; // PLC PASS����ʱ�� ��Χ 0 - 999.9s
	file_info[0].buzzertime	=	2; // ����������ʱ�䣨PASSʱ����ʱ�䣩
}

/*
 * ��������memorymanagement_init
 * ����  ��memorymanagement��ʼ��
 * ����  ����
 * ���  ����
 */
void memorymanagement_init(void)
{	
//	uint16_t  index = 0;
//	uint8_t *pdata = (uint8_t *)0x40024000;
	/* ϵͳ������ʼ�� */
	memory_systems_init();
	
	FLASH_CS_SET(1);	// ѡ�����flash
	sf_ReadBuffer((uint8_t *)&flash_info,FLASH_BKP_ADDR,sizeof(flash_info));//��ȡflash�ļ���Ϣ
	
//	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//	PWR->CR      |= PWR_CR_DBP;
//	RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;
//	PWR->CSR     |= PWR_CSR_EWUP;
	// �жϱ���ֵ
	if(flash_info.bkp != FLASH_BKP_VALUE)
	{
		// flash δ��ʼ���� �� �״ο���
		
		// ��ʼ���û�����
		file_default_init();
		init_list();
		save_files_to_flash();
		save_steps_to_flash(0);
		
		// д������ �� flash��Ϣ
		flash_info.bkp = FLASH_BKP_VALUE;
		flash_info.totalsize = 16*1024*1024;
		flash_info.yongsize = 0;
		flash_info.offset = FLASH_OFFSET;
		flash_info.current_file = 0;
		sf_WriteBuffer((uint8_t *)&flash_info,FLASH_BKP_ADDR,sizeof(flash_info));
		
//		for(index=0;index<4095;index++)
//		{
//			*(pdata+index) = 0;
//		}
	
	

	}
	
	
	init_list();
	read_flash_to_files();
	read_flash_to_list(flash_info.current_file);
	
	
	
	/* ��ʼ���������flash */
//	FLASH_CS_SET(4);	// ѡ�����flash
//	sf_ReadBuffer((uint8_t *)&result_headinfo,FLASH_BKP_ADDR,sizeof(result_headinfo));//��ȡflash�ļ���Ϣ
	{
			int fd;
			
			fd = open("//info.bin", O_RDONLY, 0);
		
			read(fd,(uint8_t *)&result_headinfo,sizeof(result_headinfo));
			
			close(fd);
	}
	// �жϱ���ֵ
	if(result_headinfo.bkp != FLASH_BKP_VALUE)
	{
		/* ͳ����Ϣ */
		result_headinfo.test_count		= 0;
		result_headinfo.pass_count		= 0;
		/* ������Ϣ */
		result_headinfo.total_size		= 8000;
		result_headinfo.single_size		= sizeof(struct result_info_t);
		/* ��ʼ��ַ */
		result_headinfo.start_addr		= FLASH_FILE_START;
		/* ��ǰ��� */
		result_headinfo.current_numb	= 0;
		// д������ �� flash��Ϣ
		result_headinfo.bkp = FLASH_BKP_VALUE;
//		FLASH_CS_SET(4);	// ѡ�����flash
//		sf_WriteBuffer((uint8_t *)&result_headinfo,FLASH_BKP_ADDR,sizeof(result_headinfo));
		{
			//�ѽ���浽TF����
			 
			int fd;
		
			fd = open("//info.bin",  O_WRONLY | O_CREAT, 0);
			
			write(fd,(uint8_t *)&result_headinfo,sizeof(result_headinfo));
			
			close(fd);
		}
		
	}
	
	 
	
}

uint32_t memory_result_delete(void)
{
	/* ͳ����Ϣ */
	result_headinfo.test_count		= 0;
	result_headinfo.pass_count		= 0;
	/* ������Ϣ */
	result_headinfo.total_size		= 8000;
	result_headinfo.single_size		= sizeof(struct result_info_t);
	/* ��ʼ��ַ */
	result_headinfo.start_addr		= FLASH_FILE_START;
	/* ��ǰ��� */
	result_headinfo.current_numb	= 0;
	// д������ �� flash��Ϣ
	result_headinfo.bkp = FLASH_BKP_VALUE;
//	FLASH_CS_SET(4);	// ѡ�����flash
//	sf_WriteBuffer((uint8_t *)&result_headinfo,FLASH_BKP_ADDR,sizeof(result_headinfo));
	{
			//�ѽ���浽TF����
			
			int fd;
		
			fd = open("//info.bin",  O_WRONLY | O_CREAT, 0);
			
			write(fd,(uint8_t *)&result_headinfo,sizeof(result_headinfo));
			
			close(fd);
	}
	
	unlink("//result.bin");
	
	return 0;
}


struct result_info_t *memory_result_read(uint32_t numb)
{
//	static struct result_info_t result_temp;
//	if(numb < result_headinfo.test_count)
//	{
//		FLASH_CS_SET(4);	// ѡ�����flash
//		sf_ReadBuffer((uint8_t *)&result_temp,result_headinfo.start_addr + numb * result_headinfo.single_size,sizeof(struct result_info_t));
//		return &result_temp;
//	}
//	else return 0;
	
	static struct result_info_t result_temp;
	if(numb < result_headinfo.test_count)
	{
//		FLASH_CS_SET(4);	// ѡ�����flash
//		sf_ReadBuffer((uint8_t *)&result_temp,result_headinfo.start_addr + numb * result_headinfo.single_size,sizeof(struct result_info_t));
//		{
//			//�ѽ���浽TF����
//			
			int fd;
			
			fd = open("//result.bin", O_RDONLY, 0);
			
			lseek(fd,sizeof(struct result_info_t)*numb,0);
		
			read(fd,(uint8_t *)&result_temp,sizeof(struct result_info_t));
			
			close(fd);
//		}
		return &result_temp;
	}
	else return 0;
}



uint32_t memory_result_write(struct result_info_t *temp)
{
//	if(result_headinfo.current_numb < result_headinfo.total_size)
//	{
//		FLASH_CS_SET(4);	// ѡ�����flash
//		sf_WriteBuffer((uint8_t *)temp,result_headinfo.start_addr + result_headinfo.current_numb * result_headinfo.single_size,sizeof(struct result_info_t));
//		result_headinfo.test_count++;
//		result_headinfo.current_numb++;
//		if(temp -> error == PASS)
//			result_headinfo.pass_count++;
//		sf_WriteBuffer((uint8_t *)&result_headinfo,FLASH_BKP_ADDR,sizeof(result_headinfo));
//		return 0;
//	}
//	else return (uint32_t)temp;
	
	if(result_headinfo.current_numb < result_headinfo.total_size)
	{
//		FLASH_CS_SET(4);	// ѡ�����flash
//		sf_WriteBuffer((uint8_t *)temp,result_headinfo.start_addr + result_headinfo.current_numb * result_headinfo.single_size,sizeof(struct result_info_t));
		
		{
			//�ѽ���浽TF����
			
			int fd;
		
			fd = open("//result.bin",  O_WRONLY | O_CREAT, 0);
			
			lseek(fd,sizeof(struct result_info_t)*result_headinfo.current_numb,0);
			
			write(fd,(uint8_t *)temp,sizeof(struct result_info_t));
			
			close(fd);
		}
        
		result_headinfo.test_count++;
		result_headinfo.current_numb++;
        
		if(temp -> error == PASS)
        {
			result_headinfo.pass_count++;
        }
//		sf_WriteBuffer((uint8_t *)&result_headinfo,FLASH_BKP_ADDR,sizeof(result_headinfo));
		{
			//�ѽ���浽TF����
			
			int fd;
            
			fd = open("//info.bin",  O_WRONLY | O_CREAT, 0);
			
			write(fd, (uint8_t *)&result_headinfo, sizeof(result_headinfo));
			
			close(fd);
	  }
		return 0;
	}
	else return (uint32_t)temp;
}


void result_save(enum test_mode_type mode,enum error_type	error,void *p_Parm,
								uint16_t	voltage,
								uint16_t	current,
								uint16_t	currents,
								uint32_t	resister,
								uint16_t	testtime)
{
	rt_device_t device;
	struct rtc_time_type	time;
	struct result_info_t  temp;
	
	
	/* ������Ϣ */
	strcpy(temp.name,file_info[flash_info.current_file].name);  
	switch(mode)
	{
		case ACW:
			temp.s_voltage     = (*(struct step_acw_t *)p_Parm).outvol;
			temp.s_current     = current;
			temp.s_hightlimit  = (*(struct step_acw_t *)p_Parm).curhigh;
			temp.s_lowlimit    = (*(struct step_acw_t *)p_Parm).curlow;
			temp.s_arc         = (*(struct step_acw_t *)p_Parm).arc;
			temp.s_outfreq     = (*(struct step_acw_t *)p_Parm).outfreq;
			temp.s_testtime    = (*(struct step_acw_t *)p_Parm).testtime;
			temp.s_gear        = (*(struct step_acw_t *)p_Parm).curgear;
		break;
		case DCW:
			temp.s_voltage     = (*(struct step_dcw_t *)p_Parm).outvol;
			temp.s_current     = current;
			temp.s_hightlimit  = (*(struct step_dcw_t *)p_Parm).curhigh;
			temp.s_lowlimit    = (*(struct step_dcw_t *)p_Parm).curlow;
			temp.s_arc         = (*(struct step_dcw_t *)p_Parm).arc;
			temp.s_testtime    = (*(struct step_dcw_t *)p_Parm).testtime;
			temp.s_gear        = (*(struct step_dcw_t *)p_Parm).curgear;
		break;
		case GR:
// 			temp.s_voltage     = (*(struct step_gr_t *)p_Parm).outvol;
			temp.s_current     = (*(struct step_gr_t *)p_Parm).outcur;
			temp.s_hightlimit  = (*(struct step_gr_t *)p_Parm).reshigh;
			temp.s_lowlimit    = (*(struct step_gr_t *)p_Parm).reslow;
// 			temp.s_arc         = (*(struct step_gr_t *)p_Parm).arc;
			temp.s_outfreq     = (*(struct step_gr_t *)p_Parm).outfreq;
			temp.s_testtime    = (*(struct step_gr_t *)p_Parm).testtime;
			temp.s_gear        = 0;
		break;
		case LC:
			temp.s_voltage     = (*(struct step_lc_t *)p_Parm).outvol;
			temp.s_current     = (*(struct step_lc_t *)p_Parm).assistvol;
			temp.s_hightlimit  = (*(struct step_lc_t *)p_Parm).curhigh;
			temp.s_lowlimit    = (*(struct step_lc_t *)p_Parm).curlow;
// 			temp.s_arc         = (*(struct step_lc_t *)p_Parm).arc;
			temp.s_outfreq     = (*(struct step_lc_t *)p_Parm).outfreq;
			temp.s_testtime    = (*(struct step_lc_t *)p_Parm).testtime;
			temp.s_gear        = (*(struct step_lc_t *)p_Parm).curgear;
		break;
		
		case IR:
 			temp.s_voltage     = (*(struct step_ir_t *)p_Parm).outvol;
//			temp.s_current     = (*(struct step_ir_t *)p_Parm).outcur;
			temp.s_hightlimit  = (*(struct step_ir_t *)p_Parm).reshigh;
			temp.s_lowlimit    = (*(struct step_ir_t *)p_Parm).reslow;
// 			temp.s_arc         = (*(struct step_gr_t *)p_Parm).arc;
//			temp.s_outfreq     = (*(struct step_ir_t *)p_Parm).outfreq;
			temp.s_testtime    = (*(struct step_ir_t *)p_Parm).testtime;
			temp.s_gear        = (*(struct step_ir_t *)p_Parm).autogear;
		break;
		default:
		
		break;
	}
	/* ������Ϣ */
	temp.current_step = current_step_num - 1;
	temp.total_step   = file_info[flash_info.current_file].totalstep;
	
	/* ģʽ */
	temp.mode = mode;
	
	/* ������Ϣ */
	temp.error = error;
	
	/* ʱ����Ϣ */
	device = rt_device_find("rtc");
	rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time);
	temp.time.year = time.year+1792;
	temp.time.month = time.month;
	temp.time.date = time.date;
	temp.time.hours = time.hours;
	temp.time.minutes = time.minutes;
	temp.time.seconds = time.seconds;
	
	/* �����Ϣ */
	temp.voltage  = voltage;
	temp.current  = current;
	temp.currents = currents;
	temp.resister = resister;
	temp.testtime = testtime;
	
	if(usb_host_flag == 1)//���������ɨ��ǹ
	{
		memcpy(temp._bar_code,bar_code,40);
	}
	else
	{
		memset(temp._bar_code,'\0',40);
	}
	
	
	{
//		uint32_t level;
//		 
//		level = rt_hw_interrupt_disable();
		
//		extern void spi_flash_init(void);
//		spi_flash_init();
//		
		memory_result_write(&temp);
		
//		rt_hw_interrupt_enable(level);
		
	}
	
}

void result_save_cw_gr(enum test_mode_type mode,enum error_type	error,void *p_Parm,
								uint16_t	voltage,
								uint16_t	current,
								uint16_t	currents,
								uint16_t	resister,
								uint16_t	testtime)
{
	rt_device_t device;
	struct rtc_time_type	time;
	struct result_info_t  temp;
	
	
	/* ������Ϣ */
	strcpy(temp.name,file_info[flash_info.current_file].name);  
	switch(mode)
	{
		
		
		case ACW_GR:
			temp.s_current     = (*(struct step_acw_gr_t *)p_Parm).groutcur;
			temp.s_hightlimit  = (*(struct step_acw_gr_t *)p_Parm).grreshigh;
			temp.s_lowlimit    = (*(struct step_acw_gr_t *)p_Parm).grreslow;
			temp.s_outfreq     = (*(struct step_acw_gr_t *)p_Parm).groutfreq;
			temp.s_testtime    = (*(struct step_acw_gr_t *)p_Parm).testtime;
			temp.s_gear        = 0;
		break;
		
		case DCW_GR:
			temp.s_current     = (*(struct step_dcw_gr_t *)p_Parm).groutcur;
			temp.s_hightlimit  = (*(struct step_dcw_gr_t *)p_Parm).grreshigh;
			temp.s_lowlimit    = (*(struct step_dcw_gr_t *)p_Parm).grreslow;
			temp.s_outfreq     = (*(struct step_dcw_gr_t *)p_Parm).groutfreq;
			temp.s_testtime    = (*(struct step_dcw_gr_t *)p_Parm).testtime;
			temp.s_gear        = 0;
		break;
		
		default:
		
		break;
	}
	/* ������Ϣ */
	temp.current_step = current_step_num - 1;
	temp.total_step   = file_info[flash_info.current_file].totalstep;
	
	/* ģʽ */
	temp.mode = mode;
	
	/* ������Ϣ */
	temp.error = error;
	
	/* ʱ����Ϣ */
	device = rt_device_find("rtc");
	rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time);
	temp.time.year = time.year+1792;
	temp.time.month = time.month;
	temp.time.date = time.date;
	temp.time.hours = time.hours;
	temp.time.minutes = time.minutes;
	temp.time.seconds = time.seconds;
	
	/* �����Ϣ */
	temp.voltage  = voltage;
	temp.current  = current;
	temp.currents = currents;
	temp.resister = resister;
	temp.testtime = testtime;
	
	if(usb_host_flag == 1)//���������ɨ��ǹ
	{
		memcpy(temp._bar_code,bar_code,40);
	}
	else
	{
		memset(temp._bar_code,'\0',40);
	}
	
	
	{
//		uint32_t level;
//		 
//		level = rt_hw_interrupt_disable();
		
//		extern void spi_flash_init(void);
//		spi_flash_init();
//		
		memory_result_write(&temp);
		
//		rt_hw_interrupt_enable(level);
		
	}
	
}


void result_save_pw(enum test_mode_type mode,enum error_type	error,void *p_Parm,
								uint16_t	voltage,
								uint16_t	current,
								uint16_t	power,
								uint32_t	factor,
								uint16_t	testtime)
{
	rt_device_t device;
	struct rtc_time_type	time;
	struct result_info_t  temp;
	
	
	/* ������Ϣ */
	strcpy(temp.name,file_info[flash_info.current_file].name);  
	
	temp.s_voltage     = (*(struct step_pw_t *)p_Parm).outvol;
	temp.s_current     = 0;
	temp.s_hightlimit  = (*(struct step_pw_t *)p_Parm).curhigh;
	temp.s_lowlimit    = (*(struct step_pw_t *)p_Parm).curlow;
	temp.s_powerhigh   = (*(struct step_pw_t *)p_Parm).pwhigh;
	temp.s_powerlow    = (*(struct step_pw_t *)p_Parm).pwlow;
	temp.s_factorhigh  = (*(struct step_pw_t *)p_Parm).factorhigh;
	temp.s_factorlow   = (*(struct step_pw_t *)p_Parm).factorlow;
	temp.s_arc         = 0;
	temp.s_outfreq     = (*(struct step_pw_t *)p_Parm).outfreq;
	temp.s_testtime    = (*(struct step_pw_t *)p_Parm).testtime;
	temp.s_gear        = 0;
		
	/* ������Ϣ */
	temp.current_step = current_step_num - 1;
	temp.total_step   = file_info[flash_info.current_file].totalstep;
	
	/* ģʽ */
	temp.mode = mode;
	
	/* ������Ϣ */
	temp.error = error;
	
	/* ʱ����Ϣ */
	device = rt_device_find("rtc");
	rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time);
	temp.time.year = time.year+1792;
	temp.time.month = time.month;
	temp.time.date = time.date;
	temp.time.hours = time.hours;
	temp.time.minutes = time.minutes;
	temp.time.seconds = time.seconds;
	
	/* �����Ϣ */
	temp.voltage  = voltage;
	temp.current  = current;
	temp.currents = 0;
	temp.resister = 0;
	temp.power    = power;
	temp.factor   = factor;
	temp.testtime = testtime;
	
	if(usb_host_flag == 1)//���������ɨ��ǹ
	{
		memcpy(temp._bar_code,bar_code,40);
	}
	else
	{
		memset(temp._bar_code,'\0',40);
	}
	
	
	{
//		uint32_t level;
//		 
//		level = rt_hw_interrupt_disable();
		
//		extern void spi_flash_init(void);
//		spi_flash_init();
//		
		memory_result_write(&temp);
		
//		rt_hw_interrupt_enable(level);
		
	}
	
}


/***********************************************
			ϵͳ�����洢�ӿں���
************************************************/
static void memory_systems_default_setting(void)
{
	/* ����ֵ */
	system_parameter_t.bkp = FLASH_BKP_VALUE;
	
	/* ������ */
	system_parameter_t.key_lock = 0;
	
	
	
	/* ϵͳ���� */
	system_parameter_t.psd.systempasswordm_en		=0;
	system_parameter_t.psd.systempasswordm			=0;
	system_parameter_t.psd.keylockmem_en			=0;
	system_parameter_t.psd.keylockpasswordm_en		=0;
	system_parameter_t.psd.keylockpasswordm			=0;
	
	/* �������� */
	system_parameter_t.env.lcdlight					=10;
	system_parameter_t.env.beepvolume				=0;
	system_parameter_t.env.GFI						=0;
	system_parameter_t.env.resultsave				=0;
	system_parameter_t.env.memorymargintips			=0;
	system_parameter_t.env.portmode					=0;
	system_parameter_t.env.systemlanguage			=0;
	system_parameter_t.env.resetstop				=0;
	system_parameter_t.env.listdisplay				=0;
	system_parameter_t.env.numberingrules			=0;
	
	/* ���ڲ��� */
	system_parameter_t.com.interface				=0;
	system_parameter_t.com.control					=0;
	system_parameter_t.com.address					=0;
	system_parameter_t.com.baud						=0;
	system_parameter_t.com.endcode					=1;  //'#'
	system_parameter_t.com.networkinterface			=0;
	system_parameter_t.com.matchinterface			=0;
	
	/* ϵͳ������־ */
	system_parameter_t.ope.model					=0;
	system_parameter_t.ope.testmodes				=0;
	system_parameter_t.ope.softwareversion			=0;
	system_parameter_t.ope.hardwareversion			=0;
	system_parameter_t.ope.factoryinspectiondate	=0;
	system_parameter_t.ope.boottotalnumber			=0;
	system_parameter_t.ope.testtotalnumber			=0;
	system_parameter_t.ope.startuptime				=0;
	system_parameter_t.ope.totalruntime				=0;
}

void memory_systems_init(void)
{
	memory_systems_open();
	// �жϱ���ֵ
	if(system_parameter_t.bkp != FLASH_BKP_VALUE)
	{
		memory_systems_defa();
		memory_systems_save();
	}
	else
	{
		if(system_parameter_t.psd.keylockmem_en ==0)
		{
			system_parameter_t.key_lock = 0;
			memory_systems_save();
		}
	}
	
	/* ͨѶ�� */
	system_parameter_t.Com_lock = 0;
}

void memory_systems_open(void)
{
	FLASH_CS_SET(1);	
	sf_ReadBuffer((uint8_t *)&system_parameter_t,F_PAGE_SIZE*15,sizeof(struct system_parameter));
}

void memory_systems_save(void)
{
	FLASH_CS_SET(1);	// ѡ�����flash
	sf_WriteBuffer((uint8_t *)&system_parameter_t,F_PAGE_SIZE*15,sizeof(struct system_parameter));
}

void memory_systems_defa(void)
{
	/* �ָ�Ĭ��ֵ */
	memory_systems_default_setting();
	
	/* �����ʼ��ֵ */
	memory_systems_save();
}
/***********************************************/
