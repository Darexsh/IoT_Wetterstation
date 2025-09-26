#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include "TFT_Touch.h"
#include "logo.h"

#include <Adafruit_BME680.h>
#include <Adafruit_VCNL4040.h>
#include <Adafruit_LTR390.h>

// #define REAL_SENSORS

// Display and Touch
TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch(15, 16, 7, 6);

#define SCREEN_W 800
#define SCREEN_H 480

// Layout constants
const int MARGIN = 20;
const int SPACING_X = 20;
const int SPACING_Y = 20;
const int NUM_COLS = 3;
const int NUM_ROWS = 3;
const int LOGO_WIDTH = 120;
const int LOGO_HEIGHT = 120;
const int BOX_RADIUS = 10;

// Sensor objects
#ifdef REAL_SENSORS
#define SDA 38
#define SCL 37
Adafruit_BME680 bme;
Adafruit_VCNL4040 vcnl;
Adafruit_LTR390 uvSensor;
#endif

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

// Colors
uint32_t bgColor = 0xAAAAAA;
uint32_t boxColor = TFT_WHITE;
uint32_t titleColor = TFT_BLACK;
uint32_t valueColor = TFT_BLACK;

// Box structure
struct Box {
  const char* title;
  float* value;
  const char* unit;
  int x, y, w, h;
  int decimals;
};

// Box definitions
Box boxes[] = {
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
  int numBoxes = sizeof(boxes) / sizeof(boxes[0]);
  int boxW = (SCREEN_W - 2*MARGIN - (NUM_COLS-1)*SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - 2*MARGIN - (NUM_ROWS-1)*SPACING_Y) / NUM_ROWS;

  int boxIndex = 0;
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      if (!(row == 1 && col == 1)) { // Skip center for logo
        boxes[boxIndex].x = MARGIN + col * (boxW + SPACING_X);
        boxes[boxIndex].y = MARGIN + row * (boxH + SPACING_Y);
        boxes[boxIndex].w = boxW;
        boxes[boxIndex].h = boxH;
        boxIndex++;
        if (boxIndex >= numBoxes) return;
      }
    }
  }
}

// Draw logo in center box
void drawLogo() {
  int boxW = (SCREEN_W - 2*MARGIN - (NUM_COLS-1)*SPACING_X) / NUM_COLS;
  int boxH = (SCREEN_H - 2*MARGIN - (NUM_ROWS-1)*SPACING_Y) / NUM_ROWS;
  int x = MARGIN + 1 * (boxW + SPACING_X);
  int y = MARGIN + 1 * (boxH + SPACING_Y);

  int xCenter = x + (boxW - LOGO_WIDTH) / 2;
  int yCenter = y + (boxH - LOGO_HEIGHT) / 2;

  tft.pushImage(xCenter, yCenter, LOGO_WIDTH, LOGO_HEIGHT, logo);
}

// Draw boxes
void drawBox(int i) {
  tft.fillRoundRect(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, BOX_RADIUS, boxColor);
  tft.setTextColor(titleColor, boxColor);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawString(boxes[i].title, boxes[i].x + boxes[i].w / 2, boxes[i].y + 15, 1);
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
                    boxes[i].w - 20, 40, 5, boxColor);

  // Draw new value centered
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(valueColor, boxColor);
  int textW = tft.textWidth(fullText, 4);
  int xCenter = boxes[i].x + boxes[i].w / 2 - textW / 2;
  int yCenter = boxes[i].y + boxes[i].h / 2 - 10;

  tft.drawString(fullText, xCenter, yCenter, 4);

  // Draw degree symbol for temperature
  if (strcmp(boxes[i].title, "Temperatur") == 0) {
    int cWidth = tft.textWidth("C", 4);
    tft.drawCircle(xCenter + textW - cWidth - 2, yCenter, 3, valueColor);
    tft.drawCircle(xCenter + textW - cWidth - 2, yCenter, 2, valueColor);
  }
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

  // UTR390 UV
  uvValue      = uvSensor.readUVS(); 
#else
  // Dummywerte
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


void setup() {
  Serial.begin(115200);

  #ifdef REAL_SENSORS
    Wire.begin(SDA, SCL);
      if (!bme.begin()) Serial.println("BME688 nicht gefunden!");
    if (!vcnl.begin()) Serial.println("VCNL4040 nicht gefunden!");
    if (!uvSensor.begin()) Serial.println("UTR390 nicht gefunden!");
  #endif

  tft.begin();
  tft.setRotation(1);

  touch.setRotation(1);
  touch.setCal(610, 3190, 1070, 3180, SCREEN_W, SCREEN_H, false);

  tft.fillScreen(bgColor);
  layoutBoxes();
  drawLogo();
  for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++) {
    drawBox(i);
  }
}

void loop() {
  updateValues();
  for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++) {
    updateValue(i);
  }

  if (touch.Pressed()) {
    Serial.printf("Touch X:%d Y:%d\n", touch.X(), touch.Y());
    delay(150);
  }

  delay(500);
}
