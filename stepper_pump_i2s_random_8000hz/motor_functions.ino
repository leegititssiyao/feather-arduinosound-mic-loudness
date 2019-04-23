
//================================================================================
/**
 * Set Direction of motor based on current destination
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
    dir = (dir == FORWARD ? BACKWARD : FORWARD);
  }
}
//================================================================================
/*
 * Check if the current position has taken the prviously set number of steps 
 * and if so, generate a new destination. 
 */
void checkSteps()
{
  if (pos == steps)
    {
      myStepper->release();
      pos = 0;
      steps = random (1, (stepperRange - 1));

      switch (dir)
      {
        case FORWARD:
          des = des + steps;
        case BACKWARD:
          des = des - steps;
      }

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
    }
}
//================================================================================
