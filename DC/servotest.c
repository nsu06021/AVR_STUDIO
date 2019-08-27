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
   TCCR1A = 0x82;
   TCCR1B = 0x1A;
   TCCR1C = 0; 
   TCNT1 = 0;
   OCR1A = 80;//3400~2600~   4¸¸
   ICR1 = 100;
   TIFR = 0x00;
   TIMSK = 0x00;
}

void init_Port(void)
{
   DDRB = 0x38;
   PORTB = 0x30;
}

int main(void)
{
   unsigned int i;

   init_Port();
   
   //PORTB = 0x30;
   ms_delay(100);
   init_TIMER1();
   
   /*while(1)
   {
      for(i=100; i>=75; i--)
	  {
	     OCR1A = i;
		 ms_delay(100);
	  }
      
	  ms_delay(1000);

	  for(i=75; i<=100; i++)
	  {
	     OCR1A = i;
		 ms_delay(100);
	  }
   }*/
}
