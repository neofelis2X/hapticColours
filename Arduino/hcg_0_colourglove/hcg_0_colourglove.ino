/*
  This


 *******************************************************************************
*/
#include "Adafruit_TCS34725.h"
#include "Adafruit_DRV2605.h"
#include <Wire.h>  // for I2C communication

// This allows to write serial messages that are
// specific debug messages and can be switched off
#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

//___

//needed for I2C communication
#define button1Pin 20
#define led1Pin LED_BUILTIN
volatile bool button1State = LOW;  // this variable is used in an interrupt, for that it need to be volatile

//___
// This section prepares the use the TCA9548A I2C Multiplexer.
#define TCAADDR 0x70  // device adress
#define tcaPinA 2
#define tcaPinB 5

//___
// In this section are experimental elements for the serial menu
/*
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

using namespace Menu;
#define MAX_DEPTH 1
*/
//___
// The Adafruit TCS34725 colour sensor is set up here
// including experimental autorange code
#define tcsLedPin 5
const byte tcsInterruptPin = 7;
volatile bool tcsReady = true;

#define CLEARTHRESHHOLD 2000

enum colours {
  red,
  yellow,
  green,
  cyan,
  blue,
  magenta,
  white,
  grey,
  black,
};

byte colourCase = 0;

// An experimental wrapper class that implements the improved lux and color temperature from
// TAOS and a basic autorange mechanism.
//
// Written by ductsoup, public domain
//

// RGB Color Sensor with IR filter and White LED - TCS34725
// I2C 7-bit address 0x29, 8-bit address 0x52
//
// http://www.adafruit.com/product/1334
// http://learn.adafruit.com/adafruit-color-sensors/overview
// http://www.adafruit.com/datasheets/TCS34725.pdf
// http://www.ams.com/eng/Products/Light-Sensors/Color-Sensor/TCS34725
// http://www.ams.com/eng/content/view/download/265215 <- DN40, calculations
// http://www.ams.com/eng/content/view/download/181895 <- DN39, some thoughts on autogain
// http://www.ams.com/eng/content/view/download/145158 <- DN25 (original Adafruit calculations)
//
// connect LED to digital 4 or GROUND for ambient light sensing
// connect SCL to analog 5
// connect SDA to analog 4
// connect Vin to 3.3-5V DC
// connect GROUND to common ground

// some magic numbers for this device from the DN40 application note
#define TCS34725_R_Coef 0.136
#define TCS34725_G_Coef 1.000
#define TCS34725_B_Coef -0.444
#define TCS34725_GA 1.0
#define TCS34725_DF 310.0
#define TCS34725_CT_Coef 3810.0
#define TCS34725_CT_Offset 1391.0

// class for TCS34725 autorange and colour correction
class tcs34725 {
private:
  struct tcs_agc {
    tcs34725Gain_t ag;
    uint8_t at;
    uint16_t mincnt;
    uint16_t maxcnt;
  };
  static const tcs_agc agc_lst[];
  uint16_t agc_cur;

  void setGainTime(void);

public:
  Adafruit_TCS34725 tcs;  //Object creation for the tcs library class
  tcs34725(void);
  boolean begin(void);
  void getRawDataInt(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);
  void calculateDN40(void);
  bool autoRange(void);
  bool autoRangeInt(void);

  boolean isAvailable, isSaturated;
  uint16_t againx, atime, atime_ms;
  uint16_t r, g, b, c;
  uint16_t ir;
  uint16_t r_comp, g_comp, b_comp, c_comp;
  uint16_t saturation, saturation75;
  float cratio, cpl, ct, lux, maxlux;
};

tcs34725 tcs40;  //Object creation for the autorange and colour correction class

//
// Gain/time combinations to use and the min/max limits for hysteresis
// that avoid saturation. They should be in order from dim to bright.
//
// Also set the first min count and the last max count to 0 to indicate
// the start and end of the list.
//
const tcs34725::tcs_agc tcs34725::agc_lst[] = {
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_614MS, 0, 20000 },
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_154MS, 4990, 63000 },
  { TCS34725_GAIN_16X, TCS34725_INTEGRATIONTIME_154MS, 16790, 63000 },
  { TCS34725_GAIN_4X, TCS34725_INTEGRATIONTIME_154MS, 15740, 63000 },
  { TCS34725_GAIN_1X, TCS34725_INTEGRATIONTIME_154MS, 15740, 0 }
};

tcs34725::tcs34725()
  : agc_cur(0), isAvailable(0), isSaturated(0) {
}

//___
// The Adafruit DRV2605 haptic driver is set up here
Adafruit_DRV2605 drv;

byte menueActive = 0;
byte waveform = 0;  // active waveform to play
byte wfPause = 60;  // Pause between every repeat, 8 Slots therefore only 4 repeats possible
byte wfRepeat = 4;  // Number of repeats of waveform, 8 Slots therefore only 4 repeats possible
byte wfLib = 6;
const byte maxInput = 10;

//___
