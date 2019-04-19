//======================================================================
#include <ArduinoSound.h>
AmplitudeAnalyzer amplitudeAnalyzer; // create an amplitude analyzer to be used with the I2S input
//======================================================================
#define SAMPLES 128
int movingAvg[SAMPLES] = {0};
//======================================================================
void setup()
{
  Serial.begin(115200);
  while (!Serial) {}

  if (!AudioInI2S.begin(44100, 32))  // setup the I2S audio input for 44.1 kHz with 32-bits per sample
  {
    Serial.println("Failed to initialize I2S input!");
    while (1); // do nothing
  }

  if (!amplitudeAnalyzer.input(AudioInI2S))  // configure the I2S input as the input for the amplitude analyzer
  {
    Serial.println("Failed to set amplitude analyzer input!");
    while (1); // do nothing
  }
}

//======================================================================

void loop()
{
  //--------------------------------------------------------------------
//  int samples[SAMPLES]= {0}; // audio buffer
  printAudio();
//  fillSampleBuffer(samples);
//  printBuffer(samples);
//  float meanval = getMeanValue(samples);
//  normaliseBuffer(samples, meanval);
  //--------------------------------------------------------------------
//  printLoudness(samples);
//  Serial.println(1);
}
