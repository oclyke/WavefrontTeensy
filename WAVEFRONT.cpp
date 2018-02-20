/* 
WAVEFRONT.cpp

Header: WAVEFRONT.h

Written by Owen Lyke

Updated: February 2018

TO DO:


*/


/*
-----------------------------------
Obligatory includes
-----------------------------------
*/
#include "WAVEFRONT.h"
#include <SPI.h>		




/*
-----------------------------------
ADT7320 Temperature Sensor Support:
-----------------------------------
*/
SPISettings ADT7320_SPI_SETTINGS(ADT7320_SPI_MAX_F,MSBFIRST,SPI_MODE0);

ADT7320::ADT7320(unsigned char CS)
{
	CS_PIN = CS;

	TEMP = 0;
	STATUS = 0;
	CONFIG = 0;
}

ADT7320::ADT7320(unsigned char CS, unsigned char IN, unsigned char CT)
{
	CS_PIN = CS;
	INT_PIN = IN;
	CT_PIN = CT;

	TEMP = 0;
	STATUS = 0;
	CONFIG = 0;
}

void ADT7320::initialize()
{
	// Setup the CS pin
	pinMode(CS_PIN, OUTPUT);


	// Now set the desired (high resolution and continuous measurement) modes (This is done by setting the configuration register to 0b10000000)
	SPI.beginTransaction(SPISettings(ADT7320_SPI_MAX_F,MSBFIRST,SPI_MODE0));
	digitalWrite(CS_PIN,LOW);
	write_register_8(0b10000000, ADT7320_REG_Config);
	digitalWrite(CS_PIN,HIGH);
	SPI.endTransaction();
}

int ADT7320::read_temperature()
{
	int val = 0;

	SPI.beginTransaction(SPISettings(ADT7320_SPI_MAX_F,MSBFIRST,SPI_MODE0));
	digitalWrite(CS_PIN,LOW);
	val = (int)read_register_16(ADT7320_REG_Temp_val);
	digitalWrite(CS_PIN,HIGH);
	SPI.endTransaction();

	TEMP = val;

	return val;
}

unsigned char ADT7320::read_status()
{
	unsigned char val = 0;

	SPI.beginTransaction(ADT7320_SPI_SETTINGS);
	digitalWrite(CS_PIN,LOW);
	val = read_register_8(ADT7320_REG_Status);
	digitalWrite(CS_PIN,HIGH);
	SPI.endTransaction();

	STATUS = val;

	return val;
}

unsigned char ADT7320::read_config()
{
	unsigned char val = 0;

	SPI.beginTransaction(ADT7320_SPI_SETTINGS);
	digitalWrite(CS_PIN,LOW);
	val = read_register_8(ADT7320_REG_Config);
	digitalWrite(CS_PIN,HIGH);
	SPI.endTransaction();

	CONFIG = val;

	return val;
}

void ADT7320::write_register_8(unsigned char val, unsigned char register_addr)
{
	SPI.transfer( (0 << 6) | (register_addr << 3) );		// Send the command byte
	SPI.transfer(val);										// Send the data to write
}

void ADT7320::write_register_16(unsigned int val, unsigned char register_addr)
{
	SPI.transfer( (0 << 6) | (register_addr << 3) );		// Send the command byte
	SPI.transfer((val && 0xFF00) >> 8);						// Send the data to write, high byte first
	SPI.transfer((val && 0x00FF) >> 0);						// Send the data to write, low byte second
}

unsigned char ADT7320::read_register_8(unsigned char register_addr)
{
	unsigned char val = 0;
	SPI.transfer( (1 << 6) | (register_addr << 3) );		// Send the command byte
	val = SPI.transfer(0);									// It doesn't matter what you transfer here, the data from the command is returned at this time

	return val;												// Pass out the result
}

unsigned int ADT7320::read_register_16(unsigned char register_addr)
{
	unsigned char low,high;
	unsigned int val = 0;
	SPI.transfer( (1 << 6) | (register_addr << 3) );		// Send the command byte
	high = SPI.transfer(0);									// The high byte comes first
	low = SPI.transfer(0);									// The low byte comes next

	val = ((high << 8) | (low << 0));

	return val;												// Pass out the result
}

























/*
------------------------------------
ADXL344 Acceleration Sensor Support:
------------------------------------
*/
SPISettings ADXL344_SPI_SETTINGS(ADXL344_SPI_MAX_F,MSBFIRST,SPI_MODE3);


ADXL344::ADXL344(unsigned char CS)
{
	CS_PIN = CS;

	ACX = 0;
	ACY = 0;
	ACZ = 0;

	acc_buff_ptr = &acc_buff[0];
}

void ADXL344::initialize()
{
	pinMode(CS_PIN, OUTPUT);																	// Setup the CS pin
	digitalWrite(CS_PIN,HIGH);																	// Make sure to trigger a read

	SPI.beginTransaction(ADXL344_SPI_SETTINGS);													// Start the transaction with the settings defined for the ADXL344
	SPI.transfer(0xAA);																			// I am using this to get the bus into the right state before we begin talking...

	digitalWrite(CS_PIN,LOW);																	// Selects the slave device
	write_register_8(( (0 << 4) | (ADXL344_DRC_160000) ), ADXL344_REG_BW_RATE); 				// This sets the output rate to 1600 Hz, meaning a 800 Hz bandwidth, and also sets high power mode (higher resolution)
	write_register_8( 0b00001000, ADXL344_REG_POWER_CTL); 										// This specifies the device in measurement mode as opposed to standby, and also puts it in normal operation as opposed to sleep mode
	write_register_8( 0b00001000, ADXL344_REG_DATA_FORMAT); 									// Sets No self-test, 4-wire spi mode, interrupts to active high, full resolution (16 bit), right-justified mode with sign extension, and +/- 2g range
	write_register_8( 0b00011111, ADXL344_REG_FIFO_CTL); 										// Sets FIFO to BYPASS
	digitalWrite(CS_PIN,HIGH);																	// De-select the slave device
	SPI.endTransaction();																		// Release the SPI bus for others
}

void ADXL344::update_accel_counts()
{
	// The purpose of this function is to update the ACX, ACY, and ACZ values, after which the data can be retreived if desired. 

	fill_acc_buff();					// Get the new values and put them in the buffer

	// Determine the proper sign for each axis
	// if(*(acc_buff_ptr + 1) & 0x02){ ACX = (signed int)((0xFF << 10) | ( ((*(acc_buff_ptr + 1) & 0x03) << 8) | (*(acc_buff_ptr + 0) << 0) )); }
	// else{ ACX = (signed int)( ((*(acc_buff_ptr + 1) & 0x03) << 8) | (*(acc_buff_ptr + 0) << 0) ); }

	// if(*(acc_buff_ptr + 3) & 0x02){ ACX = (signed int)((0xFF << 10) | ( ((*(acc_buff_ptr + 3) & 0x03) << 8) | (*(acc_buff_ptr + 2) << 0) )); }
	// else{ ACX = (signed int)( ((*(acc_buff_ptr + 3) & 0x03) << 8) | (*(acc_buff_ptr + 2) << 0) ); }

	// if(*(acc_buff_ptr + 5) & 0x02){ ACX = (signed int)((0xFF << 10) | ( ((*(acc_buff_ptr + 5) & 0x03) << 8) | (*(acc_buff_ptr + 4) << 0) )); }
	// else{ ACX = (signed int)( ((*(acc_buff_ptr + 5) & 0x03) << 8) | (*(acc_buff_ptr + 4) << 0) ); }
	
	ACX = (signed int)( ((*(acc_buff_ptr + 1) & 0x03) << 8) | (*(acc_buff_ptr + 0) << 0) );
	ACY = (signed int)( ((*(acc_buff_ptr + 3) & 0x03) << 8) | (*(acc_buff_ptr + 2) << 0) );
	ACZ = (signed int)( ((*(acc_buff_ptr + 5) & 0x03) << 8) | (*(acc_buff_ptr + 4) << 0) );
}

void ADXL344::fill_acc_buff()
{
	// This fills the acc_buff with the values from registers {DATAX0, DATAX1, DATAY0, DATAY1, DATAZ0, DATAZ1} in that order. 

	SPI.beginTransaction(ADXL344_SPI_SETTINGS);													// Start the transaction with the settings defined for the ADXL344
	SPI.transfer(0xAA);																			// Used to force the bus into the correct state before selecting the device
	digitalWrite(CS_PIN,LOW);																	// Selects the slave device
	read_multiple_8(acc_buff_ptr, ADXL344_ACC_BUFF_LENGTH, ADXL344_REG_DATAX0);					// Perform the read (6 bytes, as long as the acc buff length is 6)
	digitalWrite(CS_PIN,HIGH);																	// De-select the slave device
	SPI.endTransaction();																		// Release the SPI bus for others
}

void ADXL344::write_register_8(unsigned char val, unsigned char register_addr)
{
	SPI.transfer( (0b00 << 6) | (register_addr & 0x3F) );		// Send the command byte for a single-byte write 
	SPI.transfer(val);										// Send the data to write
}

unsigned char ADXL344::read_register_8(unsigned char register_addr)
{
	unsigned char val = 0;

	SPI.transfer( (0b10 << 6) | (register_addr & 0x3F) );		// Send the command byte for a single-byte read
	val = SPI.transfer(0x00);									// Transfer anything, the result is the desired data

	return val;
}

void ADXL344::read_multiple_8(unsigned char * buffer_ptr, unsigned char num_reads, unsigned char init_register_addr)
{
	SPI.transfer( (0b11 << 6) | (init_register_addr & 0x3F) );		// Send the command byte for a multiple-byte read
	for(unsigned char indi = 0; indi < num_reads; indi++)
	{
		*(buffer_ptr + indi) = SPI.transfer(0x00); 					// Fill the buffer with values from the read
	}
}

































