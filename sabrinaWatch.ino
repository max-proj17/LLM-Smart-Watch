#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <TimeLib.h>

// Define pins for display interface. You'll probably need to edit this for
// your own needs:

#if defined(ARDUINO_SEEED_XIAO_RP2040)

// Pinout when using Seed Round Display for XIAO in combination with
// Seeed XIAO RP2040. Other (non-RP2040) XIAO boards, any Adafruit Qt Py
// boards, and other GC9A01A display breakouts will require different pins.
#define TFT_CS D1  // Chip select
#define TFT_DC D3  // Data/command
#define TFT_BL D6  // Backlight control

#else  // ALL OTHER BOARDS - EDIT AS NEEDED

// Other RP2040-based boards might not have "D" pin defines as shown above
// and will use GPIO bit numbers. On non-RP2040 boards, you can usually use
// pin numbers silkscreened on the board.
#define TFT_DC 7
#define TFT_CS 10
// If display breakout has a backlight control pin, that can be defined here
// as TFT_BL. On some breakouts it's not needed, backlight is always on.

#endif

// Display constructor for primary hardware SPI connection -- the specific
// pins used for writing to the display are unique to each board and are not
// negotiable. "Soft" SPI (using any pins) is an option but performance is
// reduced; it's rarely used, see header file for syntax if needed.
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);


void setup() {
  Serial.begin(9600);  //initialize serial monitor to print out debug messages

  setTime(18, 0, 0, 19, 2, 2024);  //hard code time for now

  tft.begin();                    //initialize font printing object
  tft.fillScreen(GC9A01A_BLACK);  //fill the screen with black to clear out


#if defined(TFT_BL)
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  // Backlight on
#endif                         // end TFT_BL
}

void loop(void) {

  tft.fillScreen(GC9A01A_BLACK);  //fill the screen with black to clear out
  testText();    //print
  delay(60000);  //check to print every second
}


unsigned long testText() {
  //tft.fillScreen(GC9A01A_BLACK);

  time_t currTime = now();  //get the current time
  int currMin = minute(currTime);
  char minStr[12];
  if (currMin < 10) {
    sprintf(minStr, "%02d", currMin);
  }


  unsigned long start = micros();
  tft.setCursor(75, 50);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
  tft.print("Batt 90%");
  tft.setCursor(35, 90);
  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(6);
  tft.print(hour(currTime));
  tft.setTextColor(GC9A01A_YELLOW);
  tft.setTextSize(6);
  tft.print(":");
  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(6);
  tft.print(minStr);
  tft.println();
  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(2);
  tft.setCursor(60, 160);

  // Format date as DD/MM/YYYY
  char dateStr[12];
  sprintf(dateStr, "%02d/%02d/%04d", day(currTime), month(currTime), year(currTime));

  tft.println(dateStr);

  return micros() - start;
}
