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


void init_TIMER3(void)
{
   TCCR3A = 0x82;
   TCCR3B = 0x1A;
   TCCR3C = 0; 
   TCNT3 = 0;
   OCR3A = 5000;   //3400~2600~
   ICR3 = 40000;
   TIFR = 0x00;
   TIMSK = 0x00;
   ETIMSK = 0x00;
   ETIFR = 0x00;
}

void init_Port(void)
{
   DDRE = 0xff;
}

int main(void)
{
   unsigned int i;

   init_Port();
   init_TIMER3();

   /*for(i=2500; i>=1800; i=i-50)
   {
      OCR3A = i;
	  ms_delay(100);
   }*/

}
 
