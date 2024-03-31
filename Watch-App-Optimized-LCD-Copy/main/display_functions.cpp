#include "display_functions.h"

//extern volatile DisplayMode displayMode;
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


// Function to display the alphabet, repeated as necessary to fill the screen
void displayText(const String& textToDisplay) {
  tft.fillScreen(GC9A01A_BLACK); // Clear the screen

  // Set text properties
  tft.setTextSize(2); // Choose text size 2 for larger characters
  tft.setTextColor(GC9A01A_WHITE);

  // Estimate the width and height of a character block at textSize(2)
  int charWidth = 12; // Width of a character block at textSize(2), adjust as needed
  int charHeight = 24; // Height of a character block at textSize(2), adjust as needed

  // Calculate the maximum square size within the circular display
  int maxSquareSide = (int)(tft.width() / sqrt(2)); // Side length of the largest square that fits within the circle

  // Calculate the number of characters per line and number of lines that fit in the square
  int charsPerLine = maxSquareSide / charWidth; 
  int lineCount = maxSquareSide / charHeight;

  // Calculate the starting positions to center the square in the circle
  int startX = (tft.width() - maxSquareSide) / 2;
  int startY = (tft.height() - maxSquareSide) / 2;

  int textLength = textToDisplay.length();

  // Start from the calculated Y position
  for (int line = 0; line < lineCount; line++) {
    // Set cursor position for this line, starting from the calculated X position
    tft.setCursor(startX, startY + (line * charHeight));
    
    // Determine the substring to print for this line
    int startCharPos = line * charsPerLine;
    int endCharPos = startCharPos + charsPerLine;
    endCharPos = endCharPos > textLength ? textLength : endCharPos; // Don't go past the end of the text
    String lineText = textToDisplay.substring(startCharPos, endCharPos);

    // Print characters for this line
    tft.print(lineText);
    
    // If we've reached the end of the text, break out of the loop
    if (endCharPos >= textLength) {
      break;
    }
  }
}



