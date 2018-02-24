#include "WAVEFRONT.h"  // Includes functions I wrote for the temperature sensor, and I left a space for possible ADXL support

// Declares a accelerometer object using CS on pin #0. This is the upper left (by the USB connector) connection.
unsigned char ACC1_CS = 0;
ADXL344 ACC1(ACC1_CS);

float ADXL344_C2G(int counts)
{
  float val = ((3.9*counts)/1000);
  if((counts & 0x200))
  {
    val = -3.9936+val;
  }
  return val;
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);     // Beginning serial so we can see whats happening
  SPI.begin();            // start the SPI library
  ACC1.initialize();      // Now initialize the sensor (this sets the CS pin as output, and sets some registers appropriately)
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5);

  ACC1.update_accel_counts();

  float AGX = ADXL344_C2G(ACC1.ACX);
  float AGY = ADXL344_C2G(ACC1.ACY);
  float AGZ = ADXL344_C2G(ACC1.ACZ);
  float norm = sqrt(sq(AGX)+sq(AGY)+sq(AGZ));

  Serial.print("G [x,y,z,(norm)]: [ "); Serial.print(AGX,3); Serial.print(", "); Serial.print(AGY,3); Serial.print(", "); Serial.print(AGZ,3); Serial.print(", ("); Serial.print(norm,5); Serial.println(")]");
  

//  Serial.print(" Contents of the accel buffer: [0x");
//  Serial.print(ACC1.acc_buff[0],HEX); Serial.print(ACC1.acc_buff[1], HEX); Serial.print(", 0x");
//  Serial.print(ACC1.acc_buff[2],HEX); Serial.print(ACC1.acc_buff[3], HEX); Serial.print(", 0x");
//  Serial.print(*(ACC1.acc_buff_ptr + 4),HEX); Serial.print(*(ACC1.acc_buff_ptr + 5), HEX); // Just showing the pointer method
//  Serial.println("]");

  
  
}
