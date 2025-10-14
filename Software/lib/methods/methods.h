#ifndef METHODS_H
#define METHODS_H

#include <Arduino.h>
#include <Adafruit_BME680.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_VCNL4040.h>
#include <config.h>
#include <logo.h>

// Box structure
struct Box {
  const char* title;
  float* value;
  const char* unit;
  int x, y, w, h;
  int decimals;
};

extern Box boxes[NUM_BOXES];

void layoutBoxes();
void drawLogo();
void drawBox(int);
void updateValues();
void updateValue(int);
void drawDetailPage(int);
void drawDetailPageWithSprite(int);
void drawDetailPageTitle(int);

#endif // METHODS_H