/**
 * @file methods.h
 * @brief Functions for managing display boxes, sensor updates, and detail pages
 *
 * Contains:
 * - Box structure definition
 * - Sensor configuration
 * - Functions for layout and drawing of boxes
 * - Updating sensor values and the display
 * - Drawing detail pages for individual sensors
 */

#ifndef METHODS_H
#define METHODS_H

#include <Adafruit_BME680.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_VCNL4040.h>
#include <Arduino.h>
#include <config.h>
#include <logo.h>

/**
 * @brief Structure representing a single box on the display
 */
struct Box {
  const char* title;  ///< Box title
  float* value;       ///< Pointer to sensor value displayed
  const char* unit;   ///< Unit of measurement
  int x, y;           ///< Position on screen
  int w, h;           ///< Width and height of box
  int decimals;       ///< Number of decimals for display
};

extern Box boxes[NUM_BOXES];  ///< Array of boxes on screen

/**
 * @brief Configure all sensors with default settings
 */
void configureSensors();

/**
 * @brief Layout boxes on screen in grid
 */
void layoutBoxes();

/**
 * @brief Draw the center logo
 */
void drawLogo();

/**
 * @brief Draw a single box
 * @param i Index of box in boxes array
 */
void drawBox(int i);

/**
 * @brief Update all sensor values
 */
void updateValues();

/**
 * @brief Update a single box value if changed
 * @param i Index of box
 */
void updateValue(int i);

/**
 * @brief Draw the detail page for a box
 * @param boxIndex Index of box
 */
void drawDetailPage(int boxIndex);

/**
 * @brief Draw the detail page using a sprite for the value
 * @param boxIndex Index of box
 */
void drawDetailPageWithSprite(int boxIndex);

/**
 * @brief Draw the detail page title
 * @param boxIndex Index of box
 */
void drawDetailPageTitle(int boxIndex);

#endif  // METHODS_H