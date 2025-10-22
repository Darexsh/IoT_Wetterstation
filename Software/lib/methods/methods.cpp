/**
 * @file methods.cpp
 * @brief Implementation of functions for managing display boxes, sensor updates, and detail pages
 *
 * This file contains the implementation of:
 * - Sensor configuration (BME680, LTR390, VCNL4040)
 * - Layout and drawing of boxes on the main screen
 * - Updating sensor values (real or dummy)
 * - Display backlight control based on ambient light and proximity
 * - Detail page rendering with TFT sprites for smooth updates
 */


#include <methods.h>

extern TFT_eSPI tft;            ///< TFT object
extern Adafruit_BME680 bme;     ///< BME680 sensor
extern Adafruit_LTR390 ltr;     ///< LTR390 sensor
extern Adafruit_VCNL4040 vcnl;  ///< VCNL4040 sensor
extern bool bme_ok;
extern bool vcnl_ok;
extern bool ltr_ok;

/// Dummy sensor values for testing
// float tempValue = 23.5;
// float humidValue = 45.2;
// float pressureValue = 1012.0;
// float gasValue = 120.0;
// float distanceValue = 150.0;
// float ambientValue = 500.0;
// float whiteValue = 800.0;
// float uvValue = 0.8;
// float uvIndexValue = 2.0;
// float proximityValue = 300.0;

/// Sensor values for testing
float tempValue = 0.0;
float humidValue = 0.0;
float pressureValue = 0.0;
float gasValue = 0.0;
float distanceValue = 0.0;
float ambientValue = 0.0;
float whiteValue = 0.0;
float uvValue = 0.0;
float uvIndexValue = 0.0;
float proximityValue = 0.0;

/// Last detail page value
extern float lastDetailValue;  

/// Array of boxes displayed on screen
Box boxes[NUM_BOXES] = {
    {"Temperatur", &tempValue, "C", 0, 0, 0, 0, 1},
    {"Luftfeuchtigkeit", &humidValue, "%", 0, 0, 0, 0, 1},
    {"Luftdruck", &pressureValue, "hPa", 0, 0, 0, 0, 0},
    {"Umgebungslicht", &ambientValue, "lux", 0, 0, 0, 0, 0},
    {"Weisses Licht", &whiteValue, "", 0, 0, 0, 0, 0},
    {"Gas", &gasValue, "", 0, 0, 0, 0, 0},
    {"UV Licht", &uvValue, "", 0, 0, 0, 0, 2},
    {"UV Index", &uvIndexValue, "", 0, 0, 0, 0, 1}};

/**
 * @brief Configure all sensors (BME680, LTR390, VCNL4040) with desired parameters
 */
void configureSensors(bool bme_ok, bool vcnl_ok, bool ltr_ok) {
  if (ltr_ok) {
    ltr.setMode(LTR390_MODE_UVS);
    ltr.setGain(LTR390_GAIN_18);
    ltr.setResolution(LTR390_RESOLUTION_20BIT);
  }

  if (vcnl_ok) {
    vcnl.enableAmbientLight(true);
    vcnl.enableProximity(true);
    vcnl.enableWhiteLight(true);
    vcnl.setProximityHighResolution(true);
    vcnl.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
    vcnl.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
    vcnl.setProximityLEDCurrent(VCNL4040_LED_CURRENT_75MA);
    vcnl.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_80);
  }

  if (bme_ok) {
    bme.setTemperatureOversampling(BME68X_OS_8X);
    bme.setPressureOversampling(BME68X_OS_4X);
    bme.setHumidityOversampling(BME68X_OS_2X);
    bme.setIIRFilterSize(BME68X_FILTER_SIZE_3);
    bme.setODR(BME68X_ODR_NONE);
    bme.setGasHeater(HEATER_TEMP, HEATER_DURATION);
  }
}

/**
 * @brief Layout all boxes on screen in a grid, skipping center for logo
 */
void layoutBoxes() {
  int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;

  int boxIndex = 0;

  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      if (!(row == 1 && col == 1)) {  ///< Skip center box for logo
        boxes[boxIndex].x = MARGIN + col * (boxW + SPACING_X);
        boxes[boxIndex].y = MARGIN + row * (boxH + SPACING_Y);
        boxes[boxIndex].w = boxW;
        boxes[boxIndex].h = boxH;
        boxIndex++;
        if (boxIndex >= NUM_BOXES)
          return;
      }
    }
  }
}

/**
 * @brief Draw logo in center box
 */
void drawLogo() {
  int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;
  int x = MARGIN + 1 * (boxW + SPACING_X);
  int y = MARGIN + 1 * (boxH + SPACING_Y);

  int xCenter = x + (boxW - LOGO_WIDTH) / 2;
  int yCenter = y + (boxH - LOGO_HEIGHT) / 2;

  tft.pushImage(xCenter, yCenter, LOGO_WIDTH, LOGO_HEIGHT, logo);
}

/**
 * @brief Draw a single box
 * @param i Index of box in boxes array
 */
void drawBox(int i) {
  tft.fillRoundRect(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, BOX_RADIUS, BOX_COLOR);
  tft.setTextColor(TITLE_COLOR, BOX_COLOR);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(boxes[i].title, boxes[i].x + boxes[i].w / 2, boxes[i].y + 15, 1);
}

/**
 * @brief Update all sensor values, read real sensors if available, else dummy values
 *
 * Also sets display backlight:
 * - Full brightness if hand is near (proximity)
 * - Else adaptive brightness based on ambient light
 */
void updateValues() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500)
    return;
  lastUpdate = millis();

#ifdef REAL_SENSORS
  // Read BME680 sensor values
  if (bme_ok && bme.performReading()) {
    tempValue = bme.temperature;
    humidValue = bme.humidity;
    pressureValue = bme.pressure / 100.0F;
    gasValue = bme.gas_resistance / 1000.0F;
  }

  // Read VCNL4040 sensor values
  if (vcnl_ok) {
    ambientValue = vcnl.getAmbientLight();
    whiteValue = vcnl.getWhiteLight();
    proximityValue = vcnl.getProximity();
  }

  // Set display backlight: full brightness if hand near, else adaptive brightness
  // if (proximityValue > HAND_NEAR_TRESHOLD) {
  //   analogWrite(LED_PWM, 255);  ///< Full brightness
  // } else {
  //   uint8_t brightness = map(constrain(ambientValue, 0, MAX_AMBIENT_LIGHT),
  //                            0, MAX_AMBIENT_LIGHT, MIN_PWM, MAX_PWM);
  //   analogWrite(LED_PWM, brightness);  ///< Adaptive brightness
  // }

  // Read LTR390 UV sensor and calculate UV Index
  if (ltr_ok) {
    uvValue = ltr.readUVS();
    float uv_mW_per_cm2 = (float)uvValue / 1048575.0 * 15.0;
    uvIndexValue = uv_mW_per_cm2 / 0.25;
    if (uvIndexValue > 11.0) uvIndexValue = 11.0;
  }
#else
  // Dummy values for testing
  tempValue = constrain(tempValue + random(-2, 3) / 10.0, -20, 50);
  humidValue = constrain(humidValue + random(-5, 6) / 10.0, 0, 100);
  pressureValue = constrain(pressureValue + random(-1, 2), 900, 1100);
  gasValue = constrain(gasValue + random(-2, 3), 0, 1000);
  distanceValue = constrain(distanceValue + random(-5, 6), 0, 1000);
  ambientValue = max(0.0f, ambientValue + random(-10, 11));
  whiteValue = max(0.0f, whiteValue + random(-10, 11));
  uvValue = max(0.0f, uvValue + random(-1, 2) / 20.0f);
  uvIndexValue = max(0.0f, uvIndexValue + random(-1, 2) / 20.0f);
#endif
}

/**
 * @brief Update a single box value on the main screen if it has changed
 * @param i Index of the box in the boxes array
 *
 * Uses a TFT sprite to draw the value to reduce flicker.
 */
void updateValue(int i) {
  static float lastValues[NUM_BOXES] = {0};
  float newVal = *(boxes[i].value);

  ///< Only update if value changed significantly
  if (abs(newVal - lastValues[i]) < 0.001)
    return;
  lastValues[i] = newVal;

  ///< Prepare value string
  char buf[20];
  sprintf(buf, "%.*f", boxes[i].decimals, newVal);

  String fullText = String(buf);
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    fullText += "  C";  ///< Extra space for temperature circle
  } else {
    fullText += " " + String(boxes[i].unit);
  }

  ///< Create a sprite for smooth drawing
  TFT_eSprite spr = TFT_eSprite(&tft);
  spr.createSprite(boxes[i].w - 20, 40);
  spr.fillSprite(BOX_COLOR);
  spr.setTextColor(VALUE_COLOR, BOX_COLOR);
  spr.setTextDatum(TL_DATUM);

  ///< Center text within sprite
  int textW = spr.textWidth(fullText, 4);
  int xCenter = (boxes[i].w - 20) / 2 - textW / 2;
  int yCenter = 10;

  ///< Draw the value string
  spr.drawString(fullText, xCenter, yCenter, 4);

  ///< Draw small circle for temperature unit "C"
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    int cWidth = spr.textWidth("C", 4);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
  }

  ///< Push sprite to TFT screen
  spr.pushSprite(boxes[i].x + 10, boxes[i].y + boxes[i].h / 2 - 20);
  spr.deleteSprite();
}

/**
 * @brief Draw the detail page for a specific box
 * @param boxIndex Index of the box to display
 *
 * Draws the title, current value, unit, and a hint text to return.
 */
void drawDetailPage(int boxIndex) {
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  tft.setFreeFont(&FreeSansBold18pt7b);

  ///< Title and value
  String title = "Details: " + String(boxes[boxIndex].title);
  String valueStr = String(*boxes[boxIndex].value, boxes[boxIndex].decimals) + " " + boxes[boxIndex].unit;

  ///< Draw title and value
  tft.drawString(title, SCREEN_W / 2, SCREEN_H / 2 - 40, 1);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(valueStr, SCREEN_W / 2, SCREEN_H / 2 + 20, 1);

  ///< Hint text
  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Tippen, um zur Hauptseite zu gelangen", SCREEN_W / 2, SCREEN_H - 40, 1);
}

/**
 * @brief Draw the detail page for a box using a sprite for the value
 * @param boxIndex Index of the box
 *
 * Updates the detail page only if the value changed.
 * Uses a TFT sprite for smooth animation.
 */
void drawDetailPageWithSprite(int boxIndex) {
  float currentValue = *boxes[boxIndex].value;

  ///< Only update if value changed
  if (abs(currentValue - lastDetailValue) < 0.001)
    return;
  lastDetailValue = currentValue;

  ///< Prepare value string
  char buf[20];
  sprintf(buf, "%.*f", boxes[boxIndex].decimals, currentValue);

  ///< Create sprite for drawing
  TFT_eSprite spr = TFT_eSprite(&tft);
  spr.createSprite(300, 60);
  spr.fillSprite(COLOR_BACKGROUND);

  ///< Text settings
  spr.setTextDatum(TL_DATUM);
  spr.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  spr.setFreeFont(&FreeSansBold12pt7b);

  ///< Full text with unit
  String fullText = String(buf);
  if (strcmp(boxes[boxIndex].title, "Temperatur") == 0) {
    fullText += "  C";  ///< Extra space for temperature circle
  } else {
    fullText += " " + String(boxes[boxIndex].unit);
  }

  ///< Center text in sprite
  int textW = spr.textWidth(fullText, 4);
  int xCenter = (300 - textW) / 2;
  int yCenter = 10;

  spr.drawString(fullText, xCenter, yCenter, 4);

  ///< Draw temperature circle if needed
  if (strcmp(boxes[boxIndex].title, "Temperatur") == 0) {
    int cWidth = spr.textWidth("C", 4);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
  }

  ///< Push sprite to screen
  spr.pushSprite((SCREEN_W - 300) / 2, SCREEN_H / 2 + 10);
  spr.deleteSprite();
}

/**
 * @brief Draw the title and hint text on detail page
 * @param boxIndex Index of the box
 *
 * Clears the screen and displays title and "tap to return" hint.
 */
void drawDetailPageTitle(int boxIndex) {
  tft.fillScreen(COLOR_BACKGROUND);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  tft.setFreeFont(&FreeSansBold18pt7b);
  String title = "Details: " + String(boxes[boxIndex].title);
  tft.drawString(title, SCREEN_W / 2, SCREEN_H / 2 - 40, 1);

  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Tippen, um zur Hauptseite zu gelangen", SCREEN_W / 2, SCREEN_H - 40, 1);
}
