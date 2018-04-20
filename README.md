# Data-transfer-protocols

Here i'll uploade the data protocoles(I2c and SPI) i've implemented between an Arduino Mega 2560(slave) and a Raspberry Pi3(Master).

I2C:

Arduino: - Send a message to Pi3 ( "Arduino: Hello!" ) with the help of the i2c library made by me.

i2c lib: - It's a library that takes data inputs and store them intro TDCR (TwoWire Controll Register) and send them.
            It also takes data sent by Pi3 in the TDCR and store it in a 1 byte storeg "rxBuffer".
            All the functionalities in the library are made just for Arduino as a slave.
            
Raspberry Pi3: - It's python code that import a library ("smbus") wich helped send and recive data in master mode.
                     
