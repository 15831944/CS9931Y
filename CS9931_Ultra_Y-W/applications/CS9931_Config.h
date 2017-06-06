#ifndef  __CS9931_CONFIG_H__
#define  __CS9931_CONFIG_H__

#define HARD_VERISON		"V1.00"			//Ӳ���汾
#define SOFT_VERISON		"V1.00.00"		//����汾
#define TEST_SOFT_VERISON	"V1.00.11-DZ"		//���Գ���汾



#define 	LC_WY		0	//LC��Դ
#define 	LC_YY		1	//LC��Դ


/* �����ͺŶ��� */
#define CS9975F_1 //CS9931YS_PT_2K //CS9931WYS // CS9931YS_1_40A //


#ifdef CS9975F_1
#define LC_TEST_MODE	LC_YY////LC_WY //
#define TYPE_NAME	{"CS9975F-1 �̿�ҽ��й©������", "CS9975F-1                    "}, "CS9975F-1"
#define _ACW_ENABLE		0
#define _DCW_ENABLE		0
#define _GR_ENABLE		0
#define _LC_ENABLE		1
#define _ACW_GR_ENABLE	0
#define _DCW_GR_ENABLE	0
#define _IR_ENABLE		0
#define _PW_ENABLE		0
#define	LC_MAX_VOL_250V // 	LC_MAX_VOL_300V // 	 ///<LC����Դ�������ѹֵ
#endif

#ifdef CS9931YS_1_40A
#define LC_TEST_MODE	LC_YY////LC_WY //
#define TYPE_NAME	{"CS9931YS-1-40A ҽ���ۺϰ��������", "CS9931YS-1-40A                    "}, "CS9931YS-1-40A"
#define _ACW_ENABLE		1
#define _DCW_ENABLE		1
#define _GR_ENABLE		1
#define _LC_ENABLE		1
#define _ACW_GR_ENABLE	0
#define _DCW_GR_ENABLE	0
#define _IR_ENABLE		0
#define _PW_ENABLE		0
#define LC_MAX_VOL_300V // LC_MAX_VOL_250V // 	 ///<LC����Դ�������ѹֵ
#endif

#ifdef CS9931WYS
#define LC_TEST_MODE	LC_WY //LC_YY//
#define TYPE_NAME	{"CS9931WYS ҽ���ۺϰ��������", "CS9931WYS                    "}, "CS9931WYS"
#define _ACW_ENABLE		1
#define _DCW_ENABLE		1
#define _GR_ENABLE		1
#define _LC_ENABLE		1
#define _ACW_GR_ENABLE	0
#define _DCW_GR_ENABLE	0
#define _IR_ENABLE		0
#define _PW_ENABLE		0
#define LC_MAX_VOL_300V // LC_MAX_VOL_250V // 	 ///<LC����Դ�������ѹֵ
#endif

#ifdef CS9931YS_PT_2K
#define LC_TEST_MODE	LC_WY //LC_YY//
#define TYPE_NAME	{"Elitech CS9931YS-PT-2K", "Elitech CS9931YS-PT-2K"}, "Elitech CS9931YS-PT-2K"
#define _ACW_ENABLE		1
#define _DCW_ENABLE		1
#define _GR_ENABLE		1
#define _LC_ENABLE		1
#define _ACW_GR_ENABLE	0
#define _DCW_GR_ENABLE	0
#define _IR_ENABLE		0
#define _PW_ENABLE		0
#define LC_MAX_VOL_300V // LC_MAX_VOL_250V // 	 ///<LC����Դ�������ѹֵ
#endif

#define MODE_ACW_ENABLE      (1<<7)
#define MODE_DCW_ENABLE      (1<<6)
#define MODE_IR_ENABLE       (1<<5)
#define MODE_GR_ENABLE       (1<<4)
#define MODE_LC_ENABLE       (1<<3)
#define MODE_ACW_GR_ENABLE   (1<<2)
#define MODE_DCW_GR_ENABLE   (1<<1)

typedef struct
{
	char Decive_Name[2][60];
	char Type_Name[60];
	char ACW_Enable;
	char DCW_Enable;
	char GR_Enable;
	char LC_Enable;
	char ACW_GR_Enable;
	char DCW_GR_Enable;
	char IR_Enable;
	char PW_Enable;

}CS9931_CONFIG_STRUCT;





extern CS9931_CONFIG_STRUCT CS9931_Config;






#endif
