//================================================================================
float getLoudness()
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

  for (int i = 0; i < SAMPLES; i++) // subtract it from all sapmles to get a 'normalized' output
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

//================================================================================

void calculateAvgStateValue()
{
  for (int i = 0; i < numberOfPreviousStates; ++i)
  {
    avgStateValue += previousStateBuffer[i];   
  }  
  avgStateValue /= float(numberOfPreviousStates);
}

void analyseAudioData()
{

  if ((previousStateBuffer[currentStateIndex - 4] > threshold) && (rawVal < threshold))
  {
    timeThresholdFalling = millis();
  }

  if ((previousStateBuffer[currentStateIndex - 4] < threshold) && (rawVal > threshold))
  {
    timeThresholdRising = millis();
  }
  else if ((rawVal > threshold))
  {
    // do nothing
  }
  else if ((rawVal < threshold))
  {
    timeThresholdRising = millis();
  }


  if (rawVal < threshold && (millis() - timeThresholdFalling) > timeRunThreshold) // timeThresholdRising>t1
  {
    runMotors = true;
  }
  else if (rawVal > threshold && (millis() - timeThresholdRising) > timeStopThreshold)
  {
    runMotors = false;
  }

  statesAboveThreshold = 0;
}
