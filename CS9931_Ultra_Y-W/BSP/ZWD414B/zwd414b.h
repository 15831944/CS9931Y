#ifndef __ZWD414B_H
#define __ZWD414B_H

#include "stdint.h"
#include "zwd414b_tools.h"
#include "systick.h"

typedef enum {
    CMD_TEST = 0x03,    
    CMD_SET = 0x10,
}ZWD414B_CMD;

typedef enum {
    VOLTAGE = 0X1000,           //��ѹ
    CURRENT = 0X1002,           //����
    POWER   = 0X1004,           //����
    PF  = 0X1006,               //��������
    FREQUENCY = 0X1008,         //Ƶ��
    POWER_NOT_WORK = 0X100A,      //�޹�����
    POWER_WORK = 0X100E,        //�й�����

    GRAND_TIME  = 0X1010,       //�ۼ�ʱ��

    VOLTAGE_MULTIPLE = 0X1F00,  //��ѹ����
    CURRENT_MULTIPLE = 0X1F02,  //��������

    COM_ADDRESS = 0X1F51,      //ͨ�ŵ�ַ
    BAUD_RATE = 0X1F52,         //������

    ENERGY = 0X1F55,            //����
}MEMORY_ADDRESS;                //�Ĵ�����ַ

typedef enum {
    START_ENERGY = 1,
    STOP_ENERGY,
    CLEAR_ENERGY,
}ENERGY_CMD;


#define     TEST_CMD         MEMORY_ADDRESS

typedef enum {
    T, 
    F,       
}NEW_DATA;

typedef struct {
    NEW_DATA read_flag; 
    float data;
}Float_data;

typedef struct {
    NEW_DATA read_flag; 
    uint16_t data;
}Uint16_data;

typedef struct {
    NEW_DATA  read;
    Float_data Voltage;
    Float_data Current;
    Float_data Power;
    Float_data PF;
    Float_data Frequency;
    Float_data Power_not_work;
    Float_data Power_work;
    Float_data Grand_time;
    Float_data Voltage_multiple;
    Float_data Current_multiple;
    
    Uint16_data Com_address;
    Uint16_data Buad_rate;
    Uint16_data Energy;
}Zwd414b_Receive_Inf;


extern Zwd414b_Receive_Inf Zwd414b_Receive_data;
extern void Zwd414b_Init(void);

extern uint8_t this_target_address;

extern void Zwd414b_Set_Energy(ENERGY_CMD energy_cmd);
extern void Receive_data_Dispose(void);

extern void Zwd414b_Test(TEST_CMD test_cmd[], uint8_t test_num);

extern void Zwd414b_Send_data_Dispose(void);

extern void Zwd414b_Set_Int16(TEST_CMD cmd ,uint16_t set_data);

extern uint16_t Get_ZWD414B (uint16_t ZWD414Item);


#endif
