#include "CS99xx.h"
#include "memory.h"
#include "bsp_ico.h"
#include "memorymanagement.h"
#include "bsp_listbox.h"
#include "sui_window.h"

extern rt_int32_t read_flash_to_step(rt_uint8_t n, rt_uint8_t step, UN_STR *pun);
extern rt_int16_t save_step_to_flash(rt_uint8_t n, rt_uint8_t step, UN_STR *pun);
//#define   Test_File_Name 		("\\PROGRAM.BIN")
//#define   Test_File_TITLE		" "
#include  "CH376_USB_UI.H"
static u8 file_export(void *arg)
{
	rt_uint32_t msg;
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0xf000,0XE73C,1,0,16};
	struct rect_type rects={25,100,20,250};
	rt_uint32_t i=0;
	rt_uint8_t *p_data;
		win = sui_window_create(T_STR("�ļ�����","File Export"),&rect);

	font.panel = win;

	font_draw(94,44,&font,T_STR("ȷ��������","Confirm Export ?"));
	font_draw(26,68,&font,T_STR("��ʾ:���ļ�������USB�洢�豸�С�","Tips:exported file into the USB"));

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
						float   sn=0,n=0;
						u32		__file_len= 0;
						u16		timeout = 500;
						char buf_path[40];
						PUINT8 File_Name = (PUINT8)buf_path;
						rt_sprintf(buf_path,"\\FILE%03d.BIN",0);
						while(CH376FileCreate(File_Name) != USB_INT_SUCCESS && timeout--)rt_thread_delay(10);
						if(timeout == 0){
							clr_win(win,0XE73C,2,32,165,295);
						
							font_draw(70,68,&font,T_STR("��ʶ���USB�豸��","Cannot be identified USB"));
							sui_window_update(win);
							rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
							return 1;
						}
						else 
						{
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
						clr_win(win,0XC618,25,100,20,250);
						font_draw(70,68,&font,T_STR("��ʼ����...","Start export..."));
						
						sui_window_update(win);
						
						msg = CH376FileOpen(File_Name);  //���ļ�
						
						
						
						msg = CH376ByteWrite((u8 *)&flash_info,sizeof(flash_info),NULL);  //д��FLASH��Ϣ
						
//						fd = open(("/system32/temp/file_bk.bin"), O_CREAT | O_WRONLY , 0);
//						write(fd, (u8 *)&flash_info, sizeof(flash_info));
//						close(fd);
						
						__file_len += sizeof(flash_info);
						msg = CH376FileClose(TRUE);
						rt_thread_delay(50);
						
						sn = 250.0 / (FILE_NUM * 2);
						n=0;
						p_data = rt_malloc(FILE_OFFSET);
						FLASH_CS_SET(1);	// ѡ�����flash
						for(i=0;i<FILE_NUM;i++)
						{
							
							sf_ReadBuffer((uint8_t*)p_data,                                                    \
														FILE_BASE + (i/PER_P_FILES)*4096 + FILE_OFFSET*(i%PER_P_FILES),      \
														FILE_OFFSET);
							
//							msg = CH376FileOpen(File_Name);  //���ļ�
//							msg = CH376ByteLocate(__file_len);
//							msg = CH376ByteWrite((u8 *)p_data,FILE_OFFSET,NULL);  //д���ļ���Ϣ
//							msg = CH376FileClose(TRUE);
							while(CH376FileOpen(File_Name) != 0x14);  //���ļ�
							while(CH376ByteLocate(__file_len) != 0x14);
							while(CH376ByteWrite((u8 *)p_data,FILE_OFFSET,NULL) != 0x14);  //д���ļ���Ϣ
							while(CH376FileClose(TRUE) != 0x14);
							__file_len += FILE_OFFSET;
							
							
//							fd = open(("/system32/temp/file_bk.bin"), O_CREAT | O_WRONLY , 0);
//							lseek(fd,__file_len,0);
//							write(fd, (u8 *)&p_data, FILE_OFFSET);
//							close(fd);
							
							n+=sn;
							clr_win(win,CL_BLUE,25,100,20,n);
							window_updata(win,&rects);
						}
						rt_free(p_data);
						p_data = rt_malloc(STEP_OFFSET);
						for(i=0;i<FILE_NUM;i++)
						{
							rt_uint32_t j;
							
							for(j=0;j<TOTALSTEP_NUM;j++)
							{
//								p = j % PER_P_STEPS;
//								sf_ReadBuffer((rt_uint8_t*)(p_data),
//															GROUP_BASE + i*GROUP_OFFSET + p*F_PAGE_SIZE + STEP_OFFSET*(j / PER_P_STEPS),
//															STEP_OFFSET);
//								msg = CH376FileOpen(File_Name);  //���ļ�
//								msg = CH376ByteLocate(__file_len);
//								msg = CH376ByteWrite((u8 *)p_data,STEP_OFFSET,NULL);  //д���ļ���Ϣ
//								msg = CH376FileClose(TRUE);
								
								read_flash_to_step(i,j,(UN_STR *)p_data);
								
								while(CH376FileOpen(File_Name) != 0x14);  //���ļ�
								while(CH376ByteLocate(__file_len) != 0x14);
								while(CH376ByteWrite((u8 *)p_data,STEP_OFFSET,NULL) != 0x14);  //д���ļ���Ϣ
								while(CH376FileClose(TRUE) != 0x14);
//								fd = open(("/system32/temp/file_bk.bin"), O_CREAT | O_WRONLY , 0);
//								lseek(fd,__file_len,0);
//								write(fd, (u8 *)&p_data, STEP_OFFSET);
//								close(fd);
								
								__file_len += STEP_OFFSET;
								
								n += sn / TOTALSTEP_NUM;
								clr_win(win,CL_BLUE,25,100,20,n);
								window_updata(win,&rects);
							}
							
							
							
						}
						rt_free(p_data);
						
						msg = CH376FileClose(TRUE);      //�ر��ļ�
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

extern void refresh_file_list_box(void);

static u8 file_import(void *arg)
{
	rt_uint32_t msg;
	struct panel_type *win;
	struct rect_type rect={190,140,200,300};
	struct font_info_t font={0,0xf000,0XE73C,1,0,16};
	struct rect_type rects={25,100,20,250};
	rt_uint32_t i=0;
	rt_uint8_t *p_data;
	
	win = sui_window_create(T_STR("�ļ�����","File inport"),&rect);
	
	font.panel = win;
	
	font_draw(94,44,&font,T_STR("ȷ��������","Confirm inport ?"));
	font_draw(26,68,&font,T_STR("��ʾ:��USB�洢�豸���뵽�ļ��С�","Tips:exported USB into the file"));
	
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

						float   sn=0,n=0;
						u32		__file_len= 0;
						u16		timeout = 500;
						char buf_path[40];
						uint16_t  read_num;
						PUINT8 File_Name = (PUINT8)buf_path;
						rt_sprintf(buf_path,"\\FILE%03d.BIN",0);
						while(OPEN_FILE((char *)File_Name) != DEF_DISK_OPEN_FILE && timeout--)rt_thread_delay(10);
						if(timeout == 0){
								clr_win(win,0XE73C,2,32,165,295);
								font_draw(70,68,&font,T_STR("��ʶ���USB�豸��","Cannot be identified USB"));
								sui_window_update(win);
								rt_thread_delay(500);  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
								return 1;
						}
						else 
						{
	
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
						clr_win(win,0XC618,25,100,20,250);
						font_draw(70,68,&font,T_STR("��ʼ����...","Start inport..."));
						
						sui_window_update(win);
						
						msg = CH376FileOpen(File_Name);  //���ļ�
						
						
						
//						msg = CH376ByteWrite((u8 *)&flash_info,sizeof(flash_info),NULL);  //д��FLASH��Ϣ
						CH376ByteRead((u8 *)&flash_info, sizeof(flash_info), &read_num);    //����FLASH��Ϣ
						sf_WriteBuffer((uint8_t *)&flash_info,FLASH_BKP_ADDR,sizeof(flash_info));

						
						__file_len += sizeof(flash_info);
						msg = CH376FileClose(TRUE);
						rt_thread_delay(50);
						
						sn = 250.0 / (FILE_NUM * 2);
						n=0;
						p_data = rt_malloc(FILE_OFFSET);
						FLASH_CS_SET(1);	// ѡ�����flash
						for(i=0;i<FILE_NUM;i++)
						{
							
							
						
							while(CH376FileOpen(File_Name) != 0x14);  //���ļ�
							while(CH376ByteLocate(__file_len) != 0x14);
							while(CH376ByteRead((u8 *)p_data,FILE_OFFSET,&read_num) != 0x14);  //д���ļ���Ϣ
							while(CH376FileClose(TRUE) != 0x14);
							__file_len += FILE_OFFSET;
							
							sf_WriteBuffer((uint8_t*)p_data,                                                    \
														 FILE_BASE + (i/PER_P_FILES)*4096 + FILE_OFFSET*(i%PER_P_FILES),      \
														 FILE_OFFSET);
//							rt_memcpy(&file_info[i],(uint8_t*)p_data,FILE_OFFSET);
							
							n+=sn;
							clr_win(win,CL_BLUE,25,100,20,n);
							window_updata(win,&rects);
						}
						rt_free(p_data);
						p_data = rt_malloc(STEP_OFFSET);
						for(i=0;i<FILE_NUM;i++)
						{
							rt_uint32_t j;
							
							for(j=0;j<TOTALSTEP_NUM;j++)
							{
								
								
								while(CH376FileOpen(File_Name) != 0x14);  //���ļ�
								while(CH376ByteLocate(__file_len) != 0x14);
								while(CH376ByteRead((u8 *)p_data,STEP_OFFSET,&read_num) != 0x14);  //д���ļ���Ϣ
								while(CH376FileClose(TRUE) != 0x14);	
								__file_len += STEP_OFFSET;
								
//								p = j % PER_P_STEPS;
//								sf_WriteBuffer((rt_uint8_t*)(p_data),
//															 GROUP_BASE + i*GROUP_OFFSET + p*F_PAGE_SIZE + STEP_OFFSET*(j / PER_P_STEPS),
//															 STEP_OFFSET);
								save_step_to_flash(i,j,(UN_STR *)p_data);
								
								n += sn / TOTALSTEP_NUM;
								clr_win(win,CL_BLUE,25,100,20,n);
								window_updata(win,&rects);
							}
							
							
							
						}
						rt_free(p_data);
						
						msg = CH376FileClose(TRUE);      //�ر��ļ�
					}
					
					init_list();
					read_flash_to_files();
					read_flash_to_list(flash_info.current_file);
					
					clr_win(win,0XE73C,70,68,20,100);
					font_draw(70,68,&font,T_STR("������ɣ�","Inport finish !"));
					sui_window_update(win);
					rt_thread_delay(200);
					refresh_file_list_box();
					
					return 0;
				/* ���� */
					case KEY_EXIT | KEY_UP:
					case KEY_F6 | KEY_UP:
					return 0;
			}
		}
	}
	
}

void ui_file_port_thread(void)
{
	rt_uint8_t i,exporten = 0;
	rt_uint32_t msg;
	
	for(i=1;i<FILE_NUM;i++)
	{
		if(file_info[i].en != 0)
		{
			exporten = 1;
			break;
		}
		
	}
	exporten = 1;
	if(panel_flag == 7)
	{

	
		if(exporten == 0)
			ui_key_updata(0x78);
		else
			ui_key_updata(0x38);
	}	
	while(panel_flag == 7)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				/* ���� */
				case KEY_F1 | KEY_UP:
					file_import(0);
					rt_mb_send(&screen_mb, UPDATE_HOME);
					break;
				/* ���� */
				case KEY_F2 | KEY_UP:
					file_export(0);
					rt_mb_send(&screen_mb, UPDATE_HOME);
					break;
				/* ��Ч */
				case KEY_F3 | KEY_UP:
					
					break;
				/* ��Ч */
				case KEY_F4 | KEY_UP:
					
					break;
				/* ��Ч */
				case KEY_F5 | KEY_UP:
					
					break;
				/* ���� */
				case KEY_F6 | KEY_UP:
					panel_flag = 0;
					break;
				case CODE_RIGHT:
				case KEY_U | KEY_DOWN:
					
					break;
				case CODE_LEFT:
				case KEY_D | KEY_DOWN:
					
					break;
				case KEY_L | KEY_DOWN:
					
					break;
				case KEY_R | KEY_DOWN:
					
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


