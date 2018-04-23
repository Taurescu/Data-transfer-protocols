#ifndef MY_SPI_H
#define MY_SPI_H

#include <Arduino.h>

void spi_begin();
void spi_end();
uint8_t spi_transfer(uint8_t);
uint8_t spi_read();
void spi_select_mode(uint8_t);
void spi_select_freq(uint8_t);
void spi_bit_order(uint8_t);
void spi_attachInterrupt();
void spi_detachInterrupt();

#endif


