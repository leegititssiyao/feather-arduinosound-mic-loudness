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
unsigned int currentStateIndex = 0; // index current state
int statesAboveThreshold;
const int numberOfPreviousStates = 20; // number of previous states to record
int previousStateBuffer[numberOfPreviousStates] = {0}; // previous state buffer (or array)
float avgStateValue = 0;  // avearge state value
float previousAvgStateValue = 0;  // avearge state value
float rawVal = 0;  // current input value
unsigned long timeThresholdRising = 0;
unsigned long timeThresholdFalling = 0;
unsigned long timeRunThreshold = 5000; // time required to be above threshold before activating
unsigned long timeStopThreshold = 500; // time required to be above threshold before activating
int threshold = 800;
int timeStopPumpThreshold = 500;
int timeStartPumpThreshold = 1500;
int flagPump = 0;
unsigned long timeStateOfPumpChanges = 0;
bool runMotors = false;
bool debug = true;
//================================================================================
// Stepper Variables
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

  //  while (!Serial) {}

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
  previousAvgStateValue = avgStateValue;
  avgStateValue = 0;
  rawVal = getLoudness();
  previousStateBuffer[currentStateIndex] = rawVal;
  currentStateIndex++;
  currentStateIndex %= numberOfPreviousStates;
  //---------------------------------------------------------------
  calculateAvgStateValue();
  analyseAudioData();
  //---------------------------------------------------------------
  printValues();
  //---------------------------------------------------------------
  if (!runMotors)
  {
    digitalWrite(ledPin, LOW);
    if (runFlag == 1)
    {
      myPump->run(RELEASE);
      runFlag = 0;
      flagPump = 0;
    }
    myStepper->release();
  }
  else
  {
    digitalWrite(ledPin, HIGH);
    if (flagPump == 0) {
      myPump->setSpeed(200);
      myPump->run(FORWARD);
      timeStateOfPumpChanges = millis();
      flagPump = 1;
    }
    if ((millis() - timeStateOfPumpChanges > timeStopPumpThreshold) && (flagPump == 1)) {
      myPump->run(RELEASE);
      flagPump = 2;
    }
    if (millis() - timeStateOfPumpChanges > timeStartPumpThreshold) {
      flagPump = 0;
    }
    myStepper->setSpeed(10);
    if (runFlag == 0)
    {
      runFlag = 1;
    }

    checkSteps();
    myStepper->onestep(dir, MICROSTEP);
    pos++;

  }
  //---------------------------------------------------------------
}
