#include <i2c.h>

#define address 0x04 // Address to connect with the Pi3

  static char rxBuffer; 

  static void Recive_data(uint8_t);
  static void Send_data(void);
  
void setup() {
  
  setAddress(address);
  i2c_init();
  
  attach_Tx(Send_data);
  attach_Rx(Recive_data); 
   
  

 Serial.begin(9600);
 Serial.print("Beggining of transmision:\n");
}

void loop() {}

static void Recive_data(uint8_t inBytes)
{
  
   rxBuffer = (char)inBytes;
   Serial.print(rxBuffer);
}

static void Send_data(void)
{
  uint8_t lenght = 15;
  const uint8_t txGreetings[lenght] = {0x41, 0x72, 0x64, 0x75, 0x69,  
                                       0x6E, 0x6F, 0x3A, 0x20, 0x48,
                                       0x65, 0x6c, 0x6c, 0x6F, 0x21}; 

  transmit(txGreetings,lenght);
}

