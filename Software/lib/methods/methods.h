#ifndef METHODS_H
#define METHODS_H

#include <Adafruit_BME680.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_VCNL4040.h>
#include <Arduino.h>
#include <config.h>
#include <logo.h>

/**
 * @brief Structure representing a display box
 */
struct Box {
  const char* title;  ///< Title of the box
  float* value;       ///< Pointer to the value displayed in the box
  const char* unit;   ///< Unit of the value (e.g., "°C", "%")
  int x, y;           ///< Position of the box on screen
  int w, h;           ///< Width and height of the box
  int decimals;       ///< Number of decimal places to display
};

extern Box boxes[NUM_BOXES]; ///< Array of all boxes on the display

/**
 * @brief Configure sensors (BME680, LTR390, VCNL4040)
 */
void configureSensors();

/**
 * @brief Layout the boxes on the main display
 */
void layoutBoxes();

/**
 * @brief Draw the logo on the display
 */
void drawLogo();

/**
 * @brief Draw a single box
 * @param index Index of the box in the boxes array
 */
void drawBox(int index);

/**
 * @brief Update all sensor or dummy values
 */
void updateValues();

/**
 * @brief Update a single box's value on the display
 * @param index Index of the box in the boxes array
 */
void updateValue(int index);

/**
 * @brief Draw the detail page for a selected box
 * @param index Index of the box in the boxes array
 */
void drawDetailPage(int index);

/**
 * @brief Draw the detail page with a sprite (graphical representation)
 * @param index Index of the box in the boxes array
 */
void drawDetailPageWithSprite(int index);

/**
 * @brief Draw only the title of the detail page for a selected box
 * @param index Index of the box in the boxes array
 */
void drawDetailPageTitle(int index);

#endif  // METHODS_H
