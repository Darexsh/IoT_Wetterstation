#include <TFT_eSPI.h>
#include "TFT_Touch.h"

// Display und Touch
TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch(15, 16, 7, 6);

#define SCREEN_W 800
#define SCREEN_H 480

// Dummy Werte
float tempValue = 23.5;
float humidValue = 45.2;
float pressureValue = 1012.0;
float gasValue = 120.0;
float distanceValue = 150.0;
float ambientValue = 500.0;
float whiteValue = 800.0;
float uvValue = 0.8;
float uvIndexValue = 2.0;

// Farben
uint32_t bgColor = 0xAAAAAA;
uint32_t boxColor = TFT_WHITE;
uint32_t titleColor = TFT_BLACK;
uint32_t valueColor = TFT_BLACK;
uint32_t sidebarColor = 0x555555;
uint32_t toggleOnColor = 0x00FF00;
uint32_t toggleOffColor = 0xFF0000;

// Box-Struktur
struct Box {
  const char* title;
  float* value;
  const char* unit;
  bool visible;
  int x, y;
};

Box boxes[] = {
  {"Temperatur", &tempValue, "C", true},
  {"Feuchtigkeit", &humidValue, "%", true},
  {"Luftdruck", &pressureValue, "hPa", false},
  {"Gas", &gasValue, "", true},
  {"Abstand", &distanceValue, "cm", false},
  {"Amb. Licht", &ambientValue, "lux", false},
  {"Weisses Licht", &whiteValue, "", false},
  {"UV Licht", &uvValue, "", false},
  {"UV Index", &uvIndexValue, "", false}
};

// Toggle-Struktur für Sidebar
struct Toggle {
  int x, y, w, h;
  Box* box;
};

Toggle toggles[sizeof(boxes)/sizeof(boxes[0])];

// Sidebar-Status
bool sidebarOpen = true;
int sidebarWidth = 200;
int toggleW = 50;   // vergrößerte Kästchen für Touch
int toggleH = 50;
int arrowW = 30;    // vergrößerter Pfeil
int arrowH = 30;

// Dynamische Anordnung der sichtbaren Boxen
void layoutBoxes() {
  int xStart = sidebarOpen ? sidebarWidth + 20 : 40 + 20;
  int yStart = 20;
  int w = 180, h = 120;
  int spacingX = 20, spacingY = 20;
  int x = xStart;
  int y = yStart;

  for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++) {
    if (boxes[i].visible) {
      boxes[i].x = x;
      boxes[i].y = y;

      x += w + spacingX;
      if (x + w > SCREEN_W) {
        x = xStart;
        y += h + spacingY;
      }
    }
  }
}

// Boxen initial zeichnen
void drawBox(int i) {
  tft.fillRoundRect(boxes[i].x, boxes[i].y, 180, 120, 10, boxColor);
  tft.setTextColor(titleColor);
  tft.setTextSize(2);
  tft.setCursor(boxes[i].x + 10, boxes[i].y + 10);
  tft.println(boxes[i].title);
}

// Werte in Box aktualisieren
void updateValue(int i) {
  char buf[20];
  if (strcmp(boxes[i].title, "Temperatur") == 0 || strcmp(boxes[i].title, "Feuchtigkeit") == 0 || strcmp(boxes[i].title, "UV Index") == 0) {
    sprintf(buf, "%.1f", *(boxes[i].value));
  } else if (strcmp(boxes[i].title, "UV Licht") == 0) {
    sprintf(buf, "%.2f", *(boxes[i].value));
  } else {
    sprintf(buf, "%.0f", *(boxes[i].value));
  }

  tft.fillRoundRect(boxes[i].x + 10, boxes[i].y + 60, 160, 40, 5, boxColor);

  tft.setTextColor(valueColor);
  tft.setTextSize(3);
  tft.setCursor(boxes[i].x + 20, boxes[i].y + 60);
  tft.print(buf);

  tft.setTextSize(2);
  tft.setCursor(boxes[i].x + 120, boxes[i].y + 80);
  tft.print(boxes[i].unit);
}

// Sidebar zeichnen
void drawSidebar() {
  int w = sidebarOpen ? sidebarWidth : 40;
  tft.fillRect(0, 0, w, SCREEN_H, sidebarColor);

  if (sidebarOpen) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 10);
    tft.println("Einstellungen");

    int yOffset = 50;
    int spacing = 40;

    for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++) {
      tft.setCursor(20, yOffset + i*spacing + 5);
      tft.setTextSize(1);
      tft.println(boxes[i].title);

      toggles[i] = {w - toggleW - 10, yOffset + i*spacing, toggleW, toggleH, &boxes[i]};
      tft.fillRect(toggles[i].x, toggles[i].y, toggles[i].w, toggles[i].h,
                   boxes[i].visible ? toggleOnColor : toggleOffColor);
    }
  }

  // Pfeil unten rechts
  int arrowX = w - arrowW - 5;
  int arrowY = SCREEN_H - arrowH - 5;
  tft.fillTriangle(arrowX, arrowY,
                   arrowX + arrowW, arrowY + arrowH/2,
                   arrowX, arrowY + arrowH,
                   TFT_WHITE);
}

// Prüfen, ob Pfeil gedrückt wurde
void checkArrow(int x, int y) {
  int w = sidebarOpen ? sidebarWidth : 40;
  int arrowX = w - arrowW - 5;
  int arrowY = SCREEN_H - arrowH - 5;

  if (x >= arrowX && x <= arrowX + arrowW && y >= arrowY && y <= arrowY + arrowH) {
    sidebarOpen = !sidebarOpen;
    drawSidebar();
    tft.fillRect(sidebarWidth, 0, SCREEN_W - sidebarWidth, SCREEN_H, bgColor);
    layoutBoxes();
    for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++)
      if (boxes[i].visible) drawBox(i);
  }
}

// Prüfen, ob ein Toggle gedrückt wurde (mit 5px Puffer)
void checkToggle(int x, int y) {
  for (int i = 0; i < sizeof(toggles)/sizeof(toggles[0]); i++) {
    if (x >= toggles[i].x - 5 && x <= toggles[i].x + toggles[i].w + 5 &&
        y >= toggles[i].y - 5 && y <= toggles[i].y + toggles[i].h + 5) {

      toggles[i].box->visible = !toggles[i].box->visible;
      drawSidebar();
      tft.fillRect(sidebarWidth, 0, SCREEN_W - sidebarWidth, SCREEN_H, bgColor);
      layoutBoxes();
      for (int j = 0; j < sizeof(boxes)/sizeof(boxes[0]); j++)
        if (boxes[j].visible) drawBox(j);
      break;
    }
  }
}

// Dummy-Werte aktualisieren
void updateDummyValues() {
  tempValue += (random(-5, 6) / 10.0);
  humidValue += (random(-5, 6) / 10.0);
  pressureValue += (float)random(-3, 4);
  gasValue += (float)random(-5, 5);
  distanceValue += (float)random(-10, 10);
  ambientValue += (float)random(-50, 50);
  whiteValue += (float)random(-50, 50);
  uvValue += (float)random(-2, 3) / 10.0;
  uvIndexValue += (float)random(-1, 2) / 10.0;
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);

  touch.setRotation(1);
  touch.setCal(610, 3190, 1070, 3180, SCREEN_W, SCREEN_H, false);

  tft.fillScreen(bgColor);
  layoutBoxes();
  drawSidebar();
  for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++)
    if (boxes[i].visible) drawBox(i);
}

void loop() {
  updateDummyValues();
  for (int i = 0; i < sizeof(boxes)/sizeof(boxes[0]); i++)
    if (boxes[i].visible) updateValue(i);

  if (touch.Pressed()) {
    uint16_t x = touch.X();
    uint16_t y = touch.Y();
    Serial.printf("Touch X:%d Y:%d\n", x, y);

    if (sidebarOpen && x <= sidebarWidth) {
      checkArrow(x, y);
      checkToggle(x, y);
    } else if (!sidebarOpen && x <= 40) {
      checkArrow(x, y);
    }

    delay(150); // Entprellung
  }

  delay(500);
}
