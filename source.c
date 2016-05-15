#include<reg51.h>
#include"lcd4.h"
#include"gsm.h"

sbit Trig  = P3^5;
sbit Echo  = P3^4;
unsigned char msg1;

void init_serial(void);
void DelayMs(unsigned int);
void find_distance();

void main()
{
	lcd_init();
	init_serial();
	Echo = Trig = 0;
	TMOD |= 0x01;//Timer 0 in 16-bit mode																				 

	P1=0xf0;    // set port as input port
	P2=0x00;    // set port as output port
	DelayMs(100);
	while(1)
	{
		if(P1==0xf8)
		{
			P2=0x0a;		 //00001010	front
		}
		else if(P1==0xf1)
		{
			P2=0x02;		 //00000010	
		}    
		else if(P1==0xf2)
		{
			P2=0x08;		//00001000
		}    
		else if(P1==0xf4)
		{
			P2=0x05;		//00000101	back
		}
		else 	if(msg1=='*')
		{
			find_distance();
			msg1=0;
		}
		else
		{
			P2=0x00;
		}
	}
}

void init_serial()
{
	TMOD=0x20;
	TH1=0xFD;
	SCON=0X50;
	IE=0X90;
	TR1=1;
}

void receive_data() interrupt 4
{	 
	while(RI)
	{
		msg1=SBUF;
		RI=0;
	}		 
}

void delay_us(unsigned int us)//This function provide delay in us uS.
{
		while(us--);
}

void DelayMs(unsigned int ms)
{  // mSec Delay 11.0592 Mhz
    unsigned int i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<127;j++);
}

void find_distance()
{
	unsigned int Count,Time,Distance; 
	Trig = 1;//Send 10us start pulse to HC-SR04 as per datasheet of HC-SR04
	delay_us(10);//~10us delay
	Trig = 0;	
	while(Echo == 0);//Wait for Rising edge at Echo pin
	TR0=1;//Start Timer
	TL0=TH0=0;//Clear timer count register
	while(Echo == 1)//Wait for Falling edge at Echo pin
	{
		if(TF0 == 1)//timer over if no obstacle is detected
			break;
	}
	TR0=0;//Stop Timer.
	TF0=0;//clear Timer Over Flow Flag
	Count = TL0 + TH0*256;//Calculate number of count
	Time = Count*1.085;//Calculate total time in uS.
	Distance = Time/58;//As per datasheet of HC-SR04 Distance is in Centimeter
	TX_String("Distance: ");
	TX_Int(Distance);//Send distance to serial
	TX_String("\r\n");
	DelayMs(2000);
}
