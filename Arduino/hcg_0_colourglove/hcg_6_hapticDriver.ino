/*

   Funktionsverzeichnis der Adafruit Haptic Library
  Adafruit_DRV2605(void);
  boolean begin(TwoWire *theWire = &Wire);
  boolean init();
  void writeRegister8(uint8_t reg, uint8_t val);
  uint8_t readRegister8(uint8_t reg);
  void setWaveform(uint8_t slot, uint8_t w);
  void selectLibrary(uint8_t lib);
  void go(void);
  void stop(void);
  void setMode(uint8_t mode);
  void setRealtimeValue(uint8_t rtp);
  void useERM();
  void useLRA();

 *******************************************************************************
*/

/*
  void inputPattern() {

  Serial.println();
  Serial.println(F("Which Vibration Effect do you want to play?")); //Prompt User for Input
  Serial.println(F("| Type 200 for Settings |"));
  Serial.println();

  waveform = 0; // reset the waveform
  debugln(F("db Start reading Serial Input..."));
  waveform = readSerialInt();
  debug(F("db reading waveform was: "));
  debugln(waveform);

  if (waveform == 200) {   // start the settings dialog if needed
    waveform = 0; //reset the waveform after entering menu
    settingsMenue();

  } else if (waveform != 0 && waveform < 123) {

    drv.selectLibrary(wfLib);
    Serial.println();
    Serial.print(F("Fire Effect: "));
    Serial.print(waveform);
    Serial.print(F(", Repeats: "));
    Serial.print(wfRepeat);
    Serial.print(F(", with a pause of "));
    Serial.print(wfPause * 10);
    Serial.println(F(" ms inbetween."));
    Serial.println();

    for (int i = 0; i < 2 * wfRepeat; i = i + 2) {
      debug(F("Set Waveform "));
      debug(waveform);
      debug(F(" in Slot "));
      debug(i);
      drv.setWaveform(i, waveform);

      debug(F(" and programmed delay of "));
      debug(wfPause * 10);
      debug(F(" ms in Slot "));
      debugln(i + 1);
      //      drv.setWaveformDelay(i + 1, wfPause);
    }

    drv.go();
    for (int j = 0; j < 8; j++) {
      drv.setWaveform(j, 0x00);
    }
    waveform = 0;

  } else {
    debugln(F("Invalid Input"));
  }
  }
*/
void playVibration(byte effect, byte repetitions) {

  tcaSelect(tcaPinA);
  for (int j = 0; j < repetitions; j++) {
    drv.setWaveform(j, effect);
  }
  drv.setWaveform(repetitions + 1, 0);     // end waveform
  drv.go();

  delay(200);
  tcaSelect(tcaPinB);
  for (int j = 0; j < repetitions; j++) {
    drv.setWaveform(j, effect);
  }
  drv.setWaveform(repetitions + 1, 0);     // end waveform
  drv.go();
}

void colourVibration() {
  switch (colourCase) {
    case Red:
      playVibration(1, 1);
      break;

    case Yellow:
      playVibration(10, 1);
      break;

    case Green:
      playVibration(70, 1);
      break;

    case Cyan:
      playVibration(27, 1);
      break;

    case Blue:
      playVibration(83, 1);
      break;

    case Magenta:
      playVibration(64, 1);
      break;

    case White:
      playVibration(1, 1);
      break;

    case Grey:
      playVibration(1, 1);
      break;

    case Black:
      playVibration(1, 1);
      break;

    default:
      playVibration(12, 1);
      break;
  }
}
