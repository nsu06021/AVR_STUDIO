#include <avr/io.h>
#include <avr/interrupt.h>

asm("equ __lcd_port = 0x15");      // PORTC
//#endasm
//#include   <lcd.h>

#define ADC_VREF_TYPE 0x00

#define EN_1    (PORTG |= 0x04)      //펄스 1
#define EN_0    (PORTG &= 0xFB)      //펄스 0

#define LCD_RS_1 (PORTG |= 0x01)      //데이터 입출력
#define LCD_RS_0 (PORTG &= 0xFE)      //명령 입출력

#define LCD_RW_1 (PORTG |= 0x02)      //읽기
#define LCD_RW_0 (PORTG &= 0xFD)      //쓰기

char sbuf[60];

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

void delay_ms(unsigned int ms)
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

void lcd_init(void)
{
   delay_ms(40);

   PORTC =0x38;      //Function Set
   E_Pulse();
   usDelay(40);

   PORTC = 0x0c;      //DisPlay ON/OFF Control
   usDelay(40);
   E_Pulse();

   PORTC =  0x01;      //Display Clear
   delay_ms(2);
   E_Pulse();

   PORTC = 0x06;      //Entry Mode Set
   E_Pulse();
}

void lcd_gotoxy(char x,char y)
{
   LCD_RS_0;         //명령 모드 설정
   LCD_RW_0;         //쓰기 모드 설정

   if(y==0)
      PORTC=x+0x80;   //1번째 행
   else if(y==1)
      PORTC=x+0xc0;   //2번째 행
   E_Pulse();
}

void lcd_puts(unsigned char buf)
{
   LCD_RS_1;         //문자 입출력 모드 설정
   LCD_RW_0;         //쓰기 모드 설정
   PORTC=buf;         //문자 출력
   E_Pulse();         //펄스 생성
}

void LCD_Write(char x, char y, char *str)
{
   lcd_gotoxy(x,y);      //x열, y행으로 이동
   while(*str)
   lcd_puts(*str++);   //문자 출력
}

unsigned int read_adc(unsigned char adc_input) // ---A/D 변환 후 결과 출력 함수 -----
{
   ADMUX=adc_input|ADC_VREF_TYPE;      // Start the AD conversion
   ADCSRA|=0x40;                       // ADC freerunning

   while ((ADCSRA & 0x10)==0);         // Wait for the AD conversion to complete
   ADCSRA|=0x10;
   return ADCW;
}


int main(void)
{
   double Dist;             // 거리값 저장 변수
   float fVolt;            // 전압값 저장 변수

   ADMUX=ADC_VREF_TYPE;     // ADC초기화 : 125.000 kHz, AREF pin
   ADCSRA=0x87;

   lcd_init();

   while(1)
   {
      fVolt = read_adc(0)   * (5.0/1024.0);         // 전압값 계산
      Dist = (9.06 / (fVolt*fVolt)) - (6.63*fVolt) + 27.3; // 거리계산

      sprintf(sbuf, "Dist = %2.2f", Dist);      // LCD : "Dist = 거리값" 출력
      lcd_gotoxy(0,0);
      lcd_puts(*sbuf);

      sprintf(sbuf,"ADC = %d",(int) fVolt);    // LCD : "ADC = 전압값" 출력
      lcd_gotoxy(0,1);
      lcd_puts(*sbuf);
 
      delay_ms(100);
   }
}
