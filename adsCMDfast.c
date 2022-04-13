#include "adsCMDfast.h"
#include "ads1298.h"
#include <bcm2835.h>
#include <stdio.h>

//bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); //do NOT set any /CS automatically - repeated - just in case
//bcm2835_gpio_fsel(CS_PIN, BCM2835_GPIO_FSEL_OUTP); //CS is an output

void cs_select(void){					//CS low (remember about negative logic here)
	bcm2835_gpio_clr(CS_PIN);
}

void cs_deselect(void){				//CS high (negative logic here)
	bcm2835_gpio_set(CS_PIN);
}	


void adc_wreg(int rgstr, int val){
	// writes one (val) byte to rgstr registry
	// writing: 1st byte: 010r rrrr and 2nd byte 000n nnnn
	// WREG = 0x40 (010r rrrr to write, where r rrrr = starting reg address for write)
	// and n nnnn is the number of registers to write -1 ( 0 0000 for 1 reg to write)
	cs_select();
	char tempspibuf[3] = {0,0,0};
	tempspibuf[0]=(WREG | rgstr); // WREG = 0x40 (010r rrrr to write, where r rrrr = starting reg address for write)
	tempspibuf[1]=0;
	tempspibuf[2]=val;
	
	bcm2835_spi_transfern(tempspibuf, 3);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
}

int adc_rreg(int rgstr){
	// reads selected registry
	cs_select();
	char tempbuf[3] = {0,0,0};
	tempbuf[0]=(RREG | rgstr);
	tempbuf[1]=0;
	tempbuf[2]=0;
	
	bcm2835_spi_transfern(tempbuf, 3);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	int myval = *(int*)(tempbuf);
	//int myval = *(tempbuf);
	return myval;
}

void adc_stop_reading_data(void){
	cs_select();	
	bcm2835_spi_transfer(SDATAC); //stop read data continuous mode SDATAC = 0x11 --> 0b0001 0001
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
}

void adc_read_data_cont(void){
	cs_select();
	bcm2835_spi_transfer(RDATAC);//0x10 confirmed
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();	
}

void identify_yourself(void){
	int idVal = 0;
	int maxChan = 0;
	char buf[3] = {0,0,0};
	
	buf[0]=0x20; //(RREG | ID);
	buf[1]=0x00;
	buf[2]=0x00;
	
	//buftmp[0]=0x20;
		
	//determine model number and number of channels available
	//register read command: send two bytes to ADS1298: 001r rrrr and 000n nnnn,
	//where r rrrr - starting register, n nnnn - (number of register to read - 1) ALL IN ONE BURST (no CS change)
	
	/*	
	printf("Before spi_transfern:\n");
	printf("Hex value of buf[0] = %x \n", buf[0]);
	printf("Hex value of buf[1] = %x \n", buf[1]);
	printf("Hex value of buf[2] = %x \n", buf[2]);
*/
	
	//----------TRANSMISSION--------------------	
	cs_select();
	bcm2835_spi_transfer(SDATAC); //sending 0x11
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	bcm2835_delayMicroseconds(2); //minimum of 2 SCLK (0.5us for 3.9MHz)
	cs_select();
	bcm2835_spi_transfern(buf, 3); //when sending RREG|ID it will be 0x20 | 0x00 which means: 0x20
	//bcm2835_spi_transfern(buftmp, 6);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	//---------END OF TRANSMISSION---------------
	
	/*	
	printf("After spi_transfern:\n");
	printf("Hex value of buf[0] = %x \n", buf[0]);
	printf("Hex value of buf[1] = %x \n", buf[1]);
	printf("Hex value of buf[2] = %x \n", buf[2]);
	*/
	switch (buf[2] & 0b00011111){ //least significant bits say about channels
		case 0b10000: //16
			maxChan = 4; //asd1294
			break;
		case 0b10001: //17
			maxChan = 6; //ads1296
			break;
		case 0b10010: //18
			maxChan = 8; //ads1298
			break;
		case 0b11110: //30
			maxChan = 8; //ads1299
			break;
		default:
			maxChan = 0;
		}
		
	idVal=buf[2];
	printf("Device identyfying:\n");
	printf("idVal: %x \n", idVal); //hex92 (or dec146) expected from ads1298
	printf("Device Type (ID Control Register): ");
	printf("%d\n", idVal);
	printf("Channels: ");
	printf("%d\n\n", maxChan);
	
	delay(20);
	
	cs_select();
	bcm2835_spi_transfer(SDATAC);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
}
