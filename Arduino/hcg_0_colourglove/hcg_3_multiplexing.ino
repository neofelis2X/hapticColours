/*
This section contains all functions needed to use the TCA9548A I2C Multiplexer.
It is needed when I2C devices share the same fixed adress.

 *******************************************************************************
*/

void tcaSelect(uint8_t i) {

  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
  
  debug(F("Multiplexer Port has been changed to "));
  debugln(i);
}
