#include "Relay.h"
#include "Relay_Change.h"

#if defined(__cplusplus)

    extern "C" {     /* Make sure we have C-declarations in C++ programs */

#endif

/**************************************************************************
 *                           ���������ؼ��ֺ궨�� -- ���������
***************************************************************************/ 

#define     INLINE                                   __inline 

//�̵���״̬���ú���
/*******************************
��������  Relay_State_CHange
��  ����  state����ǰ�Ĺ���״̬
����ֵ��  ��
********************************/
static uint16_t Sampling_Relay_State = 0;
void Sampling_Relay_State_CHange(uint8_t state)
{
	Relay_OFF(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3 | W_IR_FILE4 | W_IR_FILE5 | W_IR_FILE6);
	Sampling_Relay_State = state;
	switch (state){
		case DC_100mA:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3 | W_IR_FILE4 | W_IR_FILE5 | W_IR_FILE6);
		break;
		case DC_20mA:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3 | W_IR_FILE4 | W_IR_FILE5);
		break;
		case DC_2mA:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3 | W_IR_FILE4);
		break;
		case DC_200uA:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3);
		break;
		case DC_20uA:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2);
		break;
		case DC_2uA:
			Relay_ON(W_IR_FILE1);
		break;
		case IR_1M_10M:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3 | W_IR_FILE4);
		break;
		case IR_10M_100M:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2 | W_IR_FILE3);
		break;
		case IR_100M_1000M:
			Relay_ON(W_IR_FILE1 | W_IR_FILE2);
		break;
		case IR_1G_10G:
			Relay_ON(W_IR_FILE1);
		break;
		case IR_10G_100G:
			
		break;
		default:
			
		break;
	}
}

uint16_t Sampling_Relay_State_Get(void)
{
	return Sampling_Relay_State;
}




