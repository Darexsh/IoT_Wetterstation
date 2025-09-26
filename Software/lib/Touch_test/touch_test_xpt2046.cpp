
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>

// Pinbelegung (anpassen!)
#define TOUCH_CS 15
#define TFT_CS   5

TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS);

#define SCREEN_W 800
#define SCREEN_H 480

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  
  ts.begin();
  ts.setRotation(1);  // muss zur Display-Rotation passen

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.setCursor(20, 10);
  tft.println("Touch-Test: tippe auf das Display");
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();

    // Umrechnen auf Display-Koordinaten
    // Achtung: XPT2046 liefert oft Werte 0-4095, je nach Board
    int x = map(p.x, 0, 4095, 0, SCREEN_W);
    int y = map(p.y, 0, 4095, 0, SCREEN_H);

    // Debug in Serial
    Serial.printf("Raw X=%d Y=%d -> Mapped X=%d Y=%d\n", p.x, p.y, x, y);

    // Kreise an den Touchpunkten zeichnen
    tft.fillCircle(x, y, 10, TFT_RED);

    // Koordinaten auf dem Display anzeigen
    tft.fillRect(0, SCREEN_H - 40, 300, 40, TFT_BLACK);
    tft.setCursor(20, SCREEN_H - 35);
    tft.printf("X=%d Y=%d", x, y);

    delay(50);  // Kurze Entprellung
  }
}
