// Disable all #warning messages from the TFT_eSPI library
// Useful to suppress annoying compiler warnings.
#define DISABLE_ALL_LIBRARY_WARNINGS

// Enable 8-bit parallel interface for the display (instead of SPI)
// Required for SSD1963 and other large TFTs.
#define TFT_PARALLEL_8_BIT

// Driver definition for 480x800 pixel display with SSD1963 controller
// "ALT" = alternative timing configuration.
#define SSD1963_800ALT_DRIVER

// Display color order: Red-Green-Blue
// Some displays may require TFT_BGR instead of TFT_RGB.
#define TFT_RGB_ORDER TFT_RGB

// -----------------------------
// ESP32 pins connected to TFT
// -----------------------------

#define TFT_CS   2   // Chip Select (CS) – activates the display
#define TFT_DC   14  // Data/Command (DC) – 0=Command, 1=Data; must be pin 0–31
#define TFT_RST  1   // Reset pin – resets the display
#define TFT_WR   21  // Write strobe (WR) – data clock for parallel interface
#define TFT_RD   35  // Read strobe (RD) – only needed for reading from display
// #define TFT_BL   4  // Backlight control pin

// Data pins D0–D7 for 8-bit parallel bus
// All pins must be in range 0–31 for single register writes
#define TFT_D0 13
#define TFT_D1 12
#define TFT_D2 11
#define TFT_D3 10
#define TFT_D4 9
#define TFT_D5 8 
#define TFT_D6 18 
#define TFT_D7 17

// #define T_IRQ 5
// #define T_DO 6
// #define T_DIN 7
// #define T_CS 15
// #define T_CLK 16

// -----------------------------
// Fonts to load in the library
// -----------------------------

#define LOAD_GLCD   // Font 1 – original Adafruit 8-pixel font (~1.8 KB in flash)
#define LOAD_FONT2  // Font 2 – small 16-pixel font (~3.5 KB, 96 chars)
#define LOAD_FONT4  // Font 4 – medium 26-pixel font (~5.8 KB, 96 chars)
#define LOAD_FONT6  // Font 6 – large 48-pixel font (~2.6 KB, digits and symbols only)
#define LOAD_FONT7  // Font 7 – 7-segment style 48-pixel font (~2.4 KB, digits only)
#define LOAD_FONT8  // Font 8 – huge 75-pixel font (~3.3 KB, digits and symbols only)
//#define LOAD_FONT8N // Narrow alternative to Font 8 – fits 3 digits in 160px width
#define LOAD_GFXFF  // FreeFonts – include 48 Adafruit_GFX free fonts and custom fonts

// -----------------------------
// Optional features
// -----------------------------

// Enable smooth fonts (anti-aliased)
// Comment out to save ~20 KB of flash if not needed
#define SMOOTH_FONT
