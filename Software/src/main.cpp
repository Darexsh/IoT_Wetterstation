/**
 * @file main.cpp
 * @brief Main file for the environmental sensor dashboard
 *
 * Reads data from BME680, LTR390, and VCNL4040 sensors and displays
 * values on a TFT screen. Handles touch input to switch between main
 * and detail pages. Implements automatic display brightness adjustment
 * based on ambient light and hand proximity.
 */

#include <Adafruit_BME680.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_VCNL4040.h>
#include <Arduino.h>
#include <TFT_Touch.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <config.h>
#include <logo.h>
#include <methods.h>

/// TFT display instance
TFT_eSPI tft = TFT_eSPI();

/// Touch controller instance
TFT_Touch touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT);

/// External array of boxes for main screen
extern Box boxes[];

/// Current page: 0 = main, 1 = detail
int currentPage = 0;

/// Index of selected box (-1 if none)
int selectedBox = -1;

/// Flag to detect touch release
bool touchReleased = true;

/// Last value drawn on detail page to avoid flicker
float lastDetailValue = -9999;

/// Sensor availability flags
bool bme_ok = false;
bool vcnl_ok = false;
bool ltr_ok = false;

/// Sensor instances
Adafruit_BME680 bme;
Adafruit_LTR390 ltr;
Adafruit_VCNL4040 vcnl;

/**
 * @brief Arduino setup function
 *
 * Initializes serial, sensors, display, touch, and draws initial layout.
 */
void setup() {
  Serial.begin(115200);

  ///< Configure backlight pin and set full brightness initially
  pinMode(LED_PWM, OUTPUT);
  analogWrite(LED_PWM, 255);  ///< Full backlight on

#ifdef REAL_SENSORS
  Wire.begin(SDA, SCL);

  bme_ok = bme.begin(0x76);
  if (!bme_ok) Serial.println("BME688 not found");

  vcnl_ok = vcnl.begin();
  if (!vcnl_ok) Serial.println("VCNL4040 not found");

  ltr_ok = ltr.begin();  // Default address 0x53
  if (!ltr_ok) Serial.println("LTR390 not found");

  if (bme_ok || vcnl_ok || ltr_ok) {
    configureSensors(bme_ok, vcnl_ok, ltr_ok);
  } else {
    Serial.println("⚠️ No sensors initialized, running demo mode");
  }
#endif

  ///< Initialize TFT display
  tft.begin();
  tft.setRotation(1);

  ///< Initialize touch controller
  touch.setRotation(1);
  touch.setCal(XMIN, XMAX, YMIN, YMAX, SCREEN_W, SCREEN_H, false);

  ///< Clear screen and draw initial layout
  tft.fillScreen(COLOR_BACKGROUND);
  layoutBoxes();
  drawLogo();
  for (int i = 0; i < NUM_BOXES; i++) {
    drawBox(i);
  }
}

/**
 * @brief Arduino main loop
 *
 * Updates sensor values, draws main and detail pages, handles touch
 * input, and manages display brightness based on ambient light and
 * hand proximity.
 */
void loop() {
  ///< Update all sensor values
  updateValues();

  if (currentPage == 0) {
    ///< Main page – update all boxes
    for (int i = 0; i < NUM_BOXES; i++) {
      updateValue(i);
    }

    ///< Detect touch to select box
    if (touch.Pressed() && touchReleased) {
      touchReleased = false;
      int tx = touch.X();
      int ty = touch.Y();

      ///< Check which box is touched
      for (int i = 0; i < NUM_BOXES; i++) {
        if (tx > boxes[i].x && tx < boxes[i].x + boxes[i].w &&
            ty > boxes[i].y && ty < boxes[i].y + boxes[i].h) {
          selectedBox = i;
          currentPage = 1;
          lastDetailValue = -9999;           ///< Force redraw
          drawDetailPageTitle(selectedBox);  ///< Draw title immediately
          break;
        }
      }
    }
    if (!touch.Pressed())
      touchReleased = true;
  } else if (currentPage == 1 && selectedBox >= 0) {
    ///< Detail page – update value using sprite
    drawDetailPageWithSprite(selectedBox);

    ///< Detect touch to return to main page
    if (touch.Pressed() && touchReleased) {
      touchReleased = false;
      currentPage = 0;
      selectedBox = -1;

      ///< Redraw main screen layout
      tft.fillScreen(COLOR_BACKGROUND);
      layoutBoxes();
      drawLogo();
      for (int i = 0; i < NUM_BOXES; i++) drawBox(i);
    }
    if (!touch.Pressed())
      touchReleased = true;
  }
}
