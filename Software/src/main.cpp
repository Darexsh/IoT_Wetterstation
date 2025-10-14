#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <Adafruit_BME680.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_VCNL4040.h>
#include <config.h>
#include <methods.h>
#include <logo.h>

TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT);
extern Box boxes[];
int currentPage = 0;  // 0 = main page, 1 = detail page
int selectedBox = -1; // remember which box was clicked
bool touchReleased = true;
float lastDetailValue = -9999; // global

Adafruit_BME680 bme;
Adafruit_LTR390 ltr;
Adafruit_VCNL4040 vcnl;

void setup()
{
  Serial.begin(115200);

#ifdef REAL_SENSORS
  Wire.begin(SDA, SCL);
  if (!bme.begin())
    Serial.println("BME688 nicht gefunden");
  if (!vcnl.begin())
    Serial.println("VCNL4040 nicht gefunden");
  if (!ltr.begin())
    Serial.println("LTR390 nicht gefunden");
#endif

  tft.begin();
  tft.setRotation(1);

  touch.setRotation(1);
  touch.setCal(XMIN, XMAX, YMIN, YMAX, SCREEN_W, SCREEN_H, false);

  tft.fillScreen(COLOR_BACKGROUND);

  layoutBoxes();
  drawLogo();
  for (int i = 0; i < NUM_BOXES; i++)
  {
    drawBox(i);
  }
}

void loop()
{
    // Werte immer aktualisieren
    updateValues();

    if (currentPage == 0)
    {
        // Hauptseite – alle Boxen aktualisieren
        for (int i = 0; i < NUM_BOXES; i++)
        {
            updateValue(i);
        }

        // Touch auf Box prüfen
        if (touch.Pressed() && touchReleased)
        {
            touchReleased = false;
            int tx = touch.X();
            int ty = touch.Y();

            for (int i = 0; i < NUM_BOXES; i++)
            {
                if (tx > boxes[i].x && tx < boxes[i].x + boxes[i].w &&
                    ty > boxes[i].y && ty < boxes[i].y + boxes[i].h)
                {
                    selectedBox = i;
                    currentPage = 1;
                    lastDetailValue = -9999;      // Reset für Detailseite
                    drawDetailPageTitle(selectedBox); // Titel & Hinweistext
                    break;
                }
            }
        }
        if (!touch.Pressed()) touchReleased = true;
    }
    else if (currentPage == 1 && selectedBox >= 0)
    {
        // Detailseite – nur Wert aktualisieren
        drawDetailPageWithSprite(selectedBox);

        // Touch für Rückkehr prüfen
        if (touch.Pressed() && touchReleased)
        {
            touchReleased = false;
            currentPage = 0;
            selectedBox = -1;
            tft.fillScreen(COLOR_BACKGROUND);
            layoutBoxes();
            drawLogo();
            for (int i = 0; i < NUM_BOXES; i++)
                drawBox(i);
        }
        if (!touch.Pressed()) touchReleased = true;
    }
}
