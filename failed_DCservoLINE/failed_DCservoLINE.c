#include <avr/io.h>
#include <avr/interrupt.h>

#define SI_1 (PORTF |= 0x02)
#define SI_0 (PORTF &= 0xFD)
#define CLK_1 (PORTF |= 0x01)
#define CLK_0 (PORTF &= 0xFE)
#define CLK_WIDTH     10
#define SI_WIDTH    1000

unsigned int ADdata;
unsigned int line[128];

void us_delay(unsigned int us)
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

void ms_delay(unsigned int ms)
{
   unsigned int i;

   for(i=0;i<ms;i++)
      us_delay(1000);
}

void initADC(void)
{
   ADMUX = 0x42;   //기준전압; 외부 캐퍼시터 가진AVcc(AREF 핀)
               //AC 변환 데이터 정렬; 오른쪽 정렬
               //AD 변환 채널 선택; PortF 0 핀

   ADCSRA= 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
               //ADC 클록 설정; XTAL의1/2(8MHz)
}

SIGNAL(ADC_vect)
{
   ADdata=ADC;      //AD 변환 데이터를 ADdata 에 저장
     
   ADCSRA = 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
}

void Refresh()
{
   int i;

   SI_1;
   us_delay(SI_WIDTH);
   CLK_1;
   us_delay(SI_WIDTH);
   SI_0;
   us_delay(SI_WIDTH);
   CLK_0;

   for(i=0;i<150;i++){
      CLK_1;
      us_delay(CLK_WIDTH );
      CLK_0;
      us_delay(CLK_WIDTH );
   }
   SI_1;
   us_delay(SI_WIDTH);
}

void init_TIMER1(void)
{
   TCCR1A = 0xA2;   //0x22
   TCCR1B = 0x1A;
   TCCR1C = 0; 
   TCNT1 = 0;
   OCR1A = 00000;  //75%~
   OCR1B = 2200;   //2800(L)~2200(C)~1800(R)
   ICR1 = 40000;
   TIFR = 0x00;
   TIMSK = 0x00;
   ETIMSK = 0x00;
   ETIFR = 0x00;
}

void ConShot()
{
   int i;

    //msDelay(1000);

   SI_0;
   CLK_0;
   us_delay(SI_WIDTH);
   SI_1;
   us_delay(SI_WIDTH);
   CLK_1;
   us_delay(CLK_WIDTH );
   SI_0;
   us_delay(CLK_WIDTH );
   CLK_0;
   

   for(i=0;i<250;i++){
      CLK_1;
      us_delay(CLK_WIDTH );
      CLK_0;
      us_delay(CLK_WIDTH );
      line[i] = ADdata;
   }
   
   if((line[122]+line[123]+line[124]+line[125]+line[126])/5<500)
   {
      OCR1B = 2800;
   }
   if((line[1]+line[2]+line[3]+line[4]+line[5])/5<500)
   {
      OCR1B = 1800;
   }
  
   
}

void init_Port(void)
{
   DDRB = 0x38;
   PORTB = 0x30;
   DDRB = 0x40;
   DDRF = 0xf3;
}

int main(void)
{
   init_Port();
   //ms_delay(100);
   init_TIMER1();
   initADC();

   CLK_0;
   SI_0;
   Refresh();
   SI_0;
     
   sei();                     //INT 인에이블 clear = 작동안됨

  // ms_delay(100);

   while(1)
   {
       ConShot();
    
   }
}
