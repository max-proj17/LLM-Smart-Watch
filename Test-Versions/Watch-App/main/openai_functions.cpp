#include "openai_functions.h"

String getResponseFromOpenAI(String userPrompt, const char* apiKey, const char* content) {
  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions"); // HTTPS
  http.addHeader("Content-Type", "application/json");
  String token_key = String("Bearer ") + apiKey;
  http.addHeader("Authorization", token_key);

  String payload = "{\"model\": \"gpt-4-0125-preview\", \"messages\": [{\"role\": \"system\", \"content\": " + String(content) + "}, {\"role\": \"user\", \"content\": \"" + userPrompt + "\"}], \"max_tokens\": 250}";
  
  int httpCode = http.POST(payload);
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String response = http.getString();
    DynamicJsonDocument jsonDoc(1024); 
    DeserializationError error = deserializeJson(jsonDoc, response);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      http.end();
      return "Error: JSON deserialization failed";
    }

    if (jsonDoc["choices"][0].containsKey("message") && jsonDoc["choices"][0]["message"].containsKey("content")) {
    String outputText = jsonDoc["choices"][0]["message"]["content"].as<String>();
    http.end();
    return outputText;
    }
  }

}