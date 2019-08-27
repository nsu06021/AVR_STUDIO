#include <avr/io.h>
#include <avr/interrupt.h>

#define LCD_RS_1 (PORTG |= 0x01)      //데이터 입출력
#define LCD_RS_0 (PORTG &= 0xFE)      //명령 입출력



#define EN_1    (PORTG |= 0x04)      //펄스 1
#define EN_0    (PORTG &= 0xFB)      //펄스 0

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
int i=0;

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




void init_TIMER1(void)
{
   TCCR1A = 0x22;   // TCCR1A =0x82;
   TCCR1B = 0x1A;
   TCCR1C = 0; 
   TCNT1 = 0;
   OCR1B = 2200;   //2800(L)~2200(C)~1800(R)
   ICR1 = 40000;
   TIFR = 0x00;
   TIMSK = 0x00;
   ETIMSK = 0x00;
   ETIFR = 0x00;
}


void init_TIMER2(void)
{
   TCCR1A = 0x82;
   TCCR1B = 0x1A;
   TCCR1C = 0; 
   TCNT1 = 0;
   OCR1A = 85;//3400~2600~   4만
   ICR1 = 100;
   TIFR = 0x00;
   TIMSK = 0x00;
}

void init_TIMER3(void)
{
   TCCR3A = 0x82;
   TCCR3B = 0x1A;
   TCCR3C = 0; 
   TCNT3 = 0;
   OCR3A = 2600;   //3400~2600~
   ICR3 = 40000;
   TIFR = 0x00;
   TIMSK = 0x00;
   ETIMSK = 0x00;
   ETIFR = 0x00;
}

void initADC(void)
{
   ADMUX = 0x42;   //기준전압; 외부 캐퍼시터 가진AVcc(AREF 핀)
               //AC 변환 데이터 정렬; 오른쪽 정렬
               //AD 변환 채널 선택; PortF 0 핀

   ADCSRA= 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블
               //ADC 클록 설정; XTAL의1/2(8MHz)

    DDRF = 0xf0;  //DDRF = 0x03;   //PortF[3..0] 입력으로 설정, PortF[7..4] 출력으로 설정
   //DIDR0 = 0x0f;   //디지털 입력 불가 PortF[3..0]
   
}

SIGNAL(ADC_vect)
   {
      //int i = 0; 

      ADdata=ADC;      //AD 변환 데이터를 ADdata 에 저장
     //ADCLCD();
     
     /*line[i] = ADdata;
     i++;
     if(i>127){
        i=0;
      }*/
     
      ADCSRA = 0xc8;   //ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블

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
   /*if(line[40]<700||line[44]<700||line[64]<700||line[84]<700||line[88]<700)
   {
      OCR1B = 2200;
   }*/
   /*if(line[30]<700||line[40]<700||line[50]<700)
   {
      OCR1B = 2000;
   }*/
   /*if(line[78]<700||line[88]<700||line[98]<700)
   {
      OCR1B = 2400;
   }*/
   if((line[121]+line[122]+line[123]+line[124]+line[125]+line[126])/6 <500)
   {
      OCR1B = 2800;
   }
   if((line[1]+line[2]+line[3]+line[4]+line[5]+line[6])/6 <500)
   {
      OCR1B = 1800;
   }
   /*else
   {
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
   DDRF = 0xf0;     // 0xf3
   DDRB = 0xff;     // 0x60;
   PORTB = 0x60;
   DDRE = 0xff;
}

int main(void)
{
  

   init_Port();                     //입출력 포트 초기화
   initADC();                  //AD 변환 초기화
   init_TIMER1();
   init_TIMER3();


     CLK_0;
     SI_0;
     Refresh();
     SI_0;
     


      sei();                     //INT 인에이블 clear = 작동안됨

      msDelay(100);
      init_TIMER2();
      msDelay(50); 
      

     //ConShot();
     //ADCLCD();

      while(1)
      {
        if(ADdata >= 600)
        OCR1A = 30;

        ConShot();
       //msDelay(1000);
     }
}
