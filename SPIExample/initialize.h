// initMacro.h - Defines an "Initialize()" macro
//
// The "initialize" macro inserts inline statements into the "init3" section,
// which is part of the sequence of instructions that the startup code will
// execute before calling main().
//
// Typical use:    Initialize(Nectarine) { PORTA |= 1; }

#ifndef Initialize_H
#define Initialize_H

#define Initialize(something) \
void Init_ ## something (void) __attribute((naked)) __attribute__ ((section (".init3"))); \
void Init_ ## something (void)

#endif
