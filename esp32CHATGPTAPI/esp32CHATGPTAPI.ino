#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Transponder Snail";  
const char* password = "max17$$$";      
const char* apiKey = "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"; 
const char* prompt = "\"Hello, who are you?\"";  
const char* content = "\"You are an AI assistant named Alex. You sound professional and don't talk more than needed. You are able to explain things simply and can give real life examples to complex concepts asked by the user.\"";  

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
  //Serial.print("[HTTP] begin...\n");

  http.begin("https://api.openai.com/v1/chat/completions");  // HTTPS
  http.addHeader("Content-Type", "application/json");
  String token_key = String("Bearer ") + apiKey;
  http.addHeader("Authorization", token_key);

  String payload = "{\"model\": \"gpt-4-0125-preview\", \"messages\": [{\"role\": \"system\", \"content\": " + String(content) + "}, {\"role\": \"user\", \"content\": " + String(prompt) + "}]}";
  //String payload = "{\"model\": \"gpt-4\", \"prompt\": " + String(prompt) + "}";
  //Serial.print("[HTTP] POST...\n");
  Serial.println("Payload: " + payload);  //  print the payload

  int httpCode = http.POST(payload);
  Serial.print("HTTP Response code: ");  // print the HTTP response code
  //Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String response = http.getString();
    //Serial.println("Response: " + response);  // Debug: print the response

    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);
    // Checking if the path to the text exists before trying to access it
    if (jsonDoc["choices"][0]["message"].containsKey("content")) {
      String outputText = jsonDoc["choices"][0]["message"]["content"];
      Serial.println("Output Text: " + outputText);  // print the parsed output text
    } else {
      Serial.println("Error: Unable to extract ChatGPT response.");
    }
  } else {
    Serial.print("Error: ");
    Serial.println(http.errorToString(httpCode));  // print the error string
    Serial.println("Server Response: " + http.getString());  // print the server response
  }
  http.end();
}

void loop() {
  delay(10000);  // Delay for next request
}
