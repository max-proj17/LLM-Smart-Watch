#include <Arduino.h>
#include "espcam_functions.h"
#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"
#include "esp32-hal-log.h"
#include "esp_camera.h"

// Include the necessary header for mbedtls base64 functions
#include "mbedtls/base64.h"


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

String captureAndEncodeImage() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return "";
    }

    size_t outlen = 0; // Variable to hold the length of the encoded string
    // Estimate the length of the encoded data
    size_t encodedLength = 4 * ((fb->len + 2) / 3) + 1;

    // Allocate memory for the encoded data
    unsigned char *encodedData = (unsigned char*)malloc(encodedLength);
    if (!encodedData) {
        Serial.println("Failed to allocate memory for base64 encoding");
        esp_camera_fb_return(fb);
        return "";
    }

    // Perform the encoding
    int ret = mbedtls_base64_encode(encodedData, encodedLength, &outlen, fb->buf, fb->len);
    if (ret != 0) {
        Serial.printf("Failed to encode image in base64, mbedtls error: %d\n", ret);
        free(encodedData);
        esp_camera_fb_return(fb);
        return "";
    }
    encodedData[outlen] = '\0';
    // Create a String object from the encoded data
    String encodedString = String((char*)encodedData);
    free(encodedData); // Free the allocated memory
    esp_camera_fb_return(fb); // Return the frame buffer back to the driver

    return encodedString;
}
