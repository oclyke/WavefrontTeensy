/* 
WAVEFRONT.h

A header file to contain the functions and objects required to interface to the sensors used in the environmental sensor system in the Wavefront project.

Written by Owen Lyke February 2018
Updated: February 2018

Contents: 
	Functions: 

	Classes: 
		ADT7320 - For interface to the sensor of the same name
		ADXL344 - For interface to the sensor of the same name


TO DO:
Go through and clean up all the serial print statements once it is working...

*/




#ifndef WAVEFRONT_h
#define WAVEFRONT_h


// Includes
// --------
#include "Arduino.h"
#include <SPI.h>


// #define ADT7320_SPI_MAX_F 10000000			// Hz : Maximum SPI clock frequency
//#define ADT7320_SPI_MAX_F 5000000			// Hz : The SPI frequncy we ultimately want to 
#define ADT7320_SPI_MAX_F 1000000			// Hz : 1MHz, slower to see if that was the problem

enum ADT7320_CODES {
	ADT7320_REG_Status = 0,
	ADT7320_REG_Config,
	ADT7320_REG_Temp_val,
	ADT7320_REG_ID,
	ADT7320_REG_Tcrit_set,
	ADT7320_REG_Thyst_set,
	ADT7320_REG_Thigh_set,
	ADT7320_REG_Tlow_set
};


class ADT7320
{
public:
	// Parameters
	unsigned char CS_PIN; 					// Defines which of the Teensy3.6 pins is used to select the particular device 

	unsigned char INT_PIN;					// Defines which, if any, pin is connected to the interrupt for over or under-temeprature alarms
	unsigned char CT_PIN;					// Defines which, if any, pin is connected to the interrupt for critical overtemperature alarams

	int TEMP;								// Holds the 16 bit twos complement signed value that is returned by the sensor
	unsigned char STATUS;					// Holds the most recent value read from the status register
	unsigned char CONFIG;					// Holds the most recent value read from the configuration register

	// Methods
	ADT7320(unsigned char CS);
	ADT7320(unsigned char CS, unsigned char IN, unsigned char CT);

	void initialize();								// Sets the CS pin to output, then sets the confguration register for continuous high-resolution mode
	int read_temperature();							// Get the contents of the temperature register, and also stores that reading in the TEMP parameter of the object. The units are "counts" and need to be appropriately converted to a real temperature according to the datasheet.
	unsigned char read_status();					// Get the contents of the status register
	unsigned char read_config();					// Get the contents of the configuration register
	

private:
	// These functions made private because they don't include the necessary CS action or SPI transaction begin/end. They are to be used to assist the public functions
	void write_register_8(unsigned char val, unsigned char register_addr);
	void write_register_16(unsigned int val, unsigned char register_addr);
	unsigned char read_register_8(unsigned char register_addr);
	unsigned int read_register_16(unsigned char register_addr);
};





//#define ADXL344_SPI_MAX_F 5000000		// Maximum SPI clock speed for the ADXL344
#define ADXL344_SPI_MAX_F 1000000			// A lower speed for preliminary testing
#define ADXL344_ACC_BUFF_LENGTH 6			// The number of bytes that the buffer for acceleration data will encompass

enum ADXL344_CODES {
	// Register addresses
	ADXL344_REG_DEVID = 0,
	// Reserved registers 1-28
	ADXL344_REG_THRESH_TAP = 29,
	ADXL344_REG_OFSX,
	ADXL344_REG_OFSY,
	ADXL344_REG_OFSZ,
	ADXL344_REG_DUR,
	ADXL344_REG_Latent,
	ADXL344_REG_Window,
	ADXL344_REG_THRESH_ACT,
	ADXL344_REG_THRESH_INACT,
	ADXL344_REG_TIME_INACT,
	ADXL344_REG_ACT_INACT_CTL,
	ADXL344_REG_THRESH_FF,
	ADXL344_REG_TIME_FF,
	ADXL344_REG_TAP_AXES,
	ADXL344_REG_ACT_TAP_STATUS,
	ADXL344_REG_BW_RATE,
	ADXL344_REG_POWER_CTL,
	ADXL344_REG_INT_ENABLE,
	ADXL344_REG_INT_MAP,
	ADXL344_REG_INT_SOURCE,
	ADXL344_REG_DATA_FORMAT,
	ADXL344_REG_DATAX0,
	ADXL344_REG_DATAX1,
	ADXL344_REG_DATAY0,
	ADXL344_REG_DATAY1,
	ADXL344_REG_DATAZ0,
	ADXL344_REG_DATAZ1,
	ADXL344_REG_FIFO_CTL,
	ADXL344_REG_FIFO_STATUS,
	ADXL344_REG_TAP_SIGN,
	ADXL344_REG_ORIENT_CONF,
	ADXL344_REG_ORIENT,

	// Data Rate Codes (XXXXYY = XXXX.YY Hz)
	ADXL344_DRC_000010 = 0,
	ADXL344_DRC_000020,
	ADXL344_DRC_000039,
	ADXL344_DRC_000078,
	ADXL344_DRC_000156,
	ADXL344_DRC_000313,
	ADXL344_DRC_000625,
	ADXL344_DRC_001250,
	ADXL344_DRC_002500,
	ADXL344_DRC_005000,
	ADXL344_DRC_010000,
	ADXL344_DRC_020000,
	ADXL344_DRC_040000,
	ADXL344_DRC_080000,
	ADXL344_DRC_160000,
	ADXL344_DRC_320000,


};

class ADXL344
{
public:

	// Parameters
	unsigned char CS_PIN;		// Holds the Teensy pin number that is used as the CS for the sensor

	unsigned char acc_buff[ADXL344_ACC_BUFF_LENGTH];			// A small buffer for the acceleration data to be placed in from a multiple-read operation
	unsigned char * acc_buff_ptr;

	int ACX;					// 16 bit twos complement signed number of counts on the X axis
	int ACY;					// 16 bit twos complement signed number of counts on the Y axis
	int ACZ;					// 16 bit twos complement signed number of counts on the Z axis

	ADXL344(unsigned char CS);	// Constructor, assigns the CS pin

	void initialize();								// Sets the CS pin to output, then sets the confguration register for continuous high-resolution mode

	void update_accel_counts();
	void fill_acc_buff();

private:
	// These functions made private because they don't include the necessary CS action or SPI transaction begin/end. They are to be used to assist the public functions
	void write_register_8(unsigned char val, unsigned char register_addr);
	unsigned char read_register_8(unsigned char register_addr);
	void read_multiple_8(unsigned char * buffer_ptr, unsigned char num_reads, unsigned char init_register_addr);
};




#endif
