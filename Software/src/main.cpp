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

TFT_eSPI tft = TFT_eSPI(); ///< TFT display object
TFT_Touch touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT); ///< Touchscreen object

extern Box boxes[]; ///< Array of display boxes
int currentPage = 0;   ///< Current page: 0 = main page, 1 = detail page
int selectedBox = -1;  ///< Index of the selected box, -1 = no box selected
bool touchReleased = true; ///< Helper variable for touch debounce
float lastDetailValue = -9999;  ///< Last detail value, used to force redraw

Adafruit_BME680 bme; ///< BME680 sensor
Adafruit_LTR390 ltr; ///< LTR390 sensor (UV)
Adafruit_VCNL4040 vcnl; ///< VCNL4040 sensor (ambient light & proximity)

/**
 * @brief Setup function, initializes hardware, sensors, and display
 */
void setup() {
  Serial.begin(115200);

  pinMode(LED_PWM, OUTPUT); ///< PWM pin for backlight
  analogWrite(LED_PWM, 255);  ///< Turn backlight fully on

#ifdef REAL_SENSORS
  Wire.begin(SDA, SCL); ///< Initialize I2C

  if (!bme.begin())
    Serial.println("BME688 not found");
  if (!vcnl.begin())
    Serial.println("VCNL4040 not found");
  if (!ltr.begin())
    Serial.println("LTR390 not found");
#endif

  configureSensors(); ///< Configure sensors

  tft.begin();
  tft.setRotation(1);

  touch.setRotation(1);
  touch.setCal(XMIN, XMAX, YMIN, YMAX, SCREEN_W, SCREEN_H, false); ///< Calibrate touch

  tft.fillScreen(COLOR_BACKGROUND); ///< Set background color

  layoutBoxes(); ///< Layout the display boxes
  drawLogo(); ///< Draw the logo
  for (int i = 0; i < NUM_BOXES; i++) {
    drawBox(i); ///< Draw each box
  }
}

/**
 * @brief Main loop function, updates sensor values, display, and handles touch input
 */
void loop() {
  updateValues(); ///< Update sensor or dummy values

  if (currentPage == 0) {
    ///< Main page – update all box values
    for (int i = 0; i < NUM_BOXES; i++) {
      updateValue(i); ///< Update individual box value
    }

    ///< Handle touch input for box selection
    if (touch.Pressed() && touchReleased) {
      touchReleased = false;
      int tx = touch.X();
      int ty = touch.Y();

      for (int i = 0; i < NUM_BOXES; i++) {
        if (tx > boxes[i].x && tx < boxes[i].x + boxes[i].w &&
            ty > boxes[i].y && ty < boxes[i].y + boxes[i].h) {
          selectedBox = i;
          currentPage = 1;
          lastDetailValue = -9999; ///< Reset last value to force redraw
          drawDetailPageTitle(selectedBox); ///< Draw title immediately
          break;
        }
      }
    }
    if (!touch.Pressed())
      touchReleased = true;
  } else if (currentPage == 1 && selectedBox >= 0) {
    ///< Detail page – update detail value with sprite
    drawDetailPageWithSprite(selectedBox);

    ///< Handle touch to go back
    if (touch.Pressed() && touchReleased) {
      touchReleased = false;
      currentPage = 0;
      selectedBox = -1;
      tft.fillScreen(COLOR_BACKGROUND); ///< Clear screen
      layoutBoxes();
      drawLogo();
      for (int i = 0; i < NUM_BOXES; i++)
        drawBox(i);
    }
    if (!touch.Pressed())
      touchReleased = true;
  }
}
