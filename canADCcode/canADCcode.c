/*==================================================================================
program: Analog digital converter
file: adc91.c
author: Park, Jang sik
date: 2006.10.08
register
PortG; LCD 제어 출력
portC; LCD 데이터
PortF; 아날로그 입력

ADMUX; 아날로그 입력 선택
ADCSRA; AD 변환 제어
DIDR; 디지털 입력 불가 설정
ADC; AD 변환 데이터 저장
=================================================================================*/
#include <avr/io.h>
#include <avr/interrupt.h>

#define Lcd_RS_1 (PORTG |= 0x01)      //데이터 입출력
#define Lcd_RS_0 (PORTG &= 0xFE)      //명령 입출력

#define LCD_RW_1 (PORTG |= 0x02)      //읽기
#define LCD_RW_0 (PORTG &= 0xFD)      //쓰기

#define EN_1    (PORTG |= 0x04)      //펄스 1
#define EN_0    (PORTG &= 0xFB)      //펄스 0

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

   us_delay(100);      //약 100usec 지연

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
   LCD_RS_1;         //문자 입출력 모드 설정
   LCD_RW_0;         //쓰기 모드 설정
   PORTC=buf;         //문자 출력
   E_Pulse();         //펄스 생성
}

void LCD_Disp(char x,char y)
{
   LCD_RS_0;         //명령 모드 설정
   LCD_RW_0;         //쓰기 모드 설정

   if(y==0)
      PORTC=x+0x80;   //1번째 행
   else if(y==1)
      PORTC=x+0xc0;   //2번째 행
   E_Pulse();
}

void LCD_Write(char x, char y, char *str)
{
   LCD_Disp(x,y);      //x열, y행으로 이동
   while(*str)
   Write_Char(*str++);   //문자 출력
}

void LCD_cmd(unsigned char cmd)
{
   LCD_RS_0;         //명령
   LCD_RW_0;         //쓰기
   PORTC=cmd;         //명령코드
   E_Pulse();         //펄스
   ms_delay(1);      //지연
}

void LCD_cmd_Shift_Right(void)
{
   LCD_cmd(0x1c);      // 전체 오른쪽 이동
}

void LCD_cmd_Shift_Left(void)
{
   LCD_cmd(0x18);      // 전체 왼쪽 이동
}

void LCD_cmd_Display(char disp, char cursor, char blink)
{
   char display= 0x08;   //디스플레이 제어

   LCD_RS_0;
   LCD_RW_0;

   if(disp==ON)
      disp= 0x04;      //디스플레이 ON
   else
      disp= 0x00;
   
   if(cursor == ON);   //커서 ON
      curror= 0x02;
   else
      cursor= 0x00;

   if(blink==ON)      //커서 깜박임
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
      PORTC= 0x05;   //디스플레이 ON
   else if (dir==LEFT)
      PORTC= 0x06;
   else if(dir==NO)
      PORTC= 0x06;
   E_Pulse();
   ms_delay(1)
}

void initPort(void)
{
   DDRC = 0xff;      //데이터/명령 지정
   DDRG = 0xff;      //RS,RW,E 제어선 지정
}


unsigned int ADdata;
unsigned int ADVoltage;

char table1[16]="AT90CAN128";
char table2[16]="ADC Test";


/*================================================================================
AD 변환
PF0: ADC0 가변저항
PF1: ADC1 온도
PF2: ADC2 CDS
PF3: ADC3 -
================================================================================*/
void initAdc(void)
{
   admux = 0x40;   //기준전압; 외부 캐퍼시터 가진AVcc(AREF 핀)
               //AC 변환 데이터 정렬; 오른쪽 정렬
               //AD 변환 채널 선택; PortF 0 핀

   ADCSRA= 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
               //ADC 클록 설정; XTAL의1/2(8MHz)

   DDRF = 0xf0;   //PortF[3..0] 입력으로 설정, PortF[7..4] 출력으로 설정
   DIDR0 = 0x0f;   //디지털 입력 불가 PortF[3..0]
   }

   STGNAL(ADC_vect)
   {
   ADdata=ADC;      //AD 변환 데이터를 ADdata 에 저장
   ADCSRA = 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
}

void ADcLCD(void)
{
   unsigned int Buff;
   unsigned char AD1000, AD100, AD10, AD1;

   ADVoltage= ADdata*50/1023;         //AD 변환 데이터를 전압으로 변환

   ADVoltage= ADdata/ 1000| 0x30;      //AD 변환 데이터 1000자리값 ASCII 코드화
   Buff = ADdata% 1000;
   AD100 = Buff / 100 | 0x30;         //AD 변환 데이터 100자리값 ASCII 코드화
   Buff = Buff % 100;
   AD100 = Buff / 10 | 0x30;         //AD 변환 데이터 10자리값 ASCII 코드화
   AD1 = Buff % 10;               //AD 변환 데이터 1자리값 ASCII 코드화

   LCD_Disp(10,1);                  //LCD 두번째 줄에 AD 변환 데이터 표시
   Write_Char(AD1000);
   LCD_Disp(11,1);
   Write_Char(AD100);
   LCD_Disp(12,1);
   Write_Char(AD10);
   LCD_Disp(13,1);
   Write_Char(AD1);

   AD10= ADVoltage / 10 | 0x30;       //전압 10자리값 ASCII 코드화
   AD1= ADVoltage % 10 | 0x30;       //전압 1자리값 ASCII 코드화

   LCD_Disp(10,0);                  // LCD 첫번째 줄에 전압으로 표시
   Write_Char(AD10);               // 전압 1자리값
   LCD_Write(11,0,".");            // 소수점 포시
   LCD_Disp(12,0);
   Write_Char(AD1);               // 소수점 이하 첫번째 값 표시
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
   initPort();                     //입출력 포트 초기화
      initLCD();                  //LCD 초기화
      initAdc();                  //AD 변환 초기화

      sei();                     //INT 인에이블 clear = 작동안됨

      LCD_cmd(0x01);               //LCD 화면 지우기
      ms_delay(50);               //지연

      LCD_Write(0,0, table1);         //초기 글자 출력
      LCD_Write(0,1, table2);
      ms_delay(200);

      LCD_cmd(0x01);               //LCD 화면 지우기
      ms_delay(100);

      LCD_Write(0,0,"Coltage :   V");//전압 표시
      LCD_Write(0,1, "ADC Data:");   //AD 변환 데이터 표시
      while(1)
      {
         ADcLCD();               //AD 변환 데이터와 전압값 LCD에 표시
      }

}
