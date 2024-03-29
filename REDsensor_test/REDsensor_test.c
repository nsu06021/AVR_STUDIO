#include <avr/io.h>
#include <avr/interrupt.h>

#define LCD_RS_1 (PORTG |= 0x01)      //데이터 입출력
#define LCD_RS_0 (PORTG &= 0xFE)      //명령 입출력

#define LCD_RW_1 (PORTG |= 0x02)      //읽기
#define LCD_RW_0 (PORTG &= 0xFD)      //쓰기

#define EN_1    (PORTG |= 0x04)      //펄스 1
#define EN_0    (PORTG &= 0xFB)      //펄스 0

char table1[16] = "Hello MEGA";
char table2[16] = "From HyBus";

unsigned int ADdata;
unsigned int ADVoltage;

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

   usDelay(100);      //약 100usec 지연

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
   msDelay(1);      //지연
}



void initADC(void)
{
   ADMUX = 0x43;   //기준전압; 외부 캐퍼시터 가진AVcc(AREF 핀)
               //AC 변환 데이터 정렬; 오른쪽 정렬
               //AD 변환 채널 선택; PortF 0 핀

   ADCSRA= 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
               //ADC 클록 설정; XTAL의1/2(8MHz)

   DDRF = 0xf0;   //PortF[3..0] 입력으로 설정, PortF[7..4] 출력으로 설정
   //DIDR0 = 0x0f;   //디지털 입력 불가 PortF[3..0]
}

SIGNAL(ADC_vect)
   {
      ADdata=ADC;      //AD 변환 데이터를 ADdata 에 저장
      ADCSRA = 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
   }

void ADCLCD(void)
{
   unsigned int Buff;
   unsigned char AD1000, AD100, AD10, AD1;

   ADVoltage= ADdata*50/1023;         //AD 변환 데이터를 전압으로 변환

   AD1000= ADdata/ 1000| 0x30;      //AD 변환 데이터 1000자리값 ASCII 코드화
   Buff = ADdata% 1000;
   AD100 = Buff / 100 | 0x30;         //AD 변환 데이터 100자리값 ASCII 코드화
   Buff = Buff % 100;
   AD10 = Buff / 10 | 0x30;         //AD 변환 데이터 10자리값 ASCII 코드화
   AD1 = Buff % 10 | 0x30;               //AD 변환 데이터 1자리값 ASCII 코드화

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

void init_Port(void)
{
   DDRC = 0xff;
  // PORTC = 0xff;
   DDRG = 0xff;
   //DDRE = 0xef;
   //PORTE = 0x10;
   DDRF = 0xf0;
}

int main(void)
{
   init_Port();                     //입출력 포트 초기화
      initLCD();                  //LCD 초기화
      initADC();                  //AD 변환 초기화

      sei();                     //INT 인에이블 clear = 작동안됨

      LCD_cmd(0x01);               //LCD 화면 지우기
      msDelay(50);               //지연

      LCD_Write(0,0, table1);         //초기 글자 출력
      LCD_Write(0,1, table2);
      msDelay(200);

      LCD_cmd(0x01);               //LCD 화면 지우기
      msDelay(100);

      LCD_Write(0,0,"Voltage :   V");//전압 표시
      LCD_Write(0,1, "ADC Data:");   //AD 변환 데이터 표시
      while(1)
      {
         ADCLCD();               //AD 변환 데이터와 전압값 LCD에 표시
      }

}
