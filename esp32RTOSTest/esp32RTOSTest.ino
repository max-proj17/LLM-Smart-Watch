#include <Arduino.h>

// Test LED PIN
const int ledPin = 2;

// The first task: Blink LED
void TaskBlinkLED(void *pvParameters) {
  pinMode(ledPin, OUTPUT);

  // Variable to hold the stack high water mark
  UBaseType_t uxHighWaterMark;

  while (1) {
    digitalWrite(ledPin, HIGH);
    delay(1000); 
    digitalWrite(ledPin, LOW);
    delay(1000); 

    // Check the stack high water mark. Basically how much stack space was used.
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("LED Task Stack High Water Mark: ");
    Serial.println(uxHighWaterMark);
  }
}

// Second task: Print message
void TaskSerialPrint(void *pvParameters) {
  
  UBaseType_t uxHighWaterMark;
  // Casting pvParameters to the correct type
  //TaskData *taskData = (TaskData*) pvParameters; # TaskData is an arbitrary type

  while (1) {
    Serial.println("Testing ESP32 FreeRTOS");
    delay(2000); 

    
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("Serial Print Task Stack High Water Mark: ");
    Serial.println(uxHighWaterMark);
  }
}

void setup() {
  Serial.begin(115200);

  // Create Task 1
  xTaskCreate(
    TaskBlinkLED,      // Task function
    "TaskBlinkLED",    // Name of the task 
    1000,              // Stack size in words
    NULL,              // Task input parameter
    1,                 // Priority of the task
    NULL);             // Task handle

  // Create Task 2
  xTaskCreate(
    TaskSerialPrint,   
    "TaskSerialPrint",  
    1000,              
    NULL,              
    1,               
    NULL);             
}

void loop() {
  // Empty loop
}
