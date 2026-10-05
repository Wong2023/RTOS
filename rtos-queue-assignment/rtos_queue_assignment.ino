#include <Arduino.h>

QueueHandle_t temperatureQueue;


// --------------------------------------------------
// Sensor Task
// --------------------------------------------------

void sensorTask(void *parameter)
{
  int temperature = 20;

  while (1)
  {
    if (xQueueSend(temperatureQueue, &temperature, 0) != pdPASS) {
      Serial.println("Failed to send temperature to queue (queue full)");
    }

    temperature++;


    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


// --------------------------------------------------
// Display Task
// --------------------------------------------------

void displayTask(void *parameter){
  int receivedTemperature;

  while (1)
  {
    // TODO 3:
    // Receive a temperature value from the queue
     if (xQueueReceive(temperatureQueue, &receivedTemperature, 0) == pdPASS) {
      Serial.print("Temperature: ");
      Serial.print(receivedTemperature);
      Serial.println(" C");
    }


    // TODO 4:
    // If a value was received, print:
    // Temperature: XX C


    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);
  temperatureQueue = xQueueCreate(5, sizeof(int));

  // TODO 5:
  // Create a queue that can store 5 integers

xTaskCreate(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    NULL
  );
  // TODO 6:
  // Create the Sensor Task

xTaskCreate(
    displayTask,
    "Display Task",
    2048,
    NULL,
    1,
    NULL
  );
  // TODO 7:
  // Create the Display Task
}


void loop()
{
}
