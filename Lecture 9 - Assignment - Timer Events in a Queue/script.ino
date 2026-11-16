#include <Arduino.h>

#define TASK_DELAY_MS 500

#define QUEUE_LENGTH 4
#define TIMER_FREQ_HZ 1000000      // 1 MHz (1 тик = 1 мкс)
#define ALARM_TICKS 100000         // 100 000 мкс = 100 мс

QueueHandle_t eventQueue = NULL;
hw_timer_t *timer = NULL;

volatile uint32_t isrSeqCounter = 0;

void ARDUINO_ISR_ATTR onTimer() {
  isrSeqCounter++;
  uint32_t seqToSend = isrSeqCounter;

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xQueueSendFromISR(eventQueue, &seqToSend, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

void consumerTask(void *pvParameters) {
  uint32_t receivedSeq = 0;

  while (true) {
    if (xQueueReceive(eventQueue, &receivedSeq, portMAX_DELAY) == pdTRUE) {
      Serial.println(receivedSeq);

      vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_MS));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); 

  eventQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));
  if (eventQueue == NULL) {
    Serial.println("Failed to create queue!");
    while (true);
  }

  xTaskCreate(
      consumerTask,
      "ConsumerTask",
      2048,
      NULL,
      2, 
      NULL
  );

  timer = timerBegin(TIMER_FREQ_HZ);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, ALARM_TICKS, true, 0);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
