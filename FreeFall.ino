#include <Wire.h>

#define slave_addr 0x18

#define STATUS_REG_AUX 0x07 //output

#define CTRL_REG0 0x1E  //Take the default value
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define CTRL_REG6 0x25

#define REFERENCE 0x26
#define STATUS_REG 0x27

//Output Registers
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define FIFO_CTRL_REG 0X2E  //Take the default value: bypass mode, trigger for INT1

// Interrupt Signals
#define INT1_CFG 0x30
#define INT1_SRC 0x31
#define INT1_THS 0x32
#define INT1_DURATION 0x33


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                             // Initialize serial communication at 9600
  
  Wire.begin();                                   // Initialize Arduino in I2C master.
  Wire.beginTransmission(slave_addr);                   // I2C address of LIS3DH = 0x18

  
  Wire.write(CTRL_REG1);
  Wire.write(0x57);                                //Set the mode to 100 Hz normal mode and enable the 3 axes 

  //Wire.write(CTRL_REG2);                         //Disable all high-pass filters
  //Wire.write(0x00);

  Wire.write(CTRL_REG3);                           //Generate interrupt signal on the Pin INT1 from the interrupt 1 register (0100 0000)
  Wire.write(0x40);
  

  //Wire.write(CTRL_REG4);                         //Take the default: continuous data update, big endian, +/- 2g, high resolution disabled, no self-test,no SPI
  //Wire.write(0x00);
  
  //Wire.write(CTRL_REG5);                        //Take the default: not rebooting memory, disable FIFO, no Latch Interrupt (INT1_SRC cleared after reading it), 4D detection disabled 
  //Wire.write(0x00);

  //Wire.write(CTRL_REG6);                         //Take the default: disable INT2 pin
  //Wire.write(0x00);
  
  Wire.write(INT1_CFG);                            // X = 0 AND Y = 0 AND Z = 0 (0001 0101)
  Wire.write(0x95);

  Wire.write(INT1_THS);                            // Threshold: 2 LSb = 32 mg
  Wire.write(0x02);

  Wire.write(INT1_DURATION);                       // Threshold Time: 0.32 s ODR = 100 Hz 0010000b = 0x10
  Wire.write(0x10);
  
  Wire.endTransmission();

  

  pinMode(A1,INPUT_PULLUP);
  
  Serial.println("Initialization Finished");
}

void loop() {
  // put your main code here, to run repeatedly:

  /* This part is used for reading data in the 3 axes.
     Here we choose the normal mode, and the data is 10-bit*/

  //x axis
  int16_t x,x_l;
  Wire.beginTransmission(slave_addr);
  Wire.write(OUT_X_H);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the Higher byte of X_OUT
  while(Wire.available() == 0);
  x = Wire.read();

  x = x<<8;
  
  Wire.beginTransmission(slave_addr);
  Wire.write(OUT_X_L);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the lower byte of X_OUT
  while(Wire.available() == 0);
  x_l = Wire.read();
  x = ((x | x_l)>>6)*4;

  //y axis
  int16_t y,y_l;
  Wire.beginTransmission(slave_addr);
  Wire.write(OUT_Y_H);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the Higher byte of Y_OUT
  while(Wire.available() == 0);
  y = Wire.read();

  y = y<<8;
  
  Wire.beginTransmission(slave_addr);
  Wire.write(OUT_Y_L);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the lower byte of Y_OUT
  while(Wire.available() == 0);
  y_l = Wire.read();
  y = ((y | y_l)>>6)*4;

  //z axis
  int16_t z,z_l;
  Wire.beginTransmission(slave_addr);
  Wire.write(OUT_Z_H);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the Higher byte of Z_OUT
  while(Wire.available() == 0);
  z = Wire.read();

  z = z<<8;
  
  Wire.beginTransmission(slave_addr);
  Wire.write(OUT_Z_L);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the lower byte of Z_OUT
  while(Wire.available() == 0);
  z_l = Wire.read();
  z = ((z | z_l)>>6)*4;

  /*This Part is used for debugging the alarming system in the project*/
  int8_t int_read;
  Wire.beginTransmission(slave_addr);
  Wire.write(INT1_SRC);                              
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(slave_addr,1);                     // Request the lower byte of Z_OUT
  while(Wire.available() == 0);
  int_read = Wire.read();
  // 01010101
  if(int_read == 85){
    Serial.print("Free Fall Detected!");
  }


    /*This Part is used for the actually functionality*/
  int fall_detection = digitalRead(A1);
  if(fall_detection){
    Serial.print("Emergency!");
  }
  

  
  delay(1000);
}
