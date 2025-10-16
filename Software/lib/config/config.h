/**
 * @file config.h
 * @brief Configuration and pin definitions for TFT display, touch, sensors, and layout
 *
 * Contains pin assignments, screen dimensions, layout parameters,
 * sensor settings, and backlight control parameters.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>

/// Uncomment to use real sensors instead of simulated data
#define REAL_SENSORS

/// I2C pins for sensor communication
#define SDA 38 ///< I2C data line
#define SCL 37 ///< I2C clock line

/// Touchscreen pins
#define TOUCH_DIN 7   ///< SPI DIN
#define TOUCH_DOUT 6  ///< SPI DOUT
#define TOUCH_CS 15   ///< Chip select
#define TOUCH_CLK 16  ///< SPI clock
#define TOUCH_IRQ 5   ///< Interrupt pin (not used here)

/// TFT display pins
#define TFT_RD 35
#define TFT_WR 21
#define TFT_RS 14
#define TFT_REST 1
#define TFT_CS 2

/// TFT parallel data pins
#define TFT_D0 13
#define TFT_D1 12
#define TFT_D2 11
#define TFT_D3 10
#define TFT_D4 9
#define TFT_D5 8
#define TFT_D6 18
#define TFT_D7 17

/// LED pin for backlight control
#define LED_PWM 4

/// Screen dimensions in pixels
#define SCREEN_W 800 ///< Screen width
#define SCREEN_H 480 ///< Screen height

/// Layout constants for main screen boxes
#define MARGIN 20        ///< Margin around the screen
#define SPACING_X 20     ///< Horizontal spacing between boxes
#define SPACING_Y 20     ///< Vertical spacing between boxes
#define NUM_COLS 3       ///< Number of columns in grid
#define NUM_ROWS 3       ///< Number of rows in grid
#define LOGO_WIDTH 120   ///< Logo width in pixels
#define LOGO_HEIGHT 120  ///< Logo height in pixels
#define BOX_RADIUS 10    ///< Corner radius for rounded rectangles
#define NUM_BOXES 8      ///< Total number of boxes on main screen

/// Colors for layout
#define COLOR_BACKGROUND 0xAD55 ///< Light gray background color
#define BOX_COLOR TFT_WHITE     ///< Box background color
#define TITLE_COLOR TFT_BLACK   ///< Box title text color
#define VALUE_COLOR TFT_BLACK   ///< Box value text color

/// Touchscreen calibration values
#define XMIN 610  ///< Minimum X value from touchscreen
#define XMAX 3190 ///< Maximum X value
#define YMIN 1070 ///< Minimum Y value
#define YMAX 3180 ///< Maximum Y value

/// BME688 sensor heater settings
#define HEATER_TEMP 320     ///< Heater temperature in Celsius
#define HEATER_DURATION 150 ///< Heater duration in milliseconds

/// Proximity sensor threshold for detecting a hand near the display
#define HAND_NEAR_TRESHOLD 500

/// PWM brightness control for display backlight
#define MAX_AMBIENT_LIGHT 5000 ///< Maximum ambient light considered
#define MIN_PWM 50             ///< Minimum PWM value for backlight
#define MAX_PWM 200            ///< Maximum PWM value for backlight

#endif  // CONFIG_H
