//#ifndef _SNES_H_
//#define _SNES_H_

#define Delay(x) DELAY = x; while(--DELAY){ Nop(); Nop(); }
#define DATA_LATCH 		PORTDbits.RD2

#define SNES_A			SNESL.bit0
#define SNES_X			SNESL.bit1
#define SNES_L			SNESL.bit2
#define SNES_R			SNESL.bit3

#define SNES_B			SNESH.bit0
#define SNES_Y			SNESH.bit1
#define SNES_Select	SNESH.bit2
#define SNES_Start	SNESH.bit3
#define SNES_Up  		SNESH.bit4
#define SNES_Down		SNESH.bit5
#define SNES_Left		SNESH.bit6
#define SNES_Right	SNESH.bit7

#define SNES_A_prev			SNESL_prev.bit0
#define SNES_X_prev			SNESL_prev.bit1
#define SNES_L_prev			SNESL_prev.bit2
#define SNES_R_prev			SNESL_prev.bit3

#define SNES_B_prev			SNESH_prev.bit0
#define SNES_Y_prev			SNESH_prev.bit1
#define SNES_Select_prev	SNESH_prev.bit2
#define SNES_Start_prev	SNESH_prev.bit3
#define SNES_Up_prev 		SNESH_prev.bit4
#define SNES_Down_prev		SNESH_prev.bit5
#define SNES_Left_prev		SNESH_prev.bit6
#define SNES_Right_prev	SNESH_prev.bit7


#define SHIFT_UP 2
#define SHIFT_DOWN -2
#define SHIFT_LEFT -1
#define SHIFT_RIGHT 1
#define SHIFT_L -5
#define SHIFT_R 5


typedef struct {
	char note_num;
	char volume;
	char play_state;
	char min_length_timer;
	char min_time_between_notes_timer;
	char index;		
	char relative_pitch;
	char controller_num;
} button;


typedef struct {
	char min;
	char middle;
	char max;
	
} joystick;



typedef union {
  struct {
    unsigned bit7:1;
    unsigned bit6:1;
    unsigned bit5:1;
    unsigned bit4:1;
    unsigned bit3:1;
    unsigned bit2:1;
    unsigned bit1:1;
    unsigned bit0:1;
  };
  struct {
    unsigned char byte;
  };
} uint8;



