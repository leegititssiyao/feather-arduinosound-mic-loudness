void getLoudness()
{
  // read a bunch of samples:
  int samples[SAMPLES];

  for (int i = 0; i < SAMPLES; i++)
  {
    int sample = 0;
    while ((sample == 0) || (sample == -1) )
    {
      sample = I2S.read();
    }
    sample >>= 14;
    samples[i] = sample;
  }

  //---------------------------------------------------------------

  float meanval = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    meanval += samples[i];
  }
  meanval /= SAMPLES;

  // subtract it from all sapmles to get a 'normalized' output
  for (int i = 0; i < SAMPLES; i++)
  {
    samples[i] -= meanval;
  }

  //---------------------------------------------------------------

  // find the 'peak to peak' max
  float maxsample, minsample;
  minsample = 100000;
  maxsample = -100000;
  for (int i = 0; i < SAMPLES; i++)
  {
    minsample = min(minsample, samples[i]);
    maxsample = max(maxsample, samples[i]);
  }
  return maxsample - minsample;
}

