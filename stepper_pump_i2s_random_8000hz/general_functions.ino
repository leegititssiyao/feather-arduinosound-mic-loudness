//================================================================================
void printValues()
{
//  Serial.print(rawVal);
  Serial.print((rawVal < 800) ? rawVal : 800);
  Serial.print(" ");
  Serial.print(avgStateValue);
  Serial.print(" ");
  Serial.println(threshold);
}
