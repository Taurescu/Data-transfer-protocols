#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static const char* device = "/dev/spidev0.1";
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint8_t mode = SPI_MODE_0;
static uint8_t delay = 0;
static uint32_t lenght = 1;


//take data as a parameter and send it
//store data into rx and return it
static uint8_t transfer(int fd,uint8_t *data) {

	int ret;
	uint8_t* rx = malloc(sizeof(uint8_t));
	struct spi_ioc_transfer trans;

	memset(&trans, 0, sizeof(trans));
	trans.tx_buf = (unsigned long)data;
	trans.rx_buf = (unsigned long)rx;
	trans.len = lenght;
	trans.delay_usecs = delay;
	trans.speed_hz = speed;
	trans.bits_per_word = bits;
	
	ret = ioctl(fd, SPI_IOC_MESSAGE(lenght), &trans);
		if (ret < 1)
			printf("Error: Message can't be send\n");
return *rx;
}


int main() {
	int ret = 0;
	int fd;

	fd = open(device, O_RDWR);
		if (fd < 0)
			printf("Can't open the device!\n");

	//SPI config
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
		if (ret == -1)
			printf("Cant set SPI mode\n");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("Can't set bits per word\n");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("can't set max speed hz\n");


	bool flag = true;
	int* data = malloc(sizeof(int));
	uint8_t* ptr = malloc(sizeof(int));
	*ptr = 0;

        do
	{
		printf("If you enter a number > 255 the proggram will stop!\n");
		printf("Enter your number =");
		scanf(" %d",data);
		if ((*data) > 255){
			 flag = false;
		}
		transfer(fd, (uint8_t*)data);
		for(int i = 0 ; i!= 15000; ++i){}
		printf("Your result =%d \n", transfer(fd, ptr));


	} while (flag);


	close(fd);
	free(data);
	free(ptr);
}
