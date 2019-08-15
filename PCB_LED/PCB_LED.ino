#include <Arduino.h>
#include <Wire.h>

// arduuino pinout pin mapping
int D0= 11;
int D1= 13;
int D2= 9;
int D3= 10;
int D4=12;

//CALCUL DU TEMPS
/////////////////////////////////////////////////////////////////////
unsigned long t_0 = millis(); // gives time since startup in ms. Overflow after 50 days.
unsigned long loop_cnt = 0; //loop count
const int D = 100; //loop target duration in ms

//Wire and I²C functions
/////////////////////////////////////////////////////////////////////
byte data, data0, data1, data2; // used to buffer I²C data (in and/or out)
void set_I2C_register(byte ADDRESS, byte REGISTER, byte VALUE)
{
  Wire.beginTransmission(ADDRESS);
  delayMicroseconds(1);
  Wire.write(REGISTER);
  Wire.write(VALUE);
  Wire.endTransmission();
}

byte get_I2C_register(byte ADDRESS, byte REGISTER)
{
  Wire.beginTransmission(ADDRESS);
  delayMicroseconds(1);
  Wire.write(REGISTER);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, 1); //read 1 byte
  byte x = Wire.read();
  return x;
}

//## MCP4728 (Quad DAC 12bit - Vref=2.048V)
//Address
const byte MCP4728 = B1100000; //0x60

//mode
const byte MCP4728_MODE = B10000000;  // Vref interne - PD=PD0=G=0
const byte MCP4728_FAST = B00000000; //Write one DAC imputs registers from Channel A to Channel D, see fig 5-7 p38

//Commands (see from p31 of datasheet)
const byte MCP4728_RESET = 0x06;
const byte MCP4728_WAKEUP = 0x09;
const byte MCP4728_UPDATE = 0x08;
const byte MCP4728_ADDRESS = 0x0C; //can be changed in EEPROM!
const byte MCP4728_HARDW = B01010000; //Write all DAC imputs registers & EEPROM from Channel A to Channel D, takes ~25ms, see figure 5-9 p40

//DAC value entre 0 et 4095 (12bit) où 1LSB = 2.048/2**16=0.5mV       2048=1024mV    512= 256mV
//R sense = 22 Ohm, I_max=50mA => Ud_max=1000mV => DAC_max=2000
unsigned int DACS[] = {1000, 1000, 1000, 1000}; // {D1, D3, D3, D4} Udrive

 
void setup() 
{
//set up of the pwmfrequency
 pinMode (D2, OUTPUT);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0b00101100;
  TIMSK1 = 0b00000010;
  TCNT1 = 0;  
  OCR1A = 31250; 
  interrupts();

  
// setPwmFrequency(D2, 8);
  
 pinMode(D0, OUTPUT);
 pinMode(D1, OUTPUT);
 pinMode(D2, OUTPUT);
 pinMode(D3, OUTPUT);
 pinMode(D4, OUTPUT);

   //Set and scan I²C bus  // Copy/Pasted/Tweeked from https://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.setClock(400000); //400kHz I²C 
  Serial.println("\nI2C Scanning...");
  byte error, address;
  int nDevices =0;
  
  for(address = 1; address < 128; address++ )
  {
    delay(10);
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address<16){Serial.print("0");}
      Serial.print(address,HEX);
      Serial.println("  !");
      nDevices++;
    }
    else if (error==4){
      Serial.print("Unknown error at address 0x");
      if (address<16){Serial.print("0");}
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.print("done\n");
  delay(100);


  //Set MCP4728
  Serial.println(" ");
  Serial.print("MCP4728 is RESET, ");
  Wire.beginTransmission(MCP4728);
  Wire.write(MCP4728_RESET);
  Wire.endTransmission();
  delay(100);
  Serial.print("loaded with [");
  Wire.beginTransmission(MCP4728);
  Wire.write(MCP4728_HARDW);
  
  for(int i=0; i<4; i++)
  {
    Serial.print(DACS[i]/2/22); //0.5mV per value, 20 Ohm sensing
    Serial.print(" ");
    byte Byte1 = (DACS[i]>>8); //first bits (12-8=4bits)
    Byte1 = MCP4728_MODE|Byte1;
    byte Byte2 = DACS[i]&0xff; //last byte
    Wire.write(Byte1);
    Wire.write(Byte2);
    }
  Wire.endTransmission();
  delay(100);
  Serial.println("] mA and is ready to go");
}

void loop() 
{
  analogWrite(D0, 0);
  analogWrite(D1, 0);
  analogWrite(D2,128);
  analogWrite(D3, 0);
  analogWrite(D4, 0);
  delay(1000);
}
