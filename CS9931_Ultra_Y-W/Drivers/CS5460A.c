
#include "CS5460A.h"

#define SPI_CLK_HIGH()		
#define SPI_CLK_LOW()		
#define SPI_MOSI_HIGH()		
#define SPI_MOSI_LOW()		
#define SPI_MISO_ST()		
#define SPI_CS_EN()		
#define SPI_CS_DIS()	


void send_SPI_BYTE(unsigned char data)
{
	int i = 0;
	
	for(i = 0; i < 8; i++)
	{
		SPI_CLK_HIGH();
		
		if(data & 0x80)
		{
			SPI_MOSI_HIGH();
		}
		else
		{
			SPI_MOSI_LOW();
		}
		
		data <<= 1;
		SPI_CLK_LOW();
	}
}

void Write_CS5460A(unsigned char*buf, unsigned short n)
{
	for(int i = 0; i < n; i++)
	{
		send_SPI_BYTE(buf[i]);
	}
}

void CS5460A_Init(void)
{
	unsigned char buf[5];
	
// 	reset_5460=0; //reset_5460 Ϊ CS5460A �ĸ�λ��
// 	Delay_10MS();
// 	reset_5460=1; //��λ CS5460A
	buf[0]=0xff; //SYNC1
	buf[1]=0xff; //SYNC1
	buf[2]=0xff; //SYNC1
	buf[3]=0xfe; //SYNC0
	Write_CS5460A(buf,4); //д 3 ��ͬ������ 1 ֮����д 1 ��ͬ������ 0
	buf[0]=0x40; //д���üĴ���
	buf[1]=0x01; //GI=1������ͨ������=50
	buf[2]=0x00;
	buf[3]=0x01; //DCLK=MCLK/1
	Read_Memory(&temp,phase_addr,1);
	if(temp==0xA5)
	{
	Read_Memory(&temp,phase_addr+1,1);
	buf5460[1]=temp;
	}
	//�����Ѿ�ִ�й���λ������������λ����ֵ������������λ����ֵΪ 0
	Write_CS5460A(buf,4);
	//EEPROM ����У׼�ĵ���/��ѹУ׼ֵ��
	//����ָ����ַ��Ԫ���� OXA5����������� 3BYTES ����У׼ֵ��
	Load_Rom_To_5460(0x10,0x42);//дֱ������ƫ��У׼�Ĵ���
	Load_Rom_To_5460(0x20,0x46);//дֱ����ѹƫ��У׼�Ĵ���
	Load_Rom_To_5460(0x30,0x44);//д��������У׼�Ĵ���
	Load_Rom_To_5460(0x40,0x48);//д��ѹ����У׼�Ĵ���
	Load_Rom_To_5460(0x50,0x60);//д��������ƫ��У׼�Ĵ���
	Load_Rom_To_5460(0x60,0x62);//д������ѹƫ��У׼�Ĵ���
	buf[0]=0x5e;
	buf[1]=0xff;
	buf[2]=0xff;
	buf[3]=0xff;
	Write_CS5460A(buf,4); //��״̬�Ĵ���
	buf[0]=0x74;
	buf[1]=0x00;
	buf[2]=0x00;
	buf[3]=0x00;
	Write_CS5460A(buf,4); //д�ж����μĴ�����ȱʡֵCS5460A ʹ��˵��
	5
	buf[0]=0x78;
	buf[1]=0x00;
	buf[2]=0x00;
	buf[3]=0x00; //ȱʡֵ
	Write_CS5460A(buf,4); //д���ƼĴ���
	buf[0]=0x4c;
	buf[1]=0x00;
	buf[2]=0x34;
	buf[3]=0x9C;
	Write_CS5460A(buf,4); //д EOUT ��������Ĵ���
	buf[0]=0x4A;
	buf[1]=0x00;
	buf[2]=0x01; //ÿ���Ӽ��� 10 �Σ� N=400
	buf[3]=0x90;
	Write_CS5460A(buf,4); //д CYCLE COUNT �Ĵ���
	Read_CS5460A(0x1e,buf); //��״̬�Ĵ���
	Buf[3]=buf[2];
	Buf[2]=buf[1];
	Buf[1]=buf[0];
	Buf[0]=0X5E;
	Write_CS5460A(buf,4); //д״̬�Ĵ���
	Buf[0]=0xe8;
	Write_CS5460A(buf,1); //���� CS5460A
}







