#include "openai_functions.h"

String getResponseFromOpenAI(input, apiKey, content) {
  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions"); // HTTPS
  http.addHeader("Content-Type", "application/json");
  String token_key = String("Bearer ") + apiKey;
  http.addHeader("Authorization", token_key);

  String payload = "{\"model\": \"gpt-4-0125-preview\", \"messages\": [{\"role\": \"system\", \"content\": " + String(content) + "}, {\"role\": \"user\", \"content\": \"" + userPrompt + "\"}]}";
  
  int httpCode = http.POST(payload);
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String response = http.getString();
    DynamicJsonDocument jsonDoc(1024); 
    deserializeJson(jsonDoc, response);
    if (jsonDoc["choices"][0].containsKey("message") && jsonDoc["choices"][0]["message"].containsKey("content")) {
    String outputText = jsonDoc["choices"][0]["message"]["content"].as<String>();
    http.end();
    return outputText;
  }

}