// Author: Max Finch
// Date: 02/09/2024


#include <WiFi.h>
//#include <HTTPClient.h>
//#include <ArduinoJson.h>
#include "openai_functions.h"
#include "espcam_functions.h" // Include the camera functions header
#include "display_functions.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define BUTTON_PIN  0

// const char* ssid = "Transponder Snail";
// const char* password = "max17$$$";
const char* ssid = "ICS The Nest";
const char* password = "hsv#gsxXeh";
const char* apiKey = "";
const char* content = "\"You are an AI assistant named Alex. You sound professional and don't talk more than needed. You are able to explain things simply and can give real life examples to complex concepts asked by the user.\"";  
// Global variable to toggle display modes
//volatile bool displayAlphabetMode = false;
String response = "";

enum DisplayMode {
  DISPLAY_TIME_DATE,
  IMAGE_YN,
  TAKE_IMG,
  PROCESSING,
  DISP_OUTPUT
};

volatile DisplayMode displayMode = DISPLAY_TIME_DATE; // Default to showing time and date

void DisplayTask(void * parameter) {
  for (;;) {
    if (displayMode == DISPLAY_TIME_DATE) {
      updateDisplay();
    } else if (displayMode == IMAGE_YN) {
      displayText("Image? (y/n)");

    } else if (displayMode == TAKE_IMG) {
      displayText("Take Image");

    }else if (displayMode == PROCESSING) {
      displayText("Processing...");

    }else if (displayMode == DISP_OUTPUT) {
      displayText(response);

    }

    vTaskDelay(pdMS_TO_TICKS(1000)); // Refresh rate
  }
}

enum TaskState {
    WAIT_FOR_INPUT,
    WAIT_FOR_IMAGE_DECISION,
    WAIT_FOR_IMAGE_CAPTURE, // Additional state for handling image capture
    PROCESS_QUERY
};

void AITask(void * parameter) {
    TaskState state = WAIT_FOR_INPUT;
    static bool isImageNeeded = false;
    static String imageUrl = "";
    static String input = "";

    for (;;) {
        switch (state) {
            case WAIT_FOR_INPUT:
                if (Serial.available()) {
                    input = Serial.readStringUntil('\n');
                    input.trim();

                    if (input.equalsIgnoreCase("EXIT")) {
                        Serial.println("Exiting conversation.");
                        while (true) {}
                    }
                    displayMode = IMAGE_YN;
                    Serial.println("Is an image needed for this query? (yes/no)");
                    state = WAIT_FOR_IMAGE_DECISION;
                }
                break;

            case WAIT_FOR_IMAGE_DECISION:
                if (Serial.available()) {
                    String imageNeededResponse = Serial.readStringUntil('\n');
                    imageNeededResponse.trim();

                    isImageNeeded = imageNeededResponse.equalsIgnoreCase("yes");
                    Serial.println(isImageNeeded ? "Please press the button to capture an image." : "No image needed. Processing query...");
                    displayMode = TAKE_IMG;
                    state = isImageNeeded ? WAIT_FOR_IMAGE_CAPTURE : PROCESS_QUERY;
                }
                break;

            case WAIT_FOR_IMAGE_CAPTURE:
                // Assuming button press is detected elsewhere (e.g., in an interrupt or another task monitoring the button)
                // and sets `imageUrl` appropriately. This state exists to wait for that process to complete.
                if (!imageUrl.isEmpty() || digitalRead(BUTTON_PIN) == LOW) { // Check if image URL is set or button is pressed
                    if (digitalRead(BUTTON_PIN) == LOW) {
                        delay(50); // Debounce delay
                        imageUrl = uploadImageToFirebase(); // Simulate capturing and uploading image
                    }
                    state = PROCESS_QUERY;
                }
                break;

            case PROCESS_QUERY:
                displayMode = PROCESSING;
                Serial.println(isImageNeeded && !imageUrl.isEmpty() ? "Processing query with image..." : "Processing text-only query...");
                // Fetch response from OpenAI (actual function implementation required)
                response = getResponseFromOpenAI(input, imageUrl, apiKey, content);
                displayMode = DISP_OUTPUT;
                Serial.println(response);
                // Display the response for 5-10 seconds
                vTaskDelay(pdMS_TO_TICKS(5000)); // Adjust as needed

                // Then revert to the normal watch face
                displayMode = DISPLAY_TIME_DATE;
                // Reset for the next query cycle
                isImageNeeded = false;
                imageUrl = ""; // Clear the image URL
                input = ""; // Clear the input
                state = WAIT_FOR_INPUT; // Reset state to initial
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay to prevent CPU hogging
    }
}


void setup() {
  Serial.begin(115200); // Increase baud rate for faster serial communication
  Serial.println("Starting...");
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //Initialize Camera
  cameraSetup();

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");  //PUT ON LCD
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi. Type your question or 'EXIT' to end the conversation.");

  initializeDisplay();

  // Create tasks for display and AI functionality
  xTaskCreate(DisplayTask, "Display Task", 10000, NULL, 1, NULL);
  xTaskCreate(AITask, "AI Task", 14000, NULL, 1, NULL);
}

// Inside loop() function of main.ino
void loop() {


}

