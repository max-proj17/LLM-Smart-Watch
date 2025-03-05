#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <WiFi.h>
#include "time.h"


// Define pins for ESP32-S3 for the display interface
#define TFT_CS   47  // Chip select pin for the display
#define TFT_DC   20  // Data/command pin for the display
#define TFT_RST  21  // Reset pin for the display
#define TFT_BL   48  // Backlight control pin 
#define TFT_SCLK 36  // Serial Clock for SPI
#define TFT_MOSI 14  // Master Out Slave In for SPI

// WiFi credentials
const char* ssid = "ICS The Nest";
const char* password = "hsv#gsxXeh";

// Display constructor for primary hardware SPI connection
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

void connectToWiFiAndInitTime() {
  Serial.print("Connecting to WiFi..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");

  // Set timezone to Central Standard Time (CST) and enable automatic adjustment for daylight saving
  configTime(-6 * 3600, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
  setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1); // Set the timezone environment variable
  tzset(); // Apply the timezone settings

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

void setup() {
  Serial.begin(115200);

  // Connect to WiFi and initialize time
  connectToWiFiAndInitTime();

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); // Initialize SPI

  tft.begin();  // Initialize the display
  tft.fillScreen(GC9A01A_BLACK);  // Clear the screen

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  // Turn on backlight if applicable
}

void loop() {
  tft.fillScreen(GC9A01A_BLACK);  // Clear the screen
  testText();  // Display time and date
  delay(60000); // Refresh every minute
}

void testText() {
  time_t currTime; // Variable to hold the current time
  time(&currTime); // Get the current time from NTP
  
  struct tm * timeinfo = localtime(&currTime);

  char timeStr[6]; // HH:MM format
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);

  char dateStr[11]; // DD/MM/YYYY format
  snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);

  // Clear the screen
  tft.fillScreen(GC9A01A_BLACK);

  // Display the battery level as a static text
  tft.setCursor(75, 50);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
  tft.print("Batt 90%"); 

  // Display the time
  tft.setCursor(35, 90);
  tft.setTextColor(GC9A01A_RED);
  tft.setTextSize(6);
  tft.print(timeStr);

  // Display the date
  tft.setCursor(60, 160);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_GREEN);
  tft.println(dateStr);
}

