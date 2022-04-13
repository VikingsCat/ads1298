#ifndef ADSCMDFAST_H
#define ADSCMDFAST_H 

// PIN described in below format tells an physical number of pin:
// for example LEDPIN is pin no 5 (known as GPIO3)
// (numbering as shown in terminal window after typing 'pinout')
#define STARTPIN	RPI_BPLUS_GPIO_J8_22 // START pin
#define DRDYPIN		RPI_BPLUS_GPIO_J8_26 // DRDY pin
#define RESETPIN	RPI_BPLUS_GPIO_J8_16 // RESET pin
#define CS_PIN		RPI_BPLUS_GPIO_J8_24 // SPI CS pin
//#define PWDNPIN		RPI_BPLUS_GPIO_J8_11 // PWDN
#define LEDPIN 		RPI_BPLUS_GPIO_J8_05 // LED for simple debug
#define ADC_PWRDWN  RPI_BPLUS_GPIO_J8_40
//#define USERSWITCH	RPI_BPLUS_GPIO_J8_15 // SW to connect to GND
#define CS_DELAY	1  // how many microseconds of delay

void cs_select(void);
void cs_deselect(void);
void adc_wreg(int rgstr, int val); // write register
void adc_send_command(int cmd); // send command
int adc_rreg(int rgstr); // read register

void adc_stop_reading_data(void); // send comand to ADS1298 to stop reading data continously
void adc_read_data_cont(void); // send command to start reading data continuosly

void identify_yourself(void); // to see if ADS can read and return basic data
#endif
