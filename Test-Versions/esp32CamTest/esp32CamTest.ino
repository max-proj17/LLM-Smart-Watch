#include "esp_camera.h"
#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"
#include "sd_read_write.h"
#define BUTTON_PIN  0
int photo_index = 0;
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  sdmmcInit();

  createDir(SD_MMC, "/camera");
  listDir(SD_MMC, "/camera", 0);

  delay(2000);
  cameraSetup();
  // if (cameraSetup() == 1) {
  //   // Optionally delete the first image if exists
  //   deleteFirstImage("/camera");
  // } else {
  //   return;
  // }
}

void loop() {
  // Improved debouncing mechanism
  static unsigned long lastDebounceTime = 0;
  static bool lastButtonState = HIGH;
  static bool buttonPressed = false;

  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 50) { // 50ms debounce period
    if (currentButtonState == LOW && !buttonPressed && (millis() - lastDebounceTime) > 50) {
      buttonPressed = true;
      Serial.println("Button pressed and debounced, starting countdown...");


      // Start the countdown
      for (int i = 3; i > 0; i--) {
        Serial.println(i);
        delay(1000); // Wait for 1 second between counts
      }
      Serial.println("Taking photo...");

      takeAndSavePhoto();
    }
  }

  if (currentButtonState == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  lastButtonState = currentButtonState;
}

void takeAndSavePhoto() {
  // Attempt to capture a photo with the camera
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb != NULL) {
    // Generate a unique file name based on the number of files
  
    photo_index = readFileNum(SD_MMC, "/camera") + 1;
    if (photo_index != 0) {
      // Construct the file path where the photo will be saved
      String path = "/camera/" + String(photo_index) + ".jpg";
      // Save the captured photo to the SD card
      writejpg(SD_MMC, path.c_str(), fb->buf, fb->len);
      Serial.println("Photo taken and saved: " + path);
      // Add a delay to ensure the SD card has time to write the file
      delay(500); 
    } else {
      Serial.println("Failed to determine the photo index.");
    }

    // Return the frame buffer back to the camera driver
    esp_camera_fb_return(fb);
  } else {
    Serial.println("Camera capture failed.");
  }
}


void deleteFirstImage(const char *directory) {
  File root = SD_MMC.open(directory);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  if (file && !file.isDirectory()) {
    Serial.print("Deleting first image: ");
    Serial.println(file.name());
    if (SD_MMC.remove(file.name())) {
      Serial.println("File deleted");
    } else {
      Serial.println("Delete failed");
    }
  }
}

int cameraSetup(void) {
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
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return 0;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 0); // flip it back
  s->set_brightness(s, 2); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation

  Serial.println("Camera configuration complete!");
  return 1;
}

