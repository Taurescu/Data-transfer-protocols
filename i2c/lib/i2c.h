#ifndef i2c_h
#define i2c_h

#include <inttypes.h>

#define BUFFER_LENGTH 32

void i2c_init(void);
void disable(void);
void setAddress(uint8_t address);
uint8_t transmit(const uint8_t*, uint8_t);
void attach_Rx(void(*)(uint8_t));
void attach_Tx(void(*)(void));
void ack_bit(uint8_t ack);
void releaseBus(void);

#endif