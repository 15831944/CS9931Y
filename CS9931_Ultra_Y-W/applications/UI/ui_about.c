#include "CS99xx.h"
#include "bsp_listbox.h"
#include "CS9931_Config.h"
#include "sui_window.h"
#include "Communication.h"
#include  "CH376_USB_UI.H"
#include "bsp_ico.h"
extern const unsigned char gImage_logo[];

static u8 Config_File_Copy(void);


void ui_about_thread(void)
{
// 	u8	i;
	rt_uint32_t msg;
	struct font_info_t font={&panel_home,0xffff,0x0,1,0,16};

// __system_loop:
	if(panel_flag == 6)
	{
		clr_mem((u16 *)ExternSramHomeAddr,0x19f6,ExternSramHomeSize/2);
// 		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		/* ���� */
			/* �����ж� */
// 			if(language==1)
// 				font_draw(system_title_name.x,system_title_name.y,&font,system_title_name.data_en);
// 			else
// 				font_draw(system_title_name.x,system_title_name.y,&font,system_title_name.data_ch);
		
		ico_color_set(0,0xf800,0xffff);
		
	#ifdef CS9931YS_PT_2K
        ico_copy_bm(font.panel,20,20,(u8 *)elitech_logo_bm);
		font_draw(250,40,&font,CS9931_Config.Decive_Name[language]);
		font_draw(250,70,&font,T_STR("��Ȩ����@2014.PT.SINKO PRIMA ALLOY","Property in copyright@2014.PT.SINKO PRIMA ALLOY"));
		
		font_draw(10,100,&font,T_STR("Ӳ���汾��"HARD_VERISON,"HARD_VERISON:"HARD_VERISON)); 				
		font_draw(10,120,&font,T_STR("�������汾��"SOFT_VERISON,"SOFT_VERISON:"SOFT_VERISON));			
		font_draw(10,140,&font,T_STR("������Գ���汾��"TEST_SOFT_VERISON,"TEST_VERISON:"TEST_SOFT_VERISON));
		
		font_draw(42,160,&font,T_STR("�����Բ�����Ӳ�����������PT.SINKO PRIMA ALLOY���������н���Ȩ��","This is make by PT Company,All power of interpretation owned by"));
		
		font_draw(10,180,&font,T_STR("PT.SINKO PRIMA ALLOY���С��緢������BUG����ӭ�빫˾�����̻�������л����ʹ�á�","PT Company,If found BUG,Welcome back with the company."));
		
		font_draw(400,380,&font,T_STR("PT.SINKO PRIMA ALLOY","PT.SINKO PRIMA ALLOY"));
	#else
		ico_darw(font.panel,10,10,(u8 *)gImage_logo);
		font_draw(100,40,&font,CS9931_Config.Decive_Name[language]);
		
		font_draw(100,70,&font,T_STR("��Ȩ����@2014.�Ͼ���ʢ�������޹�˾����������Ȩ����","Property in copyright@2014.NANJING CHANGSHENG INSTRUMENT CO.,LTD"));
		
		font_draw(10,100,&font,T_STR("Ӳ���汾��","HARD_VERISON:")HARD_VERISON); 				
		font_draw(10,120,&font,T_STR("�������汾��","SOFT_VERISON:")SOFT_VERISON);			
		font_draw(10,140,&font,T_STR("������Գ���汾��","TEST_VERISON:")TEST_SOFT_VERISON);
	
		font_draw(42,160,&font,T_STR("�����Բ�����Ӳ������������Ͼ���ʢ�������޹�˾���������н���Ȩ��","This is make by CS Company,All power of interpretation owned by"));

		font_draw(10,180,&font,T_STR("�Ͼ���ʢ�������޹�˾���С��緢������BUG����ӭ�빫˾�����̻�������л����ʹ�á�","CS Company,If found BUG,Welcome back with the company."));
		
		font_draw(10,210,&font,T_STR("��ַ���Ͼ��н���������������ӥ·10#","Addr   : #10 Feiying Road, Jiangning Binjiang Economical Development Zone, Nanjing"));

		font_draw(10,230,&font,T_STR("�ʱࣺ211178","Mailbox: 211178"));
		font_draw(10,250,&font,T_STR("�绰����025��68132208 68132218 68132228","Call   :��025��68132208 68132218 68132228"));
		font_draw(10,270,&font,"E-mail : cswangyi@163.com");
		font_draw(10,290,&font,T_STR("��ַ:http://www.csallwin.com","Web    : http://www.csallwin.com"));
		font_draw(10,310,&font,T_STR("�̳ǣ�www.cs-shop.cn","Store  : www.cs-shop.cn"));
		
		
		font_draw(300,380,&font,T_STR("�Ͼ���ʢ�������޹�˾","NANJING CHANGSHENG INSTRUMENT CO.,LTD"));
		
	#endif
		
		rt_mb_send(&screen_mb, UPDATE_HOME);
		ui_key_updata(0);
	}
	while(panel_flag == 6)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				case KEY_F5 | KEY_UP:
					rt_uart_write(T_STR("����ok","Connect Ok "), 6);
					break;
				/* ���� */
				case KEY_F6 | KEY_UP:
					panel_flag = 0;
					break;

				case KEY_ENTER | KEY_UP:
					break;
				case KEY_F1 | KEY_LONG:
				{
					Config_File_Copy();
				}
				default:
					break;
			}
		}
		else
		{
#if 0
			u8 n,i,s;
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
				clr_win(font.panel,0,5,338,20,160);
				font_draw(10,340,&font,(const char*)scan_text_buf);
				rt_mb_send(&screen_mb, UPDATE_HOME);
				scan_text_flag =0;
				scan_text_w = 0;
			}
#endif
		}
	}
}


static u8 Config_File_Copy(void)
{
	rt_uint32_t msg;
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0xf000,0XE73C,1,0,16};
//	struct rect_type rects={25,100,20,250};
	rt_uint32_t i=0;
		win = sui_window_create(T_STR("������������","Reconfiguration the instrument"),&rect);

	font.panel = win;

	font_draw(94,44,&font,T_STR("ȷ��������"," Determine configuration ?"));
	font_draw(26,68,&font,T_STR("��ʾ:�������ļ���U���е��롣"," Tips:file from Udisk import"));
	
// 	clr_win(win,0XC618,25,100,20,250);

	clr_win(win,CL_orange,40,145,30,80);
	clr_win(win,CL_orange,180,145,30,80);
	
	font.fontcolor = 0X4208;
	font_draw(64,152,&font,T_STR("ȷ ��","OK"));
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
					
				
						u16		timeout = 500;
						char buf_path[40];
						uint16_t  read_num;
						PUINT8 File_Name = (PUINT8)buf_path;
						rt_sprintf(buf_path,"\\CONFIG.BIN");
						while(OPEN_FILE((char *)File_Name) != DEF_DISK_OPEN_FILE && timeout--)rt_thread_delay(10);
						if(timeout == 0){
								clr_win(win,0XE73C,2,32,165,295);
								font_draw(70,68,&font,T_STR("��ʶ���USB�豸��","Cannot be identified USB"));
								sui_window_update(win);
								rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
								return 1;
						}
						else {
							msg = CH376FileClose(TRUE);
							if(msg != USB_INT_SUCCESS){
								clr_win(win,0XE73C,2,32,165,295);
								font_draw(70,68,&font,T_STR("�ļ�����","File error"));
								sui_window_update(win);
								rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
								return 1;
							}
						}
						
						clr_win(win,0XE73C,2,32,165,295);
						font_draw(70,68,&font,T_STR("��ʼ����...","Start inport..."));
						
						sui_window_update(win);
						
						msg = CH376FileOpen(File_Name);  //���ļ�
						
						CH376ByteRead((u8 *)&CS9931_Config, sizeof(CS9931_Config), &read_num);    //����FLASH��Ϣ
						
						{
							//�ѽ���浽TF����
							 
							int fd;
						
							fd = open("//Config.bin",  O_WRONLY | O_CREAT, 0);
							
							write(fd,(uint8_t *)&CS9931_Config,sizeof(CS9931_Config));
							
							close(fd);
						}
						
					
					}
					clr_win(win,0XE73C,2,32,165,295);
					font_draw(70,68,&font,T_STR("������ɣ�","inport finish !"));
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

