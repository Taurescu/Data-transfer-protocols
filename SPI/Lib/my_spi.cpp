#include "my_spi.h"

#define readBit(reg, bit)	(((reg) >> (bit)) & 0x01)
#define setBit(reg, bit)	((reg) |= ((0x01) << (bit)))
#define clearBit(reg, bit)  ((reg) &= ~((0x01) << (bit)))


//SPCR(SPI controll Register)
#define SPIE 7  //interrupt Enable
#define SPE  6	//SPI Enable "1" On / "0" Off 
#define DORD 5	//Data Order "1" LSB First / "0" MSB First
#define MSTR 4	//Master/Slave Selection "1" Master / "0" Slave
#define CPOL 3	//Clock Polarity
#define CPHA 2	//Clock phase
#define SPR1 1	//Clock rate Select 2
#define SPR0 0	//Clock rate Select 1

//SPSR(SPI Status Register)
//Bit 5-1 Reserved Bits
#define SPIF 7	//interrupt Flag
#define WCOL 6  //Write Colision Flag
#define SPI2X 0 //Double SPI speed Bit

//Set up arduino to be in the slave mode
void spi_begin(){

	pinMode(SS, INPUT);
	pinMode(SCK, INPUT);
	pinMode(MOSI, INPUT);
	pinMode(MISO, OUTPUT);

	setBit(SPCR, SPE);
	clearBit(SPCR, MSTR);
}

//Disable SPI Mode
void spi_end() { clearBit(SPCR, SPE); }

//Put 1 byte in the SPDR (SPI Data Register) and send it to the master
uint8_t spi_transfer(uint8_t data){
	SPDR = data;
	while (!(SPCR) & ((SPIE) << (0x01)));

	return 0;
}
//Read 1 byte of data from the SPDR sent by the master
 uint8_t spi_read(){
	 uint8_t junk = 0b11111111;

	 SPDR = junk;
	 while (!(SPCR) & ((SPIE) << (0x01)));

	 return SPDR;
 }

//Select 1 of the 4 modes of SPI Clock Polarity(CPOL), Clock Phase(CPHA)
//CPOL = 1 -> Serial Clock(SCK) is high when idel
//CPOL = 0 -> Serial Clock(SCK) is low when idel
//CPHA = 1 -> Data is sampled on the leading(first) edge of Serial Clock (SCK)
//CPHA = 0 -> Data is sampled on the trailing(last) edge of Serial Clock (SCK)
 void spi_select_mode(uint8_t mode) {

	 switch (mode) {
		 //CPOL = 0, CPHA = 0
	 case 0:
		 clearBit(SPCR, CPOL);
		 clearBit(SPCR, CPHA);
		 break;
		 //CPOL = 0, CPHA = 1
	 case 1:
		 clearBit(SPCR, CPOL);
		 setBit(SPCR, CPHA);
		 break;
		 //CPOL = 1, CPHA = 0
	 case 2:
		 setBit(SPCR, CPOL);
		 clearBit(SPCR, CPHA);
		 break;
		 //CPOL = 1, CPHA = 1
	 case 3:
		 setBit(SPCR, CPOL);
		 setBit(SPCR, CPHA);
		 break;
	 default:
		 break;
	 }
 }


//Select 1 of the 7 Clock Rate available
// f/2, f/4, f/8, f/16, f/32, f/64, f/128
void spi_select_freq(uint8_t frequency ){
	switch (frequency){
		//Clock Rate = f/128
		case 1:
			setBit(SPCR, SPR1);
			setBit(SPCR, SPR0);
			clearBit(SPSR, SPI2X);

			break;

		//Clock Rate = f/64
		case 2:
			setBit(SPCR, SPR1);
			clearBit(SPCR, SPR0);
			clearBit(SPSR, SPI2X);

			break;

		//Clock Rate = f/32
		case 3:
			setBit(SPCR, SPR1);
			clearBit(SPCR, SPR0);
			setBit(SPSR, SPI2X);

			break;

		//Clock Rate = f/16 
		case 4:

			clearBit(SPCR, SPR1);
			setBit(SPCR, SPR0);
			clearBit(SPSR, SPI2X);

			break;

		//Clock Rate = f/8
		case 5:
			clearBit(SPCR, SPR1);
			setBit(SPCR, SPR0);
			clearBit(SPSR, SPI2X);

			break;

		//Clock Rate = f/4
		case 6:
			clearBit(SPCR, SPR1);
			clearBit(SPCR, SPR0);
			clearBit(SPSR, SPI2X);

			break;

		//Clock Rate = f/2
		case 7:
			clearBit(SPCR, SPR1);
			clearBit(SPCR, SPR0);
			setBit(SPSR, SPI2X);
			break;
	default:
		break;
	}
}

//Set the bit order that arduino will send
// bitOrder == 1 -> Most Significant Bit will be transmited first
// bitOrder != 1 -> Least Significant Bit will be transmited first
void spi_bit_order(uint8_t bitOrder) { bitOrder == 1 ? setBit(SPCR, DORD)
													 : clearBit(SPCR, DORD); }
//Enable interrupt for SPI transaction
void spi_attachInterrupt() { setBit(SPCR, SPIE); }

//Disable interrupt for SPI transaction
void spi_detachInterrupt() { clearBit(SPCR, SPIE); }