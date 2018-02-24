#include "WAVEFRONT.h"  // Includes functions I wrote for the temperature sensor, and I left a space for possible ADXL support

// Declares a temperature sensor object using CS on pin #6. This is the upper right (by the USB connector) connection.
unsigned char TS1_CS = 6;
ADT7320 TS1(TS1_CS);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);     // Beginning serial so we can see whats happening
  SPI.begin();            // start the SPI library
  TS1.initialize();       // Now initialize the sensor (this sets the CS pin as output, and also chooses the high resolution and continuous update modes of the temperature sensors)
}



void loop() {
  // put your main code here, to run repeatedly:

  delay(300);                               // Wait a sec! (the minimum wait time is 240 ms in continuous mode, for the A/D conversion to take place)

  // In the read_BLANK() functions I wrote the value read is both returned and stored as part of the ADT7320 object. In the following lines you can see both methods being used. Why? IDK. 

  TS1.read_status();                        // The status indicator can be used to know if the data is ready (bit 7 high for ready) but we just assume it was ready
  TS1.read_config();                        // This info can be used to verify that the sensor is in high resolution mode.
  int num_counts = TS1.read_temperature();  // This reads the last temperature value as a number of ADC bins. Despite having an output argument the latest value is also saved in the ADT7320 object TS1
  
  
  
  float temperature = (num_counts)/(128.0); // This is the formula to get temperature from counts, but only in high resolution (16b) mode.

  // And finally make a serial print statement to see the fruits of our labor.
  Serial.print("Temperature Counts: 0x"); Serial.print(TS1.TEMP,HEX); Serial.print(",   Temperature (deg C): "); Serial.print(temperature,DEC); Serial.print(",   Sensor Configuration: 0x"); Serial.print(TS1.CONFIG,HEX); Serial.print(",   Sensor Status Code: 0x"); Serial.println(TS1.STATUS,HEX);

}
