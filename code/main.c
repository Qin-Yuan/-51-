#include <reg52.h>
#include <lcd.h>
#include <motor.h>
#include <temp.h>
#include <relay.h>
#include <key_scan.h>
#include <ds1302.h>
#include <i2c.h>

//�ֲ�����
uint key1=1;  //��ֵ��¼
uint key2=0;  //��¼�ϴΰ���
uint key3=0;  //�������ò���
uint key4=0;  //�����ж����ӻ��Ǽ���
uint key5=0;  //������
int temp=250;     //��¼�ɼ����¶�
uchar F_h=23;  //Ͷʳ��h
uchar F_m=40;	 // m
uchar F_s=20;  // s
uchar O_h=23;  //������h
uchar O_m=40;  // m
uchar O_s=35;  // s
uchar W_m=0; //��ˮ���ڷ���  <60 min
uchar W_s=5; //��ˮ������   <60 s
char h,m,s,h1,m1,s1,m2,s2; //��¼��ȡEEPROM������� Сʱ���֣���   h:Ͷʳ����ʱ��   h1:��������ʱ��  m2��ˮѭ������

//ȫ�ֱ���
extern uchar temp_L=22;  //�¶�����
extern uchar temp_H=26;  //�¶�����

/*
24C02C�ⲿ����ģ��
##### �¶� ####
0x00���¶�����    ox01���¶�����

##### Ͷʳ ####
0x02��ʱ  0x03����  0x04����

##### ���� ####
0x05��ʱ  0x06����  0x07����

##### ��ˮ ####
0x08����  0x09: ��
*/


//��������
void LcdDisplay(int temp); 	 //lcd��ʾ�¶�
void Timer0Init();   //��ʱ���ж�0��ʼ��
void Timer1Init();   //��ʱ���ж�1��ʼ��
void key_lcd();  //lcd_key��������
void datashow(); //LCD��ʾʱ��ʱ��
void i2c_init(); //EEPROMϵͳ���ó�ʼ��
void key_set(); //��������


//������
void main()
{
	delayms(10);
	Ds18b20Init(); //�¶ȴ�������ʼ����
	Ds1302Init();	   //ʱ�ӳ�ʼ������һ�γ�ʼ����Ϳ���ע�͸�����䣬�����´������Ͳ����ٴγ�ʼ����
	add_water(); //�̵���
	InitLcd1602(); //��ʼ��Һ��
	//LcdStar(); //��ʾ�¶Ƚ���
	//delayms(3000);
	Timer0Init(); //��ʱ���ж�0��ʼ��
	Timer1Init(); //��ʱ���ж�1��ʼ��
	i2c_init();  //ֻ��Ҫִ��һ��
	while(1);
}

/*******************************************************************************
* �� �� ��         : datashow(int x,int y)
* ��������		     : LCD��ʾʱ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void datashow()	 
{
  LcdSetCursor(4,0); 
	LcdWriteDat('0'+TIME[2]/16);				//ʱ
	LcdWriteDat('0'+(TIME[2]&0x0f));				 
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[1]/16);				//��
	LcdWriteDat('0'+(TIME[1]&0x0f));	
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[0]/16);				//��
	LcdWriteDat('0'+(TIME[0]&0x0f));
	/*
	LcdWriteCmd(0x80+0X40);
	LcdWriteDat('2');
	LcdWriteDat('0');
	LcdWriteDat('0'+TIME[6]/16);			//��
	LcdWriteDat('0'+(TIME[6]&0x0f));
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[4]/16);			//��
	LcdWriteDat('0'+(TIME[4]&0x0f));
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[3]/16);			//��
	LcdWriteDat('0'+(TIME[3]&0x0f));	 
	*/
}
/*******************************************************************************
* �� �� ��         : work()
* ��������		     : Ͷʳ������ʱ����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void work()
{
	//Ͷʳ
	if((TIME[2]/16)==(h/10)&&(TIME[2]&0x0f)==(h%10)&&(TIME[1]/16)==(m/10)&&(TIME[1]&0x0f)==(m%10)&&(TIME[0]/16)==(s/10)&&(TIME[0]&0x0f)==(s%10))
	{
		int n=200;
		while(n--){
		run_z(1);}
		delayms(1000);
		n=200;
		while(n--){
		run_f(1);}
	}
	//����
	else if((TIME[2]/16)==(h1/10)&&(TIME[2]&0x0f)==(h1%10)&&(TIME[1]/16)==(m1/10)&&(TIME[1]&0x0f)==(m1%10)&&(TIME[0]/16)==(s1/10)&&(TIME[0]&0x0f)==(s1%10))
	{
		add_ox2();
	}
}
/*******************************************************************************
* ������         : i2c()
* ��������		   : ��ʼ��д�����ݵ�EEPROM
* ����           : v
* ���         	 : ��
*******************************************************************************/
void i2c_init()
{
	At24c02Write(0x00,temp_H);   //�¶����
	delayms(5);
	At24c02Write(0x01,temp_L);   //�¶����
	delayms(5);
	At24c02Write(0x02,F_h);   //�Զ�Ͷʳ��h
	delayms(5);
	At24c02Write(0x03,F_m);   //�Զ�Ͷʳ��m
	delayms(5);
	At24c02Write(0x04,F_s);   //�Զ�Ͷʳ��s
	delayms(5);
	At24c02Write(0x05,O_h);   //�Զ�������h
	delayms(5);
	At24c02Write(0x06,O_m);   //�Զ�������m
	delayms(5);
	At24c02Write(0x07,O_s);   //�Զ�������s
	delayms(5);
	At24c02Write(0x08,W_m);   //�Զ���ˮ��m
	delayms(5);
	At24c02Write(0x09,W_s);   //�Զ���ˮ��s
	delayms(5);
}

/*******************************************************************************
* ������         : LcdDisplay()
* ��������		   : LCD��ʾ��ȡ�����¶�
* ����           : v
* ���         	 : ��
*******************************************************************************/
void LcdDisplay(int temp) 	 //lcd��ʾ
{
    uchar datas[] = {0, 0, 0, 0, 0}; //��¼�¶�000.00,ֻ��ʾ00.00
		float tp; 
	  //��ȡͶʳ���õ�ʱ��
		delayms(5);
		h = At24c02Read(0x02);
		delayms(5);
		m = At24c02Read(0x03);
		delayms(5);
		s = At24c02Read(0x04);
		//��ȡ�������õ�ʱ��
		delayms(5);
		h1 = At24c02Read(0x05);
		delayms(5);
		m1 = At24c02Read(0x06);
		delayms(5);
		s1 = At24c02Read(0x07); 
		delayms(5);
		//��ˮ����
		m2 = At24c02Read(0x08);
		delayms(5);
		s2 = At24c02Read(0x09); 
		delayms(5);
	  if(temp< 0)				//���¶�ֵΪ����
			{
				//LcdSetCursor(5,1);  		//��ʾ��
				//LcdWriteDat('-');
				//��Ϊ��ȡ���¶���ʵ���¶ȵĲ��룬���Լ�1����ȡ�����ԭ��
				temp=temp-1;
				temp=~temp;
				tp=temp;
				temp=tp*0.0625*100+0.5;	
				//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
				//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5
				//�����0.5��������С������档
			}
 	  else
			{			
				//LcdSetCursor(5,1); 		//��ʾ��
				//LcdWriteDat('+');
				tp=temp;//��Ϊ���ݴ�����С�������Խ��¶ȸ���һ�������ͱ���
				//����¶���������ô����ô������ԭ����ǲ���������
				temp=tp*0.0625*100+0.5;	
				//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
				//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
				//�����0.5��������С������档
			}
		//datas[0] = temp / 10000;			 //��
		datas[1] = temp % 10000 / 1000;  //ʮ
		datas[2] = temp % 1000 / 100;		 //��
		datas[3] = temp % 100 / 10;			 // .0
		//datas[4] = temp % 10;					 // .00
		
		//�ж��Ƿ���Ҫ����
		if((temp/100)<temp_L)
		{
			relay2=0;
		}
		else  //((temp/100)>temp_H)
		{
			relay2=1;
		}
		//�жϰ����л�����
		if(key1==1)
		{
			if(key2!=1)
			{
				LcdWriteCmd(0x01); //����
				key2 = 1 ;
			}
			//ʵʱ��ʾ�¶�
		  //LcdSetCursor(6,1); //��λ 
	    //LcdWriteDat('0'+datas[0]);
			LcdShowStr(1,0,"Temp:");
			LcdSetCursor(7,0); //ʮλ
			LcdWriteDat('0'+datas[1]);
			LcdSetCursor(8,0); //��λ 
			LcdWriteDat('0'+datas[2]);
			LcdSetCursor(9,0); 		//��ʾ ��.��
			LcdWriteDat('.');
			LcdSetCursor(10,0); //��ʾС���� 
			LcdWriteDat('0'+datas[3]);	
			//LcdSetCursor(11,1); //��ʾС���� 
			//LcdWriteDat('0'+datas[4]);
			//��ʾ�¶ȷ�Χ����
			LcdShowStr(1,1,"L: ");
			LcdWriteDat('0'+temp_L/10);
			LcdWriteDat('0'+temp_L%10);
			LcdShowStr(7,1,"H: ");
			LcdWriteDat('0'+temp_H/10);
			LcdWriteDat('0'+temp_H%10);
		}
		
		//LCD��ʾ�Զ�Ͷʳʱ��
		else if(key1==2)
		{
			if(key2!=2)
			{
				LcdWriteCmd(0x01); //����
				key2 = 2;
				LcdShowStr(0,1,"F_time: ");
				LcdWriteDat('0'+h/10);
				LcdWriteDat('0'+h%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+m/10);
				LcdWriteDat('0'+m%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+s/10);
				LcdWriteDat('0'+s%10);
			}
			datashow();
		}
		
		//LCD��ʾ�Զ�����ʱ��
		else if(key1==3)
		{
			if(key2!=3)
			{
				LcdWriteCmd(0x01); //����
				key2 = 3;
				LcdShowStr(0,1,"O_time: ");
				LcdWriteDat('0'+h1/10);
				LcdWriteDat('0'+h1%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+m1/10);
				LcdWriteDat('0'+m1%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+s1/10);
				LcdWriteDat('0'+s1%10);
			}
			datashow();
		}
		//LCD��ʾ�Զ�ˮѭ��ʱ��
		else if(key1==4)
		{
			if(key2!=4)
			{
				LcdWriteCmd(0x01); //����
				key2 = 4;
				LcdShowStr(4,1,"W_T: ");
				LcdWriteDat('0'+m2/10);
				LcdWriteDat('0'+m2%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+s2/10);
				LcdWriteDat('0'+s2%10);
			}
			datashow();
		}
}

/*
###############################
#######  ��ʱ���ж�0 ##########
########## ��ʼ�� #############
*/
void Timer0Init()
{
	TMOD|=0X01;//ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������
	TH0 = (65535-20000)/256;	 //20MS��ʱ
  TL0 = (65535-20000)%256;
	ET0=1;//�򿪶�ʱ��0�ж�����
	EA=1;//�����ж�
	TR0=1;//�򿪶�ʱ��
}
//#####  ��ʱ��0������ #######
void timer0() interrupt 1
{
	static uint i,j;
	TR0 = 0;
	TH0 = (65536-20000)/256;	 //20MS��ʱ
  TL0 = (65536-20000)%256;
	key_scan(&key1,&key3); //����ɨ�裬���Ѱ���ֵ���ݸ�key1
	key_set();
	work();
	i++;
	j++;
	//��ˮ
	if(j==30*(m2*60+s2))
	{
		relay1=0;
	}
	else if(j==30*(m2*60+s2+1))
	{
		relay1=1;
		j=0;
	}
	/*
	if(i==40)
	{	
		temp = temp+50;
		//LcdDisplay(temp) ;   //LCD�¶�ֵˢ��
		i=0;
	}
	if(temp==500)
	{temp=250;}
	*/
	TR0=1;
}
/*
###############################
#######  ��ʱ���ж�1 ##########
########## ��ʼ�� #############
*/
void Timer1Init()
{
	TMOD|=0X10;//ѡ��Ϊ��ʱ��1ģʽ��������ʽ1������TR1��������
	TH0 = (65536-20000)/256;	 //20MS��ʱ
  TL0 = (65536-20000)%256;
	ET1=1;//�򿪶�ʱ��1�ж�����
	EA=1;//�����ж�
	TR1=1;//�򿪶�ʱ��
}
//###### ��ʱ��1������ #####
void timer1() interrupt 3
{
	TR1=0;
	TH0 = (65535-20000)/256;	 //20MS��ʱ
  TL0 = (65535-20000)%256;
	Ds1302ReadTime(); //ʱ��TIME
	LcdDisplay(Ds18b20ReadTemp()); //����LCD��ʾ
	//Ds18b20ReadTemp(); //�����¶�ֵ
	TR1=1;
}

/*
#################################
#######  key���ò��� #######
*///11111
void key_set()
{
	//Ͷʳʱ���趨
	if(key3==2)
	{
		LcdWriteCmd(0x01);
		LcdShowStr(5,0,"ft_set");
		while(1)
		{	
			key_scan(&key4,&key5);
			//��ʾ��ǰ��ֵ
			LcdSetCursor(4,1);
			LcdWriteDat('0'+h/10);
			LcdWriteDat('0'+h%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+m/10);
			LcdWriteDat('0'+m%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+s/10);
			LcdWriteDat('0'+s%10);
			if(key4==3)
			{
				key4=0;
				m--;
				if(m<0)
				{
					m=59;
					h--;
					if(h<0)
					{	
						h=23;
					}
				}
				At24c02Write(0x02,h);   //�Զ�Ͷʳ��h
				delayms(5);
				At24c02Write(0x03,m);   //�Զ�������m 
				delayms(5);
			}
			else if(key4==4)
			{
				key4=0;
				m++;
				if(m>59)
				{
					m=0;
					h++;
					if(h>23)
					{	
						h=0;
					}
				}
				At24c02Write(0x02,h);   //�Զ�Ͷʳ��h
				delayms(5);
				At24c02Write(0x03,m);   //�Զ�Ͷʳ��m 
				delayms(5);
			}
			else if(key4==1)
			{
				key2=key3=key4=0;
				break;
			}
		}
	}
	//����ʱ���趨
	else if(key3==3)
	{
		LcdWriteCmd(0x01);
		LcdShowStr(5,0,"Ot_set");
		while(1)
		{	
			key_scan(&key4,&key5);
			//��ʾ��ǰ��ֵ
			LcdSetCursor(4,1);
			LcdWriteDat('0'+h1/10);
			LcdWriteDat('0'+h1%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+m1/10);
			LcdWriteDat('0'+m1%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+s1/10);
			LcdWriteDat('0'+s1%10);
			if(key4==2)
			{
				key4=0;
				m1--;
				if(m1<0)
				{
					m1=59;
					h1--;
					if(h1<0)
					{	
						h1=23;
					}
				}
				At24c02Write(0x05,h1);   //�Զ�������h
				delayms(5);
				At24c02Write(0x06,m1);   //�Զ�������m 
				delayms(5);
			}
			else if(key4==4)
			{
				key4=0;
				m1++;
				if(m1>59)
				{
					m1=0;
					h1++;
					if(h1>23)
					{	
						h1=0;
					}
				}
				At24c02Write(0x05,h1);   //�Զ�Ͷ�䣺h
				delayms(5);
				At24c02Write(0x06,m1);   //�Զ�������m 
				delayms(5);
				
			}
			else if(key4==1)
			{
				key2=key3=key4=0;
				break;
			}
		}
	}
	//ˮѭ�������趨
	else if(key3==4)
	{
		LcdWriteCmd(0x01);
		LcdShowStr(5,0,"Wt_set");
		while(1)
		{	
			key_scan(&key4,&key5);
			//��ʾ��ǰ��ֵ
			LcdSetCursor(6,1);
			LcdWriteDat('0'+m2/10);
			LcdWriteDat('0'+m2%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+s2/10);
			LcdWriteDat('0'+s2%10);
			if(key4==2)
			{
			  key4=0;
				s2=s2-5;
				if(s2<0)
				{
					s2=59;
					m2--;
					if(m2<0)
					{	
						m2=59;
					}
				}
				At24c02Write(0x08,m2);   //�Զ�������m
				delayms(5);
				At24c02Write(0x09,s2);   //�Զ�������s 
				delayms(5);
			}
			else if(key4==3)
			{
			  key4=0;
				s2=s2+5;
				if(s2>59)
				{
					s2=0;
					m2++;
					if(m2>59)
					{	
						m2=0;
					}
				}
				At24c02Write(0x08,m2);   //�Զ�������m
				delayms(5);
				At24c02Write(0x09,s2);   //�Զ�������s 
				delayms(5);
			}
			else if(key4==1)
			{
				key2=key3=key4=0;
				break;
			}
		}
	}
}