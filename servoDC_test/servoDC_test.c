#include <avr/io.h>
#include <avr/interrupt.h>

#define SI_1 (PORTF |= 0x02)
#define SI_0 (PORTF &= 0xFD)
#define CLK_1 (PORTF |= 0x01)
#define CLK_0 (PORTF &= 0xFE)
#define CLK_WIDTH    10
#define SI_WIDTH     1000

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
   ADMUX = 0x42;   //��������; �ܺ� ĳ�۽��� ����AVcc(AREF ��)
               //AC ��ȯ ������ ����; ������ ����
               //AD ��ȯ ä�� ����; PortF 0 ��

   ADCSRA= 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
               //ADC Ŭ�� ����; XTAL��1/2(8MHz)
}

void init_redADC(void)
{
   ADMUX = 0x43;   //��������; �ܺ� ĳ�۽��� ����AVcc(AREF ��)
               //AC ��ȯ ������ ����; ������ ����
               //AD ��ȯ ä�� ����; PortF 0 ��

   ADCSRA= 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
               //ADC Ŭ�� ����; XTAL��1/2(8MHz)
}

void init_EXT_INT(void)
{
   EICRB = 0xc0;
   EIMSK = 0x80;
   EIFR = 0x80;
}



SIGNAL(ADC_vect)
{
   ADdata=ADC;      //AD ��ȯ �����͸� ADdata �� ����
     
   ADCSRA = 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
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
   OCR1A = 40000;  //75%~
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
   

   for(i=0;i<128;i++){
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
   /*else
   {
      OCR1B = 2200;
   }*/
   
}

void init_Port(void)
{
   DDRB = 0x78;
   PORTB = 0x30;
   //DDRB = 0x58;
   DDRF = 0xf3;
}

int main(void)
{
   init_Port();
   ms_delay(100);
   init_TIMER1();
   initADC();
   

   sei();                     //INT �ο��̺� clear = �۵��ȵ�

   CLK_0;
   SI_0;
   Refresh();
   SI_0;
     
   

  // ms_delay(100);

   while(1)
   {
       ConShot();
	  
	   ADMUX = 0x43;
	   ADCSRA= 0xc8;
	   
	   us_delay(100);
       
       
	   if(ADdata >= 600)
       {
	     //OCR1A = 0;
		 PORTB = 0x18;
	      
	   }
	   
       else
	   {
	      
	      ADMUX = 0x42;
		  ADCSRA= 0xc8;

		  us_delay(100);
	   }

	   /*if(PORTB = 0x18)//OCR1A == 0)
	   {
	      OCR1B = 2200;
		  return 0;   
	   }*/
   }
}

