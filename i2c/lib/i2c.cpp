#include <avr/io.h>			//ATmega2560 library for pins,register, ADC etc.
#include <avr/interrupt.h>  //intrerapt functionality ISR
#include <stdlib.h>		    // memory allocation in general 
#include <inttypes.h>		// types of variables ( uint8_t, uint16_t etc)
#include <math.h>			// Mathematical equations
#include <compat/twi.h>	    // TWI vector status defines 

#include "i2c.h"
#include "Arduino.h"		// for digitalWrite


#define clearbit(reg,bit)  (_SFR_BYTE(reg) &= (~_BV(bit))) 
#define setbit(reg,bit)	   (_SFR_BYTE(reg) |= _BV(bit))

#define FREQ 100000L // Set the frequency , 100000Hz = 100 kHz

#define SDA 20
#define SCL 21

static uint8_t txBuffer[BUFFER_LENGTH];
static volatile uint8_t txBufferIndex=0;
static volatile uint8_t txBufferLength=0;

static uint8_t rxBuffer;


static void(*SlaveTransmit)(void);
static void(*SlaveReceive)(uint8_t );


void i2c_init(void)
{
	// activate internal pullups for i2c.
	digitalWrite(SDA, 1);
	digitalWrite(SCL, 1);

	// initialize i2c prescaler = 1
	// TWSR - TWI Status register 
	//TWPS0 and TWPS1 - TWI Prescaler Bits
	// if TWPS0 = 0 && TWPS1 = 0 -> Prescaler Value = 1
	//TWSE &= 0b11111100
	clearbit(TWSR, TWPS0);
	clearbit(TWSR, TWPS1);

	//Initialize bit rate 
	//TWBR - TWI bit Rate Register 
	//Set the SCK clock frequecny in master mode 
	TWBR = ((F_CPU / FREQ) - 16) / 2;

	/* twi bit rate formula from atmega128 manual pg 204
	SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
	note: TWBR should be 10 or higher for master mode
	It is 72 for a 16mhz Wiring board with 100kHz TWI */

	// enable twi module, acks, and twi interrupt
	//TWCR - TWI Controll Register
	//TWEN (bit 2) - TWI Enable Bit, TWIE(Bit 0) - TWI Intrerrupt Enable, TWEA(bit 6) - TWI Enable Acknowledge Bit
	//If TWEN = 1 -> TWI takes control over the I/O pins connected to SCL
	    //and SDA, If TWEN = 0 -> TWI is switched off 
	//If TWIE = 1 && SREG 1 bit is set -> TWI intrerrupt request will be 
		//activated for as long as the TWINT Flag is high
	//IF TWEA = 1 -> the ACK pulse is generated on the TWI bus if the condition are met 
	//TWCR = 0b01000101
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

void disable(void) {
	//disable twi intrerrupt(TWIE),module(TWEN) and acks(TWEA) 
	//TWCR &= 0b10111010
	TWCR &= ((~_BV(TWIE)) & (~_BV(TWEN)) & (~_BV(TWEA)));
	

	//Dezactivate internal pullups for pin 20,21 (SDA,SCL)
	digitalWrite(SDA, 0);
	digitalWrite(SCL, 0);

}

void setAddress(uint8_t address) {
	//TWAR - TWI(Slave) Adress Register
	//Put adress inside the TWAR skipping TWGCE(General Call Recognition Enable bit(0))
	//TWAR = 0bxxxxxxx0
	TWAR = address << 1;
}


uint8_t transmit(const uint8_t* data, uint8_t length)
{
	//Transfer data/lenght of it and store it into txbuffer/txbufferlenght
	for (uint8_t i = 0; i != length; ++i)
	{
		txBuffer[i] = data[i];
	}
	txBufferLength += length;
	return 0;
}
//Pointer callback function that pass by data to the arduino
void attach_Rx(void(*function)(uint8_t))
{
	SlaveReceive = function;
}

//Pointer callback function that pass the data from the arduino to the ISR
void attach_Tx(void(*function)(void))
{
	SlaveTransmit = function;
}

void releaseBus(void)
{
	// release bus
	/*TWINT->This bit is set by hardware when the TWI has finished its current job and expects application software response. If
	   the I-bit in SREG and TWIE in TWCR are set, the MCU will jump to the TWI Interrupt Vector. While the TWINT Flag
	   is set, the SCL low period is stretched. The TWINT Flag must be cleared by software by writing a logic one to it.*/
	 //TWCR = 0b11000101
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
}

void ack_bit(uint8_t ack)
{	
	// transmit master read ready signal, with or without ack
	if (ack) {
	//TWCR = 0b11000101
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
	}
	else {
		//TWCR = 0b01000101
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	}
}

ISR(TWI_vect)
{
	switch (TW_STATUS) {

		//Arduino As a Slave
		// Slave Receiver
	case TW_SR_SLA_ACK:   // addressed, returned ack
	case TW_SR_GCALL_ACK: // addressed generally, returned ack
	case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
	case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
		ack_bit(1);
		break;
	case TW_SR_DATA_ACK:       // data received, returned ack
	case TW_SR_GCALL_DATA_ACK: // data received genera lly, returned ack
							   // if there is still room in the rx buffer
			rxBuffer = TWDR;
			ack_bit(1);
		break;
	case TW_SR_STOP: // stop or repeated start condition received
					 // ack future responses and leave slave receiver state
		releaseBus();
		// callback to send the rxBuffer
		SlaveReceive(rxBuffer);
	
		break;
	case TW_SR_DATA_NACK:       // data received, returned nack
	case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
								// nack back at master
		ack_bit(0);
		break;


		// Slave Transmitter
	case TW_ST_SLA_ACK:          // addressed, returned ack
	case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
		//set index to start the begging of the array
		txBufferIndex = 0;
		//set tx buffer lenght to 0 to be sure
		txBufferLength = 0;

		//call back from the arduino 
		SlaveTransmit();

	case TW_ST_DATA_ACK: // byte sent, ack returned
						 // copy data from output register TWDR
		TWDR = txBuffer[txBufferIndex++];
		// if there is more to send, ack, otherwise nack
		if (txBufferIndex < txBufferLength){
			ack_bit(1);
		}
		else{
			ack_bit(0);
		}
		break;
	case TW_ST_DATA_NACK: // received nack, we are done 
	case TW_ST_LAST_DATA: // received ack, but we are done already!
						  // ack future responses
		ack_bit(1);
		break;
	}

}