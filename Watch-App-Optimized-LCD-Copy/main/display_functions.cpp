#include "display_functions.h"

// Display constructor for primary hardware SPI connection
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

void initializeDisplay() {
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); // Initialize SPI
  tft.begin(); // Initialize the display
  tft.fillScreen(GC9A01A_BLACK); // Clear the screen
  
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight

  // Assuming WiFi connection is established before this is called
  configTime(-6 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1);
  tzset();

  // Wait for time to be set
  unsigned long startAttemptTime = millis();
  while (time(nullptr) < 24 * 3600) {
    if (millis() - startAttemptTime > 5000) {
      Serial.println("Failed to obtain time, retrying...");
      startAttemptTime = millis();
    }
    delay(100);
  }
  Serial.println("Time synchronized");
}

void updateDisplay() {
  // Logic to update time, date, and static battery level
  time_t currTime;
  time(&currTime);
  struct tm *timeinfo = localtime(&currTime);

  char timeStr[6];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);

  char dateStr[11];
  snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);

  tft.fillScreen(GC9A01A_BLACK); // Clear the screen for fresh update

  // Display static battery level text
  tft.setCursor(75, 50);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
  tft.print("Batt 90%");

  // Display current time
  tft.setCursor(35, 90);
  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(6);
  tft.print(timeStr);

  // Display current date
  tft.setCursor(60, 160);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_GREEN);
  tft.println(dateStr);
}
