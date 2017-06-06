/*
 * Copyright(c) 2014,
 * All rights reserved
 * �ļ����ƣ�RA8875.C
 * ժ  Ҫ  ��������
 * ��ǰ�汾��V1.0����д
 * �޸ļ�¼��
 */

/******************* �����ļ� *********************/
#include "RA8875.h"

/*
*********************************************************************************************************
*	�� �� ��: RA8875_Delaly1us
*	����˵��: �ӳٺ���, ��׼, ��Ҫ����RA8875 PLL����֮ǰ����ָ�����ӳ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_Delaly1us(void)
{
	uint8_t i;

	for (i = 0; i < 10; i++);	/* �ӳ�, ��׼ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_Delaly1ms
*	����˵��: �ӳٺ���.  ��Ҫ����RA8875 PLL����֮ǰ����ָ�����ӳ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_Delaly1ms(void)
{
	uint16_t i;

	for (i = 0; i < 5000; i++);	/* �ӳ�, ��׼ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteCmd
*	����˵��: дRA8875ָ��Ĵ���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_WriteCmd(uint8_t _ucRegAddr)
{
	RA8875_REG = _ucRegAddr;	/* ���üĴ�����ַ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteData
*	����˵��: дRA8875ָ��Ĵ���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_WriteData(uint8_t _ucRegValue)
{
	RA8875_RAM = _ucRegValue;	/* ���üĴ�����ַ */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_WriteReg
*	����˵��: дRA8875�Ĵ���. RA8875�ļĴ�����ַ�����ݶ���8bit��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_WriteReg(uint8_t _ucRegAddr, uint8_t _ucRegValue)
{
	RA8875_WriteCmd(_ucRegAddr);
	RA8875_WriteData(_ucRegValue);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetCursor
*	����˵��: ����д�Դ�Ĺ��λ�ã�ͼ��ģʽ��
*	��    ��:  _usX : X����; _usY: Y����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RA8875_SetCursor(uint16_t _usX, uint16_t _usY)
{
	/* �����ڴ�д�������� ��ע��0x80-83 �ǹ��ͼ�ε����꡿ */
	RA8875_WriteReg(0x46, _usX);
	RA8875_WriteReg(0x47, _usX >> 8);
	RA8875_WriteReg(0x48, _usY);
	RA8875_WriteReg(0x49, _usY >> 8);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetDispWin
*	����˵��: ������ʾ���ڣ����봰����ʾģʽ���ڴ�����ʾģʽ������д�Դ棬�����Զ����趨�����ڽ��е���
*	��    ��:
*		_usX : ˮƽ����
*		_usY : ��ֱ����
*		_usHeight: ���ڸ߶�
*		_usWidth : ���ڿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{

	uint16_t usTemp;

	/* ����ϵͳʾ��ͼ: ��������
			 -----------------------------
			|(0,0)                        |
			|     --------->              |
			|         |                   |
			|         |                   |
			|         |                   |
			|         V                   |
			|     --------->              |
			|                    (479,271)|
			 -----------------------------

		���Ͻ����������, ɨ�跽���ȴ����ң��ٴ��ϵ��¡�

		�����Ҫ��������ʽ������Ҫ��������������߼������ת��
	*/

	RA8875_WriteReg(0x30, _usX);
    RA8875_WriteReg(0x31, _usX >> 8);

	RA8875_WriteReg(0x32, _usY);
    RA8875_WriteReg(0x33, _usY >> 8);

	usTemp = _usWidth + _usX - 1;
	RA8875_WriteReg(0x34, usTemp);
    RA8875_WriteReg(0x35, usTemp >> 8);

	usTemp = _usHeight + _usY - 1;
	RA8875_WriteReg(0x36, usTemp);
    RA8875_WriteReg(0x37, usTemp >> 8);

	RA8875_SetCursor(_usX, _usY);

	/* ���浱ǰ������Ϣ������Ժ�ɫ��������Ч��.
	����һ��������ͨ����ȡ0x30-0x37�Ĵ�����õ�ǰ���ڣ�����Ч�ʽϵ� */
// 	s_WinX = _usX;
// 	s_WinY = _usY;
// 	s_WinHeight = _usHeight;
// 	s_WinWidth = _usWidth;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_ReadStatus
*	����˵��: ��RA8875״̬�Ĵ���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t RA8875_ReadStatus(void)
{
	uint8_t value;

	value = RA8875_REG;

	return value;
}

/*
*********************************************************************************************************
*	�� �� ��: BTE_SetTarBlock
*	����˵��: ����RA8875 BTEĿ�������Լ�Ŀ��ͼ��
*	��    ��:
*			uint16_t _usX : ˮƽ�������
*			uint16_t _usY : ��ֱ�������
*			uint16_t _usHeight : ����߶�
*			uint16_t _usWidth : ������
*			uint8_t _ucLayer : 0 ͼ��1�� 1 ͼ��2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void BTE_SetTarBlock(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t _ucLayer)
{
	/* ����������� */
	RA8875_WriteReg(0x58, _usX);
	RA8875_WriteReg(0x59, _usX >> 8);

	RA8875_WriteReg(0x5A, _usY);
	if (_ucLayer == 0)	/* ͼ��2 */
	{
		RA8875_WriteReg(0x5B, _usY >> 8);
	}
	else
	{
		RA8875_WriteReg(0x5B, (1 << 7) | (_usY >> 8));	/* Bit7 ��ʾͼ�㣬 0 ͼ��1�� 1 ͼ��2*/
	}

	/* ���������� */
	RA8875_WriteReg(0x5C, _usWidth);
	RA8875_WriteReg(0x5D, _usWidth >> 8);

	/* ��������߶� */
	RA8875_WriteReg(0x5E, _usHeight);
	RA8875_WriteReg(0x5F, _usHeight >> 8);
}

/*
*********************************************************************************************************
*	�� �� ��: BTE_SetOperateCode
*	����˵��: �趨BTE ������͹�դ������
*	��    ��: _ucOperate : ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void BTE_SetOperateCode(uint8_t _ucOperate)
{
	/*  �趨BTE ������͹�դ������  */
	RA8875_WriteReg(0x51, _ucOperate);
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetFrontColor
*	����˵��: �趨ǰ��ɫ
*	��    ��: ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetFrontColor(uint16_t _usColor)
{
	RA8875_WriteReg(0x63, (_usColor & 0xF800) >> 11);	/* R5  */
	RA8875_WriteReg(0x64, (_usColor & 0x07E0) >> 5);	/* G6 */
	RA8875_WriteReg(0x65, (_usColor & 0x001F));			/* B5 */
}

/*
*********************************************************************************************************
*	�� �� ��: BTE_Start
*	����˵��: ����BTE����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void BTE_Start(void)
{
// 	s_ucRA8875Busy = 1;
	/* RA8875_WriteReg(0x50, 0x80);  ����ʹ�������������Ϊ�ڲ��Ѿ������� s_ucRA8875Busy ��־ */
	RA8875_WriteCmd(0x50);	/* ���üĴ�����ַ */
	RA8875_WriteData(0x80);	/* д��Ĵ���ֵ */
}

/*
*********************************************************************************************************
*	�� �� ��: BTE_Wait
*	����˵��: �ȴ�BTE��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void BTE_Wait(void)
{
	while ((RA8875_ReadStatus() & 0x40) == 0x40);
// 	s_ucRA8875Busy = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_ClrScr
*	����˵��: �����������ɫֵ����.RA8875֧��Ӳ����ɫ��䡣�ú������Ե�ǰ�������ʾ���ڽ�������. ��ʾ
*			 ���ڵ�λ�úʹ�С�� RA8875_SetDispWin() ������������
*	��    ��:  _usColor : ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_ClrScr(uint16_t _usColor)
{
	/* Ҳ����ͨ��0x30-0x37�Ĵ�����û�õ�ǰ�������ʾ���� */

	/* ��ɫ��������, ����pdf ��162ҳ
	�˹���ʹ���ڽ�ѡ���ض�������������ǉ\�����ĳ��ǰ��ɫ��R8875 ����ĵ�ɫ�趨ΪBTE ǰ��ɫ��

	��������:
		1. �趨Ŀ��ͼ���λ�� REG[58h], [59h], [5Ah], [5Bh]
		2. �趨BTE ��Ⱥ͸߶� REG[5Ch], [5Dh], [5Eh], [5Fh]
		3. �趨BTE ������͹�դ������  REG[51h] Bit[3:0] = 0Ch
		4. �趨ǰ��ɫ  REG[63h], [64h], [65h]
		5. ����BTE ����  REG[50h] Bit7 = 1
		6. ���״̬������ STSR Bit6��ȷ��BTE �Ƿ����
	*/
	BTE_SetTarBlock(0, 0, 480, 800, 0);	/* ����BTEλ�úͿ�ȸ߶��Լ�Ŀ��ͼ�㣨0��1�� */
	BTE_SetOperateCode(0x0C);		/* �趨BTE ������͹�դ������  REG[51h] Bit[3:0] = 0Ch */
	RA8875_SetFrontColor(_usColor);	/* ����BTEǰ��ɫ */
	BTE_Start();					/* ����BTE ���� */
	BTE_Wait();						/* �ȴ��������� */
}

/*
*********************************************************************************************************
*	�� �� ��: RA8875_SetBackLight
*	����˵��: ����RA8875оƬ��PWM1��ؼĴ���������LCD����
*	��    ��:  _bright ���ȣ�0����255������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RA8875_SetBackLight(uint8_t _bright)
{
	if (_bright == 0)
	{
		/* �ر�PWM, PWM1����ȱʡ���LOW  */
		RA8875_WriteReg(0x8A, 0 << 6);
	}
	else if (_bright == 255)	/* ������� */
	{
		/* �ر�PWM, PWM1����ȱʡ���HIGH */
		RA8875_WriteReg(0x8A, 1 << 6);
	}
	else
	{
		/* ʹ��PWM1, ����ռ�ձȵ��� */

		/* 	REG[8Ah] PWM1 Control Register (P1CR)

			Bit7 ������ȵ��� (PWM1) �趨
				0 : �رգ���״̬�£�PWM1 ���׼λ���մ˻�����Bit6 ������
				1 : ������

			Bit6 PWM1 �ر�ʱ��׼λ
				0 : ��PWM �رջ���˯��ģʽʱ��PWM1 ���Ϊ��Low�� ״̬��
				1 : ��PWM �رջ���˯��ģʽʱ��PWM1 ���Ϊ��High�� ״̬��

			Bit5 ����

			Bit4 PWM1 ����ѡ��
				0 : PWM1 ���ܡ�
				1 : PWM1 �̶����һƵ��Ϊ�ⲿ��������Clock (Fin) Ƶ�� 1 /16 ��Clock

			Bit3-0  PWM1 ��·��Ƶ����Դѡ��PWM_CLK	������PWM���Ƶ�ʡ�
				0000b : SYS_CLK / 1   , 1000b : SYS_CLK / 256
				0001b : SYS_CLK / 2   , 1001b : SYS_CLK / 512
				0010b : SYS_CLK / 4   , 1010b : SYS_CLK / 1024
				0011b : SYS_CLK / 8   , 1011b : SYS_CLK / 2048
				0100b : SYS_CLK / 16  , 1100b : SYS_CLK / 4096
				0101b : SYS_CLK / 32  , 1101b : SYS_CLK / 8192
				0110b : SYS_CLK / 64  , 1110b : SYS_CLK / 16384
				0111b : SYS_CLK / 128 , 1111b : SYS_CLK / 32768

				��SYS_CLK�� ����ϵͳƵ�ʣ� ����SYS_CLK Ϊ20MHz�� ��Bit[3:0] =0001b ʱ��PWM1 Ƶ����ԴΪ10MHz��
				ʵ�������PWMƵ�ʻ���Ҫ���� 256��֧��8λ�ķֱ��ʡ�

				��������4.3��(480*272)ģ�飬SYS_CLK =  68.75MHz
				��������7.0��(800*480)ģ�飬SYS_CLK =  81.25MHz

				Ϊ�˱�����Ƶ������PWMƵ�ʿ���ѡ��
				��1�� ��Ƶ100Hz
				��2�� ���� 20KHz

				���磬Bit3-0Ϊ0011bʱ SYS_CLK / 8��
					4.3�� �����PWMƵ��Ϊ (68.75MHz / 8) / 256 = 33.56KHz
					7�� �����PWMƵ��Ϊ (81.25MHz / 8) / 256 = 39.67KHz
		*/

		// RA8875_WriteReg(0x8A, (1 << 7) | 3);   5���7������������ 3 ����ƵPWM, 4.3�粻��
		RA8875_WriteReg(0x8A, (1 << 7) | 12);

		/* REG[8Bh] PWM1 Duty Cycle Register (P1DCR) */
		RA8875_WriteReg(0x8B, _bright);
	}
}


void RA8875_InitHard(void)
{
	u32 t;
	/* �ȴ�Ӳ����λ */
	for(t=0;t<200;t++)
	{
		RA8875_Delaly1ms();
	}
	
	/* ��ʼ��PLL.  ����Ƶ��Ϊ25M */
	RA8875_WriteCmd(0x88);
	RA8875_Delaly1us();		/* �ӳ�1us */
	RA8875_WriteData(12);	/* PLLDIVM [7] = 0 ;  PLLDIVN [4:0] = 10 */

	RA8875_Delaly1ms();

	RA8875_WriteCmd(0x89);
	RA8875_Delaly1us();		/* �ӳ�1us */
	RA8875_WriteData(2);	/* PLLDIVK[2:0] = 2, ����4 */

	/* RA8875 ���ڲ�ϵͳƵ�� (SYS_CLK) �ǽ���񵴵�·��PLL ��·��������Ƶ�ʼ��㹫ʽ���� :
	  SYS_CLK = FIN * ( PLLDIVN [4:0] +1 ) / ((PLLDIVM+1 ) * ( 2^PLLDIVK [2:0] ))
			  = 25M * (12 + 1) / ((0 + 1) * (2 ^ 2))
			  = 81.25MHz
	*/

	/* REG[88h]��REG[89h]���趨��Ϊ��֤PLL ����ȶ�����ȴ�һ�Ρ���Ƶʱ�䡹(< 100us)��*/
	RA8875_Delaly1ms();

	/*
		����ϵͳ���ƼĴ����� ����pdf ��18ҳ:

		bit3:2 ɫ������趨 (Color Depth Setting)
			00b : 8-bpp ��ͨ��TFT �ӿڣ� i.e. 256 ɫ��
			1xb : 16-bpp ��ͨ��TFT �ӿڣ� i.e. 65K ɫ��	 ��ѡ�����

		bit1:0 MCUIF ѡ��
			00b : 8-λMCU �ӿڡ�
			1xb : 16-λMCU �ӿڡ� ��ѡ�����
	*/
	RA8875_WriteReg(0x10, (1 <<3 ) | (1 << 1));	/* ����16λMCU���ڣ�65Kɫ */

	/* REG[04h] Pixel Clock Setting Register (PCSR)
		bit7  PCLK Inversion
			0 : PDAT ����PCLK ��Ե���� (Rising Edge) ʱ��ȡ����
			1 : PDAT ����PCLK ��Ե�½� (Falling Edge) ʱ��ȡ����
		bit1:0 PCLK Ƶ�������趨
			Pixel Clock ,PCLK Ƶ�������趨��
			00b: PCLK Ƶ������= ϵͳƵ�����ڡ�
			01b: PCLK Ƶ������= 2 ����ϵͳƵ�����ڡ�
			10b: PCLK Ƶ������= 4 ����ϵͳƵ�����ڡ�
			11b: PCLK Ƶ������= 8 ����ϵͳƵ�����ڡ�
	*/
	RA8875_WriteReg(0x04, 0x81);
	RA8875_Delaly1ms();

#if 1
	/* OTD9960 & OTA7001 ���� */
	RA8875_WriteReg(0x14, 0x63);
	RA8875_WriteReg(0x15, 0x00);
	RA8875_WriteReg(0x16, 0x03);
	RA8875_WriteReg(0x17, 0x03);
	RA8875_WriteReg(0x18, 0x0B);
	RA8875_WriteReg(0x19, 0xDF);
	RA8875_WriteReg(0x1A, 0x01);
	RA8875_WriteReg(0x1B, 0x1F);
	RA8875_WriteReg(0x1C, 0x00);
	RA8875_WriteReg(0x1D, 0x16);
	RA8875_WriteReg(0x1E, 0x00);
	RA8875_WriteReg(0x1F, 0x01);

	RA8875_WriteReg(0x20, 0x08);
#else	/* AT070TN92  setting */
	//Horizontal set
	//HDWR//Horizontal Display Width Setting Bit[6:0]
	//Horizontal display width(pixels) = (HDWR + 1)*8
	RA8875_WriteReg(0x14, 0x4F);
	RA8875_WriteReg(0x15, 0x05);

	//HNDR//Horizontal Non-Display Period Bit[4:0]
	//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8
	RA8875_WriteReg(0x16, 0x0F);

	//HSTR//HSYNC Start Position[4:0]
	//HSYNC Start Position(PCLK) = (HSTR + 1)*8
	RA8875_WriteReg(0x17, 0x01);

	//HPWR//HSYNC Polarity ,The period width of HSYNC.
	//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8
	RA8875_WriteReg(0x18, 0x00);

	//Vertical set
	//VDHR0 //Vertical Display Height Bit [7:0]
	//Vertical pixels = VDHR + 1
	RA8875_WriteReg(0x19, 0xDF);

	//VDHR1 //Vertical Display Height Bit [8]
	//Vertical pixels = VDHR + 1
	RA8875_WriteReg(0x1A, 0x01);

	//VNDR0 //Vertical Non-Display Period Bit [7:0]
	//Vertical Non-Display area = (VNDR + 1)
	RA8875_WriteReg(0x1B, 0x0A);

	//VNDR1 //Vertical Non-Display Period Bit [8]
	//Vertical Non-Display area = (VNDR + 1)
	RA8875_WriteReg(0x1C, 0x00);

	//VSTR0 //VSYNC Start Position[7:0]
	//VSYNC Start Position(PCLK) = (VSTR + 1)
	RA8875_WriteReg(0x1D, 0x0E);

	//VSTR1 //VSYNC Start Position[8]
	//VSYNC Start Position(PCLK) = (VSTR + 1)
	RA8875_WriteReg(0x1E, 0x00);

	//VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]
	//VSYNC Pulse Width(PCLK) = (VPWR + 1)
	RA8875_WriteReg(0x1F, 0x01);
#endif


	/* ����TFT���� DISP  ����Ϊ�ߣ�ʹ�����. ������TFTģ���DISP�������ӵ�RA8875оƬ��GP0X�� */
	RA8875_WriteReg(0xC7, 0x01);	/* DISP = 1 */

	/* LCD ��ʾ/�ر�Ѷ�� (LCD Display on) */
	RA8875_WriteReg(0x01, 0x80);

	/* 	REG[40h] Memory Write Control Register 0 (MWCR0)

		Bit 7	��ʾģʽ�趨
			0 : ��ͼģʽ��
			1 : ����ģʽ��

		Bit 6	����д����/�ڴ�д�����趨
			0 : �趨����/�ڴ�д����Ϊ����ʾ��
			1 : �趨����/�ڴ�д����Ϊ��ʾ��

		Bit 5	����д����/�ڴ�д������˸�趨
			0 : �α겻��˸��
			1 : �α���˸��

		Bit 4   NA

		Bit 3-2  ��ͼģʽʱ���ڴ�д�뷽��
			00b : �� -> �ң�Ȼ���� -> �¡�
			01b : �� -> ��Ȼ���� -> �¡�
			10b : �� -> �£�Ȼ���� -> �ҡ�
			11b : �� -> �ϣ�Ȼ���� -> �ҡ�

		Bit 1 	�ڴ�д�����Զ����ӹ����趨
			0 : ���ڴ�д��ʱ���λ���Զ���һ��
			1 : ���ڴ�д��ʱ���λ�ò����Զ���һ��

		Bit 0 �ڴ��ȡ����Զ����ӹ����趨
			0 : ���ڴ��ȡʱ���λ���Զ���һ��
			1 : ���ڴ��ȡʱ���λ�ò����Զ���һ��
	*/
	RA8875_WriteReg(0x40, 0x00);	/* ѡ���ͼģʽ */


	/* 	REG[41h] Memory Write Control Register1 (MWCR1)
		д��Ŀ��λ�ã�ѡ��ͼ��1
	*/
	RA8875_WriteReg(0x41, 0x00);	/* ѡ���ͼģʽ, Ŀ��ΪCGRAM */

	RA8875_SetDispWin(0, 0, 480, 800);

	RA8875_ClrScr(0);	/* ��������ʾȫ�� */
	
	RA8875_SetBackLight(255);	 /* �򿪱��⣬����Ϊȱʡ���� */
	
}


void RA8875_DrawICO(u16 x,u16 y,u16 c,u16 bc,const unsigned char* data)
{
	rt_uint16_t i,j,chr;
	rt_uint16_t h=*(rt_uint16_t *)data,w=*((rt_uint16_t *)data+1);
	rt_uint8_t *p=(rt_uint8_t *)data+4,word_bytes = (w + 7) / 8;
    
	/* ����ͼƬ��λ�úʹ�С�� ��������ʾ���� */
	RA8875_SetDispWin(x, y, h, w);
	RA8875_WriteCmd(0x02); 		/* ׼����д�Դ� */
	for (i = 0; i < h; i++)
    {
        rt_uint8_t *ptr = p + i * word_bytes;
        for (j = 0; j < w; j++)
        {
            if (j % 8 == 0)
                chr = *ptr++;
            if (chr & 0x80)
				RA8875_RAM = c;
            else
				RA8875_RAM = bc;
            chr <<= 1;
        }
    }
}


/*
*********************************************************************************************************
*	�� �� ��: RA8875_DrawBMP
*	����˵��: ��LCD����ʾһ��BMPλͼ��λͼ����ɨ�����:�����ң����ϵ���
*	��    ��:
*			_usX, _usY : ͼƬ������
*			_usHeight  :ͼƬ�߶�
*			_usWidth   :ͼƬ���
*			_ptr       :ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
u16 bmp_buf[800];
void RA8875_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
#if 0
	uint32_t index = 0;
#endif
	const uint16_t *p;
	
	rt_enter_critical();
	
	/* ����ͼƬ��λ�úʹ�С�� ��������ʾ���� */
	RA8875_SetDispWin(_usX, _usY, _usHeight, _usWidth);

// 	s_ucRA8875Busy = 1;

	RA8875_WriteCmd(0x02); 		/* ׼����д�Դ� */

	p = _ptr;
#if 1
	{
		u16 x,y;
		u16 *pbuf;
		for(y=0;y<_usHeight;y++)
		{
			pbuf=bmp_buf;
			for(x=0;x<_usWidth;x++)
				*pbuf++ = (*p++);
			pbuf=bmp_buf;
			for(x=0;x<_usWidth;x++)
				RA8875_RAM = (*pbuf++);
		}
				
	}
#else
	for (index = 0; index < _usHeight * _usWidth; index++)
	{
		/*
			armfly : �����Ż�, �����͵�չ��
			RA8875_WriteRAM(_ptr[index]);

			�˴��ɿ�����DMA����
		*/
		RA8875_RAM = (*p++);
	}
#endif
	
	rt_exit_critical();
// 	s_ucRA8875Busy = 0;

	/* �˳����ڻ�ͼģʽ */
// 	RA8875_QuitWinMode();
}




void window_updata(struct panel_type *parent,struct rect_type *rect)
{
	uint16_t i,j,offset;
	uint16_t *p,*pt;
		
	p = parent->data + rect->y*parent->w + rect->x;
	offset = parent->w - rect->w;
	
	rt_enter_critical();
	
	/* ����ͼƬ��λ�úʹ�С�� ��������ʾ���� */
	RA8875_SetDispWin(parent->x+rect->x, parent->y+rect->y, rect->h, rect->w);
	RA8875_WriteCmd(0x02); 		/* ׼����д�Դ� */
	
	for(i=0;i<rect->h;i++)
	{
		pt = bmp_buf;
		for(j=0;j<rect->w;j++)
			*pt++ = *p++;
		pt = bmp_buf;
		for(j=0;j<rect->w;j++)
			RA8875_RAM = (*pt++);
		p += offset;
	}
	
	rt_exit_critical();
}


