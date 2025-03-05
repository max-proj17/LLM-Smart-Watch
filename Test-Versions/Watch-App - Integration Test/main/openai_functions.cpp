#include "openai_functions.h"
#include <ArduinoJson.h>


// Function to check if the image URL is accessible
bool isImageUrlAccessible(const String& imageUrl) {
  HTTPClient http;
  http.begin(imageUrl);
  http.setTimeout(5000); // 5 seconds timeout
  int httpCode = http.sendRequest("HEAD");
  http.end();
  return (httpCode == HTTP_CODE_OK);
}

String getResponseFromOpenAI(const String& userPrompt, const String& imageUrl, const char* apiKey, const char* content) {

  //Serial.println("Initializing HTTP client...");
  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions"); 
  http.addHeader("Content-Type", "application/json");
  String token_key = "Bearer " + String(apiKey); // Combine strings
  http.addHeader("Authorization", token_key.c_str()); // Convert to C string
  http.setTimeout(10000); // Set timeout to 10 seconds


  //Serial.println("Preparing payload...");
  DynamicJsonDocument doc(4096); 

  // Determine if the url is present and adjust the payload accordingly
  if (!imageUrl.isEmpty()) {
    // Check if the image URL is accessible
    //Serial.println("Verifying image URL accessibility...");
    if (!isImageUrlAccessible(imageUrl)) {
      //Serial.println("Image URL is not accessible. Exiting.");
      return "Error: Image URL not accessible";
    }
    //Serial.println("Image URL is accessible. Proceeding.");
    // Proceed with including the image in the payload
    
    //Serial.println("Including image in payload...");
    doc["model"] = "gpt-4-vision-preview"; 
    JsonObject messages_0 = doc["messages"].createNestedObject();
    messages_0["role"] = "user";

    JsonArray messages_0_content = messages_0.createNestedArray("content");

    JsonObject messages_0_content_0 = messages_0_content.createNestedObject();
    messages_0_content_0["type"] = "text";
    messages_0_content_0["text"] = "What is in the image?";

    JsonObject messages_0_content_1 = messages_0_content.createNestedObject();
    messages_0_content_1["type"] = "image_url";
    messages_0_content_1["image_url"]["url"] = imageUrl.c_str(); 
  
    doc["max_tokens"] = 300;
  } else {
    //Serial.println("Creating payload without image...");
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject systemMessage = messages.createNestedObject();
    systemMessage["role"] = "system";
    systemMessage["content"] = content;
    JsonObject userMessage = messages.createNestedObject();
    userMessage["role"] = "user";
    userMessage["content"] = userPrompt;
    doc["model"] = "gpt-4-0125-preview"; 
    doc["max_tokens"] = 250;
  }


  String payload;
  serializeJson(doc, payload); // Serialize the JSON document to a string
  //Serial.println(payload);

  //Serial.println("Sending HTTP POST request...");
  int httpCode = http.POST(payload);
  //Serial.printf("HTTP response code: %d\n", httpCode);

  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String response = http.getString();
    //Serial.println("HTTP request successful. Response received.");
    
    DynamicJsonDocument jsonDoc(1024); 
    //Serial.println("Deserializing JSON response...");
    DeserializationError error = deserializeJson(jsonDoc, response);
    
    if (error) {
      //Serial.print("deserializeJson() failed: ");
      //Serial.println(error.c_str());
      http.end();
      return "Error: JSON deserialization failed";
    }

    if (jsonDoc.containsKey("choices") && jsonDoc["choices"][0].containsKey("message")) {
      String outputText = jsonDoc["choices"][0]["message"].as<String>();
      //Serial.println("Response processed successfully.");
      http.end();
      return outputText;
    } else {
     // Serial.println("Response format unexpected.");
    }
  } else {
    //Serial.printf("HTTP request failed, code: %d\n", httpCode);
  }

  http.end();
  return "Error: API request failed";
}