/**
 * @file main.cpp
 * @brief Main program for ESP32-S3 weather station with TFT display and touch
 *
 * This program initializes the TFT display, touch controller, and sensors,
 * and manages the main loop to update sensor readings and handle user
 * interactions.
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
extern bool detailGraphNeedsRedraw;

/// Sensor availability flags
bool bme_ok = false;
bool vcnl_ok = false;
bool ltr_ok = false;

/// Sensor instances
Adafruit_BME680 bme;
Adafruit_LTR390 ltr;
Adafruit_VCNL4040 vcnl;

/**
 * @brief Setup function to initialize hardware and sensors
 */
void setup() {
  Serial.begin(115200);

  ///< Configure backlight pin and set full brightness initially
  pinMode(LED_PWM, OUTPUT);
  analogWrite(LED_PWM, 100);  ///< Full backlight on

  Wire.begin(SDA, SCL);

  bme_ok = bme.begin(0x76);
  if (!bme_ok) Serial.println("BME688 not found");

  vcnl_ok = vcnl.begin();
  if (!vcnl_ok) Serial.println("VCNL4040 not found");

  ltr_ok = ltr.begin();
  if (!ltr_ok) Serial.println("LTR390 not found");

  if (bme_ok || vcnl_ok || ltr_ok) {
    configureSensors(bme_ok, vcnl_ok, ltr_ok);
  } else {
    Serial.println("No sensors initialized, running demo mode");
  }

  ///< Initialize history buffers and read initial values
  initHistory();

  ///< Perform initial sensor reading
  updateValues();

  for (int i = 0; i < NUM_BOXES; i++) updateHistory(i, *boxes[i].value);
  detailGraphNeedsRedraw = true;  // Force initial graph draw

  tft.begin();                                                               ///< Initialize TFT display
  tft.setRotation(1);                                                        ///< Set display rotation
  touch.setRotation(1);                                                      ///< Initialize touch controller
  touch.setCal(XMIN, XMAX, YMIN, YMAX, SCREEN_WIDTH, SCREEN_HEIGHT, false);  ///< Calibrate touch controller
  tft.fillScreen(COLOR_BACKGROUND);                                          ///< Clear screen and draw initial layout
  layoutBoxes();                                                             ///< Layout boxes on screen
  drawLogo();                                                                ///< Draw logo in center

  for (int i = 0; i < NUM_BOXES; i++) drawBox(i);  ///< Draw all boxes
}

/**
 * @brief Main loop to update sensor readings and handle user interaction
 */
void loop() {
  updateValues();  ///< Update all sensor values

  if (currentPage == 0) {
    ///< Main page – update all boxes
    for (int i = 0; i < NUM_BOXES; i++) updateValue(i);

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
          lastDetailValue = -9999;
          detailGraphNeedsRedraw = true;
          drawDetailPageTitle(selectedBox);
          break;
        }
      }
    }
    if (!touch.Pressed()) touchReleased = true;
  } else if (currentPage == 1 && selectedBox >= 0) {
    drawDetailPageWithSprite(selectedBox);  ///< Detail page – update value using sprite

    ///< Detect touch to return to main page
    if (touch.Pressed() && touchReleased) {
      touchReleased = false;
      currentPage = 0;
      selectedBox = -1;
      detailGraphNeedsRedraw = false;

      ///< Redraw main screen layout
      tft.fillScreen(COLOR_BACKGROUND);
      layoutBoxes();
      drawLogo();
      for (int i = 0; i < NUM_BOXES; i++) drawBox(i);
    }

    if (!touch.Pressed()) touchReleased = true;
  }
}