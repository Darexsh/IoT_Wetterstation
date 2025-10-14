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
extern float lastDetailValue;

// Box definitions
Box boxes[NUM_BOXES] = {
    {"Temperatur", &tempValue, "C", 0, 0, 0, 0, 1},
    {"Luftfeuchtigkeit", &humidValue, "%", 0, 0, 0, 0, 1},
    {"Luftdruck", &pressureValue, "hPa", 0, 0, 0, 0, 0},
    {"Umgebungslicht", &ambientValue, "lux", 0, 0, 0, 0, 0},
    {"Weisses Licht", &whiteValue, "", 0, 0, 0, 0, 0},
    {"Gas", &gasValue, "", 0, 0, 0, 0, 0},
    {"UV Licht", &uvValue, "", 0, 0, 0, 0, 2},
    {"UV Index", &uvIndexValue, "", 0, 0, 0, 0, 1}};

// Calculate layout of boxes
void layoutBoxes()
{
  int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;

  int boxIndex = 0;
  for (int row = 0; row < NUM_ROWS; row++)
  {
    for (int col = 0; col < NUM_COLS; col++)
    {
      if (!(row == 1 && col == 1))
      { // Skip center for logo
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
void drawLogo()
{
  int boxW = (SCREEN_W - (2 * MARGIN) - (NUM_COLS - 1) * SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - (2 * MARGIN) - (NUM_ROWS - 1) * SPACING_Y) / NUM_ROWS;
  int x = MARGIN + 1 * (boxW + SPACING_X);
  int y = MARGIN + 1 * (boxH + SPACING_Y);

  int xCenter = x + (boxW - LOGO_WIDTH) / 2;
  int yCenter = y + (boxH - LOGO_HEIGHT) / 2;

  tft.pushImage(xCenter, yCenter, LOGO_WIDTH, LOGO_HEIGHT, logo);
}

// Draw boxes
void drawBox(int i)
{
  tft.fillRoundRect(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, BOX_RADIUS, BOX_COLOR);
  tft.setTextColor(TITLE_COLOR, BOX_COLOR);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(boxes[i].title, boxes[i].x + boxes[i].w / 2, boxes[i].y + 15, 1);
}

// Update dummy values with random changes
void updateValues()
{
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500)
    return;
  lastUpdate = millis();
#ifdef REAL_SENSORS
  // BME688
  if (bme.performReading())
  {
    tempValue = bme.temperature;
    humidValue = bme.humidity;
    pressureValue = bme.pressure / 100.0F;
    gasValue = bme.gas_resistance / 1000.0F;
  }

  // VCNL4040
  ambientValue = vcnl.getAmbientLight();
  whiteValue = vcnl.getWhiteLight();
  proximityValue = vcnl.getProximity();

  // LTR390
  uvValue = ltr.readUVS();
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

// Update box value display
void updateValue(int i)
{
  static float lastValues[NUM_BOXES] = {0};
  float newVal = *(boxes[i].value);

  // Nur neu zeichnen, wenn sich der Wert geändert hat
  if (abs(newVal - lastValues[i]) < 0.001)
    return;
  lastValues[i] = newVal;

  // Text vorbereiten
  char buf[20];
  sprintf(buf, "%.*f", boxes[i].decimals, newVal);

  String fullText = String(buf);
  if (strcmp(boxes[i].title, "Temperatur") == 0)
  {
    fullText += "  C";
  }
  else
  {
    fullText += " " + String(boxes[i].unit);
  }

  // Sprite vorbereiten (genau so groß wie Box-Inhalt)
  TFT_eSprite spr = TFT_eSprite(&tft);
  spr.createSprite(boxes[i].w - 20, 40);
  spr.fillSprite(BOX_COLOR);
  spr.setTextColor(VALUE_COLOR, BOX_COLOR);
  spr.setTextDatum(TL_DATUM);

  // Textbreite berechnen für zentrierte Position
  int textW = spr.textWidth(fullText, 4);
  int xCenter = (boxes[i].w - 20) / 2 - textW / 2;
  int yCenter = 10;

  // Text zeichnen
  spr.drawString(fullText, xCenter, yCenter, 4);

  // Gradzeichen wie im Original
  if (strcmp(boxes[i].title, "Temperatur") == 0)
  {
    int cWidth = spr.textWidth("C", 4);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
    spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
  }

  // Sprite an die richtige Stelle im Display kopieren
  spr.pushSprite(boxes[i].x + 10, boxes[i].y + boxes[i].h / 2 - 20);
  spr.deleteSprite();
}

void drawDetailPage(int boxIndex)
{
  extern TFT_eSPI tft;

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  tft.setFreeFont(&FreeSansBold18pt7b);

  String title = "Details: " + String(boxes[boxIndex].title);
  String valueStr = String(*boxes[boxIndex].value, boxes[boxIndex].decimals) + " " + boxes[boxIndex].unit;

  tft.drawString(title, SCREEN_W / 2, SCREEN_H / 2 - 40, 1);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(valueStr, SCREEN_W / 2, SCREEN_H / 2 + 20, 1);

  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Tippe zum Zurueckkehren", SCREEN_W / 2, SCREEN_H - 40, 1);
}

void drawDetailPageWithSprite(int boxIndex)
{
    float currentValue = *boxes[boxIndex].value;

    if (abs(currentValue - lastDetailValue) < 0.001)
        return;
    lastDetailValue = currentValue;

    // --- Wertbereich-Sprite ---
    char buf[20];
    sprintf(buf, "%.*f", boxes[boxIndex].decimals, currentValue);

    TFT_eSprite spr = TFT_eSprite(&tft);
    spr.createSprite(300, 60);
    spr.fillSprite(COLOR_BACKGROUND);

    spr.setTextDatum(TL_DATUM);
    spr.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
    spr.setFreeFont(&FreeSansBold12pt7b);

    String fullText = String(buf);
    if (strcmp(boxes[boxIndex].title, "Temperatur") == 0)
    {
        fullText += "  C"; // Gradzeichen wie auf der Hauptseite
    }
    else
    {
        fullText += " " + String(boxes[boxIndex].unit);
    }

    int textW = spr.textWidth(fullText, 4);
    int xCenter = (300 - textW) / 2; // Text zentrieren in Sprite
    int yCenter = 10;

    spr.drawString(fullText, xCenter, yCenter, 4);

    // --- Temperaturkreis korrekt positionieren ---
    if (strcmp(boxes[boxIndex].title, "Temperatur") == 0)
    {
        int cWidth = spr.textWidth("C", 4);
        // Kreis direkt links vom „C“, wie auf der Hauptseite
        spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, VALUE_COLOR);
        spr.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, VALUE_COLOR);
    }

    spr.pushSprite((SCREEN_W - 300) / 2, SCREEN_H / 2 + 10);
    spr.deleteSprite();
}


void drawDetailPageTitle(int boxIndex)
{
  tft.fillScreen(COLOR_BACKGROUND);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COLOR_BACKGROUND);
  tft.setFreeFont(&FreeSansBold18pt7b);
  String title = "Details: " + String(boxes[boxIndex].title);
  tft.drawString(title, SCREEN_W / 2, SCREEN_H / 2 - 40, 1);

  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Tippe zum Zurueckkehren", SCREEN_W / 2, SCREEN_H - 40, 1);
}
