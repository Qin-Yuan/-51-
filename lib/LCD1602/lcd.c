#include <reg52.h>

#define LCD1602_DB P0
sbit LCD1602_E=P2^7;
sbit LCD1602_RW=P2^5;
sbit LCD1602_RS=P2^6;

void InitLcd1602();
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

//Һ����ʼ���桪���¶�
void LcdStar()
{
	unsigned char str[] = "benny";
	unsigned char tab[]="temp: "; 
	InitLcd1602();//��ʼ��Һ��
	LcdShowStr(5, 0, str);
	LcdShowStr(1, 1, tab);
	LcdShowStr(6, 1, "00.00");//Ĭ�ϳ�ʼ���¶�Ϊ00
	LcdShowStr(12, 1, "C");//���C�¶�	
}

//�ȴ�Һ��׼��
void LcdWaitReady()
{
	unsigned char sta;
	LCD1602_DB = 0xFF;
	LCD1602_RS = 0;
	LCD1602_RW = 1;
	do {
		LCD1602_E = 1;
		sta = LCD1602_DB; //��ȡ״̬��
		LCD1602_E = 0;
	} while (sta & 0x80); //bit7����1��ʾҺ����æ���ظ����ֱ������0Ϊֹ
}

//��LCD1602Һ��д��һ�ֽ�����cmd
void LcdWriteCmd(unsigned char cmd)
{
	LcdWaitReady();
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_DB = cmd;
	LCD1602_E = 1;
	LCD1602_E = 0;
}
//��LCD1602Һ��д��һ�ֽ�����dat
void LcdWriteDat(unsigned char dat)
{
	LcdWaitReady();
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_E = 1;
	LCD1602_E = 0;
}
//������ʾRAM��ʼλ�ã������λ��(x,y)
void LcdSetCursor(unsigned char x, unsigned char y)
{
	unsigned char addr;
	if (y == 0) //���������Ļ�������RAM�ĵ�ַ
		addr = 0x00 + x; //��һ�У�0x00
	else
		addr = 0x40 + x; //�ڶ��У�0x40
	LcdWriteCmd(addr | 0x80); //����RAM��ַ
}
//��Һ��(x,y)���괦��ʾ�ַ���str
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str)
{
	LcdSetCursor(x, y); //������ʼ����
	while (*str != '\0') //����д���ַ������ݣ�ֱ��������
	{
		LcdWriteDat(*str++); //�Լ�1
	}
}
//��ʼ��1602Һ��
void InitLcd1602()
{
	LcdWriteCmd(0x38); //16*2��ʾ, 5*7����, 8λ���ݽӿ�
	LcdWriteCmd(0x0C); //��ʾ���������ر�
	LcdWriteCmd(0x06); //���ֲ�������ַ�Զ�+1
	LcdWriteCmd(0x01); //����
}

//����
void main()
{
	InitLcd1602(); //��ʼ��Һ��
	//LcdStar(); //��ʾ�¶Ƚ���
	LcdSetCursor(4,0);
	LcdWriteDat('0');
	LcdWriteDat('1');
	LcdWriteDat('2');
	while(1);
}