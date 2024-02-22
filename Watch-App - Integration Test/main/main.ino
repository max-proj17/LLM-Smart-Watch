// Author: Max Finch
// Date: 02/09/2024


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "openai_functions.h"
#include "espcam_functions.h" // Include the camera functions header
#define BUTTON_PIN  0

const char* ssid = "Transponder Snail";
const char* password = "max17$$$";
const char* apiKey = "";
const char* content = "\"You are an AI assistant named Alex. You sound professional and don't talk more than needed. You are able to explain things simply and can give real life examples to complex concepts asked by the user.\"";  

void setup() {
  Serial.begin(115200); // Increase baud rate for faster serial communication
  Serial.println("Starting...");
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //Initialize Camera
  cameraSetup();

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi. Type your question or 'EXIT' to end the conversation.");
}

// Inside loop() function of main.ino
void loop() {
  static bool isImageNeeded = false;
  static bool isWaitingForImageResponse = false;
  static String base64Image = "";
  static String input = "";

  if (Serial.available() && !isWaitingForImageResponse) {
    input = Serial.readStringUntil('\n');
    input.trim();
    Serial.println("Received input: " + input);

    if (input.equalsIgnoreCase("EXIT")) {
      Serial.println("Exiting conversation.");
      while (true) {} // Stay here forever
    }

    // Ask if an image is needed for this query.
    Serial.println("Is an image needed for this query? (yes/no)");
    isWaitingForImageResponse = true;
    return; // Return early to wait for the next input.
  }

  if (isWaitingForImageResponse && Serial.available()) {
    String imageNeededResponse = Serial.readStringUntil('\n');
    imageNeededResponse.trim();

    if (imageNeededResponse.equalsIgnoreCase("yes")) {
      Serial.println("Please press the button to capture an image.");
      isImageNeeded = true;
    } else {
      Serial.println("No image needed. Processing query...");
      isImageNeeded = false;
      isWaitingForImageResponse = false;
      // Process the query without an image.
      String response = getResponseFromOpenAI(input, "", apiKey, content);
      Serial.println("Response from OpenAI: " + response);
      input = ""; // Clear input for the next query.
    }
  }

  // Handle button press for image capture if needed.
  if (isImageNeeded && digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Debounce the button press.
    if (digitalRead(BUTTON_PIN) == LOW) {
      while (digitalRead(BUTTON_PIN) == LOW); // Wait for button release.
      Serial.println("Capturing and encoding image...");
      base64Image = captureAndEncodeImage();
      if (base64Image.isEmpty()) {
        Serial.println("Failed to capture or encode image.");
      } else {
        Serial.println("Image captured and encoded.");
        // Send the query with the image.
        String response = getResponseFromOpenAI(input, base64Image, apiKey, content);
        Serial.println("Response from OpenAI: " + response);
      }
      // Reset the flags for the next query.
      isImageNeeded = false;
      isWaitingForImageResponse = false;
      input = ""; // Clear input for the next query.
    }
  }
}

