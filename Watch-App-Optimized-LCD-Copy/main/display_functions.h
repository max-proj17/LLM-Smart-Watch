#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H

#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "time.h"
#include "SPI.h"

// Define pins for ESP32-S3 for the display interface
#define TFT_CS   47
#define TFT_DC   20
#define TFT_RST  21
#define TFT_BL   48
#define TFT_SCLK 36
#define TFT_MOSI 14

extern Adafruit_GC9A01A tft; // Make the display object available externally

void initializeDisplay();
void updateDisplay();

#endif
