// Author: Max Finch
// Date: 02/09/2024


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "openai_functions.h"

const char* ssid = "Transponder Snail";
const char* password = "max17$$$";
const char* apiKey = "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";
const char* content = "\"You are an AI assistant named Alex. You sound professional and don't talk more than needed. You are able to explain things simply and can give real life examples to complex concepts asked by the user.\"";  

void setup() {
  Serial.begin(115200); // Increase baud rate for faster serial communication
  Serial.println("Starting...");

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi. Type your question or 'EXIT' to end the conversation.");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    // Check for exit code
    if (input.equalsIgnoreCase("EXIT")) {
      Serial.println("Exiting conversation.");
      return; // Exit loop or implement a way to break the conversation loop
    }

    
    String response = getResponseFromOpenAI(input, apiKey, content);
    Serial.println(response);

    Serial.println("Ask another question or type 'EXIT' to end the conversation.");
  }

  delay(100); // Small delay to prevent overwhelming the CPU
}




