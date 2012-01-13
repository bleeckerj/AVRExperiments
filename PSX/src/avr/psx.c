/**
 *  $Id$
 *
 */

#define F_CPU 20000000


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include "delay_x.h"

#include "psx.h"

#define ATMEGA_324

#ifdef ATMEGA_324
// CON_ATT_3V3 is on PB4 in the "white-wired" v03 board
#define CON_ATT_3V3 PB4 // input from the console, was PCINT12, now SS_
#define CON_ATT_PORT PINB
#define CON_ACK_3V3 PB0 // output
#define CON_ACK_PORT PORTB
#define CON_DDR DDRB
#define MISO PB6
//#define CON_CMD_3V3 PC7 // input
//#define CON_CMD_PORT PINC
//#define CON_DATA_3V3 PC6 // output
//#define CON_DATA_PORT PORTC
//#define CON_CLK_3V3 PA7 // input
//#define CON_CLK_PORT PINA

#define LED_ON cbi(LED_PORT, LED) // LEDs active low on the STK500
#define LED_OFF sbi(LED_PORT, LED)

#define PSX_CLK_3V3 PA3 // output to the PSX (controller)
#define PSX_CLK_PORT PORTA
#define PSX_CMD_3V3 PA4 // output to the PSX, bits for commands runs down this
#define PSX_CMD_PORT PORTA
#define PSX_DATA_3V3 PA5 // input from the PSX, bits for data from the PSX runs down this
#define PSX_DATA_PORT PINA
#define PSX_ATT_3V3 PA6 // output to the PSX, attention signal, just goes high or low to force the controller to pay attention to a data transaction
#define PSX_ATT_PORT PORTA
#define PSX_DDR DDRA

//#define PSX_ATT_3V3 ?? this would be input from the PSX

#define LED PD0
#define LED_PORT PORTD
#define LED_DDR DDRD
//#define MINIMUM_US_DELAY 4 // dependent on F_CPU, basically 3 clock cycles + a bit
//#define MAXIMUM_MS_DELAY 33 // dependent on F_CPU

#elif defined ARDUINO
#define CON_ATT_3V3 PB2 // SS_ on the Atmega168
#define CON_ATT_PORT PINB
#define CON_ACK_3V3 PB1 // output
#define CON_ACK_PORT PORTB
#define CON_DDR DDRB
#define MISO PB4

#define PSX_CLK_3V3 PC0 // output
#define PSX_CLK_PORT PORTC // output
#define PSX_CMD_3V3 PC1 // output
#define PSX_CMD_PORT PORTC // output
#define PSX_DATA_3V3 PC2 // input
#define PSX_DATA_PORT PINC // input
#define PSX_ATT_3V3 PC3 // output
#define PSX_ATT_PORT PORTC // output
#define PSX_DDR DDRC

#define LED PC0
#define LED_PORT PORTC
#define LED_DDR DDRC
#endif

#define STOP 0b00000000
#define START 0x01
// game controller command defines
#define START_CMD 0x01
#define SET_DIGITAL_MODE_AND_LOCK_CMD 0x00
#define SET_ANALOG_MODE_AND_LOCK_CMD 0x01
#define READ_GAME_CONTROLLER_CMD 0x42
#define VIBRATION_ENABLE_CMD 0x4D
#define VIBRATION_DISABLE_CMD 0x4D

#define DIGITAL_MODE_ID 0x41
#define ANALOG_MODE_ID 0x73
// with overhead of sending/receiving data, etc.
// a CLK_DELAY of 5us results in an effecitve clock period of approx XXXXX
// an entire 9 byte transaction takes about 1ms
// a CLK_DELAY of 2us results in an effective clock period of approx 6.8us, pulse width of 2.89us
// an entire 9 byte transaction takes about 716us, with intertrain delay of 1x CLK_DELAY
// a CLK_DELAY of 1us results in an effective clock period of approx 4.5us, pulse width of 1.75us and
// an entire 9 byte transaction takes about 536us, with intertrain delay of 1x CLK_DELAY
#define CLK_DELAY 5

// time to wait between 8 bits of data
#define CLK_INTERTRAIN_DELAY CLK_DELAY*4

// us (microseconds) to delay after pulling the attention line active low
#define ATT_DELAY 30

// us (microseconds) to delay after pulling the acknowledge line active low, and before bringing it back high
#define ACK_DELAY 2

u08 mState;
u08 mControllerMode;
u08 mControllerReadData_2, mControllerReadData_3, mControllerReadData_4;
u08 mControllerReadData_5, mControllerReadData_6;
u08 mControllerReadData_7, mControllerReadData_8;
u08 mControllerReadData_9;

// PCINT7:0 -> PCMSK0 and PCINT0_vect
// PCINT15:8 -> PCMSK1 and PCINT1_vect or SIG_PIN_CHANGE1
// PCINT23:16 -> PCMSK2 and PCINT2_vect
// PCINT31:24 -> PCMSK3 and PCINT3_vet


int main() {
  //  u08 count = 0;
  u08 data;
  setup();

  // put the controller into analog mode

  /*
  GameControllerCommand_ConfigModeEnter();
  GameControllerCommand_SetMode(SET_ANALOG_MODE_AND_LOCK_CMD, TRUE);
  GameControllerCommand_VibrationEnable();
  GameControllerCommand_ConfigModeExit();
  */

  //  sbi(LED_PORT, LED);
  //  delay_1s();
  //  cbi(LED_PORT, LED);
#define DELAY __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); __asm__ ("nop" ::); 

  u08 i;
  //#define ATTENTION bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)
#define ATTENTION 1

  while(1==1) {
    while(bit_is_set(CON_ATT_PORT, CON_ATT_3V3));
    outb(SPDR, 0x00);
    // ONE
    while(!(SPSR & (1<<SPIF)) && ATTENTION);
    cbi(CON_ACK_PORT, CON_ACK_3V3);
    _delay_ms(1);
    sbi(CON_ACK_PORT, CON_ACK_3V3);      
    data = SPDR;
    if((u08)data == (u08)0x01 && ATTENTION) {
      outb(SPDR, 0x73);
      // TWO
      while(!(SPSR & (1<<SPIF)) && ATTENTION);
      cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
      sbi(CON_ACK_PORT, CON_ACK_3V3);      
      data = SPDR;
      if((u08)data == (u08)0x42 && ATTENTION) {
	outb(SPDR, 0x5A);
	// THREE
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
	data = SPDR;
	outb(SPDR, 0x5A);
	// FOUR
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
	outb(SPDR, 0x00);
	// FIVE
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
	outb(SPDR, 0x00);
	// SIX
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
	outb(SPDR, 0x00);
	// SEVEN
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
	outb(SPDR, 0x00);
	// EIGHT
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
	outb(SPDR, 0x00);
	// NINE
	while(!(SPSR & (1<<SPIF)) && ATTENTION);
	cbi(CON_ACK_PORT, CON_ACK_3V3);
      _delay_ms(1);
	sbi(CON_ACK_PORT, CON_ACK_3V3);      
      }
    }
  }

  while(1==0) {
    // delay_1s();
    if(bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)) {
      //      
      outb(SPDR,0x00);
      while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
      data = SPDR;
      _delay_us(5);
      if(data == 0x01) {
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(2);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	outb(SPDR,0x73);
	//	if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;
	while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	data = SPDR;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
    _delay_us(2);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	cbi(LED_PORT, LED);
	/*
	if(data == 0x00) {
	  cbi(LED_PORT, LED);
	}
	*/
	  SPDR = 0x5A;
	  while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	  
	  data = SPDR;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	  SPDR = 0xFF;
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	//SPDR = mControllerReadData_4;
	  while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	//	  if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;

  //	sendAcknowledge_Console();
	SPDR = 0xFF;
	//	SPDR = mControllerReadData_5;
	while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	//	  if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
  //	sendAcknowledge_Console();
	cbi(LED_PORT, LED);
	//	if(mControllerMode == ANALOG_MODE_ID) {
	SPDR = 0x00;
	//	SPDR = mControllerReadData_6;
	while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	//	  if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	//	sendAcknowledge_Console();
	
	SPDR = 0x00;
	//SPDR = mControllerReadData_7;
	while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	//	  if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	//	sendAcknowledge_Console();
	cbi(LED_PORT, LED);
	SPDR = 0x00;
	//SPDR = mControllerReadData_8;
	while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	//	  if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	//	sendAcknowledge_Console();
	
	SPDR = 0x00;
	//SPDR = mControllerReadData_9;
	while(!(SPSR & (1<<SPIF)) && (bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)));
	//	  if(bit_is_set(CON_ATT_PORT, CON_ATT_3V3)) continue;
      _delay_us(5);
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  _delay_us(5);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
	//	sendAcknowledge_Console();
	//	} // if analog mode id
	cbi(LED_PORT, LED);
	//wait until attention has gone back high??
	//while(bit_is_clear(CON_ATT_PORT, CON_ATT_3V3));
	//
	// read the game controller
	//GameControllerCommand_ReadData();
      }
    } else {
      cbi(LED_PORT, LED);
    }
  }

  //  
  return 0;
}


void delay_1s()
{
  u08 i;
  for(i=0; i<100; i++) {
    _delay_ms(10);
  }


}

void delay_500ms()
{
  u16 i;
  for(i=0; i<50; i++) {
    _delay_ms(10);
  }


}

void GameControllerCommand_ReadData()
{
  // clock high
  sbi(PSX_CLK_PORT, PSX_CLK_3V3);
  // cmd high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);
  // ATT low, notify controller we're going to read
  cbi(PSX_ATT_PORT, PSX_ATT_3V3);
  _delay_ms(1);
  //  _delay_us(30);
  // send 0x01 command, 1st byte
  sendCommand_GameController(START_CMD);

  // 2nd byte
  // send 0x42 as command 
  // data returned is the mode the controller is in 
  // modes: 
  // 0x41 is digital mode
  // 0x73 is analog mode
  // 0xF3 is DS2 native mode (dual shock 2)
  // we interpret and read data differently depending on what mode we are in
  // digital mode only has a total of 3 bytes more of data representing button state
  // whereas analog mode has 7 more bytes of data revealing button and joystick data
  mControllerMode = readDataSendCommand_GameController(READ_GAME_CONTROLLER_CMD);

  // read 7 more bytes (9 bytes total) of data if we're in analog mode

    // 3rd byte
    // read data, should be the mode changed value (0x00 mode changed, 0x5A no mode change)
    mControllerReadData_3 = readData_GameController();

    // 4th byte, in analog mode, this is 
    //      7      6      5       4     3      2      1      0
    //   LEFT    DOWN   RIGHT     UP  START  JOYR   JOYL   SELECT
    mControllerReadData_4 = readData_GameController();

    // 5th byte, in analog mode, this is 
    //      7      6      5       4     3      2      1      0
    //   SQUARE    X   CIRCLE   TRINGL R1     L1     R2     L2
    mControllerReadData_5 = readData_GameController();

    //    if(mControllerMode == ANALOG_MODE_ID) {
      // turn on the LED in Analog Mode
    //      
    // 6th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the right joystick, left<->right displacement
    mControllerReadData_6 = readData_GameController();

    // 7th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the right joystick, up<->down displacement
    mControllerReadData_7 = readData_GameController();

    // 8th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the left joystick, left<->right displacement
    mControllerReadData_8 = readData_GameController();

    // 9th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the left joystick, up<->down displacement
    mControllerReadData_9 = readData_GameController();

    //    } else {
    //      cbi(LED_PORT, LED); // turn off the LED in Digital Mode (or any other mode..)
    //    }

    // flash the LED if the TRIANGLE is pressed
    /*
  if(bit_is_set(mControllerReadData_5, 4)) {
    cbi(LED_PORT, LED);
  } else {
    
  }
    */
  // take ATT back high
  sbi(PSX_ATT_PORT, PSX_ATT_3V3);
  // CMD back high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);;
  // done
  //_delay_us(CLK_INTERTRAIN_DELAY);
}


void GameControllerCommand_VibrationEnable()
{
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);
  // ATT low, notify controller we're going to read
  cbi(PSX_ATT_PORT, PSX_ATT_3V3);
  _delay_us(ATT_DELAY);

  // send 0x01 command, 1st byte
  sendCommand_GameController(START_CMD);
  mControllerMode = readDataSendCommand_GameController(VIBRATION_ENABLE_CMD);
  // 3rd byte
  // read data, should be the mode changed value (0x00 mode changed, 0x5A no mode change)
  mControllerReadData_3 = readData_GameController();
  mControllerReadData_4 = readData_GameController();
  mControllerReadData_5 = readDataSendCommand_GameController(0x01);
  if(mControllerMode == ANALOG_MODE_ID) {
    mControllerReadData_6 = readDataSendCommand_GameController(0xFF);
    mControllerReadData_7 = readDataSendCommand_GameController(0xFF);
    mControllerReadData_8 = readDataSendCommand_GameController(0xFF);
    mControllerReadData_9 = readDataSendCommand_GameController(0xFF);
  }
}

void GameControllerCommand_ReadDataAndVibrate(u08 aLargeMotorSpeed)
{
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);
  // ATT low, notify controller we're going to read
  cbi(PSX_ATT_PORT, PSX_ATT_3V3);
  _delay_us(ATT_DELAY);

  // send 0x01 command, 1st byte
  sendCommand_GameController(START_CMD);

  // 2nd byte
  // send 0x42 as the command to read data and vibrate
  // data returned is the mode the controller is in 
  // modes: 
  // 0x41 is digital mode
  // 0x73 is analog mode
  // 0xF3 is DS2 native mode (huh..?)
  mControllerMode = readDataSendCommand_GameController(READ_GAME_CONTROLLER_CMD);

  // read 7 more bytes (9 bytes total) of data if we're in analog mode

  // 3rd byte
  // read data, should be the mode changed value (0x00 mode changed, 0x5A no mode change)
  mControllerReadData_3 = readData_GameController();

  // 4th byte represents:
  //      7      6      5       4     3      2      1      0
  //   LEFT    DOWN   RIGHT     UP  START  JOYR   JOYL   SELECT
  mControllerReadData_4 = readDataSendCommand_GameController(0x01); // 0x00 turns small motor off

  // 5th byte represents:
  //      7      6      5       4     3      2      1      0
  //   SQUARE    X   CIRCLE   TRINGL R1     L1     R2     L2
  mControllerReadData_5 = readDataSendCommand_GameController(aLargeMotorSpeed);

  // if we're in analog mode, read more data for joystick displacements
  if(mControllerMode == ANALOG_MODE_ID) {
    // 6th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the right joystick, left<->right displacement
    mControllerReadData_6 = readData_GameController();

    // 7th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the right joystick, up<->down displacement
    mControllerReadData_7 = readData_GameController();

    // 8th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the left joystick, left<->right displacement
    mControllerReadData_8 = readData_GameController();

    // 9th byte, in analog mode, this is 
    // a value from 0x00 to 0xFF for the left joystick, up<->down displacement
    mControllerReadData_9 = readData_GameController();
  }
  // debug
  /*
    if(bit_is_set(mControllerReadData_5, 4)) {
    cbi(PORTB, PB3);
    } else {
    sbi(PORTB, PB3);
    }
  */

  // take ATT back high
  sbi(PSX_ATT_PORT, PSX_ATT_3V3);
  // CMD back high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);
  _delay_us(CLK_INTERTRAIN_DELAY);
}


void GameControllerCommand_ConfigModeEnter()
{
  u08 i;
  // pull ATT low
  cbi(PSX_ATT_PORT, PSX_ATT_3V3);  
  _delay_us(ATT_DELAY);  
  sendCommand_GameController(0x01);
  sendCommand_GameController(0x43); // command for CONFIG_MODE_ENTER
  sendCommand_GameController(0x00);
  sendCommand_GameController(0x01); 
  sendCommand_GameController(0x00);
  for(i=0; i<4; i++) {
    sendCommand_GameController(0x00); // four more bytes of 0's?
  }
  // ATT back high
  sbi(PSX_ATT_PORT, PSX_ATT_3V3);
  // CMD back high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);;
  // done
  _delay_us(CLK_INTERTRAIN_DELAY);
}

void GameControllerCommand_ConfigModeExit()
{
  u08 i;
  // pull ATT low
  cbi(PSX_ATT_PORT, PSX_ATT_3V3);  
  _delay_us(ATT_DELAY);  
  sendCommand_GameController(0x01);
  sendCommand_GameController(0x43);
  sendCommand_GameController(0x00);
  sendCommand_GameController(0x00); 
  sendCommand_GameController(0x00);
  for(i=0; i<4; i++) {
    sendCommand_GameController(0x00); // four more bytes of 0's?
  }
  // ATT back high
  sbi(PSX_ATT_PORT, PSX_ATT_3V3);
  // CMD back high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);;
  // done
  _delay_us(CLK_INTERTRAIN_DELAY);
  // done  
}

void GameController_Reset()
{
  delay_1s();
  delay_1s();
  delay_1s();
  delay_1s();
  delay_1s();
  delay_1s();
  delay_1s();
}

// can only lock the controller in a mode
void GameControllerCommand_SetMode(u08 aMode, s08 aLock)
{
  u08 i;
  // pull ATT low
  cbi(PSX_ATT_PORT, PSX_ATT_3V3);  
  _delay_us(ATT_DELAY);
  sendCommand_GameController(START_CMD);
  mControllerMode = readDataSendCommand_GameController(0x44);
  sendCommand_GameController(0x00);
  sendCommand_GameController(aMode); // analog mode is 0x01, digital mode would be 0x00
  if(aLock == TRUE) {
    sendCommand_GameController(0x03); // lock the mode
  } 
  /* this doesn't do anything..don't know how to get the controller to set a mode and _not_ lock
  else {
    sendCommand_GameController(0x00); // don't lock the mode??
  }
  */
  //  if(mControllerMode == ANALOG_MODE_ID) {
  for(i=0; i<4; i++) {
    sendCommand_GameController(0x00); // four more bytes of 0's?
    //  }
  }
  // ATT back high
  sbi(PSX_ATT_PORT, PSX_ATT_3V3);
  // CMD back high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3);;
  _delay_us(CLK_INTERTRAIN_DELAY);
  // done
}

// command "0x44" set the mode of the controller and lock it so that the 
// mode button can't put it in another mode
void GameControllerCommand_SetModeAndLock(u08 aMode)
{
  GameControllerCommand_SetMode(aMode, TRUE);  
}


//
//
//
void sendCommand_GameController(u08 aByteToSend)
{
  u08 i;
  sbi(PSX_CMD_PORT, PSX_CMD_3V3); // start command high
  for(i=0; i<8; i++) {
    if(bit_is_set(aByteToSend, i)) {
      sbi(PSX_CMD_PORT, PSX_CMD_3V3);
    } else {
      cbi(PSX_CMD_PORT, PSX_CMD_3V3);
    }
    // clock pulse high
    cbi(PSX_CLK_PORT, PSX_CLK_3V3);
    _delay_us(CLK_DELAY);
    sbi(PSX_CLK_PORT, PSX_CLK_3V3); // clock low to sample
    _delay_us(CLK_DELAY);
  }

  sbi(PSX_CMD_PORT, PSX_CMD_3V3); // leave command high
  sbi(PSX_CLK_PORT, PSX_CLK_3V3); // leave the clock high
  _delay_us(CLK_DELAY);
  _delay_us(CLK_DELAY);
  _delay_us(CLK_DELAY);
  _delay_us(CLK_DELAY);
}


u08 readData_GameController()
{
  u08 result = 0x00;
  u08 i;
  sbi(PSX_CMD_PORT, PSX_CMD_3V3); // cmd high
  for(i=0; i<8; i++) {
    sbi(PSX_CLK_PORT, PSX_CLK_3V3); // clock high
    _delay_us(CLK_DELAY);
    cbi(PSX_CLK_PORT, PSX_CLK_3V3); // clock low to sample
    _delay_us(CLK_DELAY);
    if(bit_is_set(PSX_DATA_PORT, PSX_DATA_3V3)) {
      result |= (1<<i);
    } 
  }  
  //  sbi(PSX_CMD_PORT, PSX_CMD_3V3);
  sbi(PSX_CLK_PORT, PSX_CLK_3V3); // leave the clock high
  _delay_us(CLK_INTERTRAIN_DELAY);
  return result;
}


// read data from the game controller while also sending a command to the game controller
u08 readDataSendCommand_GameController(u08 aCommandToSend)
{
  u08 result = 0x00;
  u08 i, j;
  j = 7;
  sbi(PSX_CLK_PORT, PSX_CLK_3V3); // clock high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3); // start command high
  for(i=0; i<8; i++) {
      // this idiom here turns the bits in aCommandToSend into
      // sequenced serial byte on the command line that goes 
      // from the PSX board to the game controller
      if(bit_is_set(aCommandToSend, i)) {
	sbi(PSX_CMD_PORT, PSX_CMD_3V3);
      } else {
	cbi(PSX_CMD_PORT, PSX_CMD_3V3);
      }

    // clock pulse low
    cbi(PSX_CLK_PORT, PSX_CLK_3V3);
    _delay_us(CLK_DELAY);
    // clock high
    sbi(PSX_CLK_PORT, PSX_CLK_3V3); 
    _delay_us(CLK_DELAY);
    // now we read the data line and cobble together the 
    // bit of data into a full byte
    // data comes back MSB first
    if(bit_is_set(PSX_DATA_PORT, PSX_DATA_3V3)) {
      result |= (1<<i);
    } 
    }
    // take the command line back high
    sbi(PSX_CMD_PORT, PSX_CMD_3V3);
    sbi(PSX_CLK_PORT, PSX_CLK_3V3);
    _delay_us(CLK_INTERTRAIN_DELAY);
    // return the result of data
    return result;
}


// *****************
//
// console functions
//
// *****************


// read command byte from console
u08 Console_ReadCommand() 
{
  u08 result = 0x00;
  /*
  u08 i;
  sbi(CON_ACK_PORT, CON_ACK_3V3);
  for(i=0; i<8; i++) {
    // for debugging just to see when we start checking the clock
    while(bit_is_clear(CON_CLK_PORT, CON_CLK_3V3)) {
      // _delay_us(1);
    }
    if(bit_is_set(CON_CMD_PORT, CON_CMD_3V3)) {
      result |= (1<<i);
    }
  }  
  // for debugging just to see when we start checking the clock
  cbi(CON_ACK_PORT, CON_ACK_3V3);

  if(result == START_CMD) {
    
    delay_1s();
  } else {
    cbi(LED_PORT, LED);
  }
  */
  return result;
}


// function to send data to the console
void sendData_Console(u08 aByte)
{
  /*
  u08 i;
  for(i=0; i<8; i++) {
    // wait for the clock to go high
    while(bit_is_set(CON_CLK_PORT, CON_CLK_3V3)) {
      _delay_us(1);
    }
    if(bit_is_set(aByte, i)) {
      sbi(CON_DATA_PORT, CON_DATA_3V3);
    } else {
      cbi(CON_DATA_PORT, CON_DATA_3V3);
    }
    // wait for the clock to go low, this is when the console will sample the data
    while(bit_is_clear(CON_CLK_PORT, CON_CLK_3V3)) {
      _delay_us(1);
    }    
  }
  */
}

// function to handle when
// console sends a command while we send data
// result is the command sent to us
u08 readCommandSendData_Console(u08 aByte)
{
  u08 result = 0x00;
  /*
  u08 i;

  for(i=0; i<8; i++) {
    while(bit_is_set(CON_CLK_PORT, CON_CLK_3V3)) {
      _delay_us(1);
    }
    if(bit_is_set(aByte, i)) {
      sbi(CON_DATA_PORT, CON_DATA_3V3);
    } else {
      cbi(CON_DATA_PORT, CON_DATA_3V3);
    }

    if(bit_is_set(CON_CMD_PORT, CON_CMD_3V3)) {
      result |= (1<<i);
    } 

    while(bit_is_clear(CON_CLK_PORT, CON_CLK_3V3)) {
      _delay_us(1);
    }
  }  
  */
  return result;
}

void sendAcknowledge_Console()
{
  //  if(bit_is_clear(CON_ATT_PORT, CON_ATT_3V3)) {
  cbi(CON_ACK_PORT, CON_ACK_3V3);
  //  _delay_us(1);
  sbi(CON_ACK_PORT, CON_ACK_3V3);
  //  }
}

#define READ_DATA_COMMAND 0x42
#define QUERY_DS2_ANALOG_MODE 0x41
//#define READ_DATA_AND_VIBRATE 0x42
#define SET_MODE_AND_LOCK 0x44
#define QUERY_MODEL_AND_MODE 0x45



void handleCommand(u08 aCommand) 
{
  switch(aCommand) {
    // handle the "read data" command (0x42)
  case READ_DATA_COMMAND:
    // start with the 3rd byte after the command byte
    // we send to the console mode changed byte..not sure what this should be
    // 0x5A means mode not changed
    // 0x00 means mode changed
    sendData_Console(0x5A);

    // now send some analog mode controller data
    sendData_Console(0xFF - 0x08); // start button, bit 3 active low
    sendData_Console(0x00); // no buttons
    sendData_Console(0x7F); // right joy h middle
    sendData_Console(0x7F); // right joy v middle
    sendData_Console(0x7F);
    sendData_Console(0x7F);
    break;
  case SET_MODE_AND_LOCK:
    

    break;
  default:
    break;
  }

}

void setup() {
  // inputs and outputs

  // INPUTS
  // (THESE CONNECTIONS TO THE CONSOLE ARE NOW ON THE SPI BUS)
 //  DDRC |= (0<<CON_CMD_3V3);
  //  DDRB |= (0<<CON_ATT_3V3);
  //  DDRA |= (0<<CON_CLK_3V3) | (0<<PSX_DATA_3V3);
  // data is input coming from the controller
  PSX_DDR |= (0<<PSX_DATA_3V3);

  // tri-state these inputs?
  //  PORTC |= (0<<CON_CMD_3V3);
  //  PORTB |= (0<<CON_ATT_3V3);
  //  PORTA |= (0<<CON_CLK_3V3) | (0<<PSX_DATA_3V3);
  //PORTA |= (0<<PSX_DATA_3V3);

  // PSX_ACK_3V3 input from PSX, but not hooked up or perhaps not even necessary
  // we can assume the controller is there

  // OUTPUTS
  //  DDRC |= (1<<CON_DATA_3V3);
  CON_DDR |= (1<<CON_ACK_3V3);// | (1<<LED);
  LED_DDR |= (1<<LED);
  PSX_DDR |= (1<<PSX_CLK_3V3) | (1<<PSX_CMD_3V3) | (1<<PSX_ATT_3V3);
  
  LED_OFF;

  // initialize signals to the game console
  sbi(CON_ACK_PORT, CON_ACK_3V3); // set ACK high, it's active low

  // initialize the signals to the game controller
  sbi(PSX_CLK_PORT, PSX_CLK_3V3); // PSX clock high
  sbi(PSX_ATT_PORT, PSX_ATT_3V3); // PSX attention high
  sbi(PSX_CMD_PORT, PSX_CMD_3V3); // PSX command high

  mControllerMode = 0x00;
  mControllerReadData_2 = 0;
  mControllerReadData_3 = 0;
  mControllerReadData_4 = 0;
  mControllerReadData_5 = 0;
  mControllerReadData_6 = 0;
  mControllerReadData_7 = 0;
  mControllerReadData_8 = 0;
  mControllerReadData_9 = 0;

  // pin change interrupt for the attention signal from the console
  //PCICR |= (1<<PCIE3) | (1<<PCIE2) | (1<<PCIE1) | (1<<PCIE0);
  //PCMSK1 |= (1<<PCINT12); // CON_ATT_3V3 on V03 board

  //PCMSK0 |= (0<<PCINT7); // CON_CLK_3V3 on V03 board

  // we're going to use the SPI hardware to communicate
  // with the console
  // PS2    Atmega324
  // _________________
  // CLK  -> SCK
  // DATA <- MISO
  // CMD  -> MOSI
  // ATT  -> SS_
  //
  // set up SPI
  // we're the console's slave device
  // sample on leading edge of falling clock
  
  //  cbi(DDRB, PB7); // sck is CLK input from the console
  //sbi(DDRB, PB6); // set MISO as output
  //cbi(DDRB, PB5); // set MOSI as input
  //  cbi(DDRB, PB4); // set SS_ input
  // MISO as output
  CON_DDR |= (1<<MISO);
  
  // SPI Control Register
  //   No interrupt    Enable MSB first    Slave mode  CLK Active High  Sample on leading (falling) edge
  cbi(SPCR, SPIE);
  cbi(SPCR, DORD);
  cbi(SPCR, MSTR);
  cbi(SPCR, CPOL); //sample on the clk high
  //sbi(SPCR, CPOL); //sample on the clk low
  sbi(SPCR, CPHA); //leading (falling) edge
  //cbi(SPCR, CPHA); //trailing (rising) edge
  // enable SPI
  sbi(SPCR, SPE);
  
  sbi(CON_ACK_PORT, CON_ACK_3V3);
   //SPCR |= (0<<SPIE) | (1<<SPE) | (1<<DORD) | (0<<MSTR) | (1<<CPOL) | (0<<CPHA);

  //  sei(); // enable interrupts
}
