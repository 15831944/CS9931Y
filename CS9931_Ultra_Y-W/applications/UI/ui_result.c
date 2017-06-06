#include "CS99xx.h"
#include "bsp_listbox.h"
#include "memory.h"
#include "memorymanagement.h"
#include "sui_window.h"

static void result_item_draw(struct font_info_t *font,rt_uint16_t index,rt_uint16_t x,rt_uint16_t y);
u8 usb_mem_flag = 0;

static u8 result_detail(void *arg);
static u8 result_delete(void *arg);
static u8 result_Stat(void *arg);
static u8 result_export(void *arg);
static u8 result_jump(void *arg);

struct 
{
    u16 x;
	u16 y;
	const char *data_en;
	const char *data_ch;
}result_title_name[5]={
	{10,10,"NO.","���"},{100,10,"Mode","����ģʽ"},{230,10,"Error","������Ŀ"},{360,10,"Result","���Խ��"},{490,10,"Time","��¼ʱ��"},
};

static struct rect_type list_box_rect={10,40,26,660};
struct rtgui_listctrl result_list_box={
	&panel_home,
	13,
	8000,
	0,0,
	&list_box_rect,
	result_item_draw
};



void ui_result_thread(void)
{
	u8	i;
	rt_uint32_t msg;
	struct font_info_t font={&panel_home,CL_YELLOW,0x0,1,0,20};

// __system_loop:
	if(panel_flag == 4)
	{
		clr_mem((u16 *)ExternSramHomeAddr,0x19f6,ExternSramHomeSize/2);
// 		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		/* ���� */
		for(i=0;i<5;i++)
		{
			/* �����ж� */
			if(language==1)
				font_draw(result_title_name[i].x,result_title_name[i].y,&font,result_title_name[i].data_en);
			else
				font_draw(result_title_name[i].x,result_title_name[i].y,&font,result_title_name[i].data_ch);
		}	
		result_list_box.current_item = 0;
		result_list_box.start_item = 0;
		listbox_draw(&result_list_box);
		rt_mb_send(&screen_mb, UPDATE_HOME);
		ui_key_updata(0);
	}
	while(panel_flag == 4)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				/* ��ϸ */
				case KEY_F1 | KEY_UP:
				case KEY_ENTER | KEY_UP:
					result_detail((void *)&(result_list_box.current_item));
					rt_mb_send(&screen_mb, UPDATE_HOME);
					break;
				/* ɾ�� */
				case KEY_F2 | KEY_UP:
					result_delete(0);
					result_list_box.current_item = 0;
					result_list_box.start_item = 0;
					listbox_draw(&result_list_box);
					rt_mb_send(&screen_mb, UPDATE_HOME);
					break;
				/* ͳ�� */
				case KEY_F3 | KEY_UP:
					result_Stat(0);
					rt_mb_send(&screen_mb, UPDATE_HOME);
					break;
				/* ���� */
				case KEY_F4 | KEY_UP:
					usb_mem_flag = 1;
					result_export(0);
					usb_mem_flag = 0;
					rt_mb_send(&screen_mb, UPDATE_HOME);
// 				{
// 					struct result_info_t temp;
// 					strcpy(temp.name,"file");
// 					temp.mode = 0;
// 					temp.error = 1;
// 					temp.time.year = 2014;
// 					temp.time.month = 2;
// 					temp.time.date = 2;
// 					temp.time.hours = 11;
// 					temp.time.minutes = 20;
// 					temp.time.seconds = 45;
// 					memory_result_write(&temp);
// 				}
					break;
				/* ��ת */
				case KEY_F5 | KEY_UP:
					result_jump(0);
					listbox_draw(&result_list_box);
					rt_mb_send(&screen_mb, UPDATE_HOME);
					break;
				/* ���� */
				case KEY_F6 | KEY_UP:
					panel_flag = 0;
					break;

				
				case CODE_RIGHT:
				case KEY_U | KEY_DOWN:
					if(result_list_box.current_item>0)result_list_box.current_item --;
					result_list_box.start_item = (result_list_box.current_item / result_list_box.items_count)*result_list_box.items_count;
					listbox_draw(&result_list_box);
					rt_mb_send(&screen_mb, UPDATE_HOME);
				
					break;
				case CODE_LEFT:
				case KEY_D | KEY_DOWN:
					if(result_list_box.current_item<result_list_box.total_items-1)result_list_box.current_item ++;
					result_list_box.start_item = (result_list_box.current_item / result_list_box.items_count)*result_list_box.items_count;
					listbox_draw(&result_list_box);
					rt_mb_send(&screen_mb, UPDATE_HOME);
				
					break;
				case KEY_L | KEY_DOWN:
					if(result_list_box.current_item >= result_list_box.items_count)
						result_list_box.current_item -= result_list_box.items_count;
					result_list_box.start_item = (result_list_box.current_item / result_list_box.items_count)*result_list_box.items_count;
					listbox_draw(&result_list_box);
					rt_mb_send(&screen_mb, UPDATE_HOME);
				
					
					break;
				case KEY_R | KEY_DOWN:
					result_list_box.current_item += result_list_box.items_count;
					if(result_list_box.current_item>result_list_box.total_items-1)result_list_box.current_item=result_list_box.total_items-1;
					result_list_box.start_item = (result_list_box.current_item / result_list_box.items_count)*result_list_box.items_count;
					listbox_draw(&result_list_box);
					rt_mb_send(&screen_mb, UPDATE_HOME);
				
					break;
				default:
					break;
			}
		}
		else
		{
			
		}
	}
}

static void result_item_draw(struct font_info_t *font,rt_uint16_t index,rt_uint16_t x,rt_uint16_t y)
{
	char buf[20];
	static struct result_info_t *temp;
	/* ��ʾ��� */
	rt_sprintf(buf,"%04d",index+1);
	font_draw(x+8,y,font,buf);
	
	temp = memory_result_read(index);
	if(temp != NULL)
	{
		/* ģʽ���� */
		font_draw(x+90+(80-rt_strlen(mode_name[temp->mode])*8)/2,y,font,mode_name[temp->mode]);
		/* ������Ŀ */
		font_draw(x+200+(120-rt_strlen(error_name[language][temp->error])*8)/2,y,font,error_name[language][temp->error]);
		/* ���Խ�� */
		font_draw(x+330+(120-rt_strlen("PASS")*8)/2,y,font,(temp->error == PASS? "PASS":"FAIL"));
		/* ��¼ʱ�� */
		rt_sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",temp->time.year+2000,temp->time.month,temp->time.date,temp->time.hours,temp->time.minutes,temp->time.seconds);
		font_draw(x+480+(120-rt_strlen(buf)*8)/2,y,font,buf);
	}
}

static u8 result_detail(void *arg)
{
	rt_uint32_t msg;
	char buf[20];
	struct panel_type *win;
	struct rect_type rect={140,105,290,400};
	struct font_info_t font={0,0X4208,0XE73C,1,0,16};
	struct result_info_t *temp;

	msg = *(rt_int16_t *)arg;
	temp = memory_result_read(msg);
	if(temp == NULL)
		return 1;
	win = sui_window_create(T_STR("�������","Results details"),&rect);
	font.panel = win;
	
#define		_XOFFSET1	10
#define		_XOFFSET2	210
#define		_XSIZE		80
#define		_YOFFSET	30

	rt_sprintf(buf,"%d",msg+1);
	font_draw(_XOFFSET1,_YOFFSET+10,&font,T_STR("������:","ResultNum:"));		font_draw(_XOFFSET1+_XSIZE,_YOFFSET+10,&font,buf);
	font_draw(_XOFFSET1,_YOFFSET+30,&font,T_STR("�� �� ��:","File Name:")); 	font_draw(_XOFFSET1+_XSIZE,_YOFFSET+30,&font,temp->name);
	font_draw(_XOFFSET1,_YOFFSET+50,&font,T_STR("����ģʽ:","Test Mode:"));		font_draw(_XOFFSET1+_XSIZE,_YOFFSET+50,&font,mode_name[temp->mode]);
	
	switch(temp->mode)
	{
		case ACW:
			rt_sprintf(buf,"%d.%03dkV",temp->s_voltage/1000,temp->s_voltage%1000);
			font_draw(_XOFFSET1,_YOFFSET+70,&font,T_STR("�����ѹ:","OutputV :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+70,&font,buf);
			switch(temp->s_gear){
				
				case I3uA:
					rt_sprintf(buf,"%d.%03duA",temp->s_hightlimit/1000,temp->s_hightlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->s_lowlimit/1000,temp->s_lowlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->current/1000,temp->current%1000);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->currents/1000,temp->currents%1000);
					font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
				break;
				case I30uA:
					rt_sprintf(buf,"%d.%02duA",temp->s_hightlimit/100,temp->s_hightlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->s_lowlimit/100,temp->s_lowlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->current/100,temp->current%100);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->currents/100,temp->currents%100);
					font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
				break;
				
				
				case I300uA:
					rt_sprintf(buf,"%d.%duA",temp->s_hightlimit/10,temp->s_hightlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%duA",temp->s_lowlimit/10,temp->s_lowlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%duA",temp->current/10,temp->current%10);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%duA",temp->currents/10,temp->currents%10);
					font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
					
					break;
				case I3mA:
					rt_sprintf(buf,"%d.%03dmA",temp->s_hightlimit/1000,temp->s_hightlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->s_lowlimit/1000,temp->s_lowlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->current/1000,temp->current%1000);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->currents/1000,temp->currents%1000);
					font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
					break;
				case I30mA:
					rt_sprintf(buf,"%d.%02dmA",temp->s_hightlimit/100,temp->s_hightlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->s_lowlimit/100,temp->s_lowlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->current/100,temp->current%100);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->currents/100,temp->currents%100);
					font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
					break;
				case I100mA:
					rt_sprintf(buf,"%d.%dmA",temp->s_hightlimit/10,temp->s_hightlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%dmA",temp->s_lowlimit/10,temp->s_lowlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%dmA",temp->current/10,temp->current%10);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%dmA",temp->currents/10,temp->currents%10);
					font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
					break;
				default:
					break;
			}
			
			rt_sprintf(buf,"%d/%d",temp->current_step,temp->total_step);
			font_draw(_XOFFSET2,_YOFFSET+30,&font,T_STR("�ļ�����:","File Step:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+30,&font,buf);
// 			rt_sprintf(buf,"%d.%03dmA",temp->s_current/1000,temp->s_current%1000);
// 			font_draw(_XOFFSET2,_YOFFSET+50,&font,"��ʵ����:");font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
			rt_sprintf(buf,T_STR("�ȼ�%d","Rank%d"),temp->s_arc);
			font_draw(_XOFFSET2,_YOFFSET+70,&font,T_STR("�绡���:","ARC      :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%d.%dHz",temp->s_outfreq/10,temp->s_outfreq%10);
			font_draw(_XOFFSET2,_YOFFSET+90,&font,T_STR("���Ƶ��:","OutputFre:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+90,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->s_testtime/10,temp->s_testtime%10);
			font_draw(_XOFFSET2,_YOFFSET+110,&font,T_STR("����ʱ��:","Test Time:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+110,&font,buf);
			
			/**************************************************************************************************************/
			rt_sprintf(buf,"%d.%03dkV",temp->voltage/1000,temp->voltage%1000);
			font_draw(_XOFFSET1,_YOFFSET+140,&font,T_STR("���Ե�ѹ:","Test Vol :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+140,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->testtime/10,temp->testtime%10);
			font_draw(_XOFFSET1,_YOFFSET+160,&font,T_STR("����ʱ��:","Test Time:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+160,&font,buf);
			font_draw(_XOFFSET1,_YOFFSET+180,&font,T_STR("������Ϣ:","ErrorInfo:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+180,&font,error_name[language][temp->error]);
			
			font_draw(_XOFFSET2,_YOFFSET+160,&font,T_STR("���Խ��:","Test Res:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+160,&font,(temp->error == PASS? "PASS":"FAIL"));
			
			break;
		case DCW:
			rt_sprintf(buf,"%d.%03dkV",temp->s_voltage/1000,temp->s_voltage%1000);
			font_draw(_XOFFSET1,_YOFFSET+70,&font,T_STR("�����ѹ:","OutputVol:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+70,&font,buf);
			switch(temp->s_gear){
				case I3uA:
					rt_sprintf(buf,"%d.%03duA",temp->s_hightlimit/1000,temp->s_hightlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up   :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->s_lowlimit/1000,temp->s_lowlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->current/1000,temp->current%1000);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					strcpy(buf,"-.---uA");
					font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��ʵ����:","Real Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
				break;
				case I30uA:
					rt_sprintf(buf,"%d.%02duA",temp->s_hightlimit/100,temp->s_hightlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up   :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->s_lowlimit/100,temp->s_lowlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->current/100,temp->current%100);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					strcpy(buf,"--.--uA");
					font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��ʵ����:","Real Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
				break;
			
				case I300uA:
					rt_sprintf(buf,"%d.%duA",temp->s_hightlimit/10,temp->s_hightlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up   : "));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%duA",temp->s_lowlimit/10,temp->s_lowlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%duA",temp->current/10,temp->current%10);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					strcpy(buf,"---.-uA");
					font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��ʵ����:","Real Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
					
					break;
				case I3mA:
					rt_sprintf(buf,"%d.%03dmA",temp->s_hightlimit/1000,temp->s_hightlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up   :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->s_lowlimit/1000,temp->s_lowlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->current/1000,temp->current%1000);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					strcpy(buf,"-.---mA");
					font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��ʵ����:","Real Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
					break;
				case I30mA:
					rt_sprintf(buf,"%d.%02dmA",temp->s_hightlimit/100,temp->s_hightlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up   :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->s_lowlimit/100,temp->s_lowlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->current/100,temp->current%100);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					strcpy(buf,"--.--mA");
					font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��ʵ����:","Real Cur  :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
					break;
				case I100mA:
					rt_sprintf(buf,"%d.%dmA",temp->s_hightlimit/10,temp->s_hightlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up   :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%dmA",temp->s_lowlimit/10,temp->s_lowlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
					rt_sprintf(buf,"%d.%dmA",temp->current/10,temp->current%10);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					strcpy(buf,"---.-mA");
					font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��ʵ����:","Real Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
					break;
				default:
					
					break;
			}
			
		
			

			rt_sprintf(buf,"%d/%d",temp->current_step,temp->total_step);
			font_draw(_XOFFSET2,_YOFFSET+30,&font,T_STR("�ļ�����:","File Step:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+30,&font,buf);
			
			rt_sprintf(buf,"�ȼ�%d",temp->s_arc);
			font_draw(_XOFFSET2,_YOFFSET+70,&font,T_STR("�绡���:","ARC      :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%d.%dHz",temp->s_outfreq/10,temp->s_outfreq%10);
			font_draw(_XOFFSET2,_YOFFSET+90,&font,T_STR("���Ƶ��:","OutputFre:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+90,&font,"---");
			rt_sprintf(buf,"%d.%ds",temp->s_testtime/10,temp->s_testtime%10);
			font_draw(_XOFFSET2,_YOFFSET+110,&font,T_STR("����ʱ��:","Test Time:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+110,&font,buf);
			
			/**************************************************************************************************************/
			rt_sprintf(buf,"%d.%03dkV",temp->voltage/1000,temp->voltage%1000);
			font_draw(_XOFFSET1,_YOFFSET+140,&font,T_STR("���Ե�ѹ:","Test Vol :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+140,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->testtime/10,temp->testtime%10);
			font_draw(_XOFFSET1,_YOFFSET+160,&font,T_STR("����ʱ��:","Test Time:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+160,&font,buf);
			font_draw(_XOFFSET1,_YOFFSET+180,&font,T_STR("������Ϣ:","ErrorInfo:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+180,&font,error_name[language][temp->error]);		
			font_draw(_XOFFSET2,_YOFFSET+160,&font,T_STR("���Խ��:","Test Res:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+160,&font,(temp->error == PASS? "PASS":"FAIL"));
			font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,"---");
			break;
		case IR:
		{
			uint32_t res;
			const char *gear_name[] = {"AUTO","10M��","100M��","1G��","10G��"};
			res = temp->resister;
			if(res<10000)//0~100M
			{
				rt_sprintf(buf,"%d.%02dM��", res/100,res%100);
			}else if(res<100000)//100M~1G
			{
				res = res / 10;											
				rt_sprintf(buf,"%d.%01dM��", res/10,res%10);
			}else if(res<1000000)//1G~10G
			{
				res = res / 100;											
				rt_sprintf(buf,"%d.%03dG��", res/1000,res%1000);
			}
			else if(res<10000000)//10G~100G
			{
				res = res / 1000;											
				rt_sprintf(buf,"%d.%02dG��", res/100,res%100);
			}
			else 
			{
				res = res / 10000;											
				rt_sprintf(buf,"%d.%01dG��", res/10,res%10);
			}
			font_draw(_XOFFSET1,_YOFFSET+70,&font,T_STR("���Ե���:","Test Res:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+70,&font,buf);
			
			res = temp->s_hightlimit;
			if(res<10000)//0~100M
			{
				rt_sprintf(buf,"%d.%02dM��", res/100,res%100);
			}else if(res<100000)//100M~1G
			{
				res = res / 10;											
				rt_sprintf(buf,"%d.%01dM��", res/10,res%10);
			}else if(res<1000000)//1G~10G
			{
				res = res / 100;											
				rt_sprintf(buf,"%d.%03dG��", res/1000,res%1000);
			}
			else if(res<10000000)//10G~100G
			{
				res = res / 1000;											
				rt_sprintf(buf,"%d.%02dG��", res/100,res%100);
			}
			else 
			{
				res = res / 10000;											
				rt_sprintf(buf,"%d.%01dG��", res/10,res%10);
			}
			font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Res  Up: "));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
			
			res = temp->s_lowlimit;
			if(res<10000)//0~100M
			{
				rt_sprintf(buf,"%d.%02dM��", res/100,res%100);
			}else if(res<100000)//100M~1G
			{
				res = res / 10;											
				rt_sprintf(buf,"%d.%01dM��", res/10,res%10);
			}else if(res<1000000)//1G~10G
			{
				res = res / 100;											
				rt_sprintf(buf,"%d.%03dG��", res/1000,res%1000);
			}
			else if(res<10000000)//10G~100G
			{
				res = res / 1000;											
				rt_sprintf(buf,"%d.%02dG��", res/100,res%100);
			}
			else 
			{
				res = res / 10000;											
				rt_sprintf(buf,"%d.%01dG��", res/10,res%10);
			}
			font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Res Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);

			rt_sprintf(buf,"%d/%d",temp->current_step,temp->total_step);
			font_draw(_XOFFSET2,_YOFFSET+30,&font,T_STR("�ļ�����:","FileStep:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+30,&font,buf);
//			rt_sprintf(buf,"-.--A");
			font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("�������:","OutputCu:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,"-.--A");
// 			rt_sprintf(buf,"�ȼ�%d",temp->s_arc);
			font_draw(_XOFFSET2,_YOFFSET+70,&font,T_STR("���Ե�λ:","TestGear:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+70,&font,gear_name[temp->s_gear]);
//			rt_sprintf(buf,"%d.%dHz",temp->s_outfreq/10,temp->s_outfreq%10);
			font_draw(_XOFFSET2,_YOFFSET+90,&font,T_STR("���Ƶ��:","Out Rate:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+90,&font,"---");
			rt_sprintf(buf,"%d.%ds",temp->s_testtime/10,temp->s_testtime%10);
			font_draw(_XOFFSET2,_YOFFSET+110,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+110,&font,buf);
			
			/**************************************************************************************************************/
			rt_sprintf(buf,"%d.%03dV",temp->voltage/1000,temp->voltage%1000);
			font_draw(_XOFFSET1,_YOFFSET+140,&font,T_STR("���Ե�ѹ:","Test Vol:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+140,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->testtime/10,temp->testtime%10);
			font_draw(_XOFFSET1,_YOFFSET+160,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+160,&font,buf);
			font_draw(_XOFFSET1,_YOFFSET+180,&font,T_STR("������Ϣ:","ErroInfo:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+180,&font,error_name[language][temp->error]);
			
//			rt_sprintf(buf,"%d.%03dA",temp->current/1000,temp->Cur%1000);
			font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,"-.--A");
			font_draw(_XOFFSET2,_YOFFSET+160,&font,T_STR("���Խ��:","Test Res:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+160,&font,(temp->error == PASS? "PASS":"FAIL"));
// 			rt_sprintf(buf,"%d.%03dmA",temp->currents/1000,temp->currents%1000);
			font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur: "));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,"---");
			break;
		}
		case GR:
            if(temp->resister == 0xffff)
            {
                rt_sprintf(buf,">510m��");
            }
            else
            {
                rt_sprintf(buf,"%d.%dm��",temp->resister/10,temp->resister%10);
            }
			font_draw(_XOFFSET1,_YOFFSET+70,&font,T_STR("���Ե���:","Test Res:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%dm��",temp->s_hightlimit);
			font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Res Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
			rt_sprintf(buf,"%dm��",temp->s_lowlimit);
			font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Res Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);

			rt_sprintf(buf,"%d/%d",temp->current_step,temp->total_step);
			font_draw(_XOFFSET2,_YOFFSET+30,&font,T_STR("�ļ�����:","FileStep:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+30,&font,buf);
			rt_sprintf(buf,"%d.%01dA",temp->s_current/10,temp->s_current%10);
			font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("�������:","Out Cur :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
// 			rt_sprintf(buf,"�ȼ�%d",temp->s_arc);
			font_draw(_XOFFSET2,_YOFFSET+70,&font,T_STR("�绡���:","ARC     :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+70,&font,"---");
			rt_sprintf(buf,"%d.%dHz",temp->s_outfreq/10,temp->s_outfreq%10);
			font_draw(_XOFFSET2,_YOFFSET+90,&font,T_STR("���Ƶ��:","Out Rate:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+90,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->s_testtime/10,temp->s_testtime%10);
			font_draw(_XOFFSET2,_YOFFSET+110,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+110,&font,buf);
			
			/**************************************************************************************************************/
			rt_sprintf(buf,"%dmV",temp->voltage);
			font_draw(_XOFFSET1,_YOFFSET+140,&font,T_STR("���Ե�ѹ:","Test Vol:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+140,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->testtime/10,temp->testtime%10);
			font_draw(_XOFFSET1,_YOFFSET+160,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+160,&font,buf);
			font_draw(_XOFFSET1,_YOFFSET+180,&font,T_STR("������Ϣ:","Err Info:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+180,&font,error_name[language][temp->error]);
			
			rt_sprintf(buf,"%d.%03dA",temp->current/1000,temp->current%1000);
			font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
			font_draw(_XOFFSET2,_YOFFSET+160,&font,T_STR("���Խ��:","Test Res:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+160,&font,(temp->error == PASS? "PASS":"FAIL"));
// 			rt_sprintf(buf,"%d.%03dmA",temp->currents/1000,temp->currents%1000);
			font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,"---");
			break;
			
		case PW:
 			rt_sprintf(buf,"%d.%01dV",temp->s_voltage/10,temp->s_voltage%10);
			font_draw(_XOFFSET1,_YOFFSET+70,&font,T_STR("���õ�ѹ:","Set Vol:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%d.%02dA",temp->s_hightlimit);
			font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
			rt_sprintf(buf,"%d.%02dA",temp->s_lowlimit);
			font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);

			rt_sprintf(buf,"%d/%d",temp->current_step,temp->total_step);
			font_draw(_XOFFSET2,_YOFFSET+30,&font,T_STR("�ļ�����:","FileStep:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+30,&font,buf);
			rt_sprintf(buf,"%d.%03dkW",temp->s_powerhigh/1000,temp->s_powerhigh%1000);
			font_draw(_XOFFSET2,_YOFFSET+50,&font,T_STR("��������:","Power Up:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
			rt_sprintf(buf,"%d.%03dkW",temp->s_powerlow/1000,temp->s_powerlow%1000);
			font_draw(_XOFFSET2,_YOFFSET+70,&font,T_STR("��������:","Pow Down:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%d.%dHz",temp->s_outfreq/10,temp->s_outfreq%10);
			font_draw(_XOFFSET2,_YOFFSET+90,&font,T_STR("���Ƶ��:","Out Rate:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+90,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->s_testtime/10,temp->s_testtime%10);
			font_draw(_XOFFSET2,_YOFFSET+110,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+110,&font,buf);
			
			/**************************************************************************************************************/
			rt_sprintf(buf,"%d.%01dV",temp->voltage/10,temp->voltage%10);
			font_draw(_XOFFSET1,_YOFFSET+140,&font,T_STR("���Ե�ѹ:","Test Vol:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+140,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->testtime/10,temp->testtime%10);
			font_draw(_XOFFSET1,_YOFFSET+160,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+160,&font,buf);
			font_draw(_XOFFSET1,_YOFFSET+180,&font,T_STR("������Ϣ:","Err Info:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+180,&font,error_name[language][temp->error]);
			
			rt_sprintf(buf,"%d.%02dA",temp->current/1000,temp->current%1000);
			font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
			font_draw(_XOFFSET2,_YOFFSET+160,&font,T_STR("���Խ��:","Test Res:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+160,&font,(temp->error == PASS? "PASS":"FAIL"));
 			rt_sprintf(buf,"%d.%03dkW",temp->power/1000,temp->power%1000);
			font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("���Թ���:","TestPwer:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,buf);
			break;
		case LC:
			rt_sprintf(buf,"%d.%dV",temp->s_voltage/10,temp->s_voltage%10);
			font_draw(_XOFFSET1,_YOFFSET+70,&font,T_STR("�����ѹ:","Out Vol :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%d.%dV",temp->s_current/10,temp->s_current%10);
			font_draw(_XOFFSET2,_YOFFSET+50,&font,"������ѹ:");font_draw(_XOFFSET2+_XSIZE,_YOFFSET+50,&font,buf);
		
			

			rt_sprintf(buf,"%d/%d",temp->current_step,temp->total_step);
			font_draw(_XOFFSET2,_YOFFSET+30,&font,T_STR("�ļ�����:","FileStep:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+30,&font,buf);
			
// 			rt_sprintf(buf,"�ȼ�%d",temp->s_arc);
			rt_sprintf(buf,"---");
			font_draw(_XOFFSET2,_YOFFSET+70,&font,T_STR("�绡���:","ARC     :"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+70,&font,buf);
			rt_sprintf(buf,"%d.%dHz",temp->s_outfreq/10,temp->s_outfreq%10);
			font_draw(_XOFFSET2,_YOFFSET+90,&font,T_STR("���Ƶ��:","Out Rate:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+90,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->s_testtime/10,temp->s_testtime%10);
			font_draw(_XOFFSET2,_YOFFSET+110,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+110,&font,buf);
			
			/**************************************************************************************************************/
			rt_sprintf(buf,"%d.%dV",temp->voltage/10,temp->voltage%10);
			font_draw(_XOFFSET1,_YOFFSET+140,&font,T_STR("���Ե�ѹ:","Test Vol:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+140,&font,buf);
			rt_sprintf(buf,"%d.%ds",temp->testtime/10,temp->testtime%10);
			font_draw(_XOFFSET1,_YOFFSET+160,&font,T_STR("����ʱ��:","TestTime:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+160,&font,buf);
			font_draw(_XOFFSET1,_YOFFSET+180,&font,T_STR("������Ϣ:","Err Info:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+180,&font,error_name[language][temp->error]);
// 			temp->current = temp->current * 1.414;
			switch(temp->s_gear){
				case I3uA:
					rt_sprintf(buf,"%d.%03duA",temp->current / 1000,temp->current % 1000);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->s_hightlimit/1000,temp->s_hightlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur  Up :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%03duA",temp->s_lowlimit/1000,temp->s_lowlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
				case I30uA:
					rt_sprintf(buf,"%d.%02duA",temp->current / 100,temp->current % 100);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->s_hightlimit/100,temp->s_hightlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Curr Up :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%02duA",temp->s_lowlimit/100,temp->s_lowlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
				case I300uA:
					rt_sprintf(buf,"%d.%01duA",temp->current / 10,temp->current % 10);	
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%01duA",temp->s_hightlimit/10,temp->s_hightlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%01duA",temp->s_lowlimit/10,temp->s_lowlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
				break;
				case I3mA:
					rt_sprintf(buf,"%d.%03dmA",temp->current / 1000,temp->current % 1000);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->s_hightlimit/1000,temp->s_hightlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur  Up :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%03dmA",temp->s_lowlimit/1000,temp->s_lowlimit%1000);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
				break;
				case I30mA:
					rt_sprintf(buf,"%d.%02dmA",temp->current / 100,temp->current % 100);
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->s_hightlimit/100,temp->s_hightlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%02dmA",temp->s_lowlimit/100,temp->s_lowlimit%100);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
				break;
				case I100mA:
					rt_sprintf(buf,"%d.%01dmA",temp->current / 10,temp->current % 10);	
					font_draw(_XOFFSET2,_YOFFSET+140,&font,T_STR("���Ե���:","Test Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+140,&font,buf);
					rt_sprintf(buf,"%d.%01dmA",temp->s_hightlimit/10,temp->s_hightlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+90,&font,T_STR("��������:","Cur Up  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+90,&font,buf);
					rt_sprintf(buf,"%d.%01dmA",temp->s_lowlimit/10,temp->s_lowlimit%10);
					font_draw(_XOFFSET1,_YOFFSET+110,&font,T_STR("��������:","Cur Down:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+110,&font,buf);
				break;
				default:	
				break;
			}
			
			font_draw(_XOFFSET2,_YOFFSET+160,&font,T_STR("���Խ��:","Test Res:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+160,&font,(temp->error == PASS? "PASS":"FAIL"));
// 			rt_sprintf(buf,"%d.%03dmA",temp->currents/1000,temp->currents%1000);
			font_draw(_XOFFSET2,_YOFFSET+180,&font,T_STR("��ʵ����:","Real Cur:"));font_draw(_XOFFSET2+_XSIZE,_YOFFSET+180,&font,"---");
			break;
	}
	
	/**************************************************************************************************************/
	rt_sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",temp->time.year+2000,temp->time.month,temp->time.date,temp->time.hours,temp->time.minutes,temp->time.seconds);
	font_draw(_XOFFSET1,_YOFFSET+210,&font,T_STR("��¼ʱ��:","RecrdTime:"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+210,&font,buf);
	font_draw(_XOFFSET1,_YOFFSET+230,&font,T_STR("������  :","Barcode  :"));font_draw(_XOFFSET1+_XSIZE,_YOFFSET+230,&font,temp->_bar_code);
		
#undef		_XOFFSET1	
#undef		_XOFFSET2	
#undef		_XSIZE		
#undef		_YOFFSET		
	sui_window_update(win);
	while(1)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				/* ���� */
				case KEY_F6   | KEY_UP:
				case KEY_EXIT | KEY_UP:
					return 0;
			}
		}
	}
}
static u8 result_delete(void *arg)
{
	rt_uint32_t msg;
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0xf000,0XE73C,1,0,16};
	struct rect_type rects={25,100,20,250};
	rt_uint32_t x=0;
		
	win = sui_window_create(T_STR("���ɾ��","Delete Results:"),&rect);
	font.panel = win;
	font_draw(94,44,&font,T_STR("ȷ����ʽ����","Ensure Esvaziar ?"));
	font_draw(26,68,&font,T_STR("��ʾ:��ʽ����ɾ�����н����Ϣ��","Tips: Esvaziar will Delete all"));
	clr_win(win,0XC618,25,100,20,250);

	clr_win(win,CL_orange,40,145,30,80);
	clr_win(win,CL_orange,180,145,30,80);
	
	font.fontcolor = 0X4208;
	font_draw(64,152,&font,T_STR("ȷ��","OK"));
	font_draw(204,152,&font,T_STR("����","Return")); 
	
	sui_window_update(win);
	while(1)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				case KEY_ENTER | KEY_UP:
			
					while(x<250)
					{
						
						clr_win(win,CL_BLUE,25+x,100,20,1);
						window_updata(win,&rects);
						x++;
						
//						rt_thread_delay(1);
					}
					while(x<250)
					{
						clr_win(win,CL_BLUE,25+x,100,20,1);
						window_updata(win,&rects);
						x++;
					}
				
					memory_result_delete();
					rt_thread_delay(100);
					return 0;
//				case KEY_ENTER | KEY_UP:
//					FLASH_CS_SET(4);	// ѡ�����flash
//					sf_EraseChip();
//					while(sf_wait_find() != 0)
//					{
//						if(x<250)
//						{
//							clr_win(win,CL_BLUE,25+x,100,20,1);
//							window_updata(win,&rects);
//							x++;
//						}
//						rt_thread_delay(RT_TICK_PER_SECOND/5);
//					}
//					while(x<250)
//					{
//						clr_win(win,CL_BLUE,25+x,100,20,1);
//						window_updata(win,&rects);
//						x++;
//					}
//					sf_wait_find_dis();
//					memory_result_delete();
//					rt_thread_delay(RT_TICK_PER_SECOND/2);
//					return 0;
				/* ���� */
				case KEY_EXIT | KEY_UP:
				case KEY_F6 | KEY_UP:
					return 0;
			}
		}
	}
}
static u8 result_Stat(void *arg)
{
	rt_uint32_t msg;
	char buf[10];
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0X4208,0XE73C,1,0,16};
    rt_uint32_t paas_rate;///<�ϸ���
    rt_uint32_t used_rate;///<ʹ����
    rt_uint32_t fail_rate;///<ʧ����

	win = sui_window_create(T_STR("���ͳ��","Results statistics"),&rect);
	font.panel = win;

#define		_XOFFSET	10
#define		_YOFFSET	30
	font_draw(_XOFFSET,_YOFFSET+10,&font,T_STR("���Խ��ͳ��:","Test statistics: "));
	font_draw(_XOFFSET,_YOFFSET+30,&font,T_STR("�����Դ���:","��Test Time:"));
	font_draw(_XOFFSET,_YOFFSET+50,&font,T_STR("���ɹ�����:","��Succ Time:"));
font_draw(150+_XOFFSET,_YOFFSET+50,&font,T_STR("���ɹ���:","��Rate  :"));
	font_draw(_XOFFSET,_YOFFSET+70,&font,T_STR("��ʧ�ܴ���:","��Fail Time:"));
font_draw(150+_XOFFSET,_YOFFSET+70,&font,T_STR("��ʧ����:","��Rate  :"));

	font_draw(_XOFFSET,_YOFFSET+100,&font,T_STR("�洢��ͳ��:","Storage statistics: "));
		font_draw(_XOFFSET,_YOFFSET+120,&font,T_STR("���� �� ��:","��Total   :"));
	font_draw(150+_XOFFSET,_YOFFSET+120,&font,T_STR("����ʹ��:","��Using   :"));
		font_draw(_XOFFSET,_YOFFSET+140,&font,T_STR("��ʹ �� ��:","��Use rate:"));
	font_draw(150+_XOFFSET,_YOFFSET+140,&font,T_STR("����  ��:","��Margin  :"));


#define		_NULLOFFSET1	100
#define		_NULLOFFSET2	80+150
    paas_rate = result_headinfo.pass_count * 1000.0 / result_headinfo.test_count;
    used_rate = result_headinfo.test_count * 1000.0 / result_headinfo.total_size;
    fail_rate = 1000 - paas_rate;
	rt_sprintf(buf,"%d",result_headinfo.test_count);	font_draw(_XOFFSET+_NULLOFFSET1,_YOFFSET+30,&font,buf);
	rt_sprintf(buf,"%d",result_headinfo.pass_count);	font_draw(_XOFFSET+_NULLOFFSET1,_YOFFSET+50,&font,buf);
	rt_sprintf(buf,"%d",result_headinfo.test_count-result_headinfo.pass_count);	font_draw(_XOFFSET+_NULLOFFSET1,_YOFFSET+70,&font,buf);
	if(result_headinfo.test_count != 0)
	{
        rt_sprintf(buf,"%d.%d%",paas_rate / 10, paas_rate % 10);//�ϸ���
		
// 		rt_sprintf(buf,"%d.%d%",(result_headinfo.pass_count*100)/result_headinfo.test_count,((result_headinfo.pass_count*1000)/result_headinfo.test_count)%10);
		font_draw(_XOFFSET+_NULLOFFSET2,_YOFFSET+50,&font,buf);
        
        rt_sprintf(buf,"%d.%d%",fail_rate / 10, fail_rate % 10);//ʧ����
// 		rt_sprintf(buf,"%d.%d%",((result_headinfo.test_count-result_headinfo.pass_count)*100)/result_headinfo.test_count,(((result_headinfo.test_count-result_headinfo.pass_count)*1000)/result_headinfo.test_count)%10);
		font_draw(_XOFFSET+_NULLOFFSET2,_YOFFSET+70,&font,buf);
	}
	else
	{
		font_draw(_XOFFSET+_NULLOFFSET2,_YOFFSET+50,&font,"0.0%");
		font_draw(_XOFFSET+_NULLOFFSET2,_YOFFSET+70,&font,"0.0%");
	}
	
	rt_sprintf(buf,"%d",result_headinfo.total_size);	font_draw(_XOFFSET+_NULLOFFSET1,_YOFFSET+120,&font,buf);
	rt_sprintf(buf,"%d",result_headinfo.current_numb);	font_draw(_XOFFSET+_NULLOFFSET2,_YOFFSET+120,&font,buf);
	rt_sprintf(buf,"%d",result_headinfo.total_size-result_headinfo.current_numb);	font_draw(_XOFFSET+_NULLOFFSET2,_YOFFSET+140,&font,buf);
    
    rt_sprintf(buf,"%d.%d%",used_rate / 10, used_rate % 10);//�ϸ���
// 	rt_sprintf(buf,"%d.%d%",(result_headinfo.current_numb*100)/result_headinfo.total_size,((result_headinfo.current_numb*1000)/result_headinfo.total_size)%10);
	font_draw(_XOFFSET+_NULLOFFSET1,_YOFFSET+140,&font,buf);
#undef		_XOFFSET
#undef		_YOFFSET
#undef		_NULLOFFSET1
#undef		_NULLOFFSET2
	
	sui_window_update(win);
	while(1)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				/* ���� */
				case KEY_EXIT | KEY_UP:
				case KEY_F6   | KEY_UP:
					return 0;
			}
		}
	}
}
#include  "CH376_USB_UI.H"
#include "rtc.h"

uint8_t set_file_create_time(uint8_t* file_name, uint16_t date, uint16_t time)
{
	FAT_DIR_INFO fdi;
	uint8_t res = 0;
	
	res = CH376FileOpenPath((uint8_t*)file_name);/* ���ļ� */
	
	if(0x14 != res)
	{
		return 0xff;
	}
	
	//1.��CMD1H_DIR_INFO_READ��ȡ��ǰ�ļ��ṹ���ڴ�
	CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ , 0XFF );
	//2.ͨ��CMD01_RD_USB_DATA0������ڴ滺������������
	CH376ReadBlock((uint8_t*)&fdi);
	//3.ͨ��CMD_DIR_INFO_READ�����ȡFAT_DIR_INFO�ṹ��������
	//4.ͨ��CMD20_WR_OFS_DATA�������ڲ�������ָ��ƫ�Ƶ�ַд���޸ĺ������
	
	fdi.DIR_CrtTime = time;/* 0EH,�ļ�������ʱ�� */
	fdi.DIR_CrtDate = date;/* 10H,�ļ����������� */
	
	fdi.DIR_WrtTime = time;/* 16H,�ļ��޸�ʱ��,�ο�ǰ��ĺ� MAKE_FILE_TIME */
	fdi.DIR_WrtDate = date;/* 18H,�ļ��޸�����,�ο�ǰ��ĺ� MAKE_FILE_DATE */
	
	CH376WriteOfsBlock((uint8_t*)&fdi, 0, sizeof fdi );
	//5.ͨ��CMD_DIR_INFO_SAVE������USB�洢�豸
	CH376DirInfoSave( );
	
	CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ , 0XFF );
	//2.ͨ��CMD01_RD_USB_DATA0������ڴ滺������������
	CH376ReadBlock((uint8_t*)&fdi);
	
	CH376FileClose(TRUE);
	
	return 0x14;
}
uint8_t SetFileCreateTime(uint8_t* file_name)
{
	uint16_t year = get_rtc_year();
	uint16_t month = get_rtc_month();
	uint16_t day = get_rtc_day();
	uint16_t hour = get_rtc_hour();
	uint16_t minute = get_rtc_minute();
	uint16_t second = get_rtc_second();
	
	return set_file_create_time(file_name, MAKE_FILE_DATE(year, month, day), MAKE_FILE_TIME(hour, minute, second));
}
#define   Test_File_Name 		T_STR("/�����ļ�.XLS", "/TEST_LOG.XLS")
#define   Test_File_TITLE		T_STR("���\t����\tģʽ\t������Ϣ\t���Խ��\t��ѹ\t����\t����\t����ʱ��\t��¼ʱ��\r", \
									"NO.\tBarCode\tTestMode\tErr.Inf.\tResult\tVoltage\tCurrent\tResistance\tTestTime\tRecordingTime\r")

static u8 result_export(void *arg)
{
	rt_uint32_t msg;
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0xf000,0XE73C,1,0,16};
	struct rect_type rects={25,100,20,250};
	rt_uint32_t i=0;
		
	win = sui_window_create(T_STR("�������","Results Export"),&rect);
	font.panel = win;


	font_draw(94,44,&font,T_STR("ȷ��������","Determine Export ? "));
	font_draw(26,68,&font,T_STR("��ʾ:�����������USB�洢�豸�С�","Tips:export Results to USB"));
// 	clr_win(win,0XC618,25,100,20,250);

	clr_win(win,CL_orange,40,145,30,80);
	clr_win(win,CL_orange,180,145,30,80);
	
	font.fontcolor = 0X4208;
	font_draw(64,152,&font,T_STR("ȷ��","OK"));
	font_draw(204,152,&font,T_STR("����","Return"));
	
	sui_window_update(win);
	USB_Device_Chg(USB_1);
	
	while(1)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				case KEY_ENTER | KEY_UP:
					if(result_headinfo.test_count==0)return 1;
					msg = CH376DiskConnect();
					font.fontcolor = 0Xf000;
					
					
					if(msg != USB_INT_SUCCESS)
					{
						clr_win(win,0XE73C,2,32,165,295);
						font_draw(70,68,&font,T_STR("�����USB�洢�豸...","Please insert the USB device"));
						sui_window_update(win);
						while (CH376DiskConnect() != USB_INT_SUCCESS)
						{ 
							if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/10) == RT_EOK)
							{
								switch(msg)
								{
									/* ���� */
									case KEY_EXIT | KEY_UP:
									case KEY_F6 | KEY_UP:
										return 0;
								}
							}/* ���U���Ƿ�����,�ȴ�U�̲���,����SD��,�����ɵ�Ƭ��ֱ�Ӳ�ѯSD�����Ĳ��״̬���� */
						}
						clr_win(win,0XE73C,2,32,165,295);
						font_draw(70,68,&font,T_STR("USB�洢�豸�Ѳ��룡","USB device already insert"));
						sui_window_update(win);
						rt_thread_delay(200);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
					}
					
					clr_win(win,0XE73C,2,32,165,295);
					font_draw(70,68,&font,T_STR("�ȴ�����......","Wait for ready..."));
					sui_window_update(win);
					rt_thread_delay(100);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
					
					/* ���ڼ�⵽USB�豸��,���ȴ�100*50mS,��Ҫ�����ЩMP3̫��,���ڼ�⵽USB�豸��������DISK_MOUNTED��,���ȴ�5*50mS,��Ҫ���DiskReady������ */
					for (i = 0; i < 100; i ++ )
					{  /* ��ȴ�ʱ��,100*50mS */
						msg = CH376DiskMount();  /* ��ʼ�����̲����Դ����Ƿ���� */
						if (msg == USB_INT_SUCCESS)
						{
							break;  /* ׼���� */
						}
						else if (msg == ERR_DISK_DISCON)
						{
							break;  /* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */
						}
						if (CH376GetDiskStatus() >= DEF_DISK_MOUNTED && i >= 5)
						{
							break;  /* �е�U�����Ƿ���δ׼����,�������Ժ���,ֻҪ�佨������MOUNTED�ҳ���5*50mS */
						}
					}
					if (msg == ERR_DISK_DISCON)
					{
						clr_win(win,0XE73C,2,32,165,295);
						font_draw(70,68,&font,T_STR("��ʶ���USB�豸��","Cannot be identified USB"));
						sui_window_update(win);
						rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
						return 1;
					}
					
					{
						u32		__num= result_headinfo.test_count;
						float   sn=0,n=0;
						u32		__file_len= 0;
						char	__buf[150];
						u16		timeout = 500;
						static struct result_info_t *temp;
						
						/* �����ļ� */
						while(CH376FileCreate(Test_File_Name) != USB_INT_SUCCESS && timeout--)
						{
							rt_thread_delay(10);
						}
						
						if(timeout == 0)
						{
							clr_win(win,0XE73C,2,32,165,295);
							font_draw(70,68,&font,T_STR("��ʶ���USB�豸��","Cannot be identified USB"));
							sui_window_update(win);
							rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
							return 1;
						}
						else
						{
							msg = CH376FileClose(TRUE);
							SetFileCreateTime(Test_File_Name);
							CH376FileOpenPath(Test_File_Name);
							__file_len = strlen(Test_File_TITLE);
							CH376ByteWrite((PUINT8)Test_File_TITLE, __file_len,NULL);
							msg = CH376FileClose(TRUE);
							
							if(msg != USB_INT_SUCCESS)
							{
								clr_win(win,0XE73C,2,32,165,295);
								font_draw(70,68,&font,T_STR("�ļ�����","File error"));
								sui_window_update(win);
								rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
								return 1;
							}
						}
						
						clr_win(win,0XE73C,2,32,165,295);
						clr_win(win,0XC618,25,100,20,250);
						font_draw(70,68,&font,T_STR("��ʼ����...","Start export..."));
						
						sui_window_update(win);
						rt_thread_delay(50);
						__num = __num + 1;
						sn = 250.0/__num;
						n=0;
						
						for(i=0;i<__num;i++)
						{
							n+=sn;
							temp = memory_result_read(i);
							
							if(temp != NULL)
							{
								rt_sprintf(__buf,"%04d\t",i+1);
								strcat(__buf,"'");//������ǰ����ϡ���ֹ��excel�޸�����
								strcat(__buf,temp->_bar_code);
                                strcat(__buf,"\t");
								strcat(__buf,mode_name[temp->mode]);strcat(__buf,"\t");
								strcat(__buf,error_name[language][temp->error]);strcat(__buf,"\t");
								strcat(__buf,(temp->error == PASS? "PASS":"FAIL"));strcat(__buf,"\t");
								switch(temp->mode)
								{
									case ACW:
									rt_sprintf(__buf+strlen(__buf),"%d.%03dkV\t",temp->s_voltage/1000,temp->s_voltage%1000);

									switch(temp->s_gear){
										
										case I3uA:
											rt_sprintf(__buf+strlen(__buf),"%d.%03duA\t",temp->current/1000,temp->current%1000);
											break;
										case I30uA:
											rt_sprintf(__buf+strlen(__buf),"%d.%02duA\t",temp->current/100,temp->current%100);
											break;
									
										case I300uA:
											
											rt_sprintf(__buf+strlen(__buf),"%d.%duA\t",temp->current/10,temp->current%10);
											break;
										
										case I3mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%03dmA\t",temp->current/1000,temp->current%1000);
											break;
										case I30mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%02dmA\t",temp->current/100,temp->current%100);
											break;
										case I100mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%dmA\t",temp->current/10,temp->current%10);
											break;
										default:
										
										break;
									}
								
									strcat(__buf,"---\t");
									rt_sprintf(__buf+strlen(__buf),"%d.%ds\t",temp->testtime/10,temp->testtime%10);
								break;
								case DCW:
									rt_sprintf(__buf+strlen(__buf),"%d.%03dkV\t",temp->s_voltage/1000,temp->s_voltage%1000);
									switch(temp->s_gear){
										case I3uA:
											rt_sprintf(__buf+strlen(__buf),"%d.%03duA\t",temp->current/1000,temp->current%1000);
											break;
										case I30uA:
											rt_sprintf(__buf+strlen(__buf),"%d.%02duA\t",temp->current/100,temp->current%100);
											break;
									
										case I300uA:
											
											rt_sprintf(__buf+strlen(__buf),"%d.%duA\t",temp->current/10,temp->current%10);
											break;
										case I3mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%03dmA\t",temp->current/1000,temp->current%1000);
										
										break;
										case I30mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%02dmA\t",temp->current/100,temp->current%100);
										
										break;
										case I100mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%dmA\t",temp->current/10,temp->current%10);
										
										break;
										default:
										
										break;
									}
									strcat(__buf,"---\t");
									rt_sprintf(__buf+strlen(__buf),"%d.%ds\t",temp->testtime/10,temp->testtime%10);
									break;
								case IR:
									break;
								case GR:
									rt_sprintf(__buf+strlen(__buf),"%dmV\t",temp->voltage);
									rt_sprintf(__buf+strlen(__buf),"%d.%01dA\t",temp->s_current/10,temp->s_current%10);
                                    if(temp->resister == 0xffff)
                                    {
                                        rt_sprintf(__buf+strlen(__buf),">510m��\t",temp->resister/10,temp->resister%10);
                                    }
                                    else
                                    {
                                        rt_sprintf(__buf+strlen(__buf),"%d.%dm��\t",temp->resister/10,temp->resister%10);
                                    }
//                                        rt_sprintf(__buf+strlen(__buf),"%d.%dm��\t",temp->resister/10,temp->resister%10);
									rt_sprintf(__buf+strlen(__buf),"%d.%ds\t",temp->testtime/10,temp->testtime%10);
									break;
								case LC:
									rt_sprintf(__buf+strlen(__buf),"%d.%dV\t",temp->s_voltage/10,temp->s_voltage%10);
									
									switch(temp->s_gear){
										case I3uA:
										case I30uA:
										case I300uA:
											rt_sprintf(__buf+strlen(__buf),"%d.%01duA\t",temp->current  / 10,temp->current  % 10);
										break;
										case I3mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%03dmA\t",temp->current / 1000,temp->current % 1000);
										break;
										case I30mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%02dmA\t",temp->current / 100,temp->current % 100);
										break;
										case I100mA:
											rt_sprintf(__buf+strlen(__buf),"%d.%01dmA\t",temp->current / 10,temp->current % 10);	
										break;
										default:	
										break;
									}
									strcat(__buf,"---\t");
									rt_sprintf(__buf+strlen(__buf),"%d.%ds\t",temp->testtime/10,temp->testtime%10);
									break;
								}
								rt_sprintf(__buf+strlen(__buf),"'%04d-%02d-%02d %02d:%02d:%02d\r",temp->time.year+2000,temp->time.month,temp->time.date,temp->time.hours,temp->time.minutes,temp->time.seconds);
								
								msg = CH376FileOpen(Test_File_Name);
								msg = CH376ByteLocate(__file_len);
								msg = CH376ByteWrite((u8 *)__buf,strlen(__buf),NULL);
								msg = CH376FileClose(TRUE);
								__file_len += strlen(__buf);
							}else{
								rt_sprintf(__buf,"\t");
								msg = CH376FileOpen(Test_File_Name);
								msg = CH376ByteLocate(__file_len);
								msg = CH376ByteWrite((u8 *)__buf,strlen(__buf),NULL);
								msg = CH376FileClose(TRUE);
							}
							clr_win(win,CL_BLUE,25,100,20,n);
							window_updata(win,&rects);
						}
					}
					
					clr_win(win,0XE73C,70,68,20,100);
					font_draw(70,68,&font,T_STR("������ɣ�","Export finish !"));
					sui_window_update(win);
					rt_thread_delay(200);
					
					return 0;
				/* ���� */
					case KEY_EXIT | KEY_UP:
				case KEY_F6 | KEY_UP:
					return 0;
			}
		}
	}
	
}

static u8 result_jump(void *arg)
{
	rt_uint32_t msg;

	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0X4208,0XE73C,1,0,16};
	struct num_format num;

	win = sui_window_create(T_STR("��ת","Skip"),&rect);
	font.panel = win;

							font_draw(20,50,&font,T_STR("��ת���:","Skip num: "));
	font_draw(20,80,&font,T_STR("���뷶Χ: 0001 - 8000","In Range: 0001 - 8000 "));
	font.fontcolor = 0Xf000;
	font_draw(20,110,&font,T_STR("������ʾ:������Ҫ��ת�ı�š�","OperaTips: Input Skip num."));

	clr_win(win,CL_orange,40,145,30,80);
	clr_win(win,CL_orange,180,145,30,80);
	
	font.fontcolor = 0X4208;
	font_draw(64,152,&font,T_STR("ȷ��","OK"));
	font_draw(204,152,&font,T_STR("����","Return"));

	sui_window_update(win);
	
	font.fontcolor = 0Xf000;
	font.center = 1;
	rect.x = 100;
	rect.y = 48;
	rect.h = 20;
	rect.w = 32;
	num.num  = result_list_box.current_item+1;
	num._int = 4;
	num._dec = 0;
	num.min  = 1;
	num.max  = 8000;
	num.unit = "";
	msg = num_input(&font,&rect,&num);
	if(msg == 0xffffffff)return 1;
	result_list_box.current_item = msg-1;
	result_list_box.start_item = (result_list_box.current_item / result_list_box.items_count)*result_list_box.items_count;
	return 0;
}


