#ifndef __RELAY_H_
#define	__RELAY_H_

//����IO��
sbit relay1 = P2^2;  //ˮѭ���̵���
sbit relay2 = P2^1;	 //���ȼ̵���
sbit relay3 = P2^0;	 //�����̵���


//ˮѭ���̵���
void add_water()
{
	relay1=0;
	delayms(100); //����ʱ��
	relay1=1;
}
//���ȼ̵���
void add_temp()
{
	relay2=0;
	delayms(3000); //����ʱ��
	relay2=1;
}
//�����̵���
void add_ox2()
{
	relay3=0;
	delayms(3000); //����ʱ��
	relay3=1;
}
#endif