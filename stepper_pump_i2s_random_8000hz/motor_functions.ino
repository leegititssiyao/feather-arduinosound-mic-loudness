
//================================================================================
/**
   Set Direction of motor based on current destination
*/
void setDirection()
{
  if (des <= 0)
  {
    dir = FORWARD;
  }
  else if (des >= stepperRange)
  {
    dir = BACKWARD;
  }
  else
  {
    switchDirection();
  }
}
//================================================================================
/*
   Check if the current position has taken the prviously set number of steps
   and if so, generate a new destination.
*/
void checkSteps()
{
  if (pos == steps)
  {
    //---------------------------------------------------------------------------
    myStepper->release();
    pos = 0;
    steps = random (1, (stepperRange - 1));
    //---------------------------------------------------------------------------
    setDirection();
    //---------------------------------------------------------------------------
    switch (dir)
    {
      case FORWARD:
        des = des + steps;
        break;
      case BACKWARD:
        des = des - steps;
        break;
    }
    //---------------------------------------------------------------------------
    if (des < 0)
    {
      steps = steps + des;
      des = 0;
    }
    else if (des > stepperRange)
    {
      steps = stepperRange - (des - steps);
      des = stepperRange;
    }
    //---------------------------------------------------------------------------
  }
}
//================================================================================

void switchDirection()
{
  dir = (dir == FORWARD ? BACKWARD : FORWARD);
}
