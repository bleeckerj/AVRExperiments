  unsigned char PS2byte;
 extern unsigned char PS2data4;
 extern unsigned char PS2data5;
  unsigned char i;

#define PS2clk 3
#define PS2cmd 2
#define PS2att 1
#define PS2dat 0
#define PS2PORT PORTB
#define PS2IN PINB


void ReadGamepad();
