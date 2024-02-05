#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Transponder Snail";  
const char* password = "max17$$$";      
const char* apiKey = ""; 
const char* prompt = "\"What is your name?\"";  

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  HTTPClient http;
  Serial.print("[HTTP] begin...\n");

  http.begin("https://api.openai.com/v1/chat/completions");  // HTTPS
  http.addHeader("Content-Type", "application/json");
  String token_key = String("Bearer ") + apiKey;
  http.addHeader("Authorization", token_key);

  String payload = "{\"model\": \"gpt-3.5-turbo-0125\", \"prompt\": " + String(prompt) + ", \"temperature\": 0, \"max_tokens\": 7}";
  //String payload = "{\"model\": \"gpt-4\", \"prompt\": " + String(prompt) + "}";
  Serial.print("[HTTP] POST...\n");
  Serial.println("Payload: " + payload);  // Debug: print the payload

  int httpCode = http.POST(payload);
  Serial.print("HTTP Response code: ");  // Debug: print the HTTP response code
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String response = http.getString();
    Serial.println("Response: " + response);  // Debug: print the response

    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);
    String outputText = jsonDoc["choices"][0]["text"];
    Serial.println("Output Text: " + outputText);  // Debug: print the parsed output text
  } else {
    Serial.print("Error: ");
    Serial.println(http.errorToString(httpCode));  // Debug: print the error string
    Serial.println("Server Response: " + http.getString());  // Debug: print the server response
  }
  http.end();
}

void loop() {
  delay(10000);  // Delay for next request
}


