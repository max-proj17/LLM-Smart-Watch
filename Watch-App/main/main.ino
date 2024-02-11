// Author: Max Finch
// Date: 02/09/2024


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "openai_functions.h"

const char* ssid = "Transponder Snail";
const char* password = "max17$$$";
const char* apiKey = "";
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

// String getResponseFromOpenAI(String userPrompt) {
//   HTTPClient http;
//   http.begin("https://api.openai.com/v1/chat/completions"); // HTTPS
//   http.addHeader("Content-Type", "application/json");
//   String token_key = String("Bearer ") + apiKey;
//   http.addHeader("Authorization", token_key);

//   String payload = "{\"model\": \"gpt-4-0125-preview\", \"messages\": [{\"role\": \"system\", \"content\": " + String(content) + "}, {\"role\": \"user\", \"content\": \"" + userPrompt + "\"}]}";
  
//   int httpCode = http.POST(payload);
//   if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//     String response = http.getString();
//     DynamicJsonDocument jsonDoc(1024); 
//     deserializeJson(jsonDoc, response);
//     if (jsonDoc["choices"][0].containsKey("message") && jsonDoc["choices"][0]["message"].containsKey("content")) {
//     String outputText = jsonDoc["choices"][0]["message"]["content"].as<String>();
//     http.end();
//     return outputText;
//   }

// }

  http.end();
  return "Error: Unable to get a response.";
}

