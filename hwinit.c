#include "adsCMDfast.h"
#include "ads1298.h"
#include <bcm2835.h>
	
void ads_pi_hw_init(void)
{
	bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); //do NOT set any /CS automatically
	//pins setup:
	bcm2835_gpio_fsel(LEDPIN, BCM2835_GPIO_FSEL_OUTP); //LED pin output
	bcm2835_gpio_fsel(RESETPIN, BCM2835_GPIO_FSEL_OUTP); //RESETPIN as output
	bcm2835_gpio_fsel(STARTPIN, BCM2835_GPIO_FSEL_OUTP); //STARTPIN as output
	bcm2835_gpio_fsel(CS_PIN, BCM2835_GPIO_FSEL_OUTP); //CS is an output
	bcm2835_gpio_fsel(DRDYPIN, BCM2835_GPIO_FSEL_INPT); //DRDY input
	//bcm2835_gpio_set_pud(DRDYPIN, BCM2835_GPIO_PUD_UP); //DRDY pull-up
	
	//bcm2835_gpio_len(DRDYPIN); //low detect enable
	
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
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);

	//Set SPI data mode
	//	BCM2835_SPI_MODE0 = 0,  // CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	//	BCM2835_SPI_MODE1 = 1,  // CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE2 = 2,  // CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE3 = 3,  // CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);		//(SPI_MODE_# | SPI_CS_HIGH)=Chip Select active high, (SPI_MODE_# | SPI_NO_CS)=1 device per bus no Chip Select

	//bcm2835_gpio_write(LEDPIN, LOW);
}

void ads_reset(void)
{
	bcm2835_gpio_write(RESETPIN, LOW);
	delay(1);
	bcm2835_gpio_write(RESETPIN, HIGH);
	delay(150); //wait  (0.128s required)
}
