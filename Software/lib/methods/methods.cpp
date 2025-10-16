#include <methods.h>

extern TFT_eSPI tft;
extern Adafruit_BME680 bme;
extern Adafruit_LTR390 ltr;
extern Adafruit_VCNL4040 vcnl;

// Dummy sensor values
float tempValue = 23.5;
float humidValue = 45.2;
float pressureValue = 1012.0;
float gasValue = 120.0;
float distanceValue = 150.0;
float ambientValue = 500.0;
float whiteValue = 800.0;
float uvValue = 0.8;
float uvIndexValue = 2.0;
float proximityValue = 300.0;
extern float lastDetailValue;

// Array of boxes
Box boxes[NUM_BOXES] = {
    {"Temperatur", &tempValue, "C", 0, 0, 0, 0, 1},
    {"Luftfeuchtigkeit", &humidValue, "%", 0, 0, 0, 0, 1},
    {"Luftdruck", &pressureValue, "hPa", 0, 0, 0, 0, 0},
    {"Umgebungslicht", &ambientValue, "lux", 0, 0, 0, 0, 0},
    {"Weisses Licht", &whiteValue, "", 0, 0, 0, 0, 0},
    {"Gas", &gasValue, "", 0, 0, 0, 0, 0},
    {"UV Licht", &uvValue, "", 0, 0, 0, 0, 2},
    {"UV Index", &uvIndexValue, "", 0, 0, 0, 0, 1}};

void configureSensors() {
#ifdef REAL_SENSORS
  // LTR390
  ltr.setMode(LTR390_MODE_UVS);
  ltr.setGain(LTR390_GAIN_18);
  ltr.setResolution(LTR390_RESOLUTION_20BIT);

  // VCNL4040
  vcnl.enableAmbientLight(true);
  vcnl.enableProximity(true);
  vcnl.enableWhiteLight(true);
  vcnl.setProximityHighResolution(true);
  vcnl.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
  vcnl.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
  vcnl.setProximityLEDCurrent(VCNL4040_LED_CURRENT_75MA);
  vcnl.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_80);

  // BME688
  bme.setTemperatureOversampling(BME68X_OS_8X);
  bme.setPressureOversampling(BME68X_OS_4X);
  bme.setHumidityOversampling(BME68X_OS_2X);
  bme.setIIRFilterSize(BME68X_FILTER_SIZE_3);
  bme.setODR(BME68X_ODR_NONE);
  bme.setGasHeater(HEATER_TEMP, HEATER_DURATION);
#endif
}

// Calculate layout of boxes
void layoutBoxes() {
  int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;

  int boxIndex = 0;

  // Position boxes in grid, skip center box for logo
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      if (!(row == 1 && col == 1)) {  // Skip center box for logo
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

// Draw logo in center box
void drawLogo() {
  int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;
  int x = MARGIN + 1 * (boxW + SPACING_X);
  int y = MARGIN + 1 * (boxH + SPACING_Y);

  int xCenter = x + (boxW - LOGO_WIDTH) / 2;
  int yCenter = y + (boxH - LOGO_HEIGHT) / 2;

  tft.pushImage(xCenter, yCenter, LOGO_WIDTH, LOGO_HEIGHT, logo);
}

// Draw a single box
void drawBox(int i) {
  tft.fillRoundRect(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, BOX_RADIUS, BOX_COLOR);
  tft.setTextColor(TITLE_COLOR, BOX_COLOR);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(boxes[i].title, boxes[i].x + boxes[i].w / 2, boxes[i].y + 15, 1);
}

// Draw title and hint text on detail page
void updateValues() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500)
    return;
  lastUpdate = millis();
#ifdef REAL_SENSORS
  // BME688
  if (bme.performReading()) {
    tempValue = bme.temperature;
    humidValue = bme.humidity;
    pressureValue = bme.pressure / 100.0F;
    gasValue = bme.gas_resistance / 1000.0F;
  }

  // VCNL4040
  ambientValue = vcnl.getAmbientLight();
  whiteValue = vcnl.getWhiteLight();
  proximityValue = vcnl.getProximity();

  if (proximityValue > HAND_NEAR_TRESHOLD) {
    analogWrite(LED_PWM, 255);
  } else {
    uint8_t brightness = map(constrain(ambientValue, 0, MAX_AMBIENT_LIGHT), 0, MAX_AMBIENT_LIGHT, MIN_PWM, MAX_PWM);
    analogWrite(LED_PWM, brightness);
  }

  // LTR390
  // UV-Index like this: https://www.wetter.com/uv-index/deutschland/deggendorf/DE0001991/
  uvValue = ltr.readUVS();
  float uv_mW_per_cm2 = (float)uvValue / 1048575.0 * 15.0; // 0..15 mW/cm^2 max
  uvIndexValue = uv_mW_per_cm2 / 0.25; // 1 UV Index = 0.25 mW/cm^2
  if (uvIndexValue > 11.0) uvIndexValue = 11.0; // Limit to 11
#else
  // Dummy values
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

// Update value in box if changed
void updateValue(int i) {
  static float lastValues[NUM_BOXES] = {0};
  float newVal = *(boxes[i].value);

  // Only update if value changed
  if (abs(newVal - lastValues[i]) < 0.001)
    return;
  lastValues[i] = newVal;

  // Prepare value string
  char buf[20];
  sprintf(buf, "%.*f", boxes[i].decimals, newVal);

  String fullText = String(buf);
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    fullText += "  C";
  } else {
    fullText += " " + String(boxes[i].unit);
  }

  // Prepare sprite
  TFT_eSprite spr = TFT_eSprite(&tft);
  spr.createSprite(boxes[i].w - 20, 40);
  spr.fillSprite(BOX_COLOR);
  spr.setTextColor(VALUE_COLOR, BOX_COLOR);
  spr.setTextDatum(TL_DATUM);

  // Text width and centering
  int textW = spr.textWidth(fullText, 4);
  int xCenter = (boxes[i].w - 20) / 2 - textW / 2;
  int yCenter = 10;

  // Draw value text
  spr.drawString(fullText, xCenter, yCenter, 4);

  // Draw temperature circle if needed
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    int cWidth = spr.textWidth("C", 4);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
  }

  // Push sprite to screen
  spr.pushSprite(boxes[i].x + 10, boxes[i].y + boxes[i].h / 2 - 20);
  spr.deleteSprite();
}

// Draw detail page with current value
void drawDetailPage(int boxIndex) {
  extern TFT_eSPI tft;

  // Clear screen
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  tft.setFreeFont(&FreeSansBold18pt7b);

  // Title and value
  String title = "Details: " + String(boxes[boxIndex].title);
  String valueStr = String(*boxes[boxIndex].value, boxes[boxIndex].decimals) + " " + boxes[boxIndex].unit;

  // Draw title and value
  tft.drawString(title, SCREEN_W / 2, SCREEN_H / 2 - 40, 1);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(valueStr, SCREEN_W / 2, SCREEN_H / 2 + 20, 1);

  // Hint text
  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Tippe zum Zurueckkehren", SCREEN_W / 2, SCREEN_H - 40, 1);
}

// Draw detail page with sprite for value
void drawDetailPageWithSprite(int boxIndex) {
  float currentValue = *boxes[boxIndex].value;

  // Only update if value changed
  if (abs(currentValue - lastDetailValue) < 0.001)
    return;
  lastDetailValue = currentValue;

  // Prepare value string
  char buf[20];
  sprintf(buf, "%.*f", boxes[boxIndex].decimals, currentValue);

  // Sprite for value
  TFT_eSprite spr = TFT_eSprite(&tft);
  spr.createSprite(300, 60);
  spr.fillSprite(COLOR_BACKGROUND);

  // Text settings
  spr.setTextDatum(TL_DATUM);
  spr.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  spr.setFreeFont(&FreeSansBold12pt7b);

  // Full text with unit
  String fullText = String(buf);
  if (strcmp(boxes[boxIndex].title, "Temperatur") == 0) {
    fullText += "  C";  // Extra space for temperature circle
  } else {
    fullText += " " + String(boxes[boxIndex].unit);
  }

  int textW = spr.textWidth(fullText, 4);
  int xCenter = (300 - textW) / 2;
  int yCenter = 10;

  spr.drawString(fullText, xCenter, yCenter, 4);

  // Draw temperature circle if needed
  if (strcmp(boxes[boxIndex].title, "Temperatur") == 0) {
    int cWidth = spr.textWidth("C", 4);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
  }

  spr.pushSprite((SCREEN_W - 300) / 2, SCREEN_H / 2 + 10);
  spr.deleteSprite();
}

// Draw title and hint text on detail page
void drawDetailPageTitle(int boxIndex) {
  tft.fillScreen(COLOR_BACKGROUND);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  tft.setFreeFont(&FreeSansBold18pt7b);
  String title = "Details: " + String(boxes[boxIndex].title);
  tft.drawString(title, SCREEN_W / 2, SCREEN_H / 2 - 40, 1);

  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Tippe zum Zurueckkehren", SCREEN_W / 2, SCREEN_H - 40, 1);
}
