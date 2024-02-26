#include <Arduino.h>
#include "espcam_functions.h"
#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"
#include "esp32-hal-log.h"
#include "esp_camera.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>



void cameraSetup() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Adjustments for PSRAM
  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 0); // Adjustments for your specific camera module
  s->set_brightness(s, 2);
  s->set_saturation(s, 0);

  Serial.println("Camera configuration complete!");
}

String uploadImageToFirebase() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return "";
    }

    HTTPClient http;
    // URL for our Firebase Storage 
    String url = "https://firebasestorage.googleapis.com/v0/b/llmwatch-bc9e5.appspot.com/o?uploadType=media&name=images/myImage.jpg";

    http.begin(url);
    http.addHeader("Content-Type", "image/jpeg");

    // POST image data
    int httpResponseCode = http.POST(fb->buf, fb->len);

    String responsePayload = "{}"; 
    if (httpResponseCode == 200) {
        responsePayload = http.getString(); 
    } else {
        Serial.print("Error during image upload: ");
        Serial.println(httpResponseCode);
    }

    http.end(); // Close the connection
    esp_camera_fb_return(fb); // Return the frame buffer back to the driver

    // Parse response to extract the download URL
    DynamicJsonDocument doc(1024); 
    deserializeJson(doc, responsePayload);
    String downloadUrl = doc["downloadTokens"].as<String>(); 

    if (!downloadUrl.isEmpty()) {
        // Construct the download URL
        downloadUrl = "https://firebasestorage.googleapis.com/v0/b/llmwatch-bc9e5.appspot.com/o/images%2FmyImage.jpg?alt=media&token=" + downloadUrl;
        Serial.println("Image uploaded successfully: " + downloadUrl);
    } else {
        Serial.println("Failed to upload image or parse response.");
    }

    return downloadUrl; // Return the download URL or an empty string if failed
}
