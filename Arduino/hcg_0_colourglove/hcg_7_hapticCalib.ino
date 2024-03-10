/*
    Arduino IDE LRA example for TI DRV2605L with ELV1411A linear resonant actuator

    DRV2605 :  Immersion Libs Patented Waveform Libraries (under License with DRV2605L chip)

    Library #   = Type / Nom Volts / Max Volts / Rise Time (mS) / Brake Time (mS)
    Library   0 = Empty
    Library A/1 = ERM  /  1.3V / 3.0V /  40 -  60 / 20 - 40
    Library B/2 = ERM  /  3.0V / 3.0V /  40 -  60 / 5  - 15
    Library C/3 = ERM  /  3.0V / 3.0V /  60 -  80 / 10 - 25
    Library D/4 = ERM  /  3.0V / 3.0V / 100 - 140 / 15 - 25
    Library E/5 = ERM  /  3.0V / 3.0V / 140 -     / 30 -
    Library   6 = LRA  /       /      /           /
    Library F/7 = ERM  /  4.5V / 5.0V /  35 -  45 / 10 - 20

   NOTE : For Calibration to function properly,
          the Actuator should be attached to a
          50-150 milliGram weighted base plate (a sled, or slug).

 *******************************************************************************
*/


bool hapticCalibERM() {
  { DRV2605_REG_MODE, 0x07 },             //! DRV2605 - Calibrate Mode!
    { DRV2605_REG_FEEDBACK, 0xAA },       //! DRV2605 - LRA mode
    { DRV2605_REG_CONTROL3, 0xA4 },       //! DRV2605 - drive amplitude updated once (0) or twice (1) per cycle
    { DRV2605_REG_RATEDV, 0x82 },         //! DRV2605 - rated voltage 3V
    { DRV2605_REG_CLAMPV, 0x96 },         //! DRV2605 - clamp voltage = 3.3V
    { DRV2605_REG_CONTROL4, 0x30 },       //! DRV2605 - Autocal time = 3 (1.2 seconds!)
    { DRV2605_REG_CONTROL1, 0x83 },       //! DRV2605 - drive_time set for 128Hz = 3.9mSec
    { DRV2605_REG_CONTROL2, 0xF5 },       //! DRV2605 - sample_time = 3, blank =1, idiss = 1
    { DRV2605_REG_LIBRARY, 0x06 },        //! DRV2605 - Library 6 is LRA
    { DRV2605_REG_WAVESEQ1, 1 },          //! DRV2605 - strong click
    { DRV2605_REG_WAVESEQ2, 0 },          //! DRV2605 - strong click
    { DRV2605_REG_OVERDRIVE, 0 },         //! DRV2605 - no overdrive
    { DRV2605_REG_SUSTAINPOS, 0 },        //! DRV2605 - Sustain Time Offset
    { DRV2605_REG_SUSTAINNEG, 0 },        //! DRV2605 - Sustain Time Offset
    { DRV2605_REG_BRAKE, 0 },             //! DRV2605 - Braking Time Offset
    { DRV2605_REG_RTPIN, 0x00 },          //! DRV2605 - real-time-playback zero
    { DRV2605_REG_AUDIOVIBECTRL, 0x00 },  //! DRV2605 - audio to vibe control   (0x00) 10ms, 100hz
    { DRV2605_REG_AUDIOMINLVL, 0x00 },    //! DRV2605 - audio-to-vibe min level
    { DRV2605_REG_AUDIOMAXLVL, 0x40 },    //! DRV2605 -  audio-to-vibe max level
    { DRV2605_REG_AUDIOMINDRV, 0x20 },    //! DRV2605 - audio-to-vibe min drive
    { DRV2605_REG_AUDIOMAXDRV, 0x64 },    //! DRV2605 - audio-to-vibe max drive
    { DRV2605_REG_GO, 0x01 },             //! DRV2605 - trigger a calibrate cycle
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec (1.25 sec max)
    { DRV2605_REG_MODE, 0x00 },           //! DRV2605 - calibrate
  {
    ACTUATOR_SCRIPT_END, 0x00
  }  //! DRV2605 - end of script flag

  return 1;
}

bool hapticCalibLRA() {
  { DRV2605_REG_MODE, 0x07 },             //! DRV2605 - Calibrate Mode!
    { DRV2605_REG_FEEDBACK, 0xAA },       //! DRV2605 - LRA mode
    { DRV2605_REG_CONTROL3, 0xA4 },       //! DRV2605 - drive amplitude updated once (0) or twice (1) per cycle
    { DRV2605_REG_RATEDV, 0x82 },         //! DRV2605 - rated voltage 3V
    { DRV2605_REG_CLAMPV, 0x96 },         //! DRV2605 - clamp voltage = 3.3V
    { DRV2605_REG_CONTROL4, 0x30 },       //! DRV2605 - Autocal time = 3 (1.2 seconds!)
    { DRV2605_REG_CONTROL1, 0x83 },       //! DRV2605 - drive_time set for 128Hz = 3.9mSec
    { DRV2605_REG_CONTROL2, 0xF5 },       //! DRV2605 - sample_time = 3, blank =1, idiss = 1
    { DRV2605_REG_LIBRARY, 0x06 },        //! DRV2605 - Library 6 is LRA
    { DRV2605_REG_WAVESEQ1, 1 },          //! DRV2605 - strong click
    { DRV2605_REG_WAVESEQ2, 0 },          //! DRV2605 - strong click
    { DRV2605_REG_OVERDRIVE, 0 },         //! DRV2605 - no overdrive
    { DRV2605_REG_SUSTAINPOS, 0 },        //! DRV2605 - Sustain Time Offset
    { DRV2605_REG_SUSTAINNEG, 0 },        //! DRV2605 - Sustain Time Offset
    { DRV2605_REG_BRAKE, 0 },             //! DRV2605 - Braking Time Offset
    { DRV2605_REG_RTPIN, 0x00 },          //! DRV2605 - real-time-playback zero
    { DRV2605_REG_AUDIOVIBECTRL, 0x00 },  //! DRV2605 - audio to vibe control   (0x00) 10ms, 100hz
    { DRV2605_REG_AUDIOMINLVL, 0x00 },    //! DRV2605 - audio-to-vibe min level
    { DRV2605_REG_AUDIOMAXLVL, 0x40 },    //! DRV2605 -  audio-to-vibe max level
    { DRV2605_REG_AUDIOMINDRV, 0x20 },    //! DRV2605 - audio-to-vibe min drive
    { DRV2605_REG_AUDIOMAXDRV, 0x64 },    //! DRV2605 - audio-to-vibe max drive
    { DRV2605_REG_GO, 0x01 },             //! DRV2605 - trigger a calibrate cycle
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec
    { ACTUATOR_SCRIPT_DELAY, 0xFF },      //! DRV2605 - delay 0.25 sec (1.25 sec max)
    { DRV2605_REG_MODE, 0x00 },           //! DRV2605 - calibrate
  {
    ACTUATOR_SCRIPT_END, 0x00
  }  //! DRV2605 - end of script flag

  return 1;
}
