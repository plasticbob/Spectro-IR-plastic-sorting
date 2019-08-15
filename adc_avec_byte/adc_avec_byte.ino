

// include the library code:
#include <SPI.h>
#include <digitalWriteFast.h>
// Set Constants
const int adcChipSelectPin = SS;      // set pin 8 as the chip select for the ADC:
int i=0;


// Start setup function:
void setup() 
{  
  pinMode(adcChipSelectPin, OUTPUT); 
  // set the ChipSelectPins high initially: 
  digitalWrite(adcChipSelectPin, HIGH);  
  // initialise SPI:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);         // Not strictly needed but just to be sure.
  SPI.setDataMode(SPI_MODE0);        // Not strictly needed but just to be sure.
  // Set SPI clock divider to 16, therfore a 1 MhZ signal due to the maximum frequency of the ADC.
  SPI.setClockDivider(SPI_CLOCK_DIV2); // on bosse à 8 Mhz
  
  Serial.begin(9600);
} // End setup function.


void loop()
{ 
    float voltage0 = readAdc(0);

    readAdc(0);
 
  //delay(5000);
  //float voltage1 = readAdc(1);
  //float voltage2 = readAdc(2);
  
  Serial.println(readAdc(0));
 // delay(10000);
}



//Function to read the ADC, accepts the channel to be read.
float readAdc(int channel)
{
  unsigned long CurrentTime;
  unsigned long StartTime= 0.000;
  unsigned long ElapsedTime;
  unsigned long TotalTime;

  int N=32;
int Buff[N];
unsigned long timing[N];
unsigned long timingTotal[N];
int i, j;
int digitalValue;

  byte adcPrimaryRegister = 0b00000110;      // Sets default Primary ADC Address register B00000110, This is a default address setting, the third LSB is set to one to start the ADC, the second LSB is to set the ADC to single ended mode, the LSB is for D2 address bit, for this ADC its a "Don't Care" bit.
  byte adcPrimaryRegisterMask = 0b00000111;  // b00000111 Isolates the three LSB.  
  byte adcPrimaryByteMask = 0b00001111;      // b00001111 isolates the 4 LSB for the value returned. 
  byte adcPrimaryConfig = adcPrimaryRegister & adcPrimaryRegisterMask; // ensures the adc register is limited to the mask and assembles the configuration byte to send to ADC.
  byte adcSecondaryConfig = channel << 6;
  
  byte Bytes1[32];
  byte Bytes2[32];


//fill data buffers
StartTime = micros(); // temps initial

for (i=0; i<N; i++)
   {
    // while( (micros()-StartTime)< (i+1)*20 ){1;}

    noInterrupts(); // disable interupts to prepare to send address data to the ADC.  
    
    digitalWriteFast(adcChipSelectPin,LOW); // take the Chip Select pin low to select the ADC.
    SPI.transfer(adcPrimaryConfig); //  send in the primary configuration address byte to the ADC.  
    Bytes1[i] = SPI.transfer(adcSecondaryConfig); // read the primary byte, also sending in the secondary address byte.  
    Bytes2[i] = SPI.transfer(0x00); // read the secondary byte, also sending 0 as this doesn't matter. 
    digitalWriteFast(adcChipSelectPin,HIGH); // take the Chip Select pin low to select the ADC.
    
   interrupts(); // Enable interupts
   
   timing[i]=micros(); 
   }

 
  //Serial.println("Les 32 premiers samples :");
  Serial.println("N \tt[µs] \tdt[µs] \tValue B");
  for (i=0; i<N; i++)
  {
    Serial.print(i);
    Serial.print("\t");
    
    Serial.print(timing[i]);
    Serial.print("\t");
    
    if(i==0)
    {
      Serial.print("-");
      }
    else
    {
      Serial.print(timing[i]-timing[i-1]);
      }
    Serial.print("\t");
  
     Bytes1[i] &= adcPrimaryByteMask; // Limits the value of the primary byte to the 4 LSB:
      digitalValue = (Bytes1[i] << 8) | Bytes2[i]; // Shifts the 4 LSB of the primary byte to become the 4 MSB of the 12 bits digital value, this is then ORed to the secondary byte value that holds the 8 LSB of the digital value.
   
    Serial.println(digitalValue);
  }
}
