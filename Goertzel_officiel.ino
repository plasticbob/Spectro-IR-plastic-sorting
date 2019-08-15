
#include <stdio.h>
#include <math.h>
#include <Arduino.h>

#define FLOATING  float
#define SAMPLE  unsigned char
//#define PI 3.14

#define SAMPLING_RATE 100000.0  //100kHz
#define TARGET_FREQUENCY  10000.0 //10 kHz
#define N 300//Block size
#define THRESHOLD 3000
//#define pinMode 13

FLOATING coeff;
FLOATING Q1;
FLOATING Q2;
FLOATING sine;
FLOATING cosine;
int Sensorpin = A0;

//_SFR_BYTE(ADCSRA) |=  _BV(ADPS2); // Set ADPS2
//_SFR_BYTE(ADCSRA) &= ~_BV(ADPS1); // Clear ADPS1
//_SFR_BYTE(ADCSRA) &= ~_BV(ADPS0); // Clear ADPS0

SAMPLE testData[N];

void setup()
{
  //pinMode(13, OUTPUT);
  Serial.begin(9600);
}

/* Call this routine before every "block" (size=N) of samples. */
void ResetGoertzel(void)
{
  Q2 = 0;
  Q1 = 0;
}

/* Call this once, to precompute the constants. */
void InitGoertzel(void)
{
  int k;
  FLOATING  floatN;
  FLOATING  omega;

  floatN = (FLOATING) N;
  k = (int) (0.5 + ((floatN * TARGET_FREQUENCY) / SAMPLING_RATE));
  omega = (2.0 * PI * k) / floatN;
  sine = sin(omega);
  cosine = cos(omega);
  coeff = 2.0 * cosine;

  Serial.println("La frequence d'echantillonnage est =");
  Serial.println(SAMPLING_RATE);
  Serial.println(" le block size est =");
  Serial.println(N);
  Serial.println(" la Target FREQUENCY est=");
  Serial.println(TARGET_FREQUENCY);
  Serial.println("La constante k est=");
  Serial.println(k);
  Serial.println("Le coeff est =");
  Serial.println(coeff);

  ResetGoertzel();
}

/* Call this routine for every sample. */
void ProcessSample(SAMPLE sample)
{
  FLOATING Q0;
  Q0 = coeff * Q1 - Q2 + (FLOATING) sample;
  Q2 = Q1;
  Q1 = Q0;
}

/* Basic Goertzel */
/* Call this routine after every block to get the complex result. */
void GetRealImag(FLOATING *realPart, FLOATING *imagPart)
{
  *realPart = (Q1 - Q2 * cosine);
  *imagPart = (Q2 * sine);
}

/*Algorithme de Goertzel Optimisé */
/* Call this after every block to get the RELATIVE magnitude squared. */
FLOATING GetMagnitudeSquared(void)
{
  FLOATING result;

  result = Q1 * Q1 + Q2 * Q2 - Q1 * Q2 * coeff;
  return result;
}

/*** End of Goertzel-specific code, the remainder is test code. */

/* Synthesize some test data at a given frequency. */
void Generate(FLOATING frequency)
{
  int index;
  FLOATING  step;

  //step = frequency * ((2.0 * PI) / SAMPLING_RATE);

  /* Generate the test data */
  for (index = 0; index < N; index++)
  {
    //testData[index] = (SAMPLE) (100.0 * sin(index * step) + 100.0);
    testData[index] = analogRead(Sensorpin);
  }
}

// Demo 1 
void GenerateAndTest(FLOATING frequency)
{
  int index;

  FLOATING  magnitudeSquared;
  FLOATING  magnitude;
  FLOATING  real;
  FLOATING  imag;

  Serial.println("Pour la frequence de test");
  Serial.println(frequency);
  Generate(frequency);

 //calcul du temps pour faire le process samples
  int temps_basic=millis();
 int temps_optimise= millis();
 
  // Process the samples 
  for (index = 0; index < N; index++)
  {
    ProcessSample(testData[index]);
  }

  // Do the "basic Goertzel" processing. 
  Serial.println("Resultat pour l'algo de Goertzel CLASSIQUE");
  
  GetRealImag(&real, &imag);

  Serial.println("La partie réel est=");
  Serial.println(real);
  Serial.println("la partie imaginaire est");
  Serial.println(imag);

  magnitudeSquared = real*real + imag*imag;
  
  Serial.println("Relative magnitude squared =");
  Serial.println(magnitudeSquared);
  
  magnitude = sqrt(magnitudeSquared);
  Serial.println("Relative magnitude =");
  Serial.println(magnitude);

  temps_basic=millis()-temps_basic;
  Serial.println("le temps pour faire le process de l'algo de Goertzel optimisé =");
  Serial.println(temps_basic);

  // Do the "optimized Goertzel" processing 
  Serial.println("Résultat pour l'algo de Goertzel OPTIMISE");
  magnitudeSquared = GetMagnitudeSquared();
  //Serial.println("Relative magnitude squared =");
  //Serial.println(magnitudeSquared);
  magnitude = sqrt(magnitudeSquared);
  Serial.println("Relative magnitude =");
  Serial.println(magnitude);

 temps_optimise=millis()-temps_optimise;
  Serial.println("le temps pour faire le process de l'algo de Goertzel optimisé =");
  Serial.println(temps_optimise);
 /*
  if (magnitude>THRESHOLD)
  {
    Serial.println("COOOOOOL, j'ai obtenu l'amplitude recherchée");
  }
  else 
  {
    Serial.println("NOPE NOT GOOD ");
  } 
  */
  
  ResetGoertzel();
}

/* Demo 2 
void GenerateAndTest2(FLOATING frequency)
{
  int index;

  FLOATING  magnitudeSquared;
  FLOATING  magnitude;
  FLOATING  real;
  FLOATING  imag;

  Serial.println("La Frequence est=");
  Serial.println(frequency);
  Generate(frequency);

   Process the samples
  for (index = 0; index < N; index++)
  {
    ProcessSample(testData[index]);
  }

   Do the "standard Goertzel" processing. 
  GetRealImag(&real, &imag);

  magnitudeSquared = real*real + imag*imag;
  Serial.println("rel mag^2=");
  Serial.println(magnitudeSquared);
  magnitude = sqrt(magnitudeSquared);
  printf("rel mag=");
  Serial.println(magnitude);
  
  ResetGoertzel();
}
*/

void loop()
{
  int index;
  //float freq;
   Serial.println("test de l'algorithme de Goertzel");
//while (1)

   InitGoertzel();

    //Demo 1 
  // GenerateAndTest(TARGET_FREQUENCY - 250);
   GenerateAndTest(TARGET_FREQUENCY);
  //  GenerateAndTest(TARGET_FREQUENCY + 250);
  
}
