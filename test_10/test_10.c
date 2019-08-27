#include<avr/io.h>
#include<avr/interrupt.h>

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

void init_TIMER1(void)
{
   TCCR1A = 0x22;
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

void init_Port(void)
{
   DDRB = 0xff;
}

int main(void)
{
   unsigned int i, d;

   init_Port();
   init_TIMER1();

   ms_delay(100);

   while(1)
   {
      

      for(i=1800; i<=2800; i=i+100)
	  {
	     OCR1B = i;
		 ms_delay(100);
	  }

	  for(d=2800; d>=1800; d=d-100)
	  {
	     OCR1B = d;
		 ms_delay(100);
	  }
	 
   }
}
