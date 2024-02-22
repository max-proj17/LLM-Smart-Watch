#ifndef ESPCAM_FUNCTIONS_H
#define ESPCAM_FUNCTIONS_H
#include <Arduino.h>
#include "esp_camera.h"

void cameraSetup();
String captureAndEncodeImage();

#endif // ESPCAM_FUNCTIONS_H