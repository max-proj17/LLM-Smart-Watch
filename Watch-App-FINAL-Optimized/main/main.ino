// Author: Max Finch
// Date: 02/09/2024


#include <WiFi.h>
#include "openai_functions.h"
#include "espcam_functions.h"
#include "display_functions.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define BUTTON_PIN  2


const char* ssid = "Transponder Snail";
const char* password = "";
const char* apiKey = "";
const char* content = "\"You are an AI assistant named Alex. You sound professional and don't talk more than needed. You are able to explain things simply and can give real life examples to complex concepts asked by the user.\"";  


enum DisplayMode {
  QUESTION_CYCLE,
  DISPLAY_TIME_DATE,
  IMAGE_YN,
  TAKE_IMG,
  PROCESSING,
  DISP_OUTPUT
};

enum TaskState {
  INITIAL,
  WAIT_FOR_INPUT,
  WAIT_FOR_IMAGE_DECISION,
  WAIT_FOR_IMAGE_CAPTURE,
  PROCESS_QUERY
};

TaskState state = INITIAL;
String response = "";
const char* questions[] = {
    "Who are you?",
    "What is in the image?",
    "What is an ESP32?"
};

const int questionsCount = sizeof(questions) / sizeof(questions[0]);
int currentQuestionIndex = 0;
unsigned long buttonPressStartTime = 0;
bool isButtonBeingPressed = false;
volatile DisplayMode displayMode = DISPLAY_TIME_DATE; // Default to showing time and date


void DisplayTask(void * parameter) {
  for (;;) {

    if (displayMode == QUESTION_CYCLE)
    {
        delay(50);
    }else if (displayMode == DISPLAY_TIME_DATE) {
      updateDisplay();
    } else if (displayMode == IMAGE_YN) {

    } else if (displayMode == TAKE_IMG) {
      displayText("Take Image", 2, 12, 24);

    }else if (displayMode == PROCESSING) {
      displayText("Processing...", 2, 12, 24);

    }else if (displayMode == DISP_OUTPUT) {
      displayText(response);

    }

    vTaskDelay(pdMS_TO_TICKS(1000)); // Refresh rate
  }
}


void AITask(void * parameter) {
    
    
    TaskState state = INITIAL;
    static String imageUrl = "";
    static String input = "";
    bool isButtonBeingPressed = false;
    unsigned long buttonPressStartTime = 0;
    bool isImageNeeded = false;

    for (;;) {
  
        switch (state) {
            case INITIAL:
              // Stay in this state until the button is pressed to start the interaction
              if (digitalRead(BUTTON_PIN) == LOW) {
                // Debounce and ensure it's a genuine press
                delay(50); 
                if (digitalRead(BUTTON_PIN) == LOW) {
                  displayMode = QUESTION_CYCLE; // Move to displaying questions
                  state = WAIT_FOR_INPUT; // Update state to start cycling questions
                  displayText(questions[currentQuestionIndex], 2, 12, 24); // Show the first question immediately
                }
              }
              vTaskDelay(pdMS_TO_TICKS(100));
              break;
            case WAIT_FOR_INPUT:

              if (digitalRead(BUTTON_PIN) == LOW ) { // Button is pressed
                if (!isButtonBeingPressed) { // Initial press
                    isButtonBeingPressed = true;
                    buttonPressStartTime = millis();
                } else if (millis() - buttonPressStartTime > 1000) { // Long press
                    // Select the current question
                    input = questions[currentQuestionIndex];
                    Serial.println(input); 
                    isButtonBeingPressed = false; // Reset press detection
                    displayMode = IMAGE_YN;
                    state = WAIT_FOR_IMAGE_DECISION;
                    displayText(isImageNeeded ? "Image? Yes" : "Image? No", 2, 12, 24);
                    
                }
              } else if (isButtonBeingPressed) { // Button was released before a long press
                  isButtonBeingPressed = false;
                  currentQuestionIndex = (currentQuestionIndex + 1) % questionsCount; // Cycle to the next question
                  displayText(questions[currentQuestionIndex], 2, 12, 24);
                  Serial.println(questions[currentQuestionIndex]); // Debug print, show next question
  
              }
              vTaskDelay(pdMS_TO_TICKS(100));
              break;
                
            case WAIT_FOR_IMAGE_DECISION:
              
              if (digitalRead(BUTTON_PIN) == LOW ) { // Button is pressed
                    if (!isButtonBeingPressed) {
                        isButtonBeingPressed = true;
                        buttonPressStartTime = millis();
                    } else if (millis() - buttonPressStartTime > 1000) { // Long press
                      state = isImageNeeded ? WAIT_FOR_IMAGE_CAPTURE : PROCESS_QUERY;
                      displayMode = isImageNeeded ? TAKE_IMG : PROCESSING;
                      delay(1000);
                      
                    }
              } else if (isButtonBeingPressed) { // Button was released before a long press
                  isButtonBeingPressed = false;
                  isImageNeeded = !isImageNeeded;
                  displayText(isImageNeeded ? "Image? Yes" : "Image? No", 2, 12, 24);
  
              }
                  
              vTaskDelay(pdMS_TO_TICKS(100));
              break;

            case WAIT_FOR_IMAGE_CAPTURE:
                // Assuming button press is detected in WAIT_FOR_IMAGE_DECISION
                // and sets `imageUrl` appropriately. This state exists to wait for that process to complete.
                
                if (!imageUrl.isEmpty() || digitalRead(BUTTON_PIN) == LOW) { // Check if image URL is set or button is pressed
                    if (digitalRead(BUTTON_PIN) == LOW) {
                        delay(50); // Debounce delay
                        
                        Serial.println("Taking image....");
                        imageUrl = uploadImageToFirebase(); // Simulate capturing and uploading image
                        Serial.println("Sent the image....");
                        state = PROCESS_QUERY;
                    }
                    
                }
                vTaskDelay(pdMS_TO_TICKS(100));
                break;

            case PROCESS_QUERY:
                displayMode = PROCESSING;
                Serial.println(isImageNeeded && !imageUrl.isEmpty() ? "Processing query with image..." : "Processing text-only query...");
                // Fetch response from OpenAI 
                response = getResponseFromOpenAI(input, imageUrl, apiKey, content);
                displayMode = DISP_OUTPUT;
                Serial.println(response);
                // Display the response for 5-10 seconds

                for(;;){
                  if(digitalRead(BUTTON_PIN) == LOW){
                    if (digitalRead(BUTTON_PIN) == LOW) {
                      delay(50); // Debounce delay
                      // Then revert to the normal watch face
                      displayMode = DISPLAY_TIME_DATE;
                      // Reset for the next query cycle
                      isImageNeeded = false;
                      imageUrl = ""; // Clear the image URL
                      input = ""; // Clear the input
                      state = INITIAL; // Reset state to initial
                       vTaskDelay(pdMS_TO_TICKS(100));
                      break;
                    }
                  }
                }
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
  Serial.print("Connecting to WiFi...");
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

// Nothing inside loop() because everything is a task
void loop() {


}

