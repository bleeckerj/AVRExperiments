// SpiSlave.c - Example SPI slave receiver application
//
#include <avr/io.h>
#include <avr/interrupt.h>
#include "initialize.h"



Initialize(SPIslave)
{
    // Enable the SPI machinery in slave mode,
    // and enable the SPI interrupt:
    //
    SPCR = ((1 << SPIE) | (1 << SPE));

    // Make MISO an output
    //
    DDRB |= (1 << PB6);

    // We're going to just copy all bytes received via SPI
    // out to PORTC, so have all PORTC bits be outputs:
    //
    DDRD = 0xFF;
}


ISR(SPI_STC_vect)
{
    PORTD = SPDR;
}


int main(void)
{
    // Enable any interrupts that installed drivers have set up:
    //
    sei();

    // Sometimes, debuggers have a hard time following completely
    // empty loops, so here's an infinite loop that at least
    // executes a few instructions per pass:
    //
    for (uint8_t dunsel = 1; dunsel; dunsel+=2) {
        ;
    }

    return 1;
}
