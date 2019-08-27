/*==================================================================================
program: Analog digital converter
file: adc91.c
author: Park, Jang sik
date: 2006.10.08
register
PortG; LCD ���� ���
portC; LCD ������
PortF; �Ƴ��α� �Է�

ADMUX; �Ƴ��α� �Է� ����
ADCSRA; AD ��ȯ ����
DIDR; ������ �Է� �Ұ� ����
ADC; AD ��ȯ ������ ����
=================================================================================*/
#include <avr/io.h>
#include <avr/interrupt.h>

#define Lcd_RS_1 (PORTG |= 0x01)      //������ �����
#define Lcd_RS_0 (PORTG &= 0xFE)      //��� �����

#define LCD_RW_1 (PORTG |= 0x02)      //�б�
#define LCD_RW_0 (PORTG &= 0xFD)      //����

#define EN_1    (PORTG |= 0x04)      //�޽� 1
#define EN_0    (PORTG &= 0xFB)      //�޽� 0

#define ON 1
#define OFF 2

#define LEFT 0
#define RIGHT 1
#define NO 2

char str1[] = "Hello CAN";
char str2[] = "From HyBus";

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

   us_delay(100);      //�� 100usec ����

   EN_0;
}

void initLCD(void)
{
   ms_delay(40);

   PORTC =0x38;      //Function Set
   E_Pulse();
   us_delay(40);

   PORTC = 0x0c;      //DisPlay ON/OFF Control
   us_delay(40);
   E_Pulse();

   PORTC =  0x01;      //Display Clear
   ms_delay(2);
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
   LCD_RS_0;         //��� ��� ����
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
   LCD_RS_0;         //���
   LCD_RW_0;         //����
   PORTC=cmd;         //����ڵ�
   E_Pulse();         //�޽�
   ms_delay(1);      //����
}

void LCD_cmd_Shift_Right(void)
{
   LCD_cmd(0x1c);      // ��ü ������ �̵�
}

void LCD_cmd_Shift_Left(void)
{
   LCD_cmd(0x18);      // ��ü ���� �̵�
}

void LCD_cmd_Display(char disp, char cursor, char blink)
{
   char display= 0x08;   //���÷��� ����

   LCD_RS_0;
   LCD_RW_0;

   if(disp==ON)
      disp= 0x04;      //���÷��� ON
   else
      disp= 0x00;
   
   if(cursor == ON);   //Ŀ�� ON
      curror= 0x02;
   else
      cursor= 0x00;

   if(blink==ON)      //Ŀ�� ������
      blink= 0x01;
   else
      blink= 0x00;

   PORTC= display | disp | cursor | blink;
   E_Pulse();
   ms_delay(1);
}

void LCD_cmd_Entry(char dir)
{
   LCD_RS_0;
   LCD_RW_0;

   if(dir==RIGHT)
      PORTC= 0x05;   //���÷��� ON
   else if (dir==LEFT)
      PORTC= 0x06;
   else if(dir==NO)
      PORTC= 0x06;
   E_Pulse();
   ms_delay(1)
}

void initPort(void)
{
   DDRC = 0xff;      //������/��� ����
   DDRG = 0xff;      //RS,RW,E ��� ����
}


unsigned int ADdata;
unsigned int ADVoltage;

char table1[16]="AT90CAN128";
char table2[16]="ADC Test";


/*================================================================================
AD ��ȯ
PF0: ADC0 ��������
PF1: ADC1 �µ�
PF2: ADC2 CDS
PF3: ADC3 -
================================================================================*/
void initAdc(void)
{
   admux = 0x40;   //��������; �ܺ� ĳ�۽��� ����AVcc(AREF ��)
               //AC ��ȯ ������ ����; ������ ����
               //AD ��ȯ ä�� ����; PortF 0 ��

   ADCSRA= 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
               //ADC Ŭ�� ����; XTAL��1/2(8MHz)

   DDRF = 0xf0;   //PortF[3..0] �Է����� ����, PortF[7..4] ������� ����
   DIDR0 = 0x0f;   //������ �Է� �Ұ� PortF[3..0]
   }

   STGNAL(ADC_vect)
   {
   ADdata=ADC;      //AD ��ȯ �����͸� ADdata �� ����
   ADCSRA = 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
}

void ADcLCD(void)
{
   unsigned int Buff;
   unsigned char AD1000, AD100, AD10, AD1;

   ADVoltage= ADdata*50/1023;         //AD ��ȯ �����͸� �������� ��ȯ

   ADVoltage= ADdata/ 1000| 0x30;      //AD ��ȯ ������ 1000�ڸ��� ASCII �ڵ�ȭ
   Buff = ADdata% 1000;
   AD100 = Buff / 100 | 0x30;         //AD ��ȯ ������ 100�ڸ��� ASCII �ڵ�ȭ
   Buff = Buff % 100;
   AD100 = Buff / 10 | 0x30;         //AD ��ȯ ������ 10�ڸ��� ASCII �ڵ�ȭ
   AD1 = Buff % 10;               //AD ��ȯ ������ 1�ڸ��� ASCII �ڵ�ȭ

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
   Write_Char(AD10);               // ���� 1�ڸ���
   LCD_Write(11,0,".");            // �Ҽ��� ����
   LCD_Disp(12,0);
   Write_Char(AD1);               // �Ҽ��� ���� ù��° �� ǥ��
}

void initPort(void)
{
   DDRC = 0xff;
   PORTC = 0xff;
   DDRG = 0xff;
   DDRE = 0xef;
   PORTE = 0x10;
   DDRF = 0xf0;
}


int main(void)
{
   initPort();                     //����� ��Ʈ �ʱ�ȭ
      initLCD();                  //LCD �ʱ�ȭ
      initAdc();                  //AD ��ȯ �ʱ�ȭ

      sei();                     //INT �ο��̺� clear = �۵��ȵ�

      LCD_cmd(0x01);               //LCD ȭ�� �����
      ms_delay(50);               //����

      LCD_Write(0,0, table1);         //�ʱ� ���� ���
      LCD_Write(0,1, table2);
      ms_delay(200);

      LCD_cmd(0x01);               //LCD ȭ�� �����
      ms_delay(100);

      LCD_Write(0,0,"Coltage :   V");//���� ǥ��
      LCD_Write(0,1, "ADC Data:");   //AD ��ȯ ������ ǥ��
      while(1)
      {
         ADcLCD();               //AD ��ȯ �����Ϳ� ���а� LCD�� ǥ��
      }

}
