//////////////////////////////////////////////////////////////////////////////
//
//				defs.h
//
//	Header file for "all" SRI projects.
//
//////////////////////////////////////////////////////////////////////////////
//
//		SRI, Inc.
//		111 Prospect Street
//		Westfield, NJ
//		908-654-4000
//
//////////////////////////////////////////////////////////////////////////////
//
//	Version		1.2
//	Date		4 Dec 97
//	Programmer	AK
//
//	Added macros "is*" from MPLABC.
//
//////////////////////////////////////////////////////////////////////////////
//
//	Version		1.1
//	Date		13 Nov 97
//	Programmer	AK
//
//	This version implements changes to support MSVC++ v1.5 and
//	Hi-Tech PIC C v7.72-1.  As you add new compilers, use the same
//	notation found below.  The Microsoft version is the default -
//	all others should refer to compiler switches defined by the
//	vendor, with nestings so as to generate the final #else to
//	default to MSVC++.
//
//////////////////////////////////////////////////////////////////////////////
//
//	Project		Generic Module
//	Version		1.0 (original, not verified)
//	Date		11 Nov 97
//	Programmer	AK
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _DEFS_H

#define	_DEFS_H

#if HI_TECH_C
//////////////////////////////////////////////////////////////////////////////
//
//			Hi-Tech C Compiler (PIC C v7.72-1)
//
//////////////////////////////////////////////////////////////////////////////

typedef bit			BOOLEAN;	/* Bit flag */
typedef	bit			BIT;		/* Bit flag */

typedef unsigned char 		BYTE;		/* Unsigned 8-bit */
typedef unsigned char 		byte;		/* Unsigned 8-bit */
typedef	unsigned char		UCHAR;		/* Unsigned 8-bit */
typedef	signed char		SBYTE;		/* Signed 8-bit */
typedef signed char		SCHAR;		/* Signed 8-bit */

typedef unsigned int		WORD;		/* Unsigned 16-bit */
typedef	unsigned int		UINT;		/* Unsigned 16-bit */

typedef	long int		LONG;		/* Signed 32-bit */
typedef	long int		LONGINT;	/* Signed 32-bit */

typedef	unsigned long int	ULONG;		/* Unsigned 32-bit */
typedef	unsigned long int	LWORD;		/* Unsigned 32-bit */

#define	ALL_BITS_SET	(~0)
#define	NOP()			asm("nop")
#else
//////////////////////////////////////////////////////////////////////////////
//
//			Default (MSVC++ v1.5)
//
//////////////////////////////////////////////////////////////////////////////

typedef unsigned char		BOOLEAN;	/* Unsigned 8-bit to hold TRUE/FALSE */
typedef unsigned char 		BYTE;		/* Unsigned 8-bit */
typedef unsigned int		WORD;		/* Unsigned 16-bit */
typedef	unsigned long int	LWORD;		/* Unsigned 32-bit */

typedef	unsigned char		UCHAR;		/* Unsigned 8-bit */
typedef	unsigned int		UINT;		/* Unsigned 16-bit */
typedef	unsigned long int	ULONG;		/* Unsigned 32-bit */

typedef	long int			LONGINT;	/* Signed 32-bit */
typedef	long int			LONG;		/* Signed 32-bit */
#define	ALL_BITS_SET	(~0)
#endif

//////////////////////////////////////////////////////////////////////////////

#define	TRUE		(1)
#define	FALSE		(0)
#define	YES		(1)
#define	NO		(0)

#define LOBYTE(w)	((BYTE)(w))
#define HIBYTE(w)	((BYTE)(((UINT)(w) >> 8) & 0xFF))

//////////////////////////////////////////////////////////////////////////////

#define   isalnum(c)  (isalpha(c) || isdigit(c))
#define   isalpha(c)  (islower(c) || isupper(c))
#define   isascii(c)  (((c) >= 0) && ((c) <= 0x7f))
#define   iscntrl(c)  ((((c) >= 0) && ((c) <= 0x1f)) || ((c) == 0x7f))
#define   isdigit(c)  (((c) >= '0') && ((c) <= '9'))
//#define   isgraph(c)  (((c) >= 0x21) && ((c) <= 0x7e))
#define   islower(c)  (((c) >= 'a') && ((c) <= 'z'))
#define   isprint(c)  (((c) >= 0x20) && ((c) <= 0x7e))
#define   ispunct(c)  ((((c) >= 0x20) && ((c) <= 0x2f)) || \
                       (((c) >= 0x3a) && ((c) <= 0x40)) || \
                       (((c) >= 0x5b) && ((c) <= 0x60)) || \
                       (((c) >= 0x7b) && ((c) <= 0x7e)))
#define   isspace(c)  (((c) == 0x09) || ((c) == 0x0a) || \
                       ((c) == 0x0b) || ((c) == 0x0c) || \
                       ((c) == 0x0d) || ((c) == 0x20))
#define   isupper(c)  (((c) >= 'A') && ((c) <= 'Z'))
#define   isxdigit(c) (isdigit(c) || (((c) >= 'a') && ((c) <= 'f')) || \
                       (((c) >= 'A') && ((c) <= 'F')))
#define   tolower(c)  (isupper(c) ? ((c) + 'a' - 'A'): (c))
#define   toupper(c)  (islower(c) ? ((c) - 'a' + 'A'): (c))

#define IDFUSE_ADDR	0x2000

#if defined(_16C74)
#define	LP_OSC		0b00000000
#define	XT_OSC		0b00000001
#define	HS_OSC		0b00000010
#define	RC_OSC		0b00000011

#define	WDT_ENABLED	0b00000100
#define	WDT_DISABLED	0b00000000

#define	PWRT_ENABLED	0b00001000
#define	PWRT_DISABLED	0b00000000

#define	CP_OFF		0x3FB0
#define	CP_1_2		0x2AA0
#define	CP_3_4		0x1590
#define	CP_FULL		0x0080
#endif

#if defined(_16C74A)
#define	LP_OSC		0x00
#define	XT_OSC		0x01
#define	HS_OSC		0x02
#define	RC_OSC		0x03

#define	WDT_ENABLED	0x04
#define	WDT_DISABLED	0x00

#define	PWRT_ENABLED	0x00
#define	PWRT_DISABLED	0x08

#define	BOD_ENABLED	0x40
#define	BOD_DISABLED	0x00

#define	CP_OFF		0x3FB0
#define	CP_1_2		0x2AA0
#define	CP_3_4		0x1590
#define	CP_FULL		0x0080
#endif

#if defined(_16C77)
#define	LP_OSC		0x00
#define	XT_OSC		0x01
#define	HS_OSC		0x02
#define	RC_OSC		0x03

#define	WDT_ENABLED	0x04
#define	WDT_DISABLED	0x00

#define	PWRT_ENABLED	0x00
#define	PWRT_DISABLED	0x08

#define	BOD_ENABLED	0x40
#define	BOD_DISABLED	0x00

#define	CP_OFF		0x3FB0
#define	CP_1_2		0x2AA0
#define	CP_3_4		0x1590
#define	CP_FULL		0x0080
#endif

#if defined(_16C873) || defined(_16C874) || defined(_16C876) || defined(_16C877) || defined(_16F873) || defined(_16F874) || defined(_16F876) || defined(_16F877)
#define	LP_OSC		0x0000
#define	XT_OSC		0x0001
#define	HS_OSC		0x0002
#define	RC_OSC		0x0003

#define	WDT_ENABLED	0x0004		// Watchdog runs
#define	WDT_DISABLED	0x0000

#define	PWRT_ENABLED	0x0000		// Power-up Timer runs
#define	PWRT_DISABLED	0x0008

#define	BOD_ENABLED	0x0040		// Brown-Out Detect active
#define	BOD_DISABLED	0x0000

#define	LVP_ENABLED	0x0080		// Low-voltage programming available
#define	LVP_DISABLED	0x0000

#define	CPD_ENABLED	0x0000		// Data EE Protected
#define	CPD_DISABLED	0x0100

#define	WRT_ENABLED	0x0200		// Internal self-write enabled
#define	WRT_DISABLED	0x0000

#define	DEBUG_ENABLED	0x0000		// RB6/7 are DEBUG pins
#define	DEBUG_DISABLED	0x0800

#define	CP_OFF		0x3430		// Protects nothing
#define	CP_TOP		0x2420		// Protects only top 256 words
#define	CP_HALF		0x1410		// Protects only top 1/2 of program space
#define	CP_FULL		0x0400		// Protects entire chip
#endif

#define	__CHIPID(x)	asm("\tpsect idfuse,class=IDFUSE,delta=2");\
			asm("\tglobal\tchipid_word"); \
			asm("chipid_word"); \
			asm("\tdw "___mkstr(x>>24&0xFF|0x3400));\
			asm("\tdw "___mkstr(x>>16&0xFF|0x3400));\
			asm("\tdw "___mkstr(x>>8&0xFF|0x3400));\
			asm("\tdw "___mkstr(x&0xFF|0x3400))

//Sample command line to use this addition:
//     picc -16C73a -Zg1 -O -L-Pidfuse=2000h sdd.c

#if defined(_16C62)|| defined(_16C64) || defined(_16C65) || defined(_16C73) || defined(_16C74)
#define	LP_OSC		0x00
#define	XT_OSC		0x01
#define	HS_OSC		0x02
#define	RC_OSC		0x03

#define	WDT_ENABLED	0x04
#define	WDT_DISABLED	0x00

#define	PWRT_ENABLED	0x08
#define	PWRT_DISABLED	0x00

#define	CP_OFF		0x3FB0
#define	CP_1_2		0x2AA0
#define	CP_3_4		0x1590
#define	CP_FULL		0x0080
#endif

#if defined(_16C62A) || defined(_16C63) || defined(_16C64A) || defined(_16C65A) || defined(_16C72) || defined(_16C73A) || defined(_16C74A)
#define	LP_OSC		0x00
#define	XT_OSC		0x01
#define	HS_OSC		0x02
#define	RC_OSC		0x03

#define	WDT_ENABLED	0x04
#define	WDT_DISABLED	0x00

#define	PWRT_ENABLED	0x00
#define	PWRT_DISABLED	0x08

#define	BOD_ENABLED	0x40
#define	BOD_DISABLED	0x00

#define	CP_OFF		0x3F30
#define	CP_1_2		0x2A20
#define	CP_3_4		0x1510
#define	CP_FULL		0x0000
#endif

#if defined(_16C76) || defined(_16C77)
#define	LP_OSC		0x00
#define	XT_OSC		0x01
#define	HS_OSC		0x02
#define	RC_OSC		0x03

#define	WDT_ENABLED	0x04
#define	WDT_DISABLED	0x00

#define	PWRT_ENABLED	0x00
#define	PWRT_DISABLED	0x08

#define	BOD_ENABLED	0x40
#define	BOD_DISABLED	0x00

#define	CP_OFF		0x3FB0
#define	CP_1_2		0x2AA0
#define	CP_3_4		0x1590
#define	CP_FULL		0x0080
#endif

#endif

