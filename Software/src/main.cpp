#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <Adafruit_BME680.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_VCNL4040.h>
#include <config.h>
#include <methods.h>
#include <logo.h>

TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT);
extern Box boxes[];

Adafruit_BME680 bme;
Adafruit_LTR390 ltr;
Adafruit_VCNL4040 vcnl;

void setup() {
  Serial.begin(115200);

  #ifdef REAL_SENSORS
    Wire.begin(SDA, SCL);
    if (!bme.begin()) Serial.println("BME688 nicht gefunden");
    if (!vcnl.begin()) Serial.println("VCNL4040 nicht gefunden");
    if (!ltr.begin()) Serial.println("LTR390 nicht gefunden");
  #endif

  tft.begin();
  tft.setRotation(1);

  touch.setRotation(1);
  touch.setCal(XMIN, XMAX, YMIN, YMAX, SCREEN_W, SCREEN_H, false);
  
  tft.fillScreen(COLOR_BACKGROUND);

  layoutBoxes();
  drawLogo();
  for (int i = 0; i < NUM_BOXES; i++) {
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