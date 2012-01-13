unsigned char PS2byte;
unsigned char PS2data4;
unsigned char PS2data5;
unsigned char PS2data6;
unsigned char i;
unsigned char ControllerType;

#include <avr/io.h>
#include "avrlibdefs.h"
#include "avrlibtypes.h"
//#include "delays.h"
#include <util/delay.h>

#define PS2clk PA3
#define PS2cmd PA4
#define PS2att PA6
#define PS2dat PA5
#define PS2PORT PORTA
#define PS2IN PINA
#define CON_ack PB0
#define CON_ACK_PORT PORTB

#define SET(x,y) (x|=(1<<y))
#define CLR(x,y) (x&=(~(1<<y)))
#define CHK(x,y) (x & (1<<y))
#define TOG(x,y) (x^=(1<<y)) 

int main(void) {
	sbi(DDRA, PS2att); // output
	sbi(DDRA, PS2clk); // output
	sbi(DDRA, PS2cmd); // output
    sbi(DDRB, CON_ack); // output

	cbi(DDRA, PS2dat); // input


	sbi(PS2PORT, PS2att); // disable joystick

	while(1==1) {
		ReadGamepad();
		_delay_ms(1);
    }
}

void ReadGamepad()
{
   sbi(CON_ACK_PORT, CON_ack); // set the ACK signal
   SET(PS2PORT,PS2cmd); 
   SET(PS2PORT,PS2clk); 
   CLR(PS2PORT,PS2att); // low enable joystick

   _delay_ms(1);
   PS2byte=0x01; // first command
   for(i=0;i<8;i++)
   {
      if(CHK(PS2byte,i)) {
	  	SET(PS2PORT,PS2cmd);
	  }
	  else  {
	  	CLR(PS2PORT,PS2cmd);
	  }
      CLR(PS2PORT,PS2clk); 
	  _delay_ms(1); 
      SET(PS2PORT,PS2clk); 
   }
   SET(PS2PORT,PS2cmd); 

   _delay_ms(1);
   PS2byte=0x42; // sec command
   ControllerType = 0x00;
   for(i=0;i<8;i++)
   {
      if(CHK(PS2byte,i)) {
	  	SET(PS2PORT,PS2cmd);
	  }
	  else  {
	  	CLR(PS2PORT,PS2cmd);
      }
	  CLR(PS2PORT,PS2clk); 
	  _delay_ms(1);
      SET(PS2PORT,PS2clk); 
	  if(bit_is_set(PINA, PS2dat)) {
		SET(ControllerType, i);
	  }
	  SET(PS2PORT, PS2clk);
   }
   SET(PS2PORT,PS2cmd); 

   _delay_ms(1);
   for(i=0;i<8;i++)  // 3 byte
   {
      CLR(PS2PORT,PS2cmd);

      CLR(PS2PORT,PS2clk); //Delay(1); 
      SET(PS2PORT,PS2clk); 
   }
   SET(PS2PORT,PS2cmd); 

   _delay_ms(1);
   PS2data4=0;
   for(i=0;i<8;i++)  // 4 byte
   {
      CLR(PS2PORT,PS2cmd);
      CLR(PS2PORT,PS2clk); _delay_ms(1);
	  if(CHK(PS2IN,PS2dat)) {
	  	SET(PS2data4,i); 
	  }
      SET(PS2PORT,PS2clk); 
   }
   SET(PS2PORT,PS2cmd); 

   _delay_ms(1);
   PS2data5=0;
   for(i=0;i<8;i++) // 5 byte
   {
      CLR(PS2PORT,PS2cmd);
      CLR(PS2PORT,PS2clk); _delay_ms(1); 
	  if(CHK(PS2IN,PS2dat)) SET(PS2data5,i);
      SET(PS2PORT,PS2clk); 
   }
     _delay_ms(1);
   PS2data6=0;
   for(i=0;i<8;i++) // 6 byte
   {
      CLR(PS2PORT,PS2cmd);
      CLR(PS2PORT,PS2clk); _delay_ms(1); 
	  if(CHK(PS2IN,PS2dat)) SET(PS2data6,i);
      SET(PS2PORT,PS2clk); 
   }
   SET(PS2PORT,PS2cmd); 
   SET(PS2PORT,PS2att); // HI disable joystick
}
