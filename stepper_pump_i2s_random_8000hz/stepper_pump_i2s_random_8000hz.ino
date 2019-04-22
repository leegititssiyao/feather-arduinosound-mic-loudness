/*
   What is this?
*/
//================================================================================
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <I2S.h>
//================================================================================
Adafruit_MotorShield AFMStop(0x61);
Adafruit_MotorShield AFMSbot(0x60);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myStepper = AFMStop.getStepper(200, 2);
Adafruit_DCMotor *myPump = AFMSbot.getMotor(1);
//================================================================================
const int ledPin1 = 13;
bool runFlag = 0;
int count = 0;
int pre;
const int presNum = 500;
int pres[presNum];
int rawVal = 0;
int avgVal = 500;
unsigned long t0 = 0;
unsigned long t1 = 0;
int p200 = 0;
int threshold = 200;
int stepperRange = 700;
uint8_t dir = 1;
int des = 0;
int pos = 0;
int steps = 0;
const int SAMPLES = 128; // make it a power of two for best DMA performance
//================================================================================
void setup()
{
  Serial.begin(115200);
  AFMSbot.begin(10);
  AFMStop.begin();

  pinMode(ledPin1, OUTPUT);

  while (!Serial) {}

  // start I2S at 16 kHz with 32-bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32)) 
  {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }

  myPump->setSpeed(10);
  myPump->run(RELEASE);
  //  myPump->run(FORWARD);
  //  myPump->run(RELEASE);
  myStepper->setSpeed(10);
  myStepper->release();
  Serial.println("Start!");
}

//================================================================================

void loop()
{
  // read a bunch of samples:
  int samples[SAMPLES];

  for (int i = 0; i < SAMPLES; i++) {
    int sample = 0;
    while ((sample == 0) || (sample == -1) ) {
      sample = I2S.read();
      //      Serial.println(sample);
    }
    sample >>= 14;
    samples[i] = sample;
  }

  // ok we hvae the samples, get the mean (avg)
  float meanval = 0;
  for (int i = 0; i < SAMPLES; i++) {
    meanval += samples[i];
  }
  meanval /= SAMPLES;
  //Serial.print("# average: " ); Serial.println(meanval);

  // subtract it from all sapmles to get a 'normalized' output
  for (int i = 0; i < SAMPLES; i++) {
    samples[i] -= meanval;
  }

  // find the 'peak to peak' max
  float maxsample, minsample;
  minsample = 100000;
  maxsample = -100000;
  for (int i = 0; i < SAMPLES; i++) {
    minsample = min(minsample, samples[i]);
    maxsample = max(maxsample, samples[i]);
  }
  rawVal = maxsample - minsample;


  pres[count] = rawVal;
  count += 1;


  if (count >= (presNum - 1) ) {
    for (int i = 0; i < (presNum - 1); i++) {
      if (pres[i] >= threshold) {
        pre += 1;
        if (pres[i] = threshold) {
          p200 += 1;
        }
      }
      pres[i] = pres[i + 1];
    }

    if ((pres[presNum - 4] > threshold) && (rawVal < threshold)) {
      t0 = millis();
    }

    // t0>t1
    if (rawVal < threshold) {
      if ((millis() - t0) < 5000) {
        if (pre <= 20)  {
          avgVal = rawVal;
        }
        else {
          avgVal = 500;
        }
      }
      else {
        avgVal = rawVal;
      }
    }

    // rawVal > threshold, t1>t0
    else {
      //      avgVal = rawVal;
      if (pre <= 20) {
        avgVal = 0;
      }
      else {
        avgVal = rawVal;
      }
    }

    count = presNum - 1;
    pre = 0;
    p200 = 0;
  }



  Serial.print(rawVal);
  Serial.print(" ");
  Serial.print(avgVal);
  Serial.print(" ");
  Serial.println(threshold);


  if (avgVal >= threshold) {
    digitalWrite(ledPin1, LOW);
    if (runFlag == 1) {
      myPump->run(RELEASE);
      runFlag = 0;
    }
    myStepper->release();

  }
  else {
    digitalWrite(ledPin1, HIGH);
    if (runFlag == 0) {
      myPump->setSpeed(200);
      myPump->run(FORWARD);
      myStepper->setSpeed(10);
      runFlag = 1;
    }
    if (pos == steps) {
      myStepper->release();
      pos = 0;
      steps = random (1, (stepperRange - 1));
      if (des == 0) {
        dir = 1;
      }
      else if (des == stepperRange) {
        dir = 2;
      }
      else {
        dir = (dir == 1 ? 2 : 1);
      }
      if (dir == 1) {
        des = des + steps;
      }
      if (dir == 2) {
        des = des - steps;
      }
      if (des < 0) {
        steps = steps + des;
        des = 0;
      }
      if (des > stepperRange) {
        steps = stepperRange - (des - steps);
        des = stepperRange;
      }
    }
    myStepper->onestep(dir, MICROSTEP);
    pos++;
    //    Serial.println(avgVal);
  }
}
