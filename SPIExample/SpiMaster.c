// SpiMaster.c - Example SPI slave transmitter application
//
#include <avr/io.h>
#include "initialize.h"



Initialize(SPImaster)
{
    // The SlaveSelect signal that we'll be outputting is
    // active-low, so the powerup condition of that pin
    // (Port B, bit 2) is unfortunately "active".
    // Make it otherwise:
    //
    PORTB |= (1 << 2);

    //
    // We want to make the MISO, SS, and SCK pins into outputs.
    // These are bits 2,3, and 5 of Port B:
    //
    DDRB |= 0b101100;

    // Enable the SPI machinery in master mode, but don't enable
    // the SPI interrupt (we'll just poll SPI status).  We'll also
    // knock the master's generated SPI clock rate down to Fosc/8,
    // so that the slave's clock can be as low as half the master's
    // frequency.
    //
    SPCR = ((1 << MSTR) | (1 << SPE) | 0b01 );  // Nominal Fosc/16
    //
    SPSR |= 1;  // Doubles the SPI clock rate, for 2/16 == 1/8 overall

    // We'll be relying on the SPI transfer complete interrupt flag
    // becoming set as an indication that we can send another byte.
    // It's kind of crude to do this, but let's initiate a transfer
    // during starutp, to ensure that the "pump is primed":
    // 
    SPDR = 0x34;

    // Wait for the "transmiter empty" flag to get set:
    //
    while (!(SPSR & (1<<SPIF)));
}


// This routine waits (blocks) until the SPI transmitter is available,
// and then loads its input argument into that transmitter:
//
void spiPutbyte(uint8_t byteToSend)
{
    // Wait for the transmitter to be available.  With luck, it will
    // have become so while our caller was off preparing its value
    //
    while (!(SPSR & (1<<SPIF)));

    SPDR = byteToSend;
}



// A trivial main program that just exercises the SPI transmitter:
//
int main(void)
{

    // Gotta have a value to send, don't we?
    //
    uint8_t value = 0;

    // Select that slave!
    //
    PORTB &= ~(1 << 2);


    while (1) {
        spiPutbyte(value++);
    }

    // We'll never get here, but for formality's sake,
    // deselect the slave
    //
    PORTB |= (1 << 2);

    return 1;
}
