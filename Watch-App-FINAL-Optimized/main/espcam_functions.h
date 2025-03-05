#ifndef ESPCAM_FUNCTIONS_H
#define ESPCAM_FUNCTIONS_H
#include <Arduino.h>
#include "esp_camera.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
void cameraSetup();
String uploadImageToFirebase();
String generateUniqueFilename();

#endif // ESPCAM_FUNCTIONS_H