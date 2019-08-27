#include <avr/io.h>
#include <avr/interrupt.h>

#define LCD_RS_1 (PORTG |= 0x01)      //������ �����
#define LCD_RS_0 (PORTG &= 0xFE)      //���� �����

#define LCD_RW_1 (PORTG |= 0x02)      //�б�
#define LCD_RW_0 (PORTG &= 0xFD)      //����

#define EN_1    (PORTG |= 0x04)      //�޽� 1
#define EN_0    (PORTG &= 0xFB)      //�޽� 0

#define SI_1 (PORTF |= 0x02)
#define SI_0 (PORTF &= 0xFD)
#define CLK_1 (PORTF |= 0x01)
#define CLK_0 (PORTF &= 0xFE)
#define CLK_WIDTH     10
#define SI_WIDTH 1000

char table1[16] = "Hello MEGA";
char table2[16] = "From HyBus";

unsigned int ADdata;
unsigned int ADVoltage;
unsigned int line[128];

void usDelay(unsigned int us)
{
   unsigned int i;

   for(i=0; i<us; i++) {
      asm("PUSH R0");
      asm("POP R0");
      asm("PUSH R0");
      asm("POP R0");
      asm("PUSH R0");
      asm("POP R0");
   }
}

void msDelay(unsigned int ms)
{
   unsigned int i;

   for(i=0;i<ms;i++)
      usDelay(1000);
}

void E_Pulse(void)
{
   EN_1;

   usDelay(100);      //�� 100usec ����

   EN_0;
}

void initLCD(void)
{
   msDelay(40);

   PORTC =0x38;      //Function Set
   E_Pulse();
   usDelay(40);

   PORTC = 0x0c;      //DisPlay ON/OFF Control
   usDelay(40);
   E_Pulse();

   PORTC =  0x01;      //Display Clear
   msDelay(2);
   E_Pulse();

   PORTC = 0x06;      //Entry Mode Set
   E_Pulse();
}

void Write_Char(unsigned char buf)
{
   LCD_RS_1;         //���� ����� ��� ����
   LCD_RW_0;         //���� ��� ����
   PORTC=buf;         //���� ���
   E_Pulse();         //�޽� ����
}

void LCD_Disp(char x,char y)
{
   LCD_RS_0;         //���� ��� ����
   LCD_RW_0;         //���� ��� ����

   if(y==0)
      PORTC=x+0x80;   //1��° ��
   else if(y==1)
      PORTC=x+0xc0;   //2��° ��
   E_Pulse();
}

void LCD_Write(char x, char y, char *str)
{
   LCD_Disp(x,y);      //x��, y������ �̵�
   while(*str)
   Write_Char(*str++);   //���� ���
}

void LCD_cmd(unsigned char cmd)
{
   LCD_RS_0;         //����
   LCD_RW_0;         //����
   PORTC=cmd;         //�����ڵ�
   E_Pulse();         //�޽�
   msDelay(1);      //����
}

void init_TIMER1(void)
{
   TCCR1A = 0x22;  //0xa2;  
   TCCR1B = 0x1A;
   TCCR1C = 0; 
   TCNT1 = 0;
   OCR1A = 36000;//32000;  //DC motor
   OCR1B = 2600;   //servo motor //2800(L)~2200(C)~1800(R)
   ICR1 = 40000;
   TIFR = 0x00;
   TIMSK = 0x00;
   ETIMSK = 0x00;
   ETIFR = 0x00;
}

/*void init_TIMER1_DC(void)
{
   TCCR1A = 0x82;  //0xa2
   TCCR1B = 0x1A;
   TCCR1C = 0; 
   TCNT1 = 0;
   OCR1A = 30000;  //85;   75% ~ 100% = 30000 ~ 40000
   ICR1 = 40000;   //100;   
   TIFR = 0x00;
   TIMSK = 0x00;
}*/

void initADC_red(void)
{
   ADMUX = 0x42;   //��������; �ܺ� ĳ�۽��� ����AVcc(AREF ��)
               //AC ��ȯ ������ ����; ������ ����
               //AD ��ȯ ä�� ����; PortF 0 ��

   ADCSRA= 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
               //ADC Ŭ�� ����; XTAL��1/2(8MHz)

   //DDRF = 0xf0;   //PortF[3..0] �Է����� ����, PortF[7..4] ������� ����
   //DIDR0 = 0x0f;   //������ �Է� �Ұ� PortF[3..0]
}

/*void initADC(void)
{
   ADMUX = 0x42;   //��������; �ܺ� ĳ�۽��� ����AVcc(AREF ��)
               //AC ��ȯ ������ ����; ������ ����
               //AD ��ȯ ä�� ����; PortF 0 ��

   ADCSRA= 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
               //ADC Ŭ�� ����; XTAL��1/2(8MHz)

   //DDRF = 0x03;   //PortF[3..0] �Է����� ����, PortF[7..4] ������� ����
   //DIDR0 = 0x0f;   //������ �Է� �Ұ� PortF[3..0]
   
}*/

void ADCLCD(void)
{
   unsigned int Buff;
   unsigned char AD1000, AD100, AD10, AD1;

   ADVoltage= /*ADdata*/line[64]*50/1023;         //AD ��ȯ �����͸� �������� ��ȯ

   AD1000= /*ADdata*/line[64]/ 1000| 0x30;      //AD ��ȯ ������ 1000�ڸ��� ASCII �ڵ�ȭ
   Buff = /*ADdata*/line[64]% 1000;
   AD100 = Buff / 100 | 0x30;         //AD ��ȯ ������ 100�ڸ��� ASCII �ڵ�ȭ
   Buff = Buff % 100;
   AD10 = Buff / 10 | 0x30;         //AD ��ȯ ������ 10�ڸ��� ASCII �ڵ�ȭ
   AD1 = Buff % 10 | 0x30;               //AD ��ȯ ������ 1�ڸ��� ASCII �ڵ�ȭ

   LCD_Disp(10,1);                  //LCD �ι�° �ٿ� AD ��ȯ ������ ǥ��
   Write_Char(AD1000);
   LCD_Disp(11,1);
   Write_Char(AD100);
   LCD_Disp(12,1);
   Write_Char(AD10);
   LCD_Disp(13,1);
   Write_Char(AD1);

   AD10= ADVoltage / 10 | 0x30;       //���� 10�ڸ��� ASCII �ڵ�ȭ
   AD1= ADVoltage % 10 | 0x30;       //���� 1�ڸ��� ASCII �ڵ�ȭ

 	

   LCD_Disp(10,0);                  // LCD ù��° �ٿ� �������� ǥ��
   Write_Char(AD10);    // ���� 1�ڸ���
   LCD_Write(11,0,".");            // �Ҽ��� ����
   LCD_Disp(12,0);
   Write_Char(AD1);
   
                 // �Ҽ��� ���� ù��° �� ǥ��
}

SIGNAL(ADC_vect)
   {
      //int i = 0; 

      ADdata=ADC;      //AD ��ȯ �����͸� ADdata �� ����
	  //ADCLCD();
	  
	  /*line[i] = ADdata;
	  i++;
	  if(i>127){
	     i=0;
      }*/
	  
      ADCSRA = 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�

   }



void Refresh()
{
	int i;

	SI_1;
	usDelay(SI_WIDTH);
	CLK_1;
	usDelay(SI_WIDTH);
	SI_0;
	usDelay(SI_WIDTH);
	CLK_0;

	for(i=0;i<150;i++){
		CLK_1;
		usDelay(CLK_WIDTH );
		CLK_0;
		usDelay(CLK_WIDTH );
	}
	SI_1;
	usDelay(SI_WIDTH);
}

void ConShot()
{
	int i;

    //msDelay(1000);

	SI_0;
	CLK_0;
	usDelay(SI_WIDTH);
	SI_1;
	usDelay(SI_WIDTH);
	CLK_1;
	usDelay(CLK_WIDTH );
	SI_0;
	usDelay(CLK_WIDTH );
	CLK_0;
	//msDelay(1000);

	for(i=0;i<128;i++){
		CLK_1;
		usDelay(CLK_WIDTH );
		//msDelay(500);
		CLK_0;
		usDelay(CLK_WIDTH );
		//msDelay(500);
		line[i] = ADdata;
	}

	/*if((line[122]+line[123]+line[124]+line[125]+line[126])/5<500)
	{
	   OCR1B = 2800;
	}
	if((line[1]+line[2]+line[3]+line[4]+line[5])/5<500)
	{
	   OCR1B = 1800;
	}*/
	if((line[1]+line[2]+line[3])/3<500)
    {
	   msDelay(1);
	   OCR1B = 2000;
	}	
	if((line[124]+line[125]+line[126])/3<500)
    {
	   msDelay(1);
	   OCR1B = 2500;
	}
	if((line[1]+line[2]+line[3]+line[4]+line[5])/5<500)
	{ 
	   msDelay(1);
	   OCR1B = 1800;
	}
	if((line[122]+line[123]+line[124]+line[125]+line[126])/5<500)
	{
	   msDelay(1);
       OCR1B = 2800;
	}
	/*else
	{
	   OCR1B = 2200;
	}*/
	/*if((line[122]+line[123]+line[124]+line[125]+line[126]+line[1]+line[2]+line[3]+line[4]+line[5])/10>500)
	{
	   msDelay(1);
       OCR1B = 2200;
	}*/
	
}

void init_Port(void)
{
   DDRC = 0xff;
  // PORTC = 0xff;
   DDRG = 0xff;
   //DDRE = 0xef;
   //PORTE = 0x10;
   DDRF = 0xf3;
   DDRB = 0x38;
   PORTB = 0x30;
}

int main(void)
{

   init_Port();                     //����� ��Ʈ �ʱ�ȭ
      initLCD();                  //LCD �ʱ�ȭ
      //initADC();                  //AD ��ȯ �ʱ�ȭ
	  init_TIMER1();
	  //init_TIMER1_DC();
	  initADC_red();


      CLK_0;
	  SI_0;
	  Refresh();
	  SI_0;
	  


      sei();                     //INT �ο��̺� clear = �۵��ȵ�

      LCD_cmd(0x01);               //LCD ȭ�� �����
      msDelay(50);               //����

      LCD_Write(0,0, table1);         //�ʱ� ���� ���
      LCD_Write(0,1, table2);
      msDelay(200);

      LCD_cmd(0x01);               //LCD ȭ�� �����
      msDelay(100);

      LCD_Write(0,0,"Voltage :    V");//���� ǥ��
      LCD_Write(0,1, "ADC Data:");   //AD ��ȯ ������ ǥ��

	  //ConShot();
	  //ADCLCD();

      while(1)
      {
	     ConShot();
		 if(ADdata >= 600)
       { OCR1A = 30;}
		 //msDelay(1000);
		 
		 ADCLCD();
		               //AD ��ȯ �����Ϳ� ���а� LCD�� ǥ��
	  }

}