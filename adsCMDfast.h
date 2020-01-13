#define STARTPIN	RPI_BPLUS_GPIO_J8_22 //START pin
#define DRDYPIN		RPI_BPLUS_GPIO_J8_26 //DRDY pin
#define RESETPIN	RPI_BPLUS_GPIO_J8_16 //RESET pin
#define CS_PIN		RPI_BPLUS_GPIO_J8_24 //SPI CS pin
//#define PWDNPIN		RPI_BPLUS_GPIO_J8_11 //PWDN
#define LEDPIN 		RPI_BPLUS_GPIO_J8_05 //LED for simple debug
//#define USERSWITCH	RPI_BPLUS_GPIO_J8_15 //SW to connect to GND
#define CS_DELAY	1  //how many microseconds of delay

void cs_select(void);
void cs_deselect(void);
void adc_wreg(int rgstr, int val); //write register
void adc_send_command(int cmd); //send command
void adc_rreg(int rgstr); //read register
