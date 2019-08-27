#include<avr/io.h>
//#include "../common/Delay.h"


//#define AO PORTE
#define SI_1 (PORTE |= 0x02)
#define SI_0 (PORTE &= 0xFD)
#define CLK_1 (PORTE |= 0x01)
#define CLK_0 (PORTE &= 0xFE)
#define CLK_WIDTH     10
#define SI_WIDTH 1000

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

void ConShot(){
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
	}
}

void main(){
    DDRE=0xff;

	CLK_0;
	SI_0;

	Refresh();

	SI_0;

	while(1){

/*		CLK_0;
		ms_delay(10);
		CLK_1;
		ms_delay(10);*/
		ConShot();
		


		/*ms_delay(10);
		for(i=0;i<129;i++){
			CLK_1;
			us_delay(10);
			CLK_0;
			us_delay(10);
		}
		ms_delay(20);*/
	}

}
