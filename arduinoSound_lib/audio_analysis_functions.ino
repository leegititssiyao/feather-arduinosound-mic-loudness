#define SAMPLES 32

void fillSampleBuffer(int *samples)
{
  int i = 0;
  while (i < SAMPLES)
  {
    if (amplitudeAnalyzer.available())
    {
      samples[i] = amplitudeAnalyzer.read();
      i++;
    }
  }
}

void printBuffer(int *samples)
{
  for (int i = 0; i < SAMPLES; i++)
  {
    Serial.println(samples[i]);
  }
}

float getMeanValue(int *samples)
{
  float meanval = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    meanval += samples[i];
  }
  meanval /= SAMPLES;
  return meanval;
}

void normaliseBuffer(int *samples, float meanval)
{
  for (int i = 0; i < SAMPLES; i++)  // subtract it from all sapmles to get a 'normalized' output
  {
    samples[i] -= meanval;
  }
}

void printLoudness(int *samples)
{
  float minsample = 100000;
  float maxsample = -100000;

  for (int i = 0; i < SAMPLES; i++)
  {
    minsample = min(minsample, samples[i]);
    maxsample = max(maxsample, samples[i]);
  }
  Serial.println(maxsample - minsample);

}


void printAudio()
{
  long meanval = 0;
  int i = 0;
  while (i < SAMPLES)
  {
    if (amplitudeAnalyzer.available())
    {
      meanval += amplitudeAnalyzer.read();
      i++;      
    }
  }
  meanval /= SAMPLES;
  Serial.println(meanval);
}
