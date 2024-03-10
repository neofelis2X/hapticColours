/*



 *******************************************************************************
*/
//___
// first we start the Serial Monitor at 115200 baud
void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed to see startup messages...
  }
  debugln(F("\tdb Serial Monitor is up and running!"));

  //___
  // DRV2605L Haptic Driver initialisation with the Adafruit library

  drvSetup(tcaPinA);
  //  delay(50);
  drvSetup(tcaPinB);

  //___
  // This is a button that can be used for any input
  pinMode(button1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1Pin), button1Interrupt, FALLING);
  pinMode(led1Pin, OUTPUT);
  digitalWrite(led1Pin, LOW);

  //___
  // TCS34725 Colour Sensor initialisation with the Adafruit library
  pinMode(tcsLedPin, OUTPUT);              // this pin goes to the LED of the colour sensor and switches it off or on
  digitalWrite(tcsLedPin, LOW);            // @gremlins Bright light, bright light! LED switched OFF
  pinMode(tcsInterruptPin, INPUT_PULLUP);  //TCS interrupt output is Active-LOW and Open-Drain
  attachInterrupt(digitalPinToInterrupt(tcsInterruptPin), tcsInterrupt, FALLING);

  if (!tcs40.tcs.begin()) {
    Serial.println(F("Error: TCS34725 Device not found - check your I2C connections?"));
    //    while (1); // halt!
  } else {
    Serial.println(F("Colour Sensor TCS34725 @ Address 0x29 was found!"));
    debugln(F("\tdb TCS34725 Gain and Integration time is set by hand!"));
    tcs40.tcs.setGain(TCS34725_GAIN_4X);  //gain values are 1x, 4x, 16x, 60x
    tcs40.tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_300MS);
    //Integration times: 2_4, 24, 50, 60, 101, 120, 154, 180, 199, 240, 300, 360, 401, 420, 480, 499, 540, 600, 614

    //  tcs40.tcs.write8(TCS34725_PERS, TCS34725_PERS_NONE); ??
    //  tcs40.tcs.setInterrupt(true);
    //  debugln(F("\tdb TCS34725 autoRange() starts."));
    //  tcs40.autoRange();

    debugln(F("\tdb TCS34725 is all set up"));
  }
  Serial.flush();
}

// This function is for the interrupt and changes the buttonstate on the event of a button click
void button1Interrupt() {
  button1State = HIGH;  // one push of the button is registered
  //  button1State = !button1State; //invert the state of the button
  //  digitalWrite(tcsLedPin, button1State); //switch the LED of the colour sensor
}

// This function uses the interrupt output of the tcs colour sensor
void tcsInterrupt() {
  tcsReady = true;
  digitalWrite(led1Pin, HIGH);
}

void drvSetup(byte activeTcaPin) {

  tcaSelect(activeTcaPin);
  debug(F("Active Multiplexer Port is "));
  debugln(activeTcaPin);
  if (!drv.begin()) {
    Serial.println(F("Error: DRV2605L Device A not found - check your I2C connections?"));

  } else {
    // okay correct chip, let's initialize it
    Serial.println(F("Haptic Driver DRV2605L A @ Address 0x5A was found!"));
    drv.init();
    drv.selectLibrary(2);
    drv.setMode(DRV2605_MODE_INTTRIG);  // default, internal trigger when sending GO command
    drv.useERM();
    //    drv.setWaveform(0, 1);
    //    drv.setWaveform(1, 0);       // end waveform
    //    drv.go();
    debugln(F("\tdb DRV2605 setup finished"));
  }
}
