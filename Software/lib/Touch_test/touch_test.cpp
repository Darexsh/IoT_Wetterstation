#include <TFT_eSPI.h>
#include "TFT_Touch.h"

TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch(15,16,7,6);

#define SCREEN_W 800
#define SCREEN_H 480
#define X_MIN 611
#define X_MAX 3451
#define Y_MIN 949
#define Y_MAX 3556

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  touch.setRotation(1);
  touch.setCal(X_MIN, X_MAX, Y_MIN, Y_MAX, SCREEN_W, SCREEN_H, false);
}

void loop() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);

  if (touch.Pressed()) {
    uint16_t x = touch.X();
    uint16_t y = touch.Y();

    tft.setCursor(20, 20);
    tft.printf("Touch X: %d\n", x);
    tft.setCursor(20, 60);
    tft.printf("Touch Y: %d\n", y);

    Serial.printf("Touch X:%d Y:%d\n", x, y);
  }

  delay(100);
}