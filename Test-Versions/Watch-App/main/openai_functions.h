#ifndef OPENAI_FUNCTIONS_H
#define OPENAI_FUNCTIONS_H
#include <HTTPClient.h>
#include <ArduinoJson.h>

String getResponseFromOpenAI(String userPrompt, const char* apiKey, const char* content);


#endif