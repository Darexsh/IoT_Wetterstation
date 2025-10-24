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

float historyBuffers[NUM_BOXES][HISTORY_LENGTH] = {-999.0};  ///< History buffers for graphs
int historyIndex[NUM_BOXES] = {0};                           ///< Current index in history buffers
bool detailGraphNeedsRedraw = true;                          ///< Flag to indicate graph redraw needed

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
  static unsigned long lastFastUpdate = 0;
  static unsigned long lastHistoryUpdate = 0;
  unsigned long currentTime = millis();

  bool doFastUpdate = false;
  bool doHistoryUpdate = false;

  // --- 1. SCHNELLES SENSOR-UPDATE PRÜFEN (für Hauptseite) ---
  if (currentTime - lastFastUpdate >= FAST_UPDATE_INTERVAL) {
    lastFastUpdate = currentTime;
    doFastUpdate = true;
  }

  // --- 2. HISTORIE-UPDATE PRÜFEN (nur alle 5 Minuten) ---
  if (doFastUpdate && (currentTime - lastHistoryUpdate >= HISTORY_UPDATE_INTERVAL)) {
    lastHistoryUpdate = currentTime;
    doHistoryUpdate = true;
    detailGraphNeedsRedraw = true;
  }

  // --- Wenn KEIN Update nötig ist, Funktion verlassen ---
  if (!doFastUpdate)
    return;

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

  // --- Nur alle 5 Minuten neuen Punkt in History speichern ---
  if (doHistoryUpdate) {
    updateHistory(0, tempValue);
    updateHistory(1, humidValue);
    updateHistory(2, pressureValue);
    updateHistory(3, ambientValue);
    updateHistory(4, whiteValue);
    updateHistory(5, gasValue);
    updateHistory(6, uvValue);
    updateHistory(7, uvIndexValue);
  }
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
  tft.setTextColor(TFT_DARKGREY, COLOR_BACKGROUND);
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

  // Nur neu zeichnen, wenn nötig
  if (abs(currentValue - lastDetailValue) < 0.001 && !detailGraphNeedsRedraw)
    return;

  lastDetailValue = currentValue;

  // --- 1. Wert oben darstellen ---
  TFT_eSprite valueSpr = TFT_eSprite(&tft);
  valueSpr.createSprite(SCREEN_W - 40, 60);  // kompakter Sprite
  valueSpr.fillSprite(COLOR_BACKGROUND);

  char buf[20];
  sprintf(buf, "%.*f", boxes[boxIndex].decimals, currentValue);
  String valuePart = String(buf);
  String unitPart = (strcmp(boxes[boxIndex].title, "Temperatur") == 0) ? "C" : String(boxes[boxIndex].unit);

  int valueWidth, unitWidth, totalWidth, startX;

  // Wert zeichnen
  valueSpr.setTextDatum(MC_DATUM);
  valueSpr.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  valueSpr.setFreeFont(&FreeSansBold24pt7b);
  valueWidth = valueSpr.textWidth(valuePart);
  totalWidth = valueWidth + 10;  // Platz für Einheit
  startX = (SCREEN_W - 40 - totalWidth) / 2;
  valueSpr.drawString(valuePart, startX + valueWidth / 2, 28, 1);

  // Einheit zeichnen
  valueSpr.setFreeFont(&FreeSansBold12pt7b);
  unitWidth = valueSpr.textWidth(unitPart);
  valueSpr.drawString(unitPart, startX + valueWidth + 15 + unitWidth / 2, 32, 1);

  // Temperatur-Kreis
  if (strcmp(boxes[boxIndex].title, "Temperatur") == 0) {
    valueSpr.drawCircle(startX + valueWidth + 10, 23, 5, TFT_BLACK);
  }
  valueSpr.pushSprite(20, 80);
  valueSpr.deleteSprite();

  if (!detailGraphNeedsRedraw)
    return;
  detailGraphNeedsRedraw = false;

  // --- 2. Min/Max bestimmen ---
  float minValue = currentValue;
  float maxValue = currentValue;
  for (int i = 0; i < HISTORY_LENGTH; i++) {
    float val = historyBuffers[boxIndex][i];
    if (val != -999.0) {
      if (val < minValue) minValue = val;
      if (val > maxValue) maxValue = val;
    }
  }
  if (fabs(maxValue - minValue) < 0.1) {
    maxValue += 0.05;
    minValue -= 0.05;
  }

  // --- 3. Graph zeichnen ---
  const int LABEL_HEIGHT = 16;  // Platz für Labels
  TFT_eSprite graphSpr = TFT_eSprite(&tft);
  graphSpr.createSprite(GRAPH_WIDTH, GRAPH_HEIGHT);
  graphSpr.fillSprite(COLOR_BACKGROUND);
  graphSpr.setTextColor(TFT_BLACK);
  graphSpr.setFreeFont(&FreeSans9pt7b);

  // Graph-Rahmen
  graphSpr.drawRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT - 10, TFT_BLACK);

  // Graphlinien
  int prevX = -1, prevY = -1;
  int oldestIndex = (historyIndex[boxIndex] + 1) % HISTORY_LENGTH;
  for (int i = 0; i < HISTORY_LENGTH; i++) {
    int dataIndex = (oldestIndex + i) % HISTORY_LENGTH;
    float val = historyBuffers[boxIndex][dataIndex];
    if (val == -999.0) {
      prevX = -1;
      prevY = -1;
      continue;
    }
    int y = map((long)(val * 100), (long)(minValue * 100), (long)(maxValue * 100), GRAPH_HEIGHT - 11, 1);
    int x = map(i, 0, HISTORY_LENGTH - 1, 1, GRAPH_WIDTH - 2);

    if (prevX != -1)
      graphSpr.drawLine(prevX, prevY, x, y, GRAPH_COLOR);

    prevX = x;
    prevY = y;
  }

  graphSpr.pushSprite((SCREEN_W - GRAPH_WIDTH) / 2, 150);
  graphSpr.deleteSprite();

  // Labels unterhalb des Graphen
  tft.setTextDatum(BL_DATUM);
  tft.setTextColor(TFT_BLACK);
  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Letzte 24 Stunden", (SCREEN_W - GRAPH_WIDTH) / 2, 150 + GRAPH_HEIGHT + 8, 1);

  tft.setTextDatum(BR_DATUM);
  tft.drawString("Jetzt", (SCREEN_W + GRAPH_WIDTH) / 2, 150 + GRAPH_HEIGHT + 8, 1);

  // --- 4. Min/Max Text unten ---
  TFT_eSprite minMaxSpr = TFT_eSprite(&tft);
  minMaxSpr.createSprite(SCREEN_W - 40, 30);
  minMaxSpr.fillSprite(COLOR_BACKGROUND);
  minMaxSpr.setTextDatum(ML_DATUM);
  minMaxSpr.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  minMaxSpr.setFreeFont(&FreeSans9pt7b);

  char minStr[80], maxStr[80];
  sprintf(minStr, "Minimum (24h): %.*f %s", boxes[boxIndex].decimals, minValue, boxes[boxIndex].unit);
  sprintf(maxStr, "Maximum (24h): %.*f %s", boxes[boxIndex].decimals, maxValue, boxes[boxIndex].unit);

  minMaxSpr.drawString(minStr, 0, 15, 1);
  minMaxSpr.setTextDatum(MR_DATUM);
  minMaxSpr.drawString(maxStr, SCREEN_W - 40, 15, 1);

  minMaxSpr.pushSprite(20, 420);
  minMaxSpr.deleteSprite();
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

  tft.drawString(title, SCREEN_W / 2, 40, 1);

  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextColor(TFT_DARKGREY, COLOR_BACKGROUND);
  tft.drawString("Tippen, um zur Hauptseite zu gelangen", SCREEN_W / 2, SCREEN_H - 20, 1);
}

/**
 * @brief Update history buffer for a box
 * @param boxIndex Index of box
 * @param newValue New sensor value to add to history
 */
void updateHistory(int boxIndex, float newValue) {
  // Speichere neuen Wert an aktueller Position
  historyBuffers[boxIndex][historyIndex[boxIndex]] = newValue;

  // Index weiterschalten
  historyIndex[boxIndex]++;
  if (historyIndex[boxIndex] >= HISTORY_LENGTH) {
    historyIndex[boxIndex] = 0;  // Ringpuffer
  }
}

/**
 * @brief Initialize all history buffers with the invalid marker value.
 */
void initHistory() {
  for (int i = 0; i < NUM_BOXES; i++) {
    for (int j = 0; j < HISTORY_LENGTH; j++) {
      historyBuffers[i][j] = -999.0;
    }
  }
}
