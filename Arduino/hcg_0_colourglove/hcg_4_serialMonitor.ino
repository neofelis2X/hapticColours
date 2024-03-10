/*



 *******************************************************************************
*/
/*
MENU(mainMenu, "Haptic Driver Settings", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
     , FIELD(wfLib, "Waveform Library", "", 1, 7, 1, 0, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(wfRepeat, "Repetitions of waveforms", "times", 1, 6, 1, 0, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(wfPause, "Pause between repetitions", "ms", 10, 900, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , OP("Run Diagnosis", runDiagnosis, enterEvent)
     , OP("Run Calibration", runCalibration, enterEvent)
     , EXIT("<Back")
    );

serialIn serial(Serial);

MENU_INPUTS(in, &serial);

MENU_OUTPUTS(out, MAX_DEPTH
             , SERIAL_OUT(Serial)
             , NONE //must have 2 items at least
            );

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);
*/
void settingsMenue() {

  Serial.println();
  Serial.println(F("1 - Repeats of Waveform"));
  Serial.println(F("2 - Delay between Waveform in ms"));
  Serial.println(F("3 - Waveform Library Number"));
  Serial.println(F("4 - Run LRA Calibration"));
  Serial.println(F("5 - Run Diagnosis"));
  Serial.println(F("6 - Exit"));
  Serial.println();  //Prompt User for Input

  menueActive = 1;
  int diag = 255;

  for (;;) {
    switch (readSerialInt()) {
      case 1:
        Serial.println(F("Please enter desired number of repeats. Not more than 4."));  //Prompt User for Input
        Serial.println();
        wfRepeat = readSerialInt();

        if (wfRepeat > 4) {
          Serial.println(F("You cannot enter more than 4 repeats."));
          wfRepeat = 4;
        }
        Serial.print(F("Repeats set to: "));
        Serial.println(wfRepeat);
        return;

      case 2:
        Serial.println(F("Please enter desired pause between effects smaller than 1280 [ms] (1.2s)."));  //Prompt User for Input
        Serial.println();

        wfPause = readSerialInt() / 10;

        if (wfPause == 0) {
          debugln(F("Couldn't read a valid input. Enter again."));
          continue;
        } else if (wfPause > 128) {
          debug(F("Pause cannot be more than 1.2s."));
          wfPause = 128;
        }

        debug(F("Pause set to: "));
        debug(wfPause);
        debugln(F(" *10 ms"));
        return;

      case 3:
        Serial.println(F("Please enter the waveform library to use. (1 - 7), 6 for LRA."));  //Prompt User for Input
        Serial.println();

        wfLib = readSerialInt();

        if (wfLib == 0) {
          debugln(F("Couldn't read a valid input. Enter again."));
          continue;
        } else if (wfLib > 7) {
          debugln(F("Has to be between 1 and 7"));
          wfLib = 6;
        }

        debug(F("Library set to: "));
        debug(wfLib);
        return;

      case 4:
        Serial.println(F("Running LRA Calibration..."));
        //        drv.setScript(8);
        //        drv.playScript(8);        // LRA Calibration
        diag = drv.readRegister8(DRV2605_REG_STATUS);
        Serial.print(F("Diagnosis says: "));
        if (bitRead(diag, 3) == 0) {
          Serial.println(F("Calibration PASSED."));
        } else {
          Serial.println(F("Calibration FAILED."));
        }
        diag = 255;
        drv.useLRA();    // pick an actuator type
        drv.setMode(0);  // haptic effects triggered by I2C register write
        return;

      case 5:
        Serial.println(F("Running DRV2605L Testing..."));
        drv.setMode(6);
        drv.go();
        diag = drv.readRegister8(DRV2605_REG_STATUS);
        Serial.print(F("Diagnosis says: "));
        if (bitRead(diag, 3) == 0) {
          Serial.println(F("Test PASSED, Actuator is functioning normally."));
        } else {
          Serial.println(F("Test FAILED, Actuator is not present or is shorted, timing out, or giving out–of-range back-EMF."));
        }
        diag = 255;
        drv.useLRA();    // pick an actuator type
        drv.setMode(0);  // haptic effects triggered by I2C register write
        return;

      case 6:
        menueActive = 0;
        return;

      default: continue;
    }
  }
}

bool menueCheck() {

  if (menueActive == 1) {
    waveform = 0;
    settingsMenue();
    return 1;
  } else {
    return 0;
  }
}

int readSerialInt() {

  static char input_line[maxInput];
  int inputInt;
  static unsigned int input_pos = 0;
  byte eoL = 0;

  debugln(F("db readSerialInt start"));
  debugln(F("db waiting for input..."));

  int timeout = millis();
  byte inByte;
  while (millis() - timeout < 50000) {

    while (Serial.available() > 0) {
      inByte = Serial.read();
      switch (inByte) {

        case '\n':  //end of line is registered
          input_line[input_pos++] = inByte;
          eoL = 1;
          break;

        case '\r':  //carriage return gets ignored
          debugln(F("db CR"));
          break;

        default:  //any other input is registered until the eoL or max amount is reached
          if (input_pos < (maxInput - 1)) {
            input_line[input_pos++] = inByte;
          }
          break;
      }
    }

    if (eoL == 1) {  //when End of Line was registered, wrap it up and send it home
      input_line[input_pos] = 0;
      input_pos = 0;  //reset buffer
      eoL = 0;        // reset End of Line

      debug(F("db readSerialInt parsed input - input is: "));
      debugln(input_line);
      inputInt = atoi(input_line);
      debug(F("db readSerialInt parsed Integer - input is: "));
      debugln(inputInt);
      return inputInt;
    } /*else {
      debugln(F("db Didn't reach End of Line. Weird. Please input again."));
      return 0;
    }*/
  }
  debugln(F("db reached timeout"));
  return 0;
}

bool runDiagnosis() {

  int diag = 255;
  Serial.println(F("Running DRV2605L Testing..."));
  drv.setMode(6);                                // go into diagnosis mode
  drv.go();                                      // run the diagnosis
  diag = drv.readRegister8(DRV2605_REG_STATUS);  // read the register that holds the diagnosis bit
  Serial.print(F("Diagnosis says: "));
  if (bitRead(diag, 3) == 0) {
    Serial.println(F("Test PASSED, Actuator is functioning normally."));
    drv.useERM();    // pick an actuator type
    drv.setMode(0);  // mode is set back to internal trigger
    return 1;
  } else {
    Serial.println(F("Test FAILED, Actuator is not present or is shorted, timing out, or giving out–of-range back-EMF."));
    drv.useERM();    // pick an actuator type
    drv.setMode(0);  // mode is set back to internal trigger
    return 0;
  }
}

bool runCalibration() {

  int diag = 255;
  Serial.println(F("Running LRA Calibration..."));
  //        drv.setScript(8);
  //        drv.playScript(8);        // LRA Calibration
  diag = drv.readRegister8(DRV2605_REG_STATUS);
  Serial.print(F("Diagnosis says: "));
  if (bitRead(diag, 3) == 0) {
    Serial.println(F("Calibration PASSED."));
    drv.useERM();    // pick an actuator type
    drv.setMode(0);  // mode is set back to internal trigger
    return 1;
  } else {
    Serial.println(F("Calibration FAILED."));
    drv.useERM();    // pick an actuator type
    drv.setMode(0);  // mode is set back to internal trigger
    return 0;
  }
}
