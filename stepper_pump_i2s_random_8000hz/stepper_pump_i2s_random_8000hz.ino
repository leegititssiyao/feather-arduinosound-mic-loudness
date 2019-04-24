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
const int ledPin = 13;
bool runFlag = 0;
int currentStateIndex = 0; // index current state
int statesAboveThreshold;
const int numberOfPreviousStates = 500; // number of previous states to record
int previousStateBuffer[numberOfPreviousStates]; // previous state buffer (or array)
int avgStateValue = 500;  // avearge state value
float rawVal = 0;  // current input value
unsigned long timeThresholdCrossed = 0;
unsigned long timeThreshold = 5000; // time required to be above threshold before activating
int threshold = 300;
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

  pinMode(ledPin, OUTPUT);

  while (!Serial) {}

  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32))   // start I2S at 8 kHz with 32-bits per sample
  {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }

  myPump->setSpeed(10);
  myPump->run(RELEASE);
  myStepper->setSpeed(10);
  myStepper->release();
  Serial.println("Start!");
}

//================================================================================

void loop()
{
  //---------------------------------------------------------------
  rawVal = getLoudness();
  previousStateBuffer[currentStateIndex] = rawVal;
  currentStateIndex++;
  //---------------------------------------------------------------
  // Calculate avergae state value
  if (currentStateIndex >= (numberOfPreviousStates - 1) )
  {
    for (int i = 0; i < (numberOfPreviousStates - 1); i++)
    {
      if (previousStateBuffer[i] >= threshold)
      {
        statesAboveThreshold += 1;
      }
      previousStateBuffer[i] = previousStateBuffer[i + 1];
    }

    if ((previousStateBuffer[numberOfPreviousStates - 4] > threshold) && (rawVal < threshold))
    {
      timeThresholdCrossed = millis();
    }

    if (rawVal < threshold) // timeThresholdCrossed>t1
    {
      if ((millis() - timeThresholdCrossed) < timeThreshold)
      {
        avgStateValue = ((statesAboveThreshold <= 20) ? rawVal : 500);
      }
      else
      {
        avgStateValue = rawVal;
      }
    }
    else // rawVal > threshold, t1>timeThresholdCrossed
    {
      avgStateValue = ((statesAboveThreshold <= 20) ? 0 : rawVal);
    }

    currentStateIndex = numberOfPreviousStates - 1;
    statesAboveThreshold = 0;
  }
  //---------------------------------------------------------------
  printValues();
  //---------------------------------------------------------------
  if (avgStateValue >= threshold)
  {
    digitalWrite(ledPin, LOW);
    if (runFlag == 1)
    {
      myPump->run(RELEASE);
      runFlag = 0;
    }
    myStepper->release();
  }
  else
  {
    digitalWrite(ledPin, HIGH);
    if (runFlag == 0)
    {
      myPump->setSpeed(200);
      myPump->run(FORWARD);
      myStepper->setSpeed(10);
      runFlag = 1;
    }
    checkSteps();
    myStepper->onestep(dir, MICROSTEP);
    pos++;
  }
}
