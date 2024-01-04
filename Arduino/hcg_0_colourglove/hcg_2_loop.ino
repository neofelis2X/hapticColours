/*



 *******************************************************************************
*/

void loop() {

/*
  tcaSelect(tcaPinA);
  drv.setWaveform(0, 1);
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
  delay(100);

  tcaSelect(tcaPinB);
  drv.setWaveform(0, 1);
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
  delay(1000);

  tcaSelect(tcaPinA);
  drv.setWaveform(0, 10);
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
  delay(100);

  tcaSelect(tcaPinB);
  drv.setWaveform(0, 10);
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
  delay(1000);
*/
  if (button1State) {
    digitalWrite(tcsLedPin, HIGH); //switch the LED of the colour sensor
    //    colourInterrupt();
    delay(60);
    tcs40.tcs.getRawData(&tcs40.r, &tcs40.g, &tcs40.b, &tcs40.c);
    // not close enough to colorful item
    if (tcs40.c < CLEARTHRESHHOLD) {
      digitalWrite(tcsLedPin, LOW); //switch the LED of the colour sensor
      button1State = LOW;
      return;
    }
    tcs40.calculateDN40();
    float colourHsv[3];
    float channelSum = tcs40.r_comp + tcs40.g_comp + tcs40.b_comp;
    rgb2hsv(mapUintTo1(tcs40.r_comp, channelSum), mapUintTo1(tcs40.g_comp, channelSum), mapUintTo1(tcs40.b_comp, channelSum), colourHsv);
    noteColour(colourHsv);
    //    colourProtocol();
    digitalWrite(tcsLedPin, LOW); //switch the LED of the colour sensor

    colourVibration();
    button1State = LOW;
  }

  //  menueCheck();
  //  inputPattern();
  //  settingsMenue();
  //  readSerialInt();
  //  tcaselect(2);
  //  hapticCalibration();

}
