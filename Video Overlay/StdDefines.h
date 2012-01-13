/*******************************************************************************
 File:			StdDefines.h

					Standard definitions file.

  Created:		1.00	04/14/03	GND	Gary Dion

 Version:		1.00

 Revisions:		1.00	04/14/03	GND	Initial Release.
*******************************************************************************/

/* General use definitions */
#	define	TRUE			(1)
#	define	FALSE			(0)

#	define	ON				(1)
#	define	OFF			(0)

#	define	HI				(1)
#	define	LO				(0)

/* General status constants. */
#	define	PASS			(1)
#	define	OK				(PASS)
#	define	FAIL			(0)
#	define	ERROR			(FAIL)

/* Define PUBLIC as extern. */
#define 		PUBLIC		extern

/* Define PRIVATE as static. */
#define 		PRIVATE		static

/* Scalar type definitions */
typedef	char					INT8;
typedef	unsigned char		UINT8;
typedef	unsigned char		UCHAR;
typedef	unsigned char		BOOL;

typedef	int					INT16;
typedef	unsigned int		UINT16;
typedef	unsigned int		UINT;

typedef	short					SHORT;
typedef	unsigned short		USHORT;

typedef	long					INT32;
typedef	unsigned long		UINT32;
typedef	unsigned long		ULONG;
typedef	long					STATUS;
