#include "CS99xx.h"
#include "bsp_listbox.h"

#include "sui_window.h"

char *help_title[2] = {"����ʹ�õİ�ȫ�淶","Instrument safety rules"};
char *help_list[2][8] = {
	{
		"1.��Ҫ����ȼ�Ļ�����ʹ�ò�����.", 
		"2.��Ҫ�ڸ��»�������ֱ������ĵط�ʹ�ò�����.",
		"3.��Ҫ�ڸ�ʪ�Ļ�����ʹ�ò�����.",
		"4.��Ҫ�ڶ�ҳ��Ļ�����ʹ�ò�����.",
		"5.��Ҫ��ͨ��ܲ�Ļ�����ʹ�ò�����.",
		"6.��Ҫ�Ѳ����Ƿ�����б�ı�������ڻζ��ĵط�ʹ�ò�����.",
		"7.��Ҫ�����еĲ����豸������豸��ʹ�ò�����.",
		"8.�����ǵ������Դ���е����Ŀ��ؿ���.",
	},
	{
		"1.Do not use tester in the flammable air.", 
		"2.Do not use tester in the high heat area.",
		"3.Do not use tester in the high humidity area.",
		"4.Do not use tester in the dusty environment.",
		"5.Do not use tester in the poor ventilated area.",
		"6.Do not let tester be on slant surfaces or use tester in shaking places.",
		"7.Do not use tester beside the sensitive equipment or receiving equipment.",
		"8.The input power of tester must have a separate switch control.",
	},
};


void ui_help_thread(void)
{
	u8	i;
	rt_uint32_t msg;
	struct font_info_t font={&panel_home,CL_YELLOW,0x0,1,0,24};
	char page[10],current_page=1,max_page=1;

	if(panel_flag == 5)
	{
		clr_mem((u16 *)ExternSramHomeAddr,0x19f6,ExternSramHomeSize/2);
// 		rt_memcpy((void *)ExternSramHomeAddr,(void *)ExternSramNullAddr,ExternSramHomeSize);
		/* ���� */
			font_draw((680-rt_strlen(help_title[language])*12)/2,10,&font,help_title[language]);
			rt_sprintf(page,"%02d/%02d",current_page,max_page);
			font.high = 16;
			font.fontcolor = 0xffff;
			font_draw(600,18,&font,page);
		
		/* ����ˮƽ�� */
			clr_win(&panel_home,0xf800,30,35,3,620);
		
		/* д���� */
			for(i=0;i<8;i++)
				font_draw(40,70+30*i,&font,help_list[language][i]);
		
		rt_mb_send(&screen_mb, UPDATE_HOME);
		ui_key_updata(0);
	}
	while(panel_flag == 5)
	{
		if (rt_mb_recv(&key_mb, (rt_uint32_t*)&msg, RT_TICK_PER_SECOND/100) == RT_EOK)
		{
			switch(msg)
			{
				/* ���� */
				case KEY_F6 | KEY_UP:
					panel_flag = 0;
					break;

				case KEY_ENTER | KEY_UP:
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
