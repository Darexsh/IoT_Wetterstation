/**
 * @file config.h
 * @brief Configuration and pin definitions for TFT display, touch, sensors, and layout
 *
 * This file contains all configuration constants and pin assignments for:
 * - TFT display
 * - Touchscreen
 * - Sensor modules (BME680, LTR390, VCNL4040)
 * - Layout and positioning of boxes
 * - Display backlight and brightness control
 *
 * The settings here control both hardware connections and UI layout parameters.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>

/// I2C pins for sensor communication
#define SDA 38
#define SCL 37

/// Touchscreen pins
#define TOUCH_DIN 7
#define TOUCH_DOUT 6
#define TOUCH_CS 15
#define TOUCH_CLK 16
#define TOUCH_IRQ 5

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
#define SCREEN_WIDTH 800   ///< Screen width
#define SCREEN_HEIGHT 480  ///< Screen height

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
#define COLOR_BACKGROUND 0xAD55  ///< Light gray background color
#define BOX_COLOR TFT_WHITE      ///< Box background color
#define TITLE_COLOR TFT_BLACK    ///< Box title text color
#define VALUE_COLOR TFT_BLACK    ///< Box value text color

/// Touchscreen calibration values
#define XMIN 610   ///< Minimum X value from touchscreen
#define XMAX 3190  ///< Maximum X value
#define YMIN 1070  ///< Minimum Y value
#define YMAX 3180  ///< Maximum Y value

/// BME688 sensor heater settings
#define HEATER_TEMP 320      ///< Heater temperature in Celsius
#define HEATER_DURATION 150  ///< Heater duration in milliseconds

/// Proximity sensor threshold for detecting a hand near the display
#define HAND_NEAR_TRESHOLD 500

/// PWM brightness control for display backlight
#define MAX_AMBIENT_LIGHT 5000  ///< Maximum ambient light considered
#define MIN_PWM 50              ///< Minimum PWM value for backlight
#define MAX_PWM 200             ///< Maximum PWM value for backlight

/// Graph display settings
#define FAST_UPDATE_INTERVAL 500        ///< Fast update interval in milliseconds
#define HISTORY_UPDATE_INTERVAL 120000  ///< History update interval (2 minutes) in milliseconds
#define HISTORY_LENGTH 720              ///< 24 hours of data at 2-minute intervals (24h * 60min / 2min)
#define GRAPH_HEIGHT 210                ///< Height of graph area in pixels
#define GRAPH_WIDTH 720                 ///< Width of graph area in pixels
#define GRAPH_COLOR TFT_RED             ///< Color for graph lines

#endif  // CONFIG_H
