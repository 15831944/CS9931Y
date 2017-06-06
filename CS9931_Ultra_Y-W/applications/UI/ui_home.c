#include "CS99xx.h"
#include "memory.h"
#include "memorymanagement.h"
#include "bsp_ico.h"
#include "Test_Sched.h"
#include "sui_window.h"
#include "zwd414b.h"
#include "ext.h"

#define	_system_environment		(system_parameter_t.env)

extern u8 scan_buf[64],scan_buf_w,scan_buf_r;
extern void ui_file_thread(void);
extern void ui_step_thread(void);
extern void ui_system_thread(void);
extern void ui_result_thread(void);
extern void ui_help_thread(void);
extern void ui_about_thread(void);
extern void ui_display_thread(void);
extern void ui_file_port_thread(void);
extern void ui_autocal_thread(void);
	
static void interface_acw(struct step_acw_t *acw);
static void interface_dcw(struct step_dcw_t *dcw);
static void interface_gr(struct step_gr_t *gr);
static void interface_lc(struct step_lc_t *lc);
static void interface_ir(struct step_ir_t *ir);
static void interface_pw(struct step_pw_t *pw);
static void interface_acw_gr(struct step_acw_gr_t *acw_gr);
static void interface_dcw_gr(struct step_dcw_gr_t *dcw_gr);

static void bar_code_scan(void *parameter);
static void ZWD414B_Sever(void *parameter);

u8 bar_code[40];
u8 bar_code_flag = 0;

extern u8		buzzer_timer;
extern void RA8875_SetBackLight(uint8_t _bright);
/*****************************/
UN_STR *dyj_next_step(void);

/**********************************/

static rt_uint8_t offset_Is_refresh = 0;  //offset�����Ƿ����¼�¼��־

static void offset_key_inHome(void)
{
	rt_uint32_t msg;
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0xf000,0XE73C,1,0,16};

    win = sui_window_create(T_STR("����offset","Set Offset"),&rect);
	font.panel = win;


	
// 	clr_win(win,0XC618,25,100,20,250);

	clr_win(win,CL_orange,40,145,30,80);
	clr_win(win,CL_orange,180,145,30,80);
	
	font.fontcolor = 0X4208;

		font_draw(64,152,&font,T_STR("ȷ��","OK  "));
		font_draw(204,152,&font,T_STR("����","Back"));
	font.fontcolor = 0xf000;
	
	if(file_info[flash_info.current_file].offset_en != 0)
	{
		rt_uint8_t IsLoopFlag = 1;
		
		font_draw(86,44,&font,T_STR("ȷ���ر�offset��","Sure turn off offset?"));
		font_draw(26,68,&font,T_STR("��ʾ:�ر�offset�󣬾ɵ�offset����","Tips:Close file after,The old offset"));
		font_draw(26,92,&font,T_STR("��������"," data will be cleared"));
		sui_window_update(win);
		while(IsLoopFlag)
		{
			if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
			{
				switch(msg)
				{
					case KEY_ENTER | KEY_UP:
						IsLoopFlag = 0;
					break;
					case KEY_EXIT | KEY_UP:
					case KEY_F6 | KEY_UP:
					return;
				}
			}
		}
		file_info[flash_info.current_file].offset_en = 0;
	}
	else
	{
		rt_uint8_t IsLoopFlag = 1;
		
		font_draw(86,44 ,&font,T_STR("ȷ����offset��","Sure turn on offset"));
		font_draw(26,68 ,&font,T_STR("��ʾ:��offset�����һ�εĲ���","Tips:open the file after,a recent test"));
		font_draw(26,92 ,&font,T_STR("�����ڼ�¼offset����","will recorded offset data"));
		font_draw(26,116,&font,"");
		sui_window_update(win);
		while(IsLoopFlag)
		{
			if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
			{
				switch(msg)
				{
					case KEY_ENTER | KEY_UP:
						IsLoopFlag = 0;
					break;
					case KEY_EXIT | KEY_UP:
					case KEY_F6 | KEY_UP:
					return;
				}
			}
		}
		file_info[flash_info.current_file].offset_en = 1;
		offset_Is_refresh = 1;
	}
	rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_OFFSET_EVENT | (file_info[flash_info.current_file].offset_en));
	save_file_to_flash(flash_info.current_file);
	return;
}




static void home_entry(void *parameter)
{
	rt_uint32_t msg;
	UN_STR *un;
	uint8_t start_key_release_flag = 0;
	while(1){
		RA8875_SetBackLight(system_parameter_t.env.lcdlight*20+55);
		
		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		/* ��ʼ����ʾ������ */
		current_step_num = 1;
		g_cur_step = cs99xx_list.head;//ָ�����ļ�ͷ
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		switch(un->com.mode)
		{
			case ACW:
				interface_acw(&un->acw);
				break;
			case DCW:
				interface_dcw(&un->dcw);
				break;
			case IR:
				interface_ir(&un->ir);
				break;
			case GR:
				interface_gr(&un->gr);
				break;
			case LC:
				interface_lc(&un->lc);
				break;
			case PW:
				interface_pw(&un->pw);
				break;
			case ACW_GR:
				interface_acw_gr(&un->acw_gr);
				break;
			case DCW_GR:
				interface_dcw_gr(&un->dcw_gr);
				break;
		}
		rt_mb_send(&screen_mb, UPDATE_HOME);
		rt_mb_send(&screen_mb, UPDATE_TITLE);
		rt_mb_send(&screen_mb, UPDATE_MENU);
		rt_mb_send(&screen_mb, UPDATE_KEY);
		rt_mb_send(&screen_mb, UPDATE_STATUS);
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_TIME_EVENT);
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_KEYLOCK_EVENT | (system_parameter_t.key_lock));
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_OFFSET_EVENT | (file_info[flash_info.current_file].offset_en));
		
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_INTERFACE_EVENT | (system_parameter_t.com.interface));
		ui_key_updata(0);
		while(panel_flag == 0)
		{
			if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
			{
				switch(msg)
				{
					case KEY_F1 | KEY_UP:
						panel_flag = 1;
	// 					com_tid = rt_thread_find("file");
	// 					rt_thread_resume(com_tid);
	// 					com_tid = rt_thread_self();
	// 					rt_thread_suspend(com_tid);
	// 					rt_schedule();
	// 					
						break;
					case KEY_F2 | KEY_UP:
						panel_flag = 2;
						break;
					case KEY_F3 | KEY_UP:
						panel_flag = 3;
						break;
					case KEY_F4 | KEY_UP:
						panel_flag = 4;
						break;
					case KEY_F5 | KEY_UP:
						panel_flag = 5;
						break;
					case KEY_F6 | KEY_UP:
						panel_flag = 6;
					{
	// 					static char num=0;
	// 					num++;
	// 					
	// 					rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_OFFSET_EVENT | (num%2));
	// 					rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_CONTENT_EVENT | (num));
	// 					rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_STATUS_EVENT | (num%2));
						
					}
						break;
					case KEY_DISPLAY | KEY_UP:
	// 					panel_flag = 7;
						break;
					case KEY_OFFSET  | KEY_UP:    //
							offset_key_inHome();
							rt_mb_send(&screen_mb, UPDATE_HOME);
							
						break;
					
					default:
						break;
				}
			}
			else
			{
				if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8) == 1){   //stop��û�б�����
					
					if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7) == 1)
					{
						rt_thread_delay( RT_TICK_PER_SECOND / 100);
						if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7) == 1)
						{
							start_key_release_flag = 1;
						}
					}
					if(start_key_release_flag)
					{
						if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7) == 0)
						{
							rt_thread_delay( RT_TICK_PER_SECOND / 100);
							if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7) == 0)
							{
								start_key_release_flag = 0;
								if(system_parameter_t.Com_lock == 0)
								if(buzzer_timer == 0)
									if(offset_Is_refresh){
										offset_Is_refresh = 0;
										Test_Sched_Param.Offset_Get_Flag = 1;  //��һ�β��Կ�ʼ��¼offset��ֵ
										Test_Sched_Param.Offset_Is_Flag  = 0;  //����ʱoffset���������
									}else{
										if(file_info[flash_info.current_file].offset_en == 1){
											Test_Sched_Param.Offset_Is_Flag  = 1;  //����ʱoffset�������
										}	
										else
										{
											Test_Sched_Param.Offset_Is_Flag  = 0;  //����ʱoffset���������
										}
									}
									Test_Sched_Start(dyj_next_step);
							}
						}
					}
				}
			}
			{
// 				extern unsigned char  ucSCoilBuf[];
// 				if(ucSCoilBuf[0] & 0x01){
// 					ucSCoilBuf[0] &= ~0x01;
// 					Test_Sched_Start(dyj_next_step);
// 				}
// 				
// 				if(ucSCoilBuf[0] & 0x02){
// 					ucSCoilBuf[0] &= ~0x02;
// 					Test_Sched_Close();
// 				}
			}
		}
		ui_file_thread();
		ui_step_thread();
		ui_system_thread();
		ui_result_thread();
		ui_help_thread();
		ui_about_thread();
		ui_display_thread();
		ui_file_port_thread();
		ui_autocal_thread();
	}
}

void ui_home_init(void)
{
    static rt_bool_t inited = RT_FALSE;

    if (inited == RT_FALSE) /* �����ظ���ʼ�������ı��� */
    {
        rt_thread_t tid;

		
        tid = rt_thread_create("home",
                               home_entry, RT_NULL,
                               2048, 14, 10);

        if (tid != RT_NULL)
            rt_thread_startup(tid);
				
				tid = rt_thread_create("bar_code_scan",
                               bar_code_scan, RT_NULL,
                               1024, 15, 10);

        if (tid != RT_NULL)
            rt_thread_startup(tid);
				
				tid = rt_thread_create("ZWD414B_Sever",
                               ZWD414B_Sever, RT_NULL,
                               1024, 15, 10);

        if (tid != RT_NULL)
            rt_thread_startup(tid);
				
        inited = RT_TRUE;
    }
}


void clear_scan_code_buf(void)
{
    memset(scan_buf,0, sizeof(scan_buf));
    scan_buf_w = 0;
    scan_buf_r = 0;
}
static void bar_code_scan(void *parameter)
{
    u8 n,i,s;
  u8 scan_text_flag=0,scan_text_buf[40],scan_text_w=0;
	struct panel_type *win;
	struct rect_type rect_win={170,140,250,360};
	struct rect_type rect;
	struct font_info_t font={0,0X4208,0XE73C,1,1,16};
	
	parameter = parameter;
    test_flag.bar_code_sw = START_SCAN_BAR_CODE;
    
	while(1){
        if(test_flag.bar_code_sw == STOP_SCAN_BAR_CODE)
        {
            clear_scan_code_buf();
            continue;
        }
#if 1
			if(scan_buf_w > scan_buf_r){
				n = scan_buf_w - scan_buf_r;
				for(i=0;i<n;i++)
				{
					s = (scan_buf[scan_buf_r++]);//Uart2_PutChar
					
					if(scan_text_flag == 0)
					{
						if(s == 0x0A || s == 0x0D)
						{
							scan_text_flag = 1;
							scan_text_buf[scan_text_w++] = 0;
						}
						else
							scan_text_buf[scan_text_w++] = s;
					}
				}
			}
			else if(scan_buf_w < scan_buf_r){
				n = 64-scan_buf_r + scan_buf_w;
				for(i=0;i<n;i++){
					s=(scan_buf[scan_buf_r++]);//Uart2_PutChar
					if(scan_buf_r >= 64)scan_buf_r = 0;
					
					if(scan_text_flag == 0)
					{
						if(s == 0x0A || s == 0x0D)
						{
							scan_text_flag = 1;
							scan_text_buf[scan_text_w++] = 0;
						}
						else
							scan_text_buf[scan_text_w++] = s;
					}
				}
			}
			if(scan_text_flag == 1)
			{
				win = sui_window_create(T_STR("��⵽������","Detect the barcode"),&rect_win);
				font.panel = win;
				
				font_draw(60,210 - 8 * 3 ,&font,T_STR("�Ѽ�¼������:","Recorded the barcode"));
				
				loadbmpbintosram("/system32/barcode.bin",win->data + 90*win->w);
//				clr_win(win,CL_orange,160,110,30,160);
// 				rect.x = 160;rect.y = 210;rect.h = 8;rect.w = 160;
				rect.x = 0;rect.y = 210;rect.h = 8;rect.w = 300;
                
                ui_text_draw_alpha(&font,&rect,(char*)scan_text_buf);
                update_bar_code(NULL, scan_text_buf);
				memcpy(bar_code,scan_text_buf,40);
				sui_window_update(win);
				
//				rt_mb_send(&screen_mb, UPDATE_HOME);
				scan_text_flag =0;
				scan_text_w = 0;
				bar_code_flag = 1;
				rt_thread_delay(100);
				rt_mb_send(&screen_mb, UPDATE_HOME);
			}
			rt_thread_delay(5);
#endif
		}
}
TEST_CMD test[5] = {VOLTAGE,CURRENT ,POWER , PF , COM_ADDRESS};
static void ZWD414B_Sever(void *parameter)
{
	// 	uint8_t err;
    Zwd414b_Init();
    Zwd414b_Test(test , 5);
    Zwd414b_Set_Energy(START_ENERGY);

		while(1)
		{
			Receive_data_Dispose();
			Zwd414b_Send_data_Dispose();
			rt_thread_delay(10);
		}
}

#include "file.hdh"
#include "step.hdh"
#include "work_mode.hdh"
#include "N_Mode.hdh"
#include "G_Mode.hdh"
#include "bsp_graph.h"
char *name_list[2][4] = {{"�ļ���:"	, "�ļ�����:"	, "����ģʽ:", "��һ����:"},
						{"File:"   , "File Step:"	, "Work Mode:", "Single Fault:"}};
static void interface_com(void)
{
	char buf[20] = "";
	struct rect_type rect = {0,0,0,0};
	struct font_info_t font={&panel_home,RTGUI_RGB(240, 160, 80),0x0,1,0,16};

	/* �ļ��� */
	rect.x	=	10;
	rect.y	=	5;
	rect.h	=	16;
	rect.w	=	16;
	draw_alphabmp(&panel_home,&rect,(u16 *)file_hdh,0x19f6);
	rect.x	+=	(rect.w+4);
	font_draw(rect.x,rect.y,&font,name_list[language][0]);
	rect.x	+=	(rt_strlen(name_list[language][0])*8);
	if(rt_strlen(file_info[flash_info.current_file].name) > 10)
	{
		rt_strncpy(buf,file_info[flash_info.current_file].name,8);
		rt_strncpy(buf+8,"..",3);
		font_draw(rect.x,rect.y,&font,buf);
	}
	else
		font_draw(rect.x+(80-rt_strlen(file_info[flash_info.current_file].name)*8)/2,rect.y,&font,file_info[flash_info.current_file].name);
	rect.x	+=	80;
	/* �ļ����� */
	rect.x	+=	8;
	draw_alphabmp(&panel_home,&rect,(u16 *)step_hdh,0x19f6);
	rect.x	+=	(rect.w+4);
	font_draw(rect.x,rect.y,&font,name_list[language][1]);
	rect.x	+=	(rt_strlen(name_list[language][1])*8);
	rt_sprintf(buf,"%d/%d",current_step_num,file_info[flash_info.current_file].totalstep);
	font_draw(rect.x+(80-rt_strlen(buf)*8)/2,rect.y,&font,buf);
	rect.x	+=	80;
	/* ����ģʽ */
	rect.x	+=	8;
	draw_alphabmp(&panel_home,&rect,(u16 *)work_mode_hdh,0x19f6);
	rect.x	+=	(rect.w+4);
	font_draw(rect.x,rect.y,&font,name_list[language][2]);
	rect.x	+=	(rt_strlen(name_list[language][2])*8);
	rect.x	+=	10;
	if(file_info[flash_info.current_file].mode == N_WORK)
		draw_alphabmp(&panel_home,&rect,(u16 *)N_Mode_hdh,0x19f6);
	else if(file_info[flash_info.current_file].mode == G_WORK)
		draw_alphabmp(&panel_home,&rect,(u16 *)G_Mode_hdh,0x19f6);
	
	/* ��һ���� */
	{
	rt_uint16_t temp;
	STEP_NODE *node = rt_list_entry(g_cur_step, STEP_NODE, list);
	
	if(node->un.com.mode == LC)
	{
		struct step_lc_t *lcx = &(node->un.lc);
		rect.x	+=	50;
		font_draw(rect.x,rect.y,&font,name_list[language][3]);
		rect.x	+=	(rt_strlen(name_list[language][3])*8);
		rect.x	+=	10;
		temp = lcx->singlefault;
		font_draw(rect.x,rect.y,&font,single_boolean_name[language][!!temp]);
	}
	else
	{
		rect.x	+=	50;
		font_draw(rect.x,rect.y,&font,"         ");
		rect.x	+=	(rt_strlen("         ")*8);
		rect.x	+=	10;
		font_draw(rect.x,rect.y,&font,"    ");
	}
	}
}

static void user_define_draw_rect(struct rect_type *rect,char **item_text,char *parameter_text,char len)
{
	u16		rect_color=0xf800,i,x;
	struct rect_type rect_t;
	struct font_info_t font={&panel_home,RTGUI_RGB(128, 240, 240),0x0,1,0,16};
	
	/* ���ƶ��� */
	rect_t		=	*rect;
	rect_t.w	=	10;
	rect_t.h	=	2;
	draw_fillrect(font.panel,&rect_t,rect_color);
	rect_t.x	+=	(rt_strlen(*item_text)*8 + 10);
	rect_t.w	=	rect->w - (rt_strlen(*item_text)*8 + 10);
	draw_fillrect(font.panel,&rect_t,rect_color);
	/* ���Ƶ��� */
	rect_t		=	*rect;
	rect_t.y	+=	rect->h;
	rect_t.h	=	2;
	draw_fillrect(font.panel,&rect_t,rect_color);
	/* �������� */
	rect_t		=	*rect;
	rect_t.w	=	2;
	draw_fillrect(font.panel,&rect_t,rect_color);
	/* �������� */
	rect_t		=	*rect;
	rect_t.x	+=	(rect->w-2);
	rect_t.w	=	2;
	draw_fillrect(font.panel,&rect_t,rect_color);
	
	/* ���Ʊ��� */
	rect_t.x	=	rect->x+10;
	rect_t.y	=	rect->y-8;
	font_draw(rect_t.x,rect_t.y,&font,*item_text);
	/* �������� */
	rect_t.x	+=	5;
	x		=	rect_t.x + 80;
	font.fontcolor	=	0xffff;
	for(i=0;i<len;i++)
	{
		item_text++;
		rect_t.y	+=	26;
		font_draw(rect_t.x,rect_t.y,&font,*item_text);
		font_draw(x+(80-rt_strlen(parameter_text)*8)/2,rect_t.y,&font,parameter_text);
		parameter_text += 10;
	}
}

const char* lc_phase[2][2]={
	{"L��","N��"},
	{"L","N"}
};

void display_LC_L_N(uint16_t x, uint16_t y, struct font_info_t *font_info)
{
	rt_uint16_t temp;
	static uint16_t p_x = 0xffff;
	static uint16_t p_y = 0xffff;
	static struct font_info_t font;
	
	struct step_lc_t *lcx = &(rt_list_entry(g_cur_step, STEP_NODE, list)->un.lc);
	
	if(x != 0 && y != 0)
	{
		p_x = x;
		p_y = y;
	}
	
	if(font_info != NULL)
	{
		font = *font_info;
	}
	
	if(p_x == 0xffff || p_y == 0xffff)
	{
		return;
	}
	
	temp = lcx->NorLphase;
	font_draw(p_x, p_y, &font, lc_phase[language][temp]);
}
static void interface_test(u8 mode,void *args)
{
	u16		rect_color=0xf800,i,temp;
	char buf[20];
	struct rect_type rect={240,40,342,400};
	struct rect_type rect_t;
	struct font_info_t font={&panel_home,0xffff,0x0,1,0,16};
	
	/* ���Ʊ߿� */
	rect_t=rect; rect_t.h = 2;
	draw_fillrect(&panel_home,&rect_t,rect_color);
	rect_t.y+=rect.h;
	draw_fillrect(&panel_home,&rect_t,rect_color);
	rect_t=rect; rect_t.w = 2; 
	draw_fillrect(&panel_home,&rect_t,rect_color);
	rect_t.x+=rect.w; rect_t.h += 2;
	draw_fillrect(&panel_home,&rect_t,rect_color);
	
	rect_t=rect; rect_t.x+=2; rect_t.y+=2; rect_t.h = 1; rect_t.w -= 2; rect_color = CL_WHITE;
	for(i=0;i<5;i++)
	{
		draw_fillrect(&panel_home,&rect_t,rect_color);
		rect_t.y += 68;
	}
	rect_t=rect; rect_t.x+=2; rect_t.y+=3; rect_t.h = 28; rect_t.w -= 2; rect_color = CL_BLUE2;
	for(i=0;i<5;i++)
	{
		draw_fillrect(&panel_home,&rect_t,rect_color);
		rect_t.y += 68;
	}
	rect_t=rect; rect_t.x+=2; rect_t.y+=31; rect_t.h = 1; rect_t.w -= 2; rect_color = RTGUI_RGB(128, 128, 128);
	for(i=0;i<5;i++)
	{
		draw_fillrect(&panel_home,&rect_t,rect_color);
		rect_t.y += 68;
	}

	switch(mode)
	{
		case ACW:
		{
			struct step_acw_t *acw=(struct step_acw_t *)args;
			rect_t = rect; rect_t.y+=2;
			if(_system_environment.systemlanguage == 0){
				font_draw(rect_t.x+(200-rt_strlen("ģʽ")*8)/2,rect_t.y+7,&font,"ģʽ");font_draw(rect_t.x+200+(200-rt_strlen("ģʽ")*8)/2,rect_t.y+7,&font,"״̬");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("���õ�ѹ")*8)/2,rect_t.y+7,&font,"���Ե�ѹ");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("���õ�ѹ")*8)/2,rect_t.y+7,&font,"���Ե���");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("���õ�ѹ")*8)/2,rect_t.y+7,&font,"��ʵ����");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("���õ�ѹ")*8)/2,rect_t.y+7,&font,"����ʱ��");rect_t.y+=68;
				rect_t = rect; rect_t.x += 2; rect_t.y += 32; font.high	=	32;
				font_draw(rect_t.x+(200-rt_strlen("������ѹ")*16)/2,rect_t.y+3,&font,"������ѹ");font_draw(rect_t.x+200+(200-rt_strlen("�ȴ�����")*16)/2,rect_t.y+3,&font,"�ȴ�����");rect_t.y+=68;
			}else{
				font_draw(rect_t.x+(200-rt_strlen("Mode")*8)/2,rect_t.y+7,&font,"Mode");font_draw(rect_t.x+200+(200-rt_strlen("State")*8)/2,rect_t.y+7,&font,"State");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("SetVol")*8)/2,rect_t.y+7,&font,"SetVol");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("SetCur")*8)/2,rect_t.y+7,&font,"SetCur");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("RealCur")*8)/2,rect_t.y+7,&font,"RealCur");rect_t.y+=68;
				font_draw(rect_t.x+(400-rt_strlen("TestTime")*8)/2,rect_t.y+7,&font,"TestTime");rect_t.y+=68;
				rect_t = rect; rect_t.x += 2; rect_t.y += 32; font.high	=	32;
				font_draw(rect_t.x+(200-rt_strlen("ACW")*16)/2,rect_t.y+3,&font,"ACW");
				font_draw(rect_t.x+200+(200-rt_strlen("WAIT")*16)/2,rect_t.y+3,&font,"WAIT");
				rect_t.y+=68;
			}
				
			temp = 0;
// 			temp = acw->outvol;
			rt_sprintf(buf,"%d.%03dkV", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			temp = 0;
// 			temp = acw->curhigh;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
// 			temp = acw->rmscur;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			temp = acw->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			
		}
			break;
		case DCW:
		{
			struct step_dcw_t *dcw=(struct step_dcw_t *)args;
			rect_t = rect;rect_t.y+=2;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("ģʽ","Mode"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","Mode"));rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե�ѹ","Test Voltage"))*8)/2,rect_t.y+7,&font,T_STR("���Ե�ѹ","Test Voltage"));rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե���","Test Current"))*8)/2,rect_t.y+7,&font,T_STR("���Ե���","Test Current"));rect_t.y+=68;
			
			rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));rect_t.y+=68;
			rect_t = rect; rect_t.x += 2; rect_t.y += 32; font.high	=	32;
			font_draw(rect_t.x+(200-rt_strlen(T_STR("������ѹ","ACW"))*16)/2,rect_t.y+3,&font,T_STR("ֱ����ѹ","DCW"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ȴ�����","WAIT"))*16)/2,rect_t.y+3,&font,T_STR("�ȴ�����","WAIT"));rect_t.y+=68;
		
			temp = 0;
// 			temp = dcw->outvol;
			rt_sprintf(buf,"%d.%03dkV", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			temp = 0;
// 			temp = dcw->curhigh;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			/*temp = dcw->curlow;
			temp==0?buf[0]=0:rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);*/rect_t.y+=68;
			temp = dcw->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
		}
			break;
		case GR:
		{
			struct step_gr_t *gr=(struct step_gr_t *)args;
			rect_t = rect;rect_t.y+=2;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&
			font,T_STR("ģʽ","Mode"));font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","State"));rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե���","Test Current"))*8)/2,rect_t.y+7,&font,T_STR("���Ե���","Test Current"));rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե���","Test Res."))*8)/2,rect_t.y+7,&font,T_STR("���Ե���","Test Res."));rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե�ѹ","Test Voltage"))*8)/2,rect_t.y+7,&font,T_STR("���Ե�ѹ","Test Voltage"));rect_t.y+=68;   //douyijun
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));rect_t.y+=68;
			
			rect_t = rect; rect_t.x += 2; rect_t.y += 32; font.high	=	32;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("�ӵز���","GR"))*16)/2,rect_t.y+3,&font,T_STR("�ӵز���","GR"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ȴ�����","WAIT"))*16)/2,rect_t.y+3,&font,T_STR("�ȴ�����","WAIT"));rect_t.y+=68;
						
			temp = 0;
// 			temp = gr->outcur;
			rt_sprintf(buf,"%d.%dA", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			temp = 0;
// 			temp = gr->reshigh;
			rt_sprintf(buf,"%dm��", temp);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
//			temp = gr->reslow;
			rt_sprintf(buf,"%d.%02dV", temp/100,temp%100);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			temp = gr->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
		}
			break;
		case LC:
		{
			struct step_lc_t *lc=(struct step_lc_t *)args;
			rect_t = rect;rect_t.y+=2;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("ģʽ","Mode"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","State"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���õ�ѹ","Set Voltage"))*8)/2,rect_t.y+7,&font,T_STR("���Ե�ѹ","Test Voltage"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���õ�ѹ","Set Voltage"))*8)/2,rect_t.y+7,&font,T_STR("���Ե���","Test Current"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen("SELV")*8)/2,rect_t.y+7,&font,"SELV");
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("������ѹ","Assist Voltage"))*8)/2,rect_t.y+7,&font,T_STR("������ѹ","Assist Voltage"));
			rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));
			rect_t.y+=68;
			rect_t = rect; 
			rect_t.x+= 2;
			rect_t.y += 32; 
			font.high	=	32;
			font_draw(rect_t.x+(200-rt_strlen(T_STR("й©����","LC"))*16)/2,rect_t.y+3,&font,T_STR("й©����","LC"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ȴ�����","WAIT"))*16)/2,rect_t.y+3,&font,T_STR("�ȴ�����","WAIT"));

			rect_t.y+=68;
			rt_mb_send(&screen_mb, UPDATE_HOME);
			
// 			temp = lc->outvol;
			temp = 0;
			rt_sprintf(buf,"%d.%dV", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			
// 			temp = lc->curhigh;
			temp = 0;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			temp = lc->curdetection;
			switch(temp)
			{
				case 0:    // AC
					strcat(buf," (AC)");
				break;
				case 1:    // AC+DC
					strcat(buf," (RMS)");
				break;
				case 2:    // PEAK
					strcat(buf," (PEAK)");
				break;
				case 3:    // DC
					strcat(buf," (DC)");
				break;
				
				default:
				
				break;
			}
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.x+=130;
		//	font_draw(rect_t.x+(398-rt_strlen("N")*16)/2,rect_t.y+3,&font,"N");
			display_LC_L_N(rect_t.x+(398-rt_strlen(lc_phase[language][temp])*16)/2 + 20,rect_t.y+3,&font);

			
			rect_t.x-=130;
			rect_t.y+=68;
			
// 			temp = lc->curlow;
			temp = 0;
			rt_sprintf(buf,"%d.%02dV", temp/100,temp%100);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			
			temp = lc->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
		}
			break;
		
		case IR:
		{
			struct step_ir_t *ir=(struct step_ir_t *)args;
			rect_t = rect; rect_t.y+=2;
			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("ģʽ","Mode"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","State"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե�ѹ","Test Voltage"))*8)/2,rect_t.y+7,&font,T_STR("���Ե�ѹ","Test Voltage"));	
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե���","IR"))*8)/2,rect_t.y+7,&font,T_STR("���Ե���","IR"));
			
			rect_t.y+=68;
			/*font_draw(rect_t.x+(400-rt_strlen("")*8)/2,rect_t.y+7,&font,"");*/            
			rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));
			rect_t.y+=68;
			rect_t = rect; 
			rect_t.x += 2; 
			rect_t.y += 32;
			font.high	=	32;
			font_draw(rect_t.x+(200-rt_strlen(T_STR("��Ե����","IR"))*16)/2,rect_t.y+3,&font,T_STR("��Ե����","IR"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ȴ�����","WAIT"))*16)/2,rect_t.y+3,&font,T_STR("�ȴ�����","WAIT"));
			rect_t.y+=68;
		
			temp = 0;
			rt_sprintf(buf,"%d.%03dkV", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"d.%02dM��", temp/1000,temp%1000);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;

			rect_t.y+=68;
			
			temp = ir->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			
		break;
		}
		case PW:
		{
			struct step_pw_t *pw=(struct step_pw_t *)args;
			rect_t = rect;rect_t.y+=2;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("ģʽ","Mode"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","State"));
			
			rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե�ѹ","Test Voltage"))*8)/2,rect_t.y+7,&font,T_STR("���Ե�ѹ","Test Voltage"));
			rect_t.y+=68;
			font_draw(rect_t.x+(400-rt_strlen(T_STR("���Ե���","Test Current"))*8)/2,rect_t.y+7,&font,T_STR("���Ե���","Test Current"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("���Թ���","Test Power"))*8)/2,rect_t.y+7,&font,T_STR("���Թ���","Test Power"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("��������","Power Factor"))*8)/2,rect_t.y+7,&font,T_STR("��������","Power Factor"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));
			rect_t.y+=68;
			rect_t = rect;
			rect_t.x+= 2; 
			rect_t.y += 32; 
			font.high	=	32;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("���ʲ���","Power Test"))*16)/2,rect_t.y+3,&font,T_STR("���ʲ���","Power Test"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ȴ�����","WAIT"))*16)/2,rect_t.y+3,&font,T_STR("�ȴ�����","WAIT"));
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%01dV", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%02d A", temp/100,temp%100);
			
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%03dkW", temp/1000,temp%1000);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			temp = pw->factorhigh;
			rt_sprintf(buf,"%d.%03d", temp/1000,temp%1000);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = pw->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
		
			break;
		}
		case ACW_GR:
		{
			struct step_acw_gr_t *acw_gr=(struct step_acw_gr_t *)args;
			rect_t = rect;rect_t.y+=2;

			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("ģʽ","Mode"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","State"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("������ѹ","Alternating Voltage"))*8)/2,rect_t.y+7,&font,T_STR("������ѹ","Alternating Voltage"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ӵص���","Earth Current"))*8)/2,rect_t.y+7,&font,T_STR("�ӵص���","Earth Current"));
			rect_t.y+=68;
			
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("������ѹ","Alternating Voltage"))*8)/2,rect_t.y+7,&font,T_STR("������ѹ","Alternating Voltage"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ӵص���","Earth Resistance"))*8)/2,rect_t.y+7,&font,T_STR("�ӵص���","Earth Resistance"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("��ʵ����","Real Current"))*8)/2,rect_t.y+7,&font,T_STR("��ʵ����","Real Current"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ӵص�ѹ","Earth Voltage"))*8)/2,rect_t.y+7,&font,T_STR("�ӵص�ѹ","Earth Voltage"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));
			rect_t.y+=68;
			
			rect_t = rect; rect_t.x+= 2; rect_t.y += 32; font.high	=	32;
			
			font_draw(rect_t.x+(200-rt_strlen("ACW+GR")*16)/2,rect_t.y+3,&font,"ACW+GR");
			font_draw(rect_t.x+200+(200-rt_strlen("�ȴ�����")*16)/2,rect_t.y+3,&font,"�ȴ�����");
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%03dkV", temp/1000,temp%1000);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			temp = 0;
			rt_sprintf(buf,"%d.%dA", temp/10,temp%10);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			temp = 0;
			rt_sprintf(buf,"%dm��", temp);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			temp = 0;
			rt_sprintf(buf,"%d.%02dV", temp/100,temp%100);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			
			temp = acw_gr->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
		}	
		break;
		
		case DCW_GR:
		{
			struct step_acw_gr_t *dcw_gr=(struct step_acw_gr_t *)args;
			rect_t = rect;rect_t.y+=2;
			font_draw(rect_t.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("ģʽ","Mode"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect_t.y+7,&font,T_STR("״̬","State"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("������ѹ","AC Voltage"))*8)/2,rect_t.y+7,&font,T_STR("������ѹ","AC Voltage"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ӵص���","Earth Current"))*8)/2,rect_t.y+7,&font,T_STR("�ӵص���","Earth Current"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen(T_STR("��������","AC Current"))*8)/2,rect_t.y+7,&font,T_STR("��������","AC Current"));
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ӵص���","GR"))*8)/2,rect_t.y+7,&font,T_STR("�ӵص���","GR"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(200-rt_strlen("")*8)/2,rect_t.y+7,&font,"");
			
			font_draw(rect_t.x+200+(200-rt_strlen(T_STR("�ӵص�ѹ","Earth Voltage"))*8)/2,rect_t.y+7,&font,T_STR("�ӵص�ѹ","Earth Voltage"));
			rect_t.y+=68;
			
			font_draw(rect_t.x+(400-rt_strlen(T_STR("����ʱ��","Test Time"))*8)/2,rect_t.y+7,&font,T_STR("����ʱ��","Test Time"));
			rect_t.y+=68;
			
			rect_t = rect; rect_t.x+= 2; rect_t.y += 32; font.high	=	32;
			
			font_draw(rect_t.x+(200-rt_strlen("DCW+GR")*16)/2,rect_t.y+3,&font,"DCW+GR");font_draw(rect_t.x+200+(200-rt_strlen("�ȴ�����")*16)/2,rect_t.y+3,&font,"�ȴ�����");rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%03dkV", temp/1000,temp%1000);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			temp = 0;
			rt_sprintf(buf,"%d.%dA", temp/10,temp%10);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%03dmA", temp/1000,temp%1000);
			font_draw(rect_t.x+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			temp = 0;
			rt_sprintf(buf,"%dm��", temp);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = 0;
			rt_sprintf(buf,"%d.%02dV", temp/100,temp%100);
			font_draw(rect_t.x+199+(199-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);
			rect_t.y+=68;
			
			temp = dcw_gr->testtime;
			rt_sprintf(buf,"%d.%ds", temp/10,temp%10);
			font_draw(rect_t.x+(398-rt_strlen(buf)*16)/2,rect_t.y+3,&font,buf);rect_t.y+=68;
		}	
		break;
		
		default:
			
		break;
					
				
	}
	
}

uint8_t *SW_ST[2]={"�ر�", "����"};

uint8_t *SW_ST_ENG[2]={"OFF","ON"}; 

static void interface_acw_gr(struct step_acw_gr_t *acw_gr)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",
			"�����ѹ:",
			"�������:",
			"��������:",
 			"��������:",
//			"�绡���:",			
			"����ʱ��:",
			"��������:",
		},
		{"StepParamInfo",
			"Voltage :",
			"Current :",
			"Cur Up  :",
			"Res Up  :",
//   		 "ARC:",
			"TestTime:",
			"StepCon :",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{"ϵͳ״̬��Ϣ",
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{"SystemStatus",
			"Test Inf:",	
			"PortMode:",
			"DUT Stat:",
			"GFI Proe:",
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	u16 temp;
	
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%03dkV", acw_gr->outvol/1000,acw_gr->outvol%1000);
	rt_sprintf(buf[1],"%d.%dA", acw_gr->groutcur/10,acw_gr->groutcur%10);
	temp = acw_gr->curhigh;
	switch(acw_gr->curgear)
	{
		case I300uA:
			rt_sprintf(buf[2],"%d.%duA", temp/10,temp%10);
			break;
		case I3mA:
			rt_sprintf(buf[2],"%d.%03dmA", temp/1000,temp%1000);
			break;
		case I30mA:
			rt_sprintf(buf[2],"%d.%02dmA", temp/100,temp%100);
			break;
		case I100mA:
			rt_sprintf(buf[2],"%d.%dmA", temp/10,temp%10);
			break;
		default:
			
			break;
	}
	
	rt_sprintf(buf[3],"%dm��", acw_gr->grreshigh);
	
	rt_sprintf(buf[4],"%d.%ds", acw_gr->testtime/10,acw_gr->testtime%10);
	strcpy(buf[5],boolean_name[language][acw_gr->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"ACW_GR->");
	
	strcpy(buf[1],T_STR("����","Float"));
	strcpy(buf[2],T_STR("�ر�","OFF"));
	strcpy(buf[3],T_ARRY(SW_ST, SW_ST_ENG ,!!system_parameter_t.env.GFI));
	
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(ACW_GR,(void *)acw_gr);
}

static void interface_dcw_gr(struct step_dcw_gr_t *dcw_gr)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",// title
			"�����ѹ:",
			"�������:",
			"��������:",
 			"��������:",
//			"�绡���:",			
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",// title 
			"Voltage:",	
			"Current:",
			"Cur Up :",
			"Res Up :",
//    "ARC:",
			"TestTime:",
			"StepCon:",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{"ϵͳ״̬��Ϣ",// title
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{"System Status",// title 
			"Test Inf:",
			"PortMode:",
			"DUT Stat:",
			"GFI Pro :",
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	u16 temp;
	
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%03dkV", dcw_gr->outvol/1000,dcw_gr->outvol%1000);
	rt_sprintf(buf[1],"%d.%dA", dcw_gr->groutcur/10,dcw_gr->groutcur%10);
	temp = dcw_gr->curhigh;
	switch(dcw_gr->curgear)
	{
		case I3uA:
			rt_sprintf(buf[2],"%d.%03duA", temp/1000,temp%1000);
			break;
		case I30uA:
			rt_sprintf(buf[2],"%d.%02duA", temp/100,temp%100);
			break;
		case I300uA:
			rt_sprintf(buf[2],"%d.%duA", temp/10,temp%10);
			break;
		case I3mA:
			rt_sprintf(buf[2],"%d.%03dmA", temp/1000,temp%1000);
			break;
		case I30mA:
			rt_sprintf(buf[2],"%d.%02dmA", temp/100,temp%100);
			break;
		case I100mA:
			rt_sprintf(buf[2],"%d.%dmA", temp/10,temp%10);
			break;
		default:
			
			break;
	}
	rt_sprintf(buf[3],"%dm��", dcw_gr->grreshigh);

//	rt_sprintf(buf[4],"%d", acw_gr->arc);
	rt_sprintf(buf[4],"%d.%ds", dcw_gr->testtime/10,dcw_gr->testtime%10);
	strcpy(buf[5],boolean_name[language][dcw_gr->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"DCW_GR->");
	/*
	strcpy(buf[1],"����");
	strcpy(buf[2],"�ر�");
	strcpy(buf[3],SW_ST[!!system_parameter_t.env.GFI]);
	*/
	strcpy(buf[1],T_STR("����","Float"));
	strcpy(buf[2],T_STR("�ر�","OFF"));
	strcpy(buf[3],T_ARRY(SW_ST, SW_ST_ENG ,!!system_parameter_t.env.GFI));
	
	
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(DCW_GR,(void *)dcw_gr);
}

static void interface_acw(struct step_acw_t *acw)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",
			"�����ѹ:",
			"��������:",
			"��ʵ����:",
// 			"���Ƶ��:",
			"�绡���:",
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",
			"Voltage :",			
			"Curr  Up:",
			"RealCur :",
//			"OutFrequency:",
			"ARC     :",
			"TestTime:",
			"StepCon :",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{"ϵͳ״̬��Ϣ",
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{"SystemStatus", 
			"Test Inf:",
			"PortMode:",
			"DUT Stat:",
			"GFI Prot:",
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	u16 temp;
	
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%03dkV", acw->outvol/1000,acw->outvol%1000);
	temp = acw->curhigh;
	switch(acw->curgear)
	{
		case I300uA:
			rt_sprintf(buf[1],"%d.%duA", temp/10,temp%10);
			break;
		case I3mA:
			rt_sprintf(buf[1],"%d.%03dmA", temp/1000,temp%1000);
			break;
		case I30mA:
			rt_sprintf(buf[1],"%d.%02dmA", temp/100,temp%100);
			break;
		case I100mA:
			rt_sprintf(buf[1],"%d.%dmA", temp/10,temp%10);
			break;
		default:
			
			break;
	}
// 	rt_sprintf(buf[1],"%d.%03dmA", acw->curhigh/1000,acw->curhigh%1000);
	temp = acw->rmscur;
	switch(acw->curgear)
	{
		case I300uA:
			rt_sprintf(buf[2],"%d.%duA", temp/10,temp%10);
			break;
		case I3mA:
			rt_sprintf(buf[2],"%d.%03dmA", temp/1000,temp%1000);
			break;
		case I30mA:
			rt_sprintf(buf[2],"%d.%02dmA", temp/100,temp%100);
			break;
		case I100mA:
			rt_sprintf(buf[2],"%d.%dmA", temp/10,temp%10);
			break;
		default:
			
			break;
	}
// 	rt_sprintf(buf[2],"%d.%03dmA", acw->rmscur/1000,acw->rmscur%1000);
	rt_sprintf(buf[3],"%d", acw->arc);
	rt_sprintf(buf[4],"%d.%ds", acw->testtime/10,acw->testtime%10);
	strcpy(buf[5],boolean_name[language][acw->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"ACW->");
	if(_system_environment.systemlanguage == 0){
		strcpy(buf[1],"����");
		strcpy(buf[2],"�ر�");
		strcpy(buf[3],SW_ST[!!system_parameter_t.env.GFI]);
	}else{
		strcpy(buf[1],"Float");
		strcpy(buf[2],"OFF");
		strcpy(buf[3],"ON");
	}
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(ACW,(void *)acw);
}

static void interface_dcw(struct step_dcw_t *dcw)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",
			"�����ѹ:",
			"��������:",
			"��������:",
// 			"���Ƶ��:",
			"�绡���:",
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",
			"Voltage:",
			"Cur Up :",
			"CurDown:",
//      "OutFrequency:",
			"ARC:",
			"TestTime:",
			"StepContinue:",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{"ϵͳ״̬��Ϣ",
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{"System Status",
			"Test Inf:",
			"Port Mode:",
			"DUT Status:",
			"GFI Protect:",
			
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	u16 temp;
	
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%03dkV", dcw->outvol/1000,dcw->outvol%1000);
	temp = dcw->curhigh;
	switch(dcw->curgear)
	{
		case I3uA:
			rt_sprintf(buf[1],"%d.%03duA", temp/1000,temp%1000);
			break;
		case I30uA:
			rt_sprintf(buf[1],"%d.%02duA", temp/100,temp%100);
			break;
		case I300uA:
			rt_sprintf(buf[1],"%d.%duA", temp/10,temp%10);
			break;
		case I3mA:
			rt_sprintf(buf[1],"%d.%03dmA", temp/1000,temp%1000);
			break;
		case I30mA:
			rt_sprintf(buf[1],"%d.%02dmA", temp/100,temp%100);
			break;
		case I100mA:
			rt_sprintf(buf[1],"%d.%dmA", temp/10,temp%10);
			break;
		default:
			
			break;
	}
// 	rt_sprintf(buf[1],"%d.%03dmA", dcw->curhigh/1000,dcw->curhigh%1000);
	temp = dcw->curlow;
	switch(dcw->curgear)
	{
		case I3uA:
			rt_sprintf(buf[2],"%d.%03duA", temp/1000,temp%1000);
			break;
		case I30uA:
			rt_sprintf(buf[2],"%d.%02duA", temp/100,temp%100);
			break;
		case I300uA:
			rt_sprintf(buf[2],"%d.%duA", temp/10,temp%10);
			break;
		case I3mA:
			rt_sprintf(buf[2],"%d.%03dmA", temp/1000,temp%1000);
			break;
		case I30mA:
			rt_sprintf(buf[2],"%d.%02dmA", temp/100,temp%100);
			break;
		case I100mA:
			rt_sprintf(buf[2],"%d.%dmA", temp/10,temp%10);
			break;
		default:
			
			break;
	}
// 	rt_sprintf(buf[2],"%d.%03dmA", dcw->curlow/1000,dcw->curlow%1000);
	rt_sprintf(buf[3],"%d", dcw->arc);
	rt_sprintf(buf[4],"%d.%ds", dcw->testtime/10,dcw->testtime%10);
	strcpy(buf[5],boolean_name[language][dcw->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"DCW->");
	
	/*
	strcpy(buf[1],"����");
	strcpy(buf[2],"�ر�");
	strcpy(buf[3],SW_ST[!!system_parameter_t.env.GFI]);
	*/
	
	strcpy(buf[1],T_STR("����","Float"));
	strcpy(buf[2],T_STR("�ر�","OFF"));
	strcpy(buf[3],T_ARRY(SW_ST, SW_ST_ENG ,!!system_parameter_t.env.GFI));
	
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(DCW,(void *)dcw);
}

static void interface_gr(struct step_gr_t *gr)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",// title
			"�������:",
			"��������:",
			"��������:",
			"���Ƶ��:",
// 			"�绡���:",
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",// title 			 
			"Current :",			
			"ResUp   :",
			"ResDown :",
			"OutRate :",
//    "ARC:",
			"TestTime:",
			"StepCon :",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{"ϵͳ״̬��Ϣ",// title
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{"SystemStatus",// title
			"Test Inf :",		
			"Port Mode:",
			"DUT Stat :",
			"GFI Pro  :",
			
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%dA", gr->outcur/10,gr->outcur%10);
	rt_sprintf(buf[1],"%dm��", gr->reshigh);
	rt_sprintf(buf[2],"%dm��", gr->reslow);
	rt_sprintf(buf[3],"%d.%dHz", gr->outfreq/10,gr->outfreq%10);
	rt_sprintf(buf[4],"%d.%ds", gr->testtime/10,gr->testtime%10);
	strcpy(buf[5],boolean_name[language][gr->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"GR->");
	
	/*
	strcpy(buf[1],"����");
	strcpy(buf[2],"�ر�");
	strcpy(buf[3],SW_ST[!!system_parameter_t.env.GFI]);
	
	*/
	
	strcpy(buf[1],T_STR("����","Float"));
	strcpy(buf[2],T_STR("�ر�","OFF"));
	strcpy(buf[3],T_ARRY(SW_ST, SW_ST_ENG ,!!system_parameter_t.env.GFI));
	
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(GR,(void *)gr);
}


static void interface_ir(struct step_ir_t *ir)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",// title
			"�����ѹ:",
			"��������:",
			"��������:",
			"����ʱ��:",
// 			"�绡���:",
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",// title 
			"Voltage :",			
			"Res.Up  :",
			"Res.Low :",
			"RiseTime:",
//    "ARC:",
			"TestTime:",
			"StepCon :",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{
			"ϵͳ״̬��Ϣ",// title
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{
			"System Status",// title
			"Test Inf :",
			"Port Mode:",
			"DUT Stat :",
			"GFI Pro  :",
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	u32 temp;
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%03dkV", ir->outvol/1000,ir->outvol%1000);
	switch(ir->autogear)
	{
		case 0:   //AUTO
		{
			temp = ir->reshigh;		
			if(temp<10000)//0~100M
			{
				rt_sprintf(buf[1],"%d.%02dM��", temp/100,temp%100);
			}else if(temp<100000)//100M~1G
			{
				temp = ir->reshigh / 10;											
				rt_sprintf(buf[1],"%d.%01dM��", temp/10,temp%10);
			}else if(temp<1000000)//1G~10G
			{
				temp = ir->reshigh / 100;											
				rt_sprintf(buf[1],"%d.%03dG��", temp/1000,temp%1000);
			}
			else if(temp<10000000)//10G~100G
			{
				temp = ir->reshigh / 1000;											
				rt_sprintf(buf[1],"%d.%02dG��", temp/100,temp%100);
			}
			else 
			{
				temp = ir->reshigh / 10000;											
				rt_sprintf(buf[1],"%d.%01dG��", temp/10,temp%10);
			}
			
				
			
			temp = ir->reslow;		
			if(temp<10000)//0~100M
			{
				rt_sprintf(buf[2],"%d.%02dM��", temp/100,temp%100);
			}else if(temp<100000)//100M~1G
			{
				temp = ir->reslow / 10;											
				rt_sprintf(buf[2],"%d.%01dM��", temp/10,temp%10);
			}else if(temp<1000000)//1G~10G
			{
				temp = ir->reslow / 100;											
				rt_sprintf(buf[2],"%d.%03dG��", temp/1000,temp%1000);
			}
			else if(temp<10000000)//10G~100G
			{
				temp = ir->reslow / 1000;											
				rt_sprintf(buf[2],"%d.%02dG��", temp/100,temp%100);
			}
			else 
			{
				temp = ir->reslow / 10000;											
				rt_sprintf(buf[2],"%d.%01dG��", temp/10,temp%10);
			}

		}
		break;
		case 1:  //10M��
		{
//			temp = temp;											
			temp = ir->reshigh;											
			rt_sprintf(buf[1],"%d.%02dM��", temp/100,temp%100);
			temp = ir->reslow;											
			rt_sprintf(buf[2],"%d.%02dM��", temp/100,temp%100);
		}
		break;
		case 2: //100M��
		{
//			temp = temp / 10;											
//			rt_sprintf(buf,"%d.%01dM��", temp/10,temp%10);
			temp = ir->reshigh / 10;											
			rt_sprintf(buf[1],"%d.%01dM��", temp/10,temp%10);
			temp = ir->reslow / 10;											
			rt_sprintf(buf[2],"%d.%01dM��", temp/10,temp%10);
		}
		break;
		case 3: //1G��
		{
//			temp = temp / 100;											
//			rt_sprintf(buf,"%d.%03dG��", temp/1000,temp%1000);
			temp = ir->reshigh / 100;											
			rt_sprintf(buf[1],"%d.%03dG��", temp/1000,temp%1000);
			temp = ir->reslow / 100;											
			rt_sprintf(buf[2],"%d.%03dG��", temp/1000,temp%1000);
		}
		break;
		case 4: //10G��
		{
//			temp = temp / 1000;											
//			rt_sprintf(buf,"%d.%02dG��", temp/100,temp%100);
			temp = ir->reshigh / 1000;											
			rt_sprintf(buf[1],"%d.%02dG��", temp/100,temp%100);
			temp = ir->reslow / 1000;											
			rt_sprintf(buf[2],"%d.%02dG��", temp/100,temp%100);
		}
		break;	
		case 5: //100G��
		{
//			temp = temp / 10000;											
//			rt_sprintf(buf,"%d.%01dG��", temp/10,temp%10);
			temp = ir->reshigh / 10000;											
			rt_sprintf(buf[1],"%d.%01dG��", temp/10,temp%10);
			temp = ir->reslow / 10000;											
			rt_sprintf(buf[2],"%d.%01dG��", temp/10,temp%10);
		}							
		default:
			
		break;
	}	
	rt_sprintf(buf[3],"%d.%ds", ir->ramptime/10,ir->ramptime%10);
	rt_sprintf(buf[4],"%d.%ds", ir->testtime/10,ir->testtime%10);
	strcpy(buf[5],boolean_name[language][ir->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"IR->");
	/*
	strcpy(buf[1],"����");
	strcpy(buf[2],"�ر�");
	strcpy(buf[3],SW_ST[!!system_parameter_t.env.GFI]);
	*/
	strcpy(buf[1],T_STR("����","Float"));
	strcpy(buf[2],T_STR("�ر�","OFF"));
	strcpy(buf[3],T_ARRY(SW_ST, SW_ST_ENG ,!!system_parameter_t.env.GFI));
	
	
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(IR,(void *)ir);
}

static void interface_pw(struct step_pw_t *pw)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",// title
			"�����ѹ:",
			"��������:",
			"��������:",
			"��������:",
// 			"�绡���:",
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",// title 
			"Voltage :",
			"Cur  Up :",
			"PowerUp :",
			"FactorUp:",
//    "ARC:",
			"TestTime:",
			"Step Con:",
		}
	};
	static char *system_status_info_item[2][5] =
	{
		{"ϵͳ״̬��Ϣ",// title
			"������Ϣ:",
			"�˿�ģʽ:",
			"DUT ״̬:",
			"GFI ����:",
		},
		{"System Status",// title
			"Test Inf:",
			"Port Mode:",
			"DUT Status:",
			"GFI Protect:",
			
		}
	};
	struct rect_type rect = {10,40,6*26+16,200};
	char buf[6][10];
	
	/* ���ƹ������� */
	interface_com();
	/* ���Ʋ�����Ϣ */
	rt_sprintf(buf[0],"%d.%01d V", pw->outvol/10,pw->outvol%10);
	rt_sprintf(buf[1],"%d.%02d A", pw->curhigh/100,pw->curhigh%100);
	rt_sprintf(buf[2],"%d.%03d kW", pw->pwhigh/1000,pw->pwhigh%1000);
	rt_sprintf(buf[3],"%d.%03d", pw->factorhigh/1000,pw->factorhigh%1000);
	rt_sprintf(buf[4],"%d.%ds", pw->testtime/10,pw->testtime%10);
	strcpy(buf[5],boolean_name[language][pw->stepcontinuation]);
	user_define_draw_rect(&rect,step_item[language],buf[0],6);
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+30);
	rect.h	=	140;//4*26+16;
	strcpy(buf[0],"PW->");
	/*
	strcpy(buf[1],"����");
	strcpy(buf[2],"�ر�");
	strcpy(buf[3],SW_ST[!!system_parameter_t.env.GFI]);
	*/
	
	strcpy(buf[1],T_STR("����","Float"));
	strcpy(buf[2],T_STR("�ر�","OFF"));
	strcpy(buf[3],T_ARRY(SW_ST, SW_ST_ENG ,!!system_parameter_t.env.GFI));
	
	
	
	user_define_draw_rect(&rect,system_status_info_item[language],buf[0],4);
	
	interface_test(PW,(void *)pw);
}

#include "sw.h"
#include "Es.h"
#include "Fs.h"
#include "Gs.h"
static void lc_sw_image(u16 x, u16 y, struct step_lc_t *lc)
{
	ico_color_set(1,0xffff,0x0000);
	ico_darw(&panel_home,x,y,(u8 *)gImage_sw);
	
	ico_color_set(1,0xf800,0x0000);
	/* S5 */
	ico_darw(&panel_home,x+89,y+4,(lc->NorLphase == 0?(u8 *)gImage_sw22_up:(u8 *)gImage_sw22_down));
	/* S1 */
	ico_darw(&panel_home,x+27,y+34,(lc->singlefault == 1?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S11 */
	ico_darw(&panel_home,x+40,y+56,(lc->S11 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S8 */
	ico_darw(&panel_home,x+40,y+71,(lc->S8 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S10 */
	ico_darw(&panel_home,x+40,y+86,(lc->S10 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S7 */
	ico_darw(&panel_home,x+40,y+101,(lc->S7 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S12 */
	ico_darw(&panel_home,x+40,y+116,(lc->S12 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S13 */
	ico_darw(&panel_home,x+40,y+131,(lc->S13 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	/* S14 */
	ico_darw(&panel_home,x+40,y+146,(lc->S14 == 0?(u8 *)gImage_sw_off:(u8 *)gImage_sw_on));
	
	/* SH */
	ico_darw(&panel_home,x+139,y+75,(lc->SH == 0?(u8 *)gImage_sh_a:(u8 *)gImage_sh_b));
	/* SL */
	ico_darw(&panel_home,x+139,y+123,(lc->SL == 0?(u8 *)gImage_sl_a:(u8 *)gImage_sl_b));
}
static void interface_lc(struct step_lc_t *lc)
{
	static char *step_item[2][8] =
	{
		{"���������Ϣ",// title
			"�����ѹ:",
			"��������:",
			"�첨��ʽ:",
			"���Ƶ��:",
// 			"�绡���:",
			"����ʱ��:",
			"��������:",
		},
		{"Step Parameter Info",// title 
			"Voltage  :",
			"CurUp    :",
			"CheckMode:",
			"OutRate  :",
//    "ARC:",
			"TestTime :",
			"StepCon  :",
		}
	};
	static char *system_status_info_item[1] =
	{
		{"MD��������",// title
		},
	};
	
	
	static char *system_status_info_item_eng[1] =
	{
		{"MD  Human network ",// title
		},
	};

	static char *sw_info_item[1] =
	{
		{"��·״̬",// title
		},
	};
	
	static char *sw_info_item_eng[1] =
	{
		{"Circuit state",// title
		},
	};

	
	struct rect_type rect = {10,40,182,210};
	char buf[6][10];
	
	/* ���ƹ������� */
	interface_com();
	if(lc->MDnetwork == MD_E || lc->MDnetwork == MD_F || lc->MDnetwork == MD_G)
	{
		/* Ӣ�� */
		if(language)
		{
			user_define_draw_rect(&rect,sw_info_item_eng,buf[0],0);
		}
		/*����*/
		else
		{
			user_define_draw_rect(&rect,sw_info_item,buf[0],0);
		}
			
		
		lc_sw_image(rect.x+5,rect.y+15,lc);
	}
	else
	{
		/* ���Ʋ�����Ϣ */
		rt_sprintf(buf[0],"%d.%dV", lc->outvol/10,lc->outvol%10);
		rt_sprintf(buf[1],"%d.%03dmA", lc->curhigh/1000,lc->curhigh%1000);
		rt_sprintf(buf[2],"%s", lc_detection_name[lc->curdetection]);
		rt_sprintf(buf[3],"%d.%dHz", lc->outfreq/10,lc->outfreq%10);
		rt_sprintf(buf[4],"%d.%ds", lc->testtime/10,lc->testtime%10);
		strcpy(buf[5],boolean_name[language][lc->stepcontinuation]);
		user_define_draw_rect(&rect,step_item[language],buf[0],6);
	}	
	/* ����ϵͳ��Ϣ */
	rect.y	+=	(rect.h+20);
	rect.h	=	140;
	
	/* Ӣ�� */
	if(language)
	{
		user_define_draw_rect(&rect,system_status_info_item_eng,buf[0],0);
	}
	/*����*/
	else
	{
		user_define_draw_rect(&rect,system_status_info_item,buf[0],0);
	}
		
	ico_color_set(1,0xffff,0x0000);
	switch(lc->MDnetwork)
	{
		case MD_E:
			rect.x	+=	10;
			rect.y	+=	15;
			ico_darw(&panel_home,rect.x,rect.y,(u8 *)gImage_Es);
			break;
		case MD_F:
			rect.x	+=	10;
			rect.y	+=	15;
			ico_darw(&panel_home,rect.x,rect.y,(u8 *)gImage_Fs);
			break;
		case MD_G:
			rect.x	+=	10;
			rect.y	+=	15;
			ico_darw(&panel_home,rect.x,rect.y,(u8 *)gImage_Gs);
			break;
	}
	
	interface_test(LC,(void *)lc);
	
}

void ui_title_darw(struct font_info_t *font,struct rect_type *rect,char *str)
{
	window_copy(&panel_title,&panel_title_back,rect);
// 	rt_enter_critical();
	if(font->center!=0)
		font_draw(rect->x+(rect->w-rt_strlen(str)*font->high/2)/2,rect->y+(rect->h-font->high)/2,font,str);
	else 
		font_draw(rect->x,rect->y,font,str);
// 	rt_exit_critical();
	window_updata(font->panel,rect);
	
}
void ui_teststr_darw(struct font_info_t *font,struct rect_type *rect,char *str)
{
	window_copy(&panel_home,&panel_homeback,rect);
// 	rt_enter_critical();
	if(font->center!=0)
		font_draw(rect->x+(rect->w-rt_strlen(str)*font->high/2)/2,rect->y+(rect->h-font->high)/2,font,str);
	else 
		font_draw(rect->x,rect->y,font,str);
// 	rt_exit_critical();
	window_updata(font->panel,rect);
	
}
void clear_home_panel(struct rect_type *rect)
{
	window_copy(&panel_home,&panel_homeback,rect);
}
void lc_ui_teststr_darw(struct font_info_t *font,struct rect_type *rect,char *str)
{	
// 	rt_enter_critical();
	if(font->center!=0)
		font_draw(rect->x+(rect->w-rt_strlen(str)*font->high/2)/2,rect->y+(rect->h-font->high)/2,font,str);
	else 
		font_draw(rect->x,rect->y,font,str);
// 	rt_exit_critical();
	window_updata(font->panel,rect);
	
}



void dis_test_pass(void)
{
	struct font_info_t font = {&panel_home, 0xffff, 0x0, 1, 1, 32};
	char buf[20] = "";
	struct rect_type rect = {440, 72, 38, 200};
	
	strcpy(buf,T_STR("���Ժϸ�","Pass"));
	ui_teststr_darw(&font, &rect, buf);
}










/*************************************************************************************************************/
/*************************************************************************************************************/
/*	
*
*																����ΪС��Ҫʹ�õĻص�����
*
*
***************************************************************************************************************/


void save_offset_value(uint32_t offset_value)
{

}


UN_STR *dyj_next_step(void)
{
	UN_STR *un;
	struct rect_type rect={242,43,28,398};
	struct font_info_t font={&panel_home,0xffff,0x0,1,0,16};
	uint8_t loopflag = 0;     //ѭ����־
	if(current_step_num == 1)
	{
		g_cur_step = cs99xx_list.head;//ָ�����ļ�ͷ
		draw_fillrect(&panel_home,&rect,0xf800);
		
		font_draw(rect.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("ģʽ","Mode"));
		font_draw(rect.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("״̬","State"));
		window_updata(&panel_home,&rect);
	}
	else if(g_cur_step != NULL)
	{
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		switch(un->com.mode)
		{
			case ACW:
				if(un->acw.stepcontinuation)loopflag = 1;
				break;
			case DCW:
				if(un->dcw.stepcontinuation)loopflag = 1;
				break;
			case IR:
				if(un->ir.stepcontinuation)loopflag = 1;
				break;
			case GR:
				if(un->gr.stepcontinuation)loopflag = 1;
				break;
			case LC:
				if(un->lc.stepcontinuation)loopflag = 1;
				break;
			case PW:
				if(un->pw.stepcontinuation)loopflag = 1;
				break;
			case ACW_GR:
				if(un->acw_gr.stepcontinuation)loopflag = 1;
				break;
			case DCW_GR:
				if(un->dcw_gr.stepcontinuation)loopflag = 1;
				break;
		}
		
		g_cur_step = g_cur_step->next;
		
	}
	
	if(g_cur_step == NULL){
		if(loopflag && Test_Sched_Param.Pass_Flag){
			current_step_num = 1;
			g_cur_step = cs99xx_list.head;//ָ�����ļ�ͷ
			un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		}else{
			current_step_num = 1;
			draw_fillrect(&panel_home,&rect,CL_BLUE2);
			
			font_draw(rect.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("ģʽ","Mode"));
			font_draw(rect.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("״̬","State"));
			window_updata(&panel_home,&rect);
			return NULL;
		}
	}
	
	
	
	if(/*g_cur_step == NULL ||*/ Test_Sched_Param.Stop_Flag)	
	{
		current_step_num = 1;
		draw_fillrect(&panel_home,&rect,CL_BLUE2);
		
		font_draw(rect.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("ģʽ","Mode"));
		font_draw(rect.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("״̬","State"));
		window_updata(&panel_home,&rect);
		return NULL;
	}
	
	un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
	
	{
// 		char buf[10];
// 		rect.x = 138+(rt_strlen(name_list[language][0]) + rt_strlen(name_list[language][1]))*8;
// 		rect.y = 5;
// 		rect.h = 16;
// 		rect.w = 80;
// 		font.center = 1;
// 		font.fontcolor = RTGUI_RGB(240, 160, 80);
// 		rt_sprintf(buf,"%d/%d",current_step_num,file_info[flash_info.current_file].totalstep);
// 		ui_teststr_darw(&font,&rect,buf);
		
// 		rect.x = 242;
// 		rect.y = 72;
// 		rect.h = 38;
// 		rect.w = 200;
// 		font.fontcolor = 0xffff;
// 		font.center = 1;
// 		font.high = 32;
// 		ui_teststr_darw(&font,&rect,(char *)mode_name[un->com.mode]);
// 		
		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		
		switch(un->com.mode)
		{
			case ACW:
				interface_acw(&un->acw);
				break;
			case DCW:
				interface_dcw(&un->dcw);
				break;
			case IR:
				interface_ir(&un->ir);
				break;
			case GR:
				interface_gr(&un->gr);
				break;
			case LC:
				interface_lc(&un->lc);
				break;
			case PW:
				interface_pw(&un->pw);
				break;
			case ACW_GR:
				interface_acw_gr(&un->acw_gr);
				break;
			case DCW_GR:
				interface_dcw_gr(&un->dcw_gr);
				break;
		}
		rect.x = 242;
		rect.y = 43;
		rect.h = 28;
		rect.w = 398;
		font.fontcolor = 0xffff;
		font.center = 0;
		font.high = 16;
		draw_fillrect(&panel_home,&rect,0xf800);
		font_draw(rect.x+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("ģʽ","Mode"));
		font_draw(rect.x+200+(200-rt_strlen(T_STR("ģʽ","Mode"))*8)/2,rect.y+6,&font,T_STR("״̬","State"));
		panel_update(panel_home);
	}
	
	current_step_num++;
	
	return un;
}


void  Reset_Home (void)
{
	
		UN_STR *un;
	
		RA8875_SetBackLight(system_parameter_t.env.lcdlight*20+55);
		
		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		/* ��ʼ����ʾ������ */
		current_step_num = 1;
		g_cur_step = cs99xx_list.head;//ָ�����ļ�ͷ
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		switch(un->com.mode)
		{
			case ACW:
				interface_acw(&un->acw);
				break;
			case DCW:
				interface_dcw(&un->dcw);
				break;
			case IR:
				interface_ir(&un->ir);
				break;
			case GR:
				interface_gr(&un->gr);
				break;
			case LC:
				interface_lc(&un->lc);
				break;
			case PW:
				interface_pw(&un->pw);
				break;
			case ACW_GR:
				interface_acw_gr(&un->acw_gr);
				break;
			case DCW_GR:
				interface_dcw_gr(&un->dcw_gr);
				break;
		}
		rt_mb_send(&screen_mb, UPDATE_HOME);
		rt_mb_send(&screen_mb, UPDATE_TITLE);
		rt_mb_send(&screen_mb, UPDATE_MENU);
		rt_mb_send(&screen_mb, UPDATE_KEY);
		rt_mb_send(&screen_mb, UPDATE_STATUS);
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_TIME_EVENT);
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_KEYLOCK_EVENT | (system_parameter_t.key_lock));
		
		
		rt_mb_send(&screen_mb, UPDATE_STATUS | STATUS_INTERFACE_EVENT | (system_parameter_t.com.interface));
		ui_key_updata(0);
		
		Test_Sched_Param.Test_Status          = TEST_STATE_WAIT;                //����״̬Ϊ�ȴ�����
}

void  refresh_com(void)
{
		UN_STR *un;
			
		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		/* ��ʼ����ʾ������ */
		un = &rt_list_entry(g_cur_step, STEP_NODE, list)->un;
		switch(un->com.mode)
		{
			case ACW:
				interface_acw(&un->acw);
				break;
			case DCW:
				interface_dcw(&un->dcw);
				break;
			case IR:
				interface_ir(&un->ir);
				break;
			case GR:
				interface_gr(&un->gr);
				break;
			case LC:
				interface_lc(&un->lc);
				break;
			case PW:
				interface_pw(&un->pw);
				break;
			case ACW_GR:
				interface_acw_gr(&un->acw_gr);
				break;
			case DCW_GR:
				interface_dcw_gr(&un->dcw_gr);
				break;
		}
		rt_mb_send(&screen_mb, UPDATE_HOME);
}


/******************************************				END				***************************************************/
