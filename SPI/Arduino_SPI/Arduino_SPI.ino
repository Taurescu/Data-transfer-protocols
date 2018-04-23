#include <my_spi.h>

static uint8_t tx;

void setup() {
  Serial.begin(115200); //debugging
  
  tx = 0;
  //Enable spi and attach the interrupt rutine
  spi_begin();
  spi_attachInterrupt();
}

//start the intrerupt with the SPI vector
//take data from Register add it to tx and store it back
ISR(SPI_STC_vect)
{
  Serial.print("Tx =");
  Serial.println(tx);
  spi_transfer(tx += spi_read());
}


void loop() { }

