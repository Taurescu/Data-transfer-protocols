# Data-transfer-protocols

Here i'll uploade the data protocols(I2c and SPI) i've implemented between an Arduino Mega 2560(slave) and a Raspberry Pi3(Master).

I2C:

Arduino: - Send a message to Pi3 ( "Arduino: Hello!" ) and recive/print messages from Pi3 with the help of the i2c library.

i2c lib: - It's a library that takes data inputs and stores them intro TDCR (TwoWire Controll Register) and sends them.
            It also takes data sent by Pi3 in the TDCR and store it in a 1 byte storeg "rxBuffer".
            All the functionalities in the library are made just for Arduino as a slave.
            
Raspberry Pi3: - It's python code that imports a library ("smbus") wich helps send and receive data in master mode.
                     
SPI:

Arduino: - Recive numbers from Pi3 and adds them tougether and send the result back

SPI lib: -It's a library made for reciving and sending data and also to set the Arduino modes and speed in Master mode

Raspberry Pi3: - C code that reads numbers from keyboard as an input. After each number is read it sends it to Arduino and recives the sum computed by Arduino. The sum is then printed and the user is asked for another number as input. The process continues until the user enters a number greater than 255. The sum can't be greater than 255.



