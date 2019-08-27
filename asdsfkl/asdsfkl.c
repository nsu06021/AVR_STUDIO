#include <avr/io.h>

void usDelay (unsigned int us)
{

unsigned int i;

	for(i=0; i<us; i++)
	{

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

for (i=0; i<ms; i++)
usDelay(1000);

}



void initport(void)
{
DDRA = 0xff;

int i;
unsigned char led = 0xfe;


}



void main (void)
{

	DDRA = 0xff;


	unsigned char led = 0xfe;


		while(1)

		{int i =0;

	
			PORTA=~led;
			msDelay(1000);
			
		
			if(led==0x7f)
			{
		
			led = 0xfe;
	
			}
			else{
			
			led=(led<<1)+1;

			}
			

		
		
		msDelay(100);

		}
		

		
		
		



}


















