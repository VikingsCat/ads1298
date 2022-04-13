/*
 * Code to perform communication with ADS1298

 * ads2.6 - added code tracked.txt, cleaning code, unnecessary comments
 * ads27 - naming change and further cleaning
 * ---------------------------------------------------------------------
 * ****Compile and build instruction for Geany/Raspberry Pi*************
 * ---------------------------------------------------------------------
 * to build: 
 * Compile each of files seperately(from Geany)
 * Next: open terminal, cd to location where all files live and type: 
 * gcc -o newname main.o adsCMD.o -lbcm2835
 * so, for this one is:
 * ***********************************************************
 * sudo gcc -o ads27 ads27.c adsCMDfast.o hwinit.o -lbcm2835 
 * ***********************************************************
 * OR: check 'make' command in Geany 'Build'->'Set Build Command' to be
 * something like:
 * gcc -o "%e" "%e.o" adsCMDfast.o -lbcm2835  
 * Be sure to run it as Shift + F9
 * --------------------------------------------------------------------- 
 * Problems:
 * 
 */

#include <bcm2835.h>
#include "ads1298.h"	// keeps ADS1298 register's offsets
#include "adsCMDfast.h" // keeps pin assignment and basic IO operations
#include "hwinit.h" 	// init of ads, RPi pin conig

int main(int argc, char **argv)
{
	char bigbuf2[27]; // buffer for SPI transferred data from ADC
	int value1; // variable for ADC ch1 converted value
	double voltvalue1; // variable for real voltage value 
	// double = float with 15 decimal places (8 bytes) 
	// normal float is 4 bytes and have 6 decimal places
	// long double has 19 decimal places (10 bytes)

	// initialisation of bcm library
	if(!bcm2835_init())
	{
		printf("bcm2835 not initialised correctly\n");
		return 1;
	}
	
	// begin SPI using bcm2835 library
	if(!bcm2835_spi_begin())
	{ //_spi_begin returns 1 if spi_begin does its job with success
		printf("SPI not initialised correctly (Maybe you are not running as root?)\n");
		return 1;
	}
	
	ads_pi_hw_init();	// hardware init, RPi pins, SPI speed, SPI Mode
	ads_reset();	// reset the device
	identify_yourself(); // check the functioning of ADS, print the result 
	adc_stop_reading_data();
	

	ads_reset();	//reset the device
	adc_stop_reading_data();
	// setup for ADS1298
	adc_wreg(CONFIG1, HR4kSPS); // HR2kSPS); // 0x84 -> 1000 0100 => HR mode, 2kSPS
	adc_wreg(CONFIG2, 0x20); 	// for external data collection (0x20 0010 in CONFIG2 WCT_CHOP freq)	
	adc_wreg(CONFIG4, 0x00);
	
	// set channels
	for(int i=1; i<=8;++i)
	{
		adc_wreg(CHnSET + i, 0x10); //Every channel has to be set seperately
		//x00 --> 0000 0000 = Normal electrode inputs, 6x gain
		//b 0 001 0 000 => 0x10 normal input, 1x gain 
	}
	
	//mkfifo(myfifo, 0666);  // preparation to use fifo

	bcm2835_gpio_write(STARTPIN, HIGH); //START signal
	adc_read_data_cont();
	
	while (1)
	{
	
		for(int k=0;k<27;k++) bigbuf2[k]=0; //clear the buffer
		
		//collect data:
		//Look for DRDY and issue 24 + n * 24 SCLKs
		while (bcm2835_gpio_lev(DRDYPIN)==HIGH){
		}
	
		// below spi transfer takes 40-80us
		cs_select();
		bcm2835_spi_transfern(bigbuf2, 27);
		bcm2835_delayMicroseconds(CS_DELAY);
		cs_deselect();
		
		// this part will convert 1st channel ADC values into numeric value
			
		if(bigbuf2[0]==0b11000000){//kind of crc.. always 192 -> 1100 0000
			value1 = ((bigbuf2[3] << 24)|(bigbuf2[4] << 16)|(bigbuf2[5]<<8))>>8;
		}
		// convert to real voltage
		voltvalue1 = value1*0.00000029802326;
		
		printf("%f\n",voltvalue1);
		fflush(stdout); //do not wait for buffer fill, release data immediately

	}
	
	// stop reading data
	adc_stop_reading_data();
	
	//Return SPI pins to default input state
	bcm2835_spi_end();
		
	return 0;
}
