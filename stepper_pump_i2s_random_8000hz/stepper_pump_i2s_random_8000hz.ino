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
Adafruit_StepperMotor *myStepper = AFMStop.getStepper(200, 1);
Adafruit_DCMotor *myPump = AFMSbot.getMotor(1);
//================================================================================
const int ledPin = 13;
const int resetPin = 6;
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
unsigned long timeRunThreshold = 3000; // time required to be above threshold before activating, 7000
unsigned long timeStopThreshold = 500; // time required to be above threshold before activating
int threshold = 8100; //8100
int timeStopPumpThreshold = 1000; //2300, 200-250
int timeStartPumpThreshold = 3000; //3000
int flagPump = 0;
unsigned long timeStateOfPumpChanges = 0;
bool runMotors = false;
bool debug = true;
//================================================================================
// Stepper Variables
int stepperRange = 270;
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
  pinMode(resetPin, INPUT);

  //  while (!Serial) {}

  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32))   // start I2S at 8 kHz with 32-bits per sample
  {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }

//  Serial.println("GOT HHERE 2");
  myPump->setSpeed(255); //255
  //  myPump->run(RELEASE);
  myStepper->setSpeed(10);

  //Take arm to the starting point
  int reading = digitalRead(resetPin);
  int start = millis();
  while ((millis() - start) < 5000) {
    while (reading == 0) {
      reading = digitalRead(resetPin);
      Serial.println(reading);
      myStepper->onestep(BACKWARD, MICROSTEP);
      myPump->run(FORWARD);
    }
    myStepper->onestep(FORWARD, MICROSTEP);
    myStepper->release();
  }
  myPump->run(RELEASE);


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
      myPump->setSpeed(140); //145
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


    if (runFlag == 0)
    {

      //      myPump->setSpeed(148);
      //      myPump->run(FORWARD);
      runFlag = 1;
    }
    myStepper->setSpeed(10);
    checkSteps();
    myStepper->onestep(dir, MICROSTEP);
    pos++;

  }
  //---------------------------------------------------------------
}
