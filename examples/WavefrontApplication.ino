#include "WAVEFRONT.h"

#define acc_period_us 1000
#define temp_period_us 1000000

#define acc_num_message_bytes 19
#define temp_num_message_bytes 15

#define temp_RT 2
#define temp_SN 3
#define temp_PL 2
#define temp_SH temp_SN+temp_PL+1
#define temp_SL temp_SN+temp_PL+2
#define temp_MH temp_SN+temp_PL+3
#define temp_ML temp_SN+temp_PL+4
#define temp_UH temp_SN+temp_PL+5
#define temp_UL temp_SN+temp_PL+6
#define temp_CS temp_SN+temp_PL+7

#define acc_RT 2
#define acc_SN 3
#define acc_PL 6
#define acc_SH acc_SN+acc_PL+1
#define acc_SL acc_SN+acc_PL+2
#define acc_MH acc_SN+acc_PL+3
#define acc_ML acc_SN+acc_PL+4
#define acc_UH acc_SN+acc_PL+5
#define acc_UL acc_SN+acc_PL+6
#define acc_CS acc_SN+acc_PL+7

IntervalTimer acc_timer;
IntervalTimer temp_timer;

unsigned char TS1_CS = 6; ADT7320 TS1(TS1_CS);
unsigned char TS2_CS = 7; ADT7320 TS2(TS2_CS);
unsigned char TS3_CS = 8; ADT7320 TS3(TS3_CS);
unsigned char TS4_CS = 9; ADT7320 TS4(TS4_CS);
unsigned char TS5_CS = 10; ADT7320 TS5(TS5_CS);
unsigned char TS6_CS = 11; ADT7320 TS6(TS6_CS);







unsigned char ACC1_CS = 0; ADXL344 ACC1(ACC1_CS);
unsigned char ACC2_CS = 0; ADXL344 ACC2(ACC2_CS);
unsigned char ACC3_CS = 0; ADXL344 ACC3(ACC3_CS);
unsigned char ACC4_CS = 0; ADXL344 ACC4(ACC4_CS);
unsigned char ACC5_CS = 0; ADXL344 ACC5(ACC5_CS);
unsigned char ACC6_CS = 0; ADXL344 ACC6(ACC6_CS);





//                                                start      RT    SN   TSP1  TSP2  SH    SL    MH    ML    uH    uL    CS       stop
uint8_t temp_message[temp_num_message_bytes] = {0xA0, 0xA1, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0D, 0x0A};

//                                                start      RT    SN   ACCP1 ACCP2 ACCP3 ACCP4 ACCP5 ACCP6 SH    SL    MH    ML    uH    uL    CS       stop
uint8_t acc_message[acc_num_message_bytes] = {0xA0, 0xA1, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0D, 0x0A};

volatile uint16_t seconds = 0;
volatile uint16_t milliseconds = 0;
volatile uint16_t useconds = 0;

void acc_ISR(void);
void temp_ISR(void);

void update_times(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){};

  SPI.begin();

  ACC1.initialize();
//  ACC2.initialize();
//  ACC3.initialize();
//  ACC4.initialize();
//  ACC5.initialize();
//  ACC6.initialize();
  
  TS1.initialize();
//  TS2.initialize();
//  TS3.initialize();
//  TS4.initialize();
//  TS5.initialize();
//  TS6.initialize();

  acc_timer.begin(acc_ISR,acc_period_us);
  temp_timer.begin(temp_ISR,temp_period_us);


}

void loop() {
  // put your main code here, to run repeatedly:

}

void acc_ISR (void)
{
  update_times();
  ACC1.initialize();
  ACC1.update_accel_counts();

//  ACC2.initialize();
//  ACC2.update_accel_counts();
//
//  ACC3.initialize();
//  ACC3.update_accel_counts();
//
//  ACC4.initialize();
//  ACC4.update_accel_counts();
//
//  ACC5.initialize();
//  ACC5.update_accel_counts();
//
//  ACC6.initialize();
//  ACC6.update_accel_counts();

  // Fill out time values
  acc_message[acc_RT] = 0;
  acc_message[acc_SH] = (((seconds) & 0xFF00) >> 8); acc_message[acc_SL] = ((seconds) & 0x00FF); 
  acc_message[acc_MH] = (((milliseconds) & 0xFF00) >> 8); acc_message[acc_ML] = ((milliseconds) & 0x00FF); 
  acc_message[acc_UH] = (((useconds) & 0xFF00) >> 8); acc_message[acc_UL] = ((useconds) & 0x00FF); 

   acc_message[acc_SN] = 1; transmit_acc(&ACC1);
//   acc_message[acc_SN] = 2; transmit_acc(&ACC2);
//   acc_message[acc_SN] = 3; transmit_acc(&ACC3);
//   acc_message[acc_SN] = 4; transmit_acc(&ACC4);
//   acc_message[acc_SN] = 5; transmit_acc(&ACC5);
//   acc_message[acc_SN] = 6; transmit_acc(&ACC6);
}

void temp_ISR (void)
{  
  update_times();
  TS1.initialize();
  TS1.read_temperature();
  
//  TS2.initialize();
//  TS2.read_temperature();
//
//  TS3.initialize();
//  TS3.read_temperature();
//
//  TS4.initialize();
//  TS4.read_temperature();
//
//  TS5.initialize();
//  TS5.read_temperature();
//
//  TS6.initialize();
//  TS6.read_temperature();

  // Fill out time values
  temp_message[temp_RT] = 1;
  temp_message[temp_SH] = (((seconds) & 0xFF00) >> 8); temp_message[temp_SL] = ((seconds) & 0x00FF); 
  temp_message[temp_MH] = (((milliseconds) & 0xFF00) >> 8); temp_message[temp_ML] = ((milliseconds) & 0x00FF); 
  temp_message[temp_UH] = (((useconds) & 0xFF00) >> 8); temp_message[temp_UL] = ((useconds) & 0x00FF); 

  temp_message[temp_SN] = 1; transmit_temp(&TS1);
//  temp_message[temp_SN] = 2; transmit_temp(&TS2);
//  temp_message[temp_SN] = 3; transmit_temp(&TS3);
//  temp_message[temp_SN] = 4; transmit_temp(&TS4);
//  temp_message[temp_SN] = 5; transmit_temp(&TS5);
//  temp_message[temp_SN] = 6; transmit_temp(&TS6);
}


void update_times( void )
{
  unsigned long microsnow = micros();

//  Serial.print(((microsnow/1000000) & 0xFF00) >> 8);Serial.println((microsnow/1000000) & 0x00FF);
  
  seconds = microsnow / 1000000;
  milliseconds = (microsnow - (seconds * 1000000)) / 1000;
  useconds = (microsnow - (seconds * 1000000) - (milliseconds * 1000));
}


void transmit_acc(ADXL344 * ptr)
{ 
   acc_message[4] = (((ptr->ACX) & 0xFF00)>>8); acc_message[5] = (((ptr->ACX) & 0x00FF));
   acc_message[6] = (((ptr->ACY) & 0xFF00)>>8); acc_message[7] = (((ptr->ACY) & 0x00FF));
   acc_message[8] = (((ptr->ACZ) & 0xFF00)>>8); acc_message[9] = (((ptr->ACZ) & 0x00FF));

    uint8_t CS = acc_message[4];
   for(uint8_t indi = 5; indi <= acc_CS; indi++)
   {
      CS = (CS ^ acc_message[indi]);
   }
   acc_message[acc_CS] = CS;
  
  Serial.write(acc_message, acc_num_message_bytes);
}

void transmit_temp(ADT7320 * ptr)
{ 
   temp_message[4] = (((ptr->TEMP) & 0xFF00)>>8); temp_message[5] = (((ptr->TEMP) & 0x00FF));

    uint8_t CS = temp_message[4];
   for(uint8_t indi = 5; indi <= temp_CS; indi++)
   {
      CS = (CS ^ temp_message[indi]);
   }

   temp_message[temp_CS] = CS;
  
  Serial.write(temp_message, temp_num_message_bytes);
}

