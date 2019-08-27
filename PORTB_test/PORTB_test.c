#include<avr/io.h>

int main()
{
   DDRB = 0xff;
   PORTB = 0xff;
   DDRB = 0xf0;
}
