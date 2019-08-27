#include<avr/io.h>
#include<avr/interrupt.h>

#define PE4_Clear     (PORTE &= 0xEF)
#define PE4_Set       (PORTE |= 0x10)
#define PE4_IN        (PORTE &= 0xEF)
#define PE4_OUT       (PORTE |= 0x10)

#define LCD_RS_1 (PORTG |= 0x01)    // 데이터 입출력
#define LCD_RS_0 (PORTG &= 0xFE)    // 명령 입출력

#define LCD_RW_1 (PORTG |= 0x02)    // 읽기
#define LCD_RW_0 (PORTG &= 0xFD)    // 쓰기

#define EN_1 (PORTG |= 0x04)        //펄스 1
#define EN_0 (PORTG &= 0xFB)        //펄스 0

#define TEMPERATURE 25

unsigned short tick=0, pulse_check=0, pulse_end=0;

void us_delay(unsigned int us_time)
{
   unsigned int i;

   for(i=0; i<us_time; i++) {
      asm("PUSH R0");    
      asm("POP R0");
      asm("PUSH R0");
      asm("POP R0");
      asm("PUSH R0");
      asm("POP R0");
   }
}

void ms_delay(unsigned int ms_time)
{
   unsigned int i;

   for(i=0;i<ms_time;i++)
      us_delay(1000);
}

//===========================================
//LCD display
void E_Pulse(void)
{
   EN_1;

   us_delay(100);    // 약 100usec 지연

   EN_0;
}

void LCD_init(void)
{
   ms_delay(40);

   PORTC = 0x38;    // Function Set
   E_Pulse();
   us_delay(40);

   PORTC = 0x0c;    // Display ON/OFF Control
   us_delay(40);
   E_Pulse();

   PORTC = 0x01;    // Display Clear
   ms_delay(2);
   E_Pulse();

   PORTC = 0x06;    // Entry Mode Set
   E_Pulse();
}

void Write_Char(unsigned char buf)
{
   LCD_RS_1;     // 문자 입출력 모드 설정
   LCD_RW_0;     // 쓰기 모드 설정
   PORTC=buf;    //문자 출력
   E_Pulse();    //펄스 생성 
}

void LCD_cmd(unsigned char cmd)
{
   LCD_RS_0;
   LCD_RW_0;
   PORTC=cmd;
   E_Pulse();
}

void LCD_Disp(char x, char y)
{
   LCD_RS_0;    // 명령 모드 설정
   LCD_RW_0;    //쓰기 모드 설정

   if(y==0)
      PORTC = x + 0x80;   // 1번째 행
   else if(y==1)
      PORTC = x + 0xc0;   // 2번째 행
   E_Pulse();  
}

void LCD_Write(char x, char y, char *str)
{
   LCD_Disp(x,y);    // x열, y행으로 이동
   while(*str)
   Write_Char(*str++);   // 문자 출력 
}


//=================================================
// ultra sound
void init_EXT_INT(void)
{
   EIMSK |= (1<<INT4);
   EICRB |= (1<<ISC41)|(1<<ISC40);
   EIFR = (1<<INT4);
}

void init_TIMER0_COMPA(void)
{
   TCCR0A |= (1<<WGM01);     // CTC Mode
   TCCR0A |= (1<<CS01);      // clk/8 Prescaler
   TCNT0 = 0;
   OCR0A = 19;
   TIMSK0 = (1<<OCIE0A);
   TIFR0 = 0x00;
}

SIGNAL(TIMER0_COMP_vect)
{
tick++;
}

SIGNAL(INT4_vect)
{
unsigned short pulse_tick;

pulse_tick = tick;

if(EICRB & 0x03)
{
   EICRB &= 0x00;
   tick = 0;
}
else
{
EICRB |= (1<<ISC41)|(1<<ISC40);
pulse_end = pulse_tick;
}
}

void initPort (void)
{
DDRC = 0xff;
DDRG = 0xff;
DDRE = 0xef;
PORTE = 0x10;
}

int main(void)
{
unsigned char thousand, hundred, ten, one;
float distance;

initPort();
LCD_init();

init_EXT_INT();
init_TIMER0_COMPA();

LCD_cmd(0x01);
ms_delay(50);

LCD_Write(0,0,"HYbus");
LCD_Write(0,1,"Distance:");
LCD_Write(13,1,"mm");
ms_delay(50);

while(1)
   {
   cli();
   PE4_OUT;
   us_delay(500);

   PE4_Set;
   us_delay(5);

   PE4_Clear;
   us_delay(100);

   PE4_IN;
   us_delay(100);

   sei();

   distance=/*(331.5+(0.6*TEMPERATURE))*/299792458*(pulse_end*0.00000000001/2)*1000;

   thousand = distance/1000;
   distance = distance - (thousand * 1000);

   hundred = distance / 100;
   distance = distance - (hundred * 100);

   ten = distance/10;
   distance = distance - (ten*10);

   one = distance;

   LCD_Disp(9,1);
   Write_Char(thousand+48);
   LCD_Disp(10,1);
   Write_Char(hundred+48);
   LCD_Disp(11,1);
   Write_Char(ten+48);
   LCD_Disp(12,1);
   Write_Char(one+48);

   ms_delay(50);
   }
   return 0;
}
