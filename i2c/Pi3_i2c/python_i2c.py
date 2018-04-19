import smbus
import time

bus = smbus.SMBus(1)

address = 0x04

def readNumber():
   number = bus.read_byte(address)
   return number

def writeNumber(value):
    bus.write_byte(address, value)
    return -1

def readBlockData(numBytes):
    res = []
    x = bus.read_i2c_block_data(address, numBytes)
    res.extend(x)
    return res;

def readBunchOfData(numBytes):
    recive_data = readBlockData(numBytes)
    print("We recive ", numBytes, " bytes of data")
    for i in range(0, numBytes):
         print(chr(recive_data[i]))

 
greetings = [0x48, 0x65, 0x6C, 0x6C,
	     0x6F, 0x20, 0x54, 0x68,
	     0x65, 0x72, 0x65, 0x21];	
for i in range(0, 12):
    writeNumber(greetings[i])
    time.sleep(0.1)
    
readBunchOfData(15)
