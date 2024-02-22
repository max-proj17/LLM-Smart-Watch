#include "openai_functions.h"

String getResponseFromOpenAI(String userPrompt, String base64Image, const char* apiKey, const char* content) {
  Serial.println("Initializing HTTP client...");
  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions"); // Adjust URL for GPT-4 Vision if needed
  http.addHeader("Content-Type", "application/json");
  String token_key = "Bearer " + String(apiKey); // Combine strings
  http.addHeader("Authorization", token_key.c_str()); // Convert to C string

  Serial.println("Preparing payload...");
  String payload;
  
  // Correcting the payload formatting based on the understanding that `image_url` might have been incorrect in the initial implementation
  // Assuming the correct key for embedding the image directly is "data" and wrapping the base64 image data in an object under "content"
  if (base64Image.length() > 0) {
    Serial.println("Including base64 image in payload...");
    payload = "{\"model\": \"gpt-4-vision-preview\", \"messages\": [{\"role\": \"system\", \"content\": " + String(content) + "}, {\"role\": \"user\", \"content\": [{\"type\": \"text\", \"text\": \"" + userPrompt + "\"}, {\"type\": \"image\", \"data\": \"data:image/jpeg;base64," + base64Image + "\"}]}], \"max_tokens\": 250}";
  } else {
    Serial.println("Creating payload without image...");
    payload = "{\"model\": \"gpt-4-0125-preview\", \"messages\": [{\"role\": \"system\", \"content\": " + String(content) + "}, {\"role\": \"user\", \"content\": \"" + userPrompt + "\"}], \"max_tokens\": 250}";
  }

  Serial.println("Sending HTTP POST request...");
  int httpCode = http.POST(payload);
  Serial.printf("HTTP response code: %d\n", httpCode);
  
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String response = http.getString();
    Serial.println("HTTP request successful. Response received.");
    
    DynamicJsonDocument jsonDoc(1024);
    Serial.println("Deserializing JSON response...");
    DeserializationError error = deserializeJson(jsonDoc, response);
    
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      http.end();
      return "Error: JSON deserialization failed";
    }

    if (jsonDoc["choices"][0].containsKey("message") && jsonDoc["choices"][0]["message"].containsKey("content")) {
      String outputText = jsonDoc["choices"][0]["message"]["content"].as<String>();
      Serial.println("Response processed successfully.");
      http.end();
      return outputText;
    } else {
      Serial.println("Response format unexpected.");
    }
  } else {
    Serial.printf("HTTP request failed, code: %d\n", httpCode);
  }

  http.end();
  return "Error: API request failed";
}

