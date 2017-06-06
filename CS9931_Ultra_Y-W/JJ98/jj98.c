
#include "jj98.h"
// #include "Usart1.h"
#include "Communication.h"

jj98_FRAME send_jj98_frame;//����֡
jj98_FRAME rece_jj98_frame;//����֡

typedef struct{
    uint16_t data;//����
    uint8_t available;//���ݿ���ʹ�ñ��
}DATA_INF;

DATA_INF vol;//��ѹ

/* ���ñ�Ƶ��Դ�������ѹ */
void jj98_set_vol(uint16_t vol, uint8_t decs)
{
	send_jj98_frame.direction = DIR_MASTER_W;
	send_jj98_frame.val.value = vol;
	send_jj98_frame.ctrl.bits.rw = CTRL_MASTER_WRITE;
	send_jj98_frame.ctrl.bits.vol  = 1;
	send_jj98_frame.ctrl.bits.freq  = 0;
	send_jj98_frame.ctrl.bits.cur  = 0;
	send_jj98_frame.ctrl.bits.pw  = 0;
	send_jj98_frame.ctrl.bits.decs_num  = decs;
	jj98_stop_test();//ʹ��stop�����ò���
}
/* ��ȡ��Ƶ��Դ�ĵ�ѹֵ */
void jj98_get_vol(uint8_t decs)
{
	send_jj98_frame.direction = DIR_MASTER_R;
	send_jj98_frame.val.value = 0;
	send_jj98_frame.ctrl.bits.rw = CTRL_MASTER_READ;
	send_jj98_frame.ctrl.bits.vol  = 1;
	send_jj98_frame.ctrl.bits.freq  = 0;
	send_jj98_frame.ctrl.bits.cur  = 0;
	send_jj98_frame.ctrl.bits.pw  = 0;
	send_jj98_frame.ctrl.bits.decs_num  = decs;
	jj98_start_test();//ʹ��start�����ò���
}
/* ����Ƶ�� */
void jj98_set_freq(uint16_t freq, uint8_t decs)
{
	send_jj98_frame.direction = DIR_MASTER_W;
	send_jj98_frame.val.value = freq;
	send_jj98_frame.ctrl.bits.rw = CTRL_MASTER_WRITE;
	send_jj98_frame.ctrl.bits.vol  = 0;
	send_jj98_frame.ctrl.bits.freq  = 1;
	send_jj98_frame.ctrl.bits.decs_num  = decs;
	jj98_stop_test();//ʹ��stop�����ò���
}
/* �������� */
void jj98_start_test(void)
{
	send_jj98_frame.ctrl.bits.ctrl = CTRL_SLAVE_START;
	jj98_send_frame_to_slove();
}
/* ֹͣ���� */
void jj98_stop_test(void)
{
	send_jj98_frame.ctrl.bits.ctrl = CTRL_SLAVE_STOP;
	jj98_send_frame_to_slove();
}
static void jj98_Delay_ms(unsigned int dly_ms)
{
  unsigned int dly_i;
  while(dly_ms--)
    for(dly_i=0;dly_i<18714;dly_i++);
}
/* ���Ƶ��Դ�������� */
void jj98_send_frame_to_slove(void)
{
	uint32_t time_out = 0xfffff;
	
	if(get_usart2_busy_st() == 1)
	{
		return;
	}
	
	set_usart2_busy_st();
	
	rt_uart_write(&send_jj98_frame, sizeof(send_jj98_frame));
	
	/* �ȴ�������� */
	while(1)
	{		
		if(get_usart2_send_st() || --time_out == 0)
		{
			break;
		}
	}
	
	reset_usart2_busy_st();
	
	jj98_Delay_ms(500);
}


void jj98_comm_analysis(uint8_t data)
{
    static uint32_t data_count;
    static uint8_t f_buf[10];
    jj98_FRAME *frame = (void*)f_buf;
    
    if(data == 0x18)
    {
        data_count = 0;
        memset(f_buf, 0, sizeof(f_buf));
    }
    
    f_buf[data_count++] = data;
    
    if(data_count == 4)
    {
        vol.data = frame->val.value;
        vol.available = 1;
        data_count = 0;
    }
}



