#include "systick.h"

extern uint8_t receive_timeout_counter;

/*
    ModBus ͨ��Э���н���һ�����ݵļ��ʱ����3.5������ʱ�䣬
        ����9600�Ĳ��������Լ��ʱ��Ϊ4mS����
        �������5mSΪ��־��ÿ���5mS��ʾһ�����ݽ������

        ÿ�ν��յ�һ���ַ���������Ҫ��ModBus��ʱ������REFRESH����
        ��1mS�Ķ�ʱ���ж������N_REFRESH�ĵ���

*/
__IO static TIMER_STATUS ModBusTimerStatus;

void  Timer_ModBus(REFRESH_STATUS Refrash)
{
    static int8_t time_counter;    

    if (receive_timeout_counter > 1)   //δ���յ�����ʱ������
		receive_timeout_counter--;

    if(Refrash == N_REFRESH )  { 
    
        if(ModBusTimerStatus == TIMER_EMPTY) {
            return ;   
        }
           
        if(time_counter < 5){
            time_counter ++;
            ModBusTimerStatus = TIMER_CONTINUE;
        }
        else {
            ModBusTimerStatus = TIMER_OVER ;   //
// 			OSMutexPost (USART3Mutex);
// 			USART3Mutex_need_to_post = 1;
        }
    }
    else {
        time_counter = 0;
        ModBusTimerStatus = TIMER_CONTINUE;
    }
	
	
}

void set_ModBus_timer_status(TIMER_STATUS state)
{
    ModBusTimerStatus = state; 
}

TIMER_STATUS get_ModBus_timer_status(void)
{
    if(ModBusTimerStatus == TIMER_OVER) {
        ModBusTimerStatus = TIMER_EMPTY;

        return TIMER_OVER;
    }

    return ModBusTimerStatus;
}




void SysTick_Dispose(void)
{
    Timer_ModBus(N_REFRESH);  //ZWD414B��

}


