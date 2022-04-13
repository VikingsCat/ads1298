#ifndef ADS1298_H
#define ADS1298_H

	enum spi_command {
		//system commands
		WAKEUP = 0x02,
		STANDBY = 0x04,
		RESET = 0x06,
		START = 0x08,
		STOP = 0x0a,
		
		//read commands
		RDATAC = 0x10,
		SDATAC = 0x11,
		RDATA = 0x12,
		
		//register commands
		RREG = 0x20,
		WREG = 0x40
	};

	enum reg {
		// device settings
		ID = 0x00,

		// global settings
		CONFIG1 = 0x01,
		CONFIG2 = 0x02,
		CONFIG3 = 0x03,
		LOFF = 0x04,

		// channel specific settings
		CHnSET = 0x04,
		CH1SET = CHnSET + 1,
		CH2SET = CHnSET + 2,
		CH3SET = CHnSET + 3,
		CH4SET = CHnSET + 4,
		CH5SET = CHnSET + 5,
		CH6SET = CHnSET + 6,
		CH7SET = CHnSET + 7,
		CH8SET = CHnSET + 8,
		RLD_SENSP = 0x0d,
		RLD_SENSN = 0x0e,
		LOFF_SENSP = 0x0f,
		LOFF_SENSN = 0x10,
		LOFF_FLIP = 0x11,

		// lead off status
		LOFF_STATP = 0x12,
		LOFF_STATN = 0x13,

		// other
		GPIO = 0x14,
		PACE = 0x15,
		RESP = 0x16,
		CONFIG4 = 0x17,
		WCT1 = 0x18,
		WCT2 = 0x19
	};
	
	enum datarate {
		// speeds available values for CONFIG1
		// SOME may be too high for RPi SPI Clk
		// to provide enough data transfers
		LP250SPS = 0x06,
		HR500SPS = 0x86,
		LP500SPS = 0x05,
		HR1kSPS = 0x85,
		LP1kSPS = 0x04,
		HR2kSPS = 0x84,
		LP2kSPS = 0x03,
		HR4kSPS = 0x83,
		LP4kSPS = 0x02,
		HR8kSPS = 0x82,
		LP8kSPS = 0x01,
		HR16kSPS = 0x81,
		LP16kSPS = 0x00,
		HR32kSPS = 0x80
	};
	
#endif
	

