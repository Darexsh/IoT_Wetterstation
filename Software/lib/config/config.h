#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>

// Uncomment the following line to use real sensors instead of simulated data
#define REAL_SENSORS

// I2C pins
#define SDA 38
#define SCL 37

// Touchscreen pins
#define TOUCH_DIN 7
#define TOUCH_DOUT 6
#define TOUCH_CS 15
#define TOUCH_CLK 16
#define TOUCH_IRQ 5

// TFT pins
#define TFT_RD 35
#define TFT_WR 21
#define TFT_RS 14
#define TFT_REST 1
#define TFT_CS 2

// TFT data pins
#define TFT_D0 13
#define TFT_D1 12
#define TFT_D2 11
#define TFT_D3 10
#define TFT_D4 9
#define TFT_D5 8
#define TFT_D6 18
#define TFT_D7 17

// LED pin (for backlight control)
#define LED_PWM 4

// Screen dimensions
#define SCREEN_W 800  // Screen width in pixels
#define SCREEN_H 480  // Screen height in pixels

// Layout constants
#define MARGIN 20        // Margin around the screen
#define SPACING_X 20     // Horizontal spacing
#define SPACING_Y 20     // Vertical spacing
#define NUM_COLS 3       // Adjusted to fit 3 columns
#define NUM_ROWS 3       // Adjusted to fit 3 rows
#define LOGO_WIDTH 120   // Logo width
#define LOGO_HEIGHT 120  // Logo height
#define BOX_RADIUS 10    // Corner radius for rounded rectangles
#define NUM_BOXES 8      // Number of boxes

// Layout colors
#define COLOR_BACKGROUND 0xAD55  // Light gray background
#define BOX_COLOR TFT_WHITE      // White boxes
#define TITLE_COLOR TFT_BLACK    // Black titles
#define VALUE_COLOR TFT_BLACK    // Black values

// Touchscreen calibration values
#define XMIN 610
#define XMAX 3190
#define YMIN 1070
#define YMAX 3180

// BME688 settings
#define HEATER_TEMP 320
#define HEATER_DURATION 150

// Proximity threshold to detect hand near the sensor
#define HAND_NEAR_TRESHOLD 500

// PWM brightness control
#define MAX_AMBIENT_LIGHT 5000
#define MIN_PWM 50
#define MAX_PWM 200

#endif  // CONFIG_H