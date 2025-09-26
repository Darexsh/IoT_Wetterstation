#include <Arduino.h>
#include "TFT_eSPI.h"
#include "TFT_Touch.h"
#include "SPI.h"
#include "U8g2_for_TFT_eSPI.h"

#define HRES 800
#define VRES 480

TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI uFonts;
TFT_Touch touch(15, 16, 7, 6);

int X_Raw = 0, Y_Raw = 0;

// --- Startwerte Kalibrierung ---
int xminCal = 580;
int xmaxCal = 3320;
int yminCal = 930;
int ymaxCal = 3380;

void test(void);
void drawCross(int x, int y, unsigned int color);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  tft.init();
  tft.setRotation(1);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  touch.setRotation(1);
  touch.setCal(xminCal, xmaxCal, yminCal, ymaxCal, HRES, VRES, 0);

  tft.fillScreen(TFT_BLACK);
  test();

  Serial.println("Kalibrierung gestartet.");
  Serial.println("Befehle im Serial Monitor:");
  Serial.println("xmin <Wert>");
  Serial.println("xmax <Wert>");
  Serial.println("ymin <Wert>");
  Serial.println("ymax <Wert>");
  Serial.println("save");
}

void loop() {
  // Serial-Eingaben einlesen
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Eingabe echoen
    Serial.print("> ");
    Serial.println(input);

    if (input.startsWith("xmin")) {
      xminCal = input.substring(5).toInt();
    } else if (input.startsWith("xmax")) {
      xmaxCal = input.substring(5).toInt();
    } else if (input.startsWith("ymin")) {
      yminCal = input.substring(5).toInt();
    } else if (input.startsWith("ymax")) {
      ymaxCal = input.substring(5).toInt();
    } else if (input.equalsIgnoreCase("save")) {
      Serial.println();
      Serial.println("Fertige Kalibrierungszeile:");
      Serial.print("touch.setCal(");
      Serial.print(xminCal); Serial.print(", ");
      Serial.print(xmaxCal); Serial.print(", ");
      Serial.print(yminCal); Serial.print(", ");
      Serial.print(ymaxCal); Serial.println(", 800, 480, 0);");
      Serial.println();
    }

    // Neue Kalibrierung anwenden
    touch.setCal(xminCal, xmaxCal, yminCal, ymaxCal, HRES, VRES, 0);

    // Bildschirm neu zeichnen
    test();
  }

  // Touch testen
  if (touch.Pressed()) {
    int X_Coord = touch.X();
    int Y_Coord = touch.Y();
    drawCross(X_Coord, Y_Coord, TFT_GREEN);
  }
}

void test(void) {
  tft.fillScreen(TFT_BLACK);

  drawCross(30, 30, TFT_WHITE);
  drawCross(tft.width() - 30, tft.height() - 30, TFT_WHITE);

  int centre = tft.width()/2;

  String text = "Screen rotation = ";
  text += tft.getRotation();
  char buffer[30];
  text.toCharArray(buffer, 30);

  tft.drawString(buffer, centre, 50, 2);
  tft.drawString("Touch anywhere on screen", centre, 70, 2);
  tft.drawString("to test settings", centre, 90, 2);

  tft.drawString("Serial Monitor:", centre, 120, 2);
  tft.drawString("xmin/xmax/ymin/ymax <value>", centre, 140, 2);
  tft.drawString("save = fertige Zeile", centre, 160, 2);
}

void drawCross(int x, int y, unsigned int color) {
  // Kreuz löschen, bevor neu gezeichnet wird
  static int lastX = -1, lastY = -1;
  if (lastX >= 0 && lastY >= 0) {
    tft.drawLine(lastX - 5, lastY, lastX + 5, lastY, TFT_BLACK);
    tft.drawLine(lastX, lastY - 5, lastX, lastY + 5, TFT_BLACK);
  }

  tft.drawLine(x - 5, y, x + 5, y, color);
  tft.drawLine(x, y - 5, x, y + 5, color);

  lastX = x;
  lastY = y;
}
