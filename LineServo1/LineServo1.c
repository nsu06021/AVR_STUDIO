#include <avr/io.h>
#include <avr/interrupt.h>

#define SI_1 (PORTF |= 0x02)
#define SI_0 (PORTF &= 0xFD)
#define CLK_1 (PORTF |= 0x01)
#define CLK_0 (PORTF &= 0xFE)
#define CLK_WIDTH     10
#define SI_WIDTH    1000

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



void initADC(void)
{
   ADMUX = 0x42;   //��������; �ܺ� ĳ�۽��� ����AVcc(AREF ��)
               //AC ��ȯ ������ ����; ������ ����
               //AD ��ȯ ä�� ����; PortF 0 ��

   ADCSRA= 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�
               //ADC Ŭ�� ����; XTAL��1/2(8MHz)

   //DDRF = 0x03;   //PortF[3..0] �Է����� ����, PortF[7..4] ������� ����
   //DIDR0 = 0x0f;   //������ �Է� �Ұ� PortF[3..0]
   
}



SIGNAL(ADC_vect)
   {
      //int i = 0; 

      ADdata=ADC;      //AD ��ȯ �����͸� ADdata �� ����
     //ADCLCD();
     
     /*line[i] = ADdata;
     i++;
     if(i>127){
        i=0;
      }*/
     
      ADCSRA = 0xc8;   //ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺�

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
   
   for(i=0;i<128;i++){
      CLK_1;
      usDelay(CLK_WIDTH );
      CLK_0;
      usDelay(CLK_WIDTH );
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
   /*if((line[122]+line[123]+line[124]+line[125]+line[126])/5<500)
   {
      OCR1B = 2800;
   }
   if((line[1]+line[2]+line[3]+line[4]+line[5])/5<500)
   {
      OCR1B = 1800;
   }*/
   	if((line[1]+line[2]+line[3]+line[4]+line[5])/5<500)
    {
	   OCR1B = 1800;
	}	
	if((line[122]+line[123]+line[124]+line[125]+line[126])/5<500)
    {
	   OCR1B = 2800;
	}
	
	/*else if((line[6]+line[7]+line[8]+line[9]+line[10])/5<500)
	{ 
	   msDelay(1);
	   OCR1B = 1800;
	}
	else if((line[117]+line[118]+line[119]+line[120]+line[121])/5<500)
	{
	   msDelay(1);
       OCR1B = 2800;
	}
   else
   {
      OCR1B = 2200;
   }*/
   
}

void init_Port(void)
{
   DDRF = 0xf3;
   DDRB = 0xff;
   PORTB = 0x30;
}

int main(void)
{

     //initADC();               //AD ��ȯ �ʱ�ȭ
     
	 init_Port();                //����� ��Ʈ �ʱ�ȭ
	 msDelay(100);
	 init_TIMER1();
	 msDelay(100);

     CLK_0;
     SI_0;
     Refresh();
     SI_0;
     


     sei();                     //INT �ο��̺� clear = �۵��ȵ�


     while(1)
     {
       ConShot();
      //msDelay(1000);
                     //AD ��ȯ �����Ϳ� ���а� LCD�� ǥ��
     }

}
//2019.6.5.������ : line+servo+DC - ���� ���� ������ �ʾ���. - fail
