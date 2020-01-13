/*
 * Code to perform communication with ADS1298
 * 
 * ads1_1 - basic communication established. ADS gives an answer to Raspberry Pi B through SPI bus.
 * ads1.3 - read device signature, reset introduced, basic initial settings, generating test signal and printf of one channel and one value
 * ads1.4 - preparation for pipeing to continously give one channel value out
 * 
 * ads2.0 - seperated files, 'manual' control for /CS chip
 * ads2.1 - output now: saving to file (to allow use with kst2 - fast plotting program)
 * ads2.2 - time stamps
 * ads2.3 - volt values instead of numbers to be saved to file
 * ads2.4 - 8MHz SPI instead 4MHz
 * ---------------------------------------------------------------------------------------
 * *************Compile and build instruction for Geany/Raspberry Pi**********************
 * ---------------------------------------------------------------------------------------
 * to build: compile each of files seperately(from Geany), next: open terminal, cd to location where
 * all files live and type: gcc -o newname main.o adsCMD.o -lsbcm2835 
 * OR: check 'make' command in Geany 'Build'->'Set Build Command' to be something like:
 * gcc -o "%e" "%e.o" adsCMDfast.o -lbcm2835  Be sure to run it as Shift + F9
 * ---------------------------------------------------------------------------------------
 * 
 * Problems:
 * 
 */

#include <stdio.h>
#include <bcm2835.h>
#include <time.h> //for timestamps
#include "ads1298.h"
#include "adsCMDfast.h" //keeps pin assignment and basic IO operations


int main(int argc, char **argv){
	
	char buf[3] = {0,0,0};
	char bigbuf[27];
	char bigbuf2[27];
	int value1, value2, value3, value4, value5, value6, value7, value8; //ch1
	clock_t t, clockbegin;
	double voltvalue1, voltvalue2, voltvalue3, voltvalue4, voltvalue5, voltvalue6, voltvalue7, voltvalue8; 
	
	//saving to file
	FILE *pFile;
	
	if(!bcm2835_init()){
		printf("Oops, bcm2835 not initialised correctly\n");
		return 1;
	}
	
	if(!bcm2835_spi_begin()){ //_spi_begin returns 1 if spi_begin does its job with success
		printf("SPI not initialised correctly (Maybe you are not running as root?)\n");
		return 1;
	}

	bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); //do NOT set any /CS automatically
	//pins setup:
	bcm2835_gpio_fsel(LEDPIN, BCM2835_GPIO_FSEL_OUTP); //LED pin output
	bcm2835_gpio_fsel(RESETPIN, BCM2835_GPIO_FSEL_OUTP); //RESETPIN as output
	bcm2835_gpio_fsel(STARTPIN, BCM2835_GPIO_FSEL_OUTP); //STARTPIN as output
	bcm2835_gpio_fsel(CS_PIN, BCM2835_GPIO_FSEL_OUTP); //CS is an output
	bcm2835_gpio_fsel(DRDYPIN, BCM2835_GPIO_FSEL_INPT); //DRDY input
	//bcm2835_gpio_set_pud(DRDYPIN, BCM2835_GPIO_PUD_UP); //DRDY pull-up
	
	//reset high
	bcm2835_gpio_write(RESETPIN, HIGH);
	
	//START low
	bcm2835_gpio_write(STARTPIN, LOW);
	
	//bit order
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	
	//Set CS pin polarity to low (LOW when CS active)
	//bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
	
	//Set which CS pin to use for next transfers
	//bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

	//Set SPI clock speed
	//	BCM2835_SPI_CLOCK_DIVIDER_65536 = 0,       ///< 65536 = 262.144us = 3.814697260kHz (total H+L clock period) 
	//	BCM2835_SPI_CLOCK_DIVIDER_32768 = 32768,   ///< 32768 = 131.072us = 7.629394531kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_16384 = 16384,   ///< 16384 = 65.536us = 15.25878906kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_8192  = 8192,    ///< 8192 = 32.768us = 30/51757813kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_4096  = 4096,    ///< 4096 = 16.384us = 61.03515625kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_2048  = 2048,    ///< 2048 = 8.192us = 122.0703125kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_1024  = 1024,    ///< 1024 = 4.096us = 244.140625kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_512   = 512,     ///< 512 = 2.048us = 488.28125kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_256   = 256,     ///< 256 = 1.024us = 976.5625MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_128   = 128,     ///< 128 = 512ns = = 1.953125MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_64    = 64,      ///< 64 = 256ns = 3.90625MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_32    = 32,      ///< 32 = 128ns = 7.8125MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_16    = 16,      ///< 16 = 64ns = 15.625MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_8     = 8,       ///< 8 = 32ns = 31.25MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_4     = 4,       ///< 4 = 16ns = 62.5MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_2     = 2,       ///< 2 = 8ns = 125MHz, fastest you can get
	//	BCM2835_SPI_CLOCK_DIVIDER_1     = 1,       ///< 1 = 262.144us = 3.814697260kHz, same as 0/65536
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);

	//Set SPI data mode
	//	BCM2835_SPI_MODE0 = 0,  // CPOL = 0, CPHA = 0, Clock idle low, data clocked in on rising edge, output data (change) on falling edge
	//	BCM2835_SPI_MODE1 = 1,  // CPOL = 0, CPHA = 1, Clock idle low, data clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE2 = 2,  // CPOL = 1, CPHA = 0, Clock idle high, data clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE3 = 3,  // CPOL = 1, CPHA = 1, Clock idle high, data clocked in on rising, edge output data (change) on falling edge
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);

	bcm2835_gpio_write(LEDPIN, LOW);
		
	//reset the device
	bcm2835_gpio_write(RESETPIN, LOW);
	delay(1);
	bcm2835_gpio_write(RESETPIN, HIGH);
	delay(150);//wait  (0.128s required)
		
	buf[0]=(RREG | ID);
	buf[1]=0;
	buf[2]=0;
	
	cs_select();	
	bcm2835_spi_transfer(SDATAC); //stop read data continuous mode SDATAC
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	
	cs_select();	
	bcm2835_spi_transfern(buf, 3);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();


	bcm2835_gpio_write(RESETPIN, LOW);
	delay(1);
	bcm2835_gpio_write(RESETPIN, HIGH);
	delay(250);

	cs_select();
	bcm2835_spi_transfer(SDATAC); //SDATAC=0x11
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	
	//send WREG CONFIG3 0xC0 - sets 7th and 6th bit of CONFIG3 (/PD_REFBUF and bit6) to 1 - internal reference
	adc_wreg(CONFIG3, 0xC0); //wreg = 0x40 | registry -> CONFIG3 = 0x03, 0xC0, so sending 0x43, 0x00, 0xC0 sent ok
	delay(1);
	
	//set device in HR mode and DR=fMOD/1024
	//WREG CONFIG1 0x86 sets HR, DR2 and DR1 bits in CONFIG1 registry(Arduino does HR, DR2, DR0, this would be 0x85 -> 1000 0101)
	//CONFIG1 bit 7 changes HR (when 1) to LowPower (when set to 0)
	//CONFIG1 setup the data rate in DR[2:0] - 
	//three LSB bits 	(110 - fMOD/1024 = 500SPS in HR mode and 250SPS in LP mode)
	//					(101 - fMOD/512  = 1kSPS  in HR		 and 500SPS in LP mode)
	//					(100 - fMOD/256  = 2kSPS  in HR
	//					(...)
	//					(010 - fMOD/64	 = 8kSPS  in HR			 4kSPS in LP mode)
	// RESET STATE OF CONFIG1 is 0000 0110 which is: LP mode, fMOD/1024
	// modes of fMOD/32 and fMOD/16 makes 16kSPS and 32kSPS, but in resolution of 19-bit and 16-bit (higher bits can be ignored in these cases)
	//adc_wreg(CONFIG1, 0x86); // 0x40|reg => 0x41 0x00 0x86 --> 1000 0110 => HR mode, 500SPS
	adc_wreg(CONFIG1, 0x85); //1000 0101 => HR mode, 1kSPS
	
	//WREG CONFIG2 0x00 dont set any bits in CONFIG2 registry
	adc_wreg(CONFIG2, 0x00); // 0x40|reg => 0x42 0x00 0x00
	
	//set all channels to input short
	//WREG CHnSET 0x01
	for(int i=1; i<=8;++i){
		adc_wreg(CHnSET + i, 0x01); //0x40|reg ->ch1 -> 0x04+i=0x45, ch2=0x46, ... ch8=0x4C
									//CHnSET 0000 0001 - input shorted (for offset or noise measurement)
									// this is happening - confirmed with scope
	}
	
	//set pin START = 1
	bcm2835_gpio_write(STARTPIN, HIGH); //monitor STARTPIN

	//send RDATAC
	cs_select();
	bcm2835_spi_transfer(RDATAC); //0x10
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	
	//look for DRDY and issue 24+n*24SCLKs
	while ((bcm2835_gpio_lev(DRDYPIN))==HIGH){ //wait for HIGH on DRDY
	}
	cs_select();
	bcm2835_spi_transfern(bigbuf, 27); //this would measure noise
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();

	bcm2835_gpio_write(STARTPIN, LOW); //monitor STARTPIN and confirm.... ok, present
	
	cs_select();
	bcm2835_spi_transfer(SDATAC);//stop data
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();	

	adc_wreg(CONFIG2, 0x00); //for external data collection

	cs_select();
	char tempspibuf[3] = {0,0,0};
	tempspibuf[0]=0x41; //write to CONFIG1
	tempspibuf[1]=0;
	tempspibuf[2]=0x85; //value to write
	
	bcm2835_spi_transfern(tempspibuf, 3);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();
	//end of settings for higher SPS
	
	for(int i=1; i<=8;++i){
		adc_wreg(CHnSET + i, 0x10); //Every channel has to be set seperately
		//x00 --> 0000 0000 = Normal electrode inputs, 6x gain
		//b 0 001 0 000 => 0x10 normal input, 1x gain 
	}

	bcm2835_gpio_write(STARTPIN, HIGH); //START signal
	
	cs_select();
	bcm2835_spi_transfer(RDATAC);//0x10 confirmed
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();	
	clockbegin=clock(); //start timestamps
	while(1){
		for(int k=0;k<27;k++) bigbuf2[k]=0; //clear the buffer
		//collect data:
		//Look for DRDY and issue 24 + n * 24 SCLKs
		while (bcm2835_gpio_lev(DRDYPIN)==HIGH){
		}
		t=clock()-clockbegin;
		double time_stamp = ((double)t)/CLOCKS_PER_SEC;
		cs_select();
		bcm2835_spi_transfern(bigbuf2, 27);
		bcm2835_delayMicroseconds(CS_DELAY);
		cs_deselect();	
		// bigbuf2[0:2] 0,1,2 - status; 3,4,5 -ch1; 6,7,8 -ch2; 9,10,11 -ch3 | 12,13,14 -ch4|
		// 15,16,17 -ch5   18,19,20 -ch6  21,22,23 -ch7  24,25,26 -ch8
		//value = ((bigbuf2[16] << 24)|(bigbuf2[17] << 16)|(bigbuf2[18]<<8))>>8; //3 seperate bytes into one 32-bit signed integer value
		//status:
		//printf("%d\n", ((bigbuf2[0] << 24)|(bigbuf2[1] << 16)|(bigbuf2[2]<<8))>>8);
		
		//ch1
		//printf("%d\n", ((bigbuf2[3] << 24)|(bigbuf2[4] << 16)|(bigbuf2[5]<<8))>>8);

		//value = ((bigbuf2[3] << 24)|(bigbuf2[4] << 16)|(bigbuf2[5]<<8))>>8; //3 seperate bytes into one 32-bit signed integer value
		if(bigbuf2[0]==0b11000000){//kind of crc.. always 192 -> 1100 0000
			value1 = ((bigbuf2[3] << 24)|(bigbuf2[4] << 16)|(bigbuf2[5]<<8))>>8;
			value2 = ((bigbuf2[6] << 24)|(bigbuf2[7] << 16)|(bigbuf2[8]<<8))>>8;
			value3 = ((bigbuf2[9] << 24)|(bigbuf2[10] << 16)|(bigbuf2[11]<<8))>>8;
			value4 = ((bigbuf2[12] << 24)|(bigbuf2[13] << 16)|(bigbuf2[14]<<8))>>8;
			value5 = ((bigbuf2[15] << 24)|(bigbuf2[16] << 16)|(bigbuf2[17]<<8))>>8;
			value6 = ((bigbuf2[18] << 24)|(bigbuf2[19] << 16)|(bigbuf2[20]<<8))>>8;
			value7 = ((bigbuf2[21] << 24)|(bigbuf2[22] << 16)|(bigbuf2[23]<<8))>>8;
			value8 = ((bigbuf2[24] << 24)|(bigbuf2[25] << 16)|(bigbuf2[26]<<8))>>8; 
			//3 seperate bytes into one 32-bit signed integer value
		}
		
		//else printf("f");
		voltvalue1 = value1*0.00000029802326;
		voltvalue2 = value2*0.00000029802326;
		voltvalue3 = value3*0.00000029802326;
		voltvalue4 = value4*0.00000029802326;
		voltvalue5 = value5*0.00000029802326;
		voltvalue6 = value6*0.00000029802326;
		voltvalue7 = value7*0.00000029802326;
		voltvalue8 = value8*0.00000029802326;
		pFile=fopen("voltdatafast.dat","a"); //open file to append(file is created if it doesn't exist)
		if(pFile==NULL){
			perror("Error opening file");
		}
		else {
			fprintf(pFile,"%f %f %f %f %f %f %f %f %f\n", time_stamp, voltvalue1, voltvalue2, voltvalue3, voltvalue4, voltvalue5, voltvalue6, voltvalue7, voltvalue8);
		}
		fclose(pFile);
	}
	cs_select();
	bcm2835_spi_transfer(SDATAC);
	bcm2835_delayMicroseconds(CS_DELAY);
	cs_deselect();	
	//Return SPI pins to default input state
	bcm2835_spi_end();
		
	return 0;
}


