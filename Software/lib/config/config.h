#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>

// Uncomment the following line to use real sensors instead of simulated data
// #define REAL_SENSORS

// Pin definitions
#define SDA 38
#define SCL 37

#define TOUCH_CS 15
#define TOUCH_CLK 16
#define TOUCH_DIN 7 
#define TOUCH_DOUT 6

// Screen dimensions
#define SCREEN_W 800 // Screen width in pixels
#define SCREEN_H 480 // Screen height in pixels

// Layout constants
#define MARGIN 20       // Margin around the screen
#define SPACING_X 20    // Horizontal spacing
#define SPACING_Y 20    // Vertical spacing
#define NUM_COLS 3      // Adjusted to fit 3 columns
#define NUM_ROWS 3      // Adjusted to fit 3 rows
#define LOGO_WIDTH 120      // Logo width
#define LOGO_HEIGHT 120      // Logo height
#define BOX_RADIUS 10   // Corner radius for rounded rectangles
#define NUM_BOXES 8     // Number of boxes

// Layout colors
#define COLOR_BACKGROUND 0xAD55 // Light gray background
#define BOX_COLOR TFT_WHITE       // White boxes
#define TITLE_COLOR TFT_BLACK     // Black titles
#define VALUE_COLOR TFT_BLACK     // Black values

// Touchscreen calibration values
#define XMIN 610
#define XMAX 3190
#define YMIN 1070
#define YMAX 3180

#endif // CONFIG_H