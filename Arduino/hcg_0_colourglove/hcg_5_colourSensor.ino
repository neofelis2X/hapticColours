/*



 *******************************************************************************
  Basic functions of the TCS library
  boolean begin;
  boolean init();
  void setIntegrationTime();
  void setGain();
  void getRawData();
  void getRGB();
  void getRawDataOneShot();
  uint16_t calculateColorTemperature();
  uint16_t calculateColorTemperature_dn40();
  uint16_t calculateLux();
  void write8();
  uint8_t read8();
  uint16_t read16();
  void setInterrupt();
  void clearInterrupt();
  void setIntLimits();
  void enable();
  void disable();


*/

// initialize the sensor
boolean tcs34725::begin(void) {
  debugln(F("\tdb tcs34725::begin started."));
  tcs = Adafruit_TCS34725(agc_lst[agc_cur].at, agc_lst[agc_cur].ag);

  if (isAvailable == tcs.begin()) {
    debugln(F("\tdb tcs.begin() finished positive."));
    debugln(F("\tdb setGainTime is about to run."));
    setGainTime();
  }

  debug(F("\tdb value of isAvailable is: "));
  debugln(isAvailable);
  return (isAvailable);
}

// Set the gain and integration time
void tcs34725::setGainTime(void) {
  tcs.setGain(agc_lst[agc_cur].ag);
  tcs.setIntegrationTime(agc_lst[agc_cur].at);
  atime = int(agc_lst[agc_cur].at);
  atime_ms = ((256 - atime) * 2.4);
  switch (agc_lst[agc_cur].ag) {
    case TCS34725_GAIN_1X:
      againx = 1;
      break;
    case TCS34725_GAIN_4X:
      againx = 4;
      break;
    case TCS34725_GAIN_16X:
      againx = 16;
      break;
    case TCS34725_GAIN_60X:
      againx = 60;
      break;
  }
  debugln(F("\tdb setGainTime completed successfull."));
}
/*
  // Retrieve data from the sensor and do the calculations
  void tcs34725::getData(void) {
  // read the sensor and autorange if necessary
  tcs40.tcs.getRawData(&r, &g, &b, &c);
  while (1) {
    if (agc_lst[agc_cur].maxcnt && c > agc_lst[agc_cur].maxcnt)
      agc_cur++;
    else if (agc_lst[agc_cur].mincnt && c < agc_lst[agc_cur].mincnt)
      agc_cur--;
    else break;

    setGainTime();
    delay((256 - atime) * 2.4 * 2); // shock absorber
    tcs.getRawData(&r, &g, &b, &c);
    break;
  }

  // DN40 calculations
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  r_comp = r - ir;
  g_comp = g - ir;
  b_comp = b - ir;
  c_comp = c - ir;
  cratio = float(ir) / float(c);

  saturation = ((256 - atime) > 63) ? 65535 : 1024 * (256 - atime);
  saturation75 = (atime_ms < 150) ? (saturation - saturation / 4) : saturation;
  isSaturated = (atime_ms < 150 && c > saturation75) ? 1 : 0;
  cpl = (atime_ms * againx) / (TCS34725_GA * TCS34725_DF);
  maxlux = 65535 / (cpl * 3);

  lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
  ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;
  }
*/
bool tcs34725::autoRange(void) {
  // read the sensor and autorange if necessary
  tcs.getRawData(&r, &g, &b, &c);

  while (1) {
    if (agc_lst[agc_cur].maxcnt && c > agc_lst[agc_cur].maxcnt)
      agc_cur++;
    else if (agc_lst[agc_cur].mincnt && c < agc_lst[agc_cur].mincnt)
      agc_cur--;
    else break;

    setGainTime();
    delay((256 - atime) * 2.4 * 2);  // shock absorber
    tcs.getRawData(&r, &g, &b, &c);
    break;
  }
  debugln(F("\tdb autoRange completed successfull."));
  return 1;
}

bool tcs34725::autoRangeInt(void) {
  // read the sensor and autorange if necessary
  float timeOut = millis();
  while (!tcsReady) {
    //wait until the sensor sends an interrupt that it is ready to fetch data
    if (millis() - timeOut > 3000) {
      debugln(F("\tdb Error: colour sensor did not send interrupt."));
      return 0;
    }
  }
  getRawDataInt(&r, &g, &b, &c);

  while (1) {
    if (agc_lst[agc_cur].maxcnt && c > agc_lst[agc_cur].maxcnt)
      agc_cur++;
    else if (agc_lst[agc_cur].mincnt && c < agc_lst[agc_cur].mincnt)
      agc_cur--;
    else break;

    setGainTime();
    //    delay((256 - atime) * 2.4 * 2); // shock absorber
    timeOut = millis();
    while (!tcsReady) {
      //wait until the sensor sends an interrupt that it is ready to fetch data
      if (millis() - timeOut > 2000) {
        debugln(F("\tdb Error: colour sensor did not send interrupt."));
        return 0;
      }
    }
    getRawDataInt(&r, &g, &b, &c);
    break;
  }
  debugln(F("\tdb autorange completed successfully."));
  return 1;
}

void tcs34725::calculateDN40(void) {
  // DN40 calculations
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  r_comp = r - ir;
  g_comp = g - ir;
  b_comp = b - ir;
  c_comp = c - ir;
  cratio = float(ir) / float(c);

  saturation = ((256 - atime) > 63) ? 65535 : 1024 * (256 - atime);
  saturation75 = (atime_ms < 150) ? (saturation - saturation / 4) : saturation;  //if IntTime is lower 150 than 75%sat otherwise 100%sat
  isSaturated = (atime_ms < 150 && c > saturation75) ? 1 : 0;                    //if
  cpl = (atime_ms * againx) / (TCS34725_GA * TCS34725_DF);
  maxlux = 65535 / (cpl * 3);

  lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
  ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;

  debugln(F("\tdb calculateDN40 finished successfull."));
}

//tcs.getRawData() does a delay(Integration_Time) after the sensor readout.
//  We don't need to wait for the next integration cycle because we receive an interrupt when the integration cycle is complete
void tcs34725::getRawDataInt(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
  *r = tcs.read16(TCS34725_RDATAL);
  *g = tcs.read16(TCS34725_GDATAL);
  *b = tcs.read16(TCS34725_BDATAL);
  *c = tcs.read16(TCS34725_CDATAL);
  tcsReady = false;
  debugln(F("\tdb getRawDataInt finished successfull."));
}

void colourInterrupt() {
  if (tcsReady) {
    uint16_t r, g, b, c, colorTemp, lux;
    tcs40.getRawDataInt(&r, &g, &b, &c);
    //        tcsReady = false; //its already in the function
    colorTemp = tcs40.tcs.calculateColorTemperature(r, g, b);
    lux = tcs40.tcs.calculateLux(r, g, b);

    Serial.print(F("Color Temp: "));
    Serial.print(colorTemp, DEC);
    Serial.print(F(" K - "));
    Serial.print(F("Lux: "));
    Serial.print(lux, DEC);
    Serial.print(F(" - "));
    Serial.print(F("R: "));
    Serial.print(r, DEC);
    Serial.print(F(" G: "));
    Serial.print(g, DEC);
    Serial.print(F(" B: "));
    Serial.print(b, DEC);
    Serial.print(F(" C: "));
    Serial.print(c, DEC);
    Serial.println(F("  "));
    Serial.flush();

    tcs40.tcs.clearInterrupt();
    digitalWrite(led1Pin, LOW);
  }
}

void colourProtocol() {

  Serial.print(F("Gain:"));
  Serial.print(tcs40.againx);
  Serial.print(F("x "));
  Serial.print(F("Time:"));
  Serial.print(tcs40.atime_ms);
  Serial.print(F("ms (0x"));
  Serial.print(tcs40.atime, HEX);
  Serial.println(F(")"));

  Serial.print(F("Raw R:"));
  Serial.print(tcs40.r);
  Serial.print(F(" G:"));
  Serial.print(tcs40.g);
  Serial.print(F(" B:"));
  Serial.print(tcs40.b);
  Serial.print(F(" C:"));
  Serial.println(tcs40.c);

  Serial.print(F("IR:"));
  Serial.print(tcs40.ir);
  Serial.print(F(" CRATIO:"));
  Serial.print(tcs40.cratio);
  Serial.print(F(" Sat:"));
  Serial.print(tcs40.saturation);
  Serial.print(F(" Sat75:"));
  Serial.print(tcs40.saturation75);
  Serial.print(F(" "));
  Serial.println(tcs40.isSaturated ? "*SATURATED*" : "");

  Serial.print(F("CPL:"));
  Serial.print(tcs40.cpl);
  Serial.print(F(" Max lux:"));
  Serial.println(tcs40.maxlux);

  Serial.print(F("Compensated R:"));
  Serial.print(tcs40.r_comp);
  Serial.print(F(" G:"));
  Serial.print(tcs40.g_comp);
  Serial.print(F(" B:"));
  Serial.print(tcs40.b_comp);
  Serial.print(F(" C:"));
  Serial.println(tcs40.c_comp);

  Serial.print(F("Lux:"));
  Serial.print(tcs40.lux);
  Serial.print(F(" CT:"));
  Serial.print(tcs40.ct);
  Serial.println(F("K"));
  //  Serial.println();

  // conversion to HEX colour like #000000
  Serial.print(F("HEX: \t\t\t# "));
  Serial.print(mapUintTo255(tcs40.r, tcs40.c), HEX);
  Serial.print(mapUintTo255(tcs40.g, tcs40.c), HEX);
  Serial.println(mapUintTo255(tcs40.b, tcs40.c), HEX);

  // conversion to HEX colour like #000000
  Serial.print(F("HEX compensated: \t# "));
  Serial.print(mapUintTo255(tcs40.r_comp, tcs40.c_comp), HEX);
  Serial.print(mapUintTo255(tcs40.g_comp, tcs40.c_comp), HEX);
  Serial.println(mapUintTo255(tcs40.b_comp, tcs40.c_comp), HEX);

  float colourHsvRaw[3];
  float channelSumRaw = tcs40.r + tcs40.g + tcs40.b;
  rgb2hsv(mapUintTo1(tcs40.r, channelSumRaw), mapUintTo1(tcs40.g, channelSumRaw), mapUintTo1(tcs40.b, channelSumRaw), colourHsvRaw);
  // conversion to HSV colour like 360°, 100%, 100%;
  Serial.print(F("HSV: \t\t\t# "));
  Serial.print(colourHsvRaw[0], 0);
  Serial.print(F(", "));
  Serial.print(colourHsvRaw[1], 0);
  Serial.print(F(", "));
  Serial.println(colourHsvRaw[2], 0);

  float colourHsv[3];
  float channelSum = tcs40.r_comp + tcs40.g_comp + tcs40.b_comp;
  rgb2hsv(mapUintTo1(tcs40.r_comp, channelSum), mapUintTo1(tcs40.g_comp, channelSum), mapUintTo1(tcs40.b_comp, channelSum), colourHsv);
  // conversion to HSV colour like 360°, 100%, 100%;
  Serial.print(F("HSV compensated: \t# "));
  Serial.print(colourHsv[0], 0);
  Serial.print(F(", "));
  Serial.print(colourHsv[1], 0);
  Serial.print(F(", "));
  Serial.println(colourHsv[2], 0);

  Serial.println(F("****************************************"));
  Serial.println();
}

//___
//This mappings are used to convert the colour values to the right form, for photoshop or other target systems

byte mapUintTo255(float value, float range) {  //input has to be float values, because integer calculation wont work!
  value /= range;
  value *= 256;
  if (value > 255)
    value = 255;
  float roundValue = (value - int(value)) * 10;
  if (roundValue > 5)
    value = byte(value) + 1;
  else value = byte(value);
  return value;
}
/*
  byte map1To255(uint16_t input) {
  // beware, map() uses integer math!
  // syntax: map(value, fromLow, fromHigh, toLow, toHigh)
  // the range 0 to 65535 equals an unsigned integer

  byte output = map(input, 0, 1, 0, 255);
  return output;
  }
*/
float mapUintTo1(float value, float range) {
  value /= range;
  return value;
}
//***********************************************************************

// ___
// The RGB to HSV conversion is code made by Karsten Schmidt alias postspectacular. Thank you for sharing
// HSV->RGB conversion based on GLSL version
// expects HSV channels defined in 0.0 .. 1.0 interval

float fract(float x) {
  return x - int(x);
}

float mix(float a, float b, float t) {
  return a + (b - a) * t;
}

float step(float e, float x) {
  return x < e ? 0.0 : 1.0;
}

// HSV->RGB conversion based on GLSL version
// expects hsv channels defined in 0.0 .. 1.0 interval
float* hsv2rgb(float h, float s, float b, float* rgb) {
  if (h > 1 || s > 1 || b > 1) {  //this checks if the values are entered in the right format and range
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;

    debugln(F("HSV input didnt fit RGB converter. No output."));
    return rgb;
  }
  rgb[0] = b * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[1] = b * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[2] = b * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  debugln(F("\tdb hsv2rgb completed successfull."));
  return rgb;
}

// RGB->HSV conversion based on GLSL version
// expects RGB channels defined in 0.0 .. 1.0 interval
float* rgb2hsv(float r, float g, float b, float* hsv) {  //a snippet of Karsten Schmidt alias postspectacular
  if (r > 1 || g > 1 || b > 1) {                         //this checks if the values are entered in the right format and range
    hsv[0] = 0;
    hsv[1] = 0;
    hsv[2] = 0;

    debugln(F("RGB input didnt fit HSV converter. No output."));
    return hsv;
  }

  float s = step(b, g);
  float px = mix(b, g, s);
  float py = mix(g, b, s);
  float pz = mix(-1.0, 0.0, s);
  float pw = mix(0.6666666, -0.3333333, s);
  s = step(px, r);
  float qx = mix(px, r, s);
  float qz = mix(pw, pz, s);
  float qw = mix(r, px, s);
  float d = qx - min(qw, py);
  hsv[0] = (abs(qz + (qw - py) / (6.0 * d + 1e-10))) * 360;  //outputs in a range of 0 - 360°
  hsv[1] = (d / (qx + 1e-10)) * 100;                         //outputs in a range of 0 - 100%
  hsv[2] = qx * 100;                                         //outputs in a range of 0 - 100%
  debugln(F("\tdb rgb2hsv completed successfull."));
  return hsv;
}

bool inRange(float val, int minVal, int maxVal) {
  return ((minVal <= val) && (val < maxVal));
}

void noteColour(float* hsv) {

  if (inRange(hsv[1], 0, 15) && inRange(hsv[2], 90, 100)) {
    colourCase = white;  //the measurement is almost white or white
    Serial.print("The measuared colour is: ");
    Serial.println("White");
    Serial.println(white);
    return;
  } else if (inRange(hsv[1], 0, 15) && inRange(hsv[2], 25, 90)) {
    colourCase = grey;  //the measurement has a low colour saturation
    Serial.print("The measuared colour is: ");
    Serial.println("Grey");
    return;
  } else if (inRange(hsv[2], 0, 25)) {
    colourCase = black;  //the measurement is almost black or black
    Serial.print("The measuared colour is: ");
    Serial.println("Black");
    return;
  } else if (inRange(hsv[0], 0, 30) || inRange(hsv[0], 330, 361)) {
    colourCase = red;  //the measurement is a reddish colour tone
    Serial.print("The measuared colour is: ");
    Serial.println("Red");
  } else if (inRange(hsv[0], 30, 90)) {
    colourCase = yellow;  //the measurement is a yellowish colour tone
    Serial.print("The measuared colour is: ");
    Serial.println("Yellow");
  } else if (inRange(hsv[0], 90, 150)) {
    colourCase = green;  //the measurement is a greenish colour tone
    Serial.print("The measuared colour is: ");
    Serial.println("Green");
  } else if (inRange(hsv[0], 150, 210)) {
    colourCase = cyan;  //the measurement is a cyanish colour tone
    Serial.print("The measuared colour is: ");
    Serial.println("Cyan");
  } else if (inRange(hsv[0], 210, 270)) {
    colourCase = blue;  //the measurement is a blueish colour tone
    Serial.print("The measuared colour is: ");
    Serial.println("Blue");
  } else if (inRange(hsv[0], 270, 330)) {
    colourCase = magenta;  //the measurement is a magentish colour tone
    Serial.print("The measuared colour is: ");
    Serial.println("Magenta");
  } else {
    Serial.println("No valid colour tone measurement!");
  }
}
