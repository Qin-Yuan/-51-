#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int
	
//����IO��
sbit relay1 = P2^2;  //ˮѭ���̵���
sbit relay2 = P2^1;	 //���ȼ̵���
sbit relay3 = P2^0;	 //�����̵���

//��ʱ����
void delay( uint ms )
{
	uchar i;
	while ( ms-- )
	{
		for (i=0;i<120;i++);
	}
}

//ˮѭ���̵���
void add_water()
{
	relay1=0;
	delay(3000); //����ʱ��
	relay1=1;
}
//���ȼ̵���
void add_temp()
{
	relay2=0;
	delay(3000); //����ʱ��
	relay2=1;
}
//�����̵���
void add_ox2()
{
	relay3=0;
	delay(3000); //����ʱ��
	relay3=1;
}

void main()
{
	while(1)
	{
	relay1=0;
	relay2=0;
	relay3=0;
	}
}