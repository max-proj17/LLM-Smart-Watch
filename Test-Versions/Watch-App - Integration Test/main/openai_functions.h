#ifndef OPENAI_FUNCTIONS_H
#define OPENAI_FUNCTIONS_H
#include <HTTPClient.h>
#include <ArduinoJson.h>


String getResponseFromOpenAI(const String& userPrompt, const String& imageUrl, const char* apiKey, const char* content);
bool isImageUrlAccessible(const String& imageUrl);

#endif