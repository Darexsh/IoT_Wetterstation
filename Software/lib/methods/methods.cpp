#include <methods.h>

extern TFT_eSPI tft;
extern Adafruit_BME680 bme;
extern Adafruit_LTR390 ltr;
extern Adafruit_VCNL4040 vcnl;

// Dummy values
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

// Box definitions
Box boxes[NUM_BOXES] = {
  {"Temperatur", &tempValue, "C", 0,0,0,0,1},
  {"Luftfeuchtigkeit", &humidValue, "%", 0,0,0,0,1},
  {"Luftdruck", &pressureValue, "hPa", 0,0,0,0,0},
  {"Umgebungslicht", &ambientValue, "lux", 0,0,0,0,0},
  {"Weisses Licht", &whiteValue, "", 0,0,0,0,0},
  {"Gas", &gasValue, "", 0,0,0,0,0},
  {"UV Licht", &uvValue, "", 0,0,0,0,2},
  {"UV Index", &uvIndexValue, "", 0,0,0,0,1}
};

// Calculate layout of boxes
void layoutBoxes() {
    int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
    int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;

    int boxIndex = 0;
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            if (!(row == 1 && col == 1)) { // Skip center for logo
                boxes[boxIndex].x = MARGIN + col * (boxW + SPACING_X);
                boxes[boxIndex].y = MARGIN + row * (boxH + SPACING_Y);
                boxes[boxIndex].w = boxW;
                boxes[boxIndex].h = boxH;
                boxIndex++;
                if (boxIndex >= NUM_BOXES) return;
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

// Draw boxes
void drawBox(int i) {
    tft.fillRoundRect(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, BOX_RADIUS, BOX_COLOR);
    tft.setTextColor(TITLE_COLOR, BOX_COLOR);
    tft.setTextDatum(TC_DATUM);
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.drawString(boxes[i].title, boxes[i].x + boxes[i].w / 2, boxes[i].y + 15, 1);
}

// Update dummy values with random changes
void updateValues() {
#ifdef REAL_SENSORS
    // BME688
    if (bme.performReading()) {
        tempValue     = bme.temperature;
        humidValue    = bme.humidity;
        pressureValue = bme.pressure / 100.0F;
        gasValue      = bme.gas_resistance / 1000.0F;
    }

    // VCNL4040
    ambientValue   = vcnl.getAmbientLight();
    whiteValue     = vcnl.getWhiteLight();
    proximityValue = vcnl.getProximity();

    // LTR390
    uvValue = ltr.readUVS();
#else
    // Dummy values
    tempValue     = constrain(tempValue + random(-2,3)/10.0, -20, 50);
    humidValue    = constrain(humidValue + random(-5,6)/10.0, 0, 100);
    pressureValue = constrain(pressureValue + random(-1,2), 900, 1100);
    gasValue      = constrain(gasValue + random(-2,3), 0, 1000);
    distanceValue = constrain(distanceValue + random(-5,6), 0, 1000);
    ambientValue  = max(0.0f, ambientValue + random(-10,11));
    whiteValue    = max(0.0f, whiteValue + random(-10,11));
    uvValue       = max(0.0f, uvValue + random(-1,2)/20.0f);
    uvIndexValue  = max(0.0f, uvIndexValue + random(-1,2)/20.0f);
#endif
}

// Update box value display
void updateValue(int i) {
  char buf[20];
  sprintf(buf, "%.*f", boxes[i].decimals, *(boxes[i].value));

  String fullText = String(buf);
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    fullText += "  C";
  } else {
    fullText += " " + String(boxes[i].unit);
  }

  // Clear previous value area
  tft.fillRoundRect(boxes[i].x + 10, boxes[i].y + boxes[i].h/2 - 20,
                    boxes[i].w - 20, 40, 5, BOX_COLOR);

  // Draw new value centered
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(VALUE_COLOR, BOX_COLOR);
  int textW = tft.textWidth(fullText, 4);
  int xCenter = boxes[i].x + boxes[i].w / 2 - textW / 2;
  int yCenter = boxes[i].y + boxes[i].h / 2 - 10;

  tft.drawString(fullText, xCenter, yCenter, 4);

  // Draw degree symbol for temperature
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    int cWidth = tft.textWidth("C", 4);
    tft.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
    tft.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
  }
}

