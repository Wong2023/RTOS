#define PROCESSOR_CORE 1

TaskHandle_t taskBHandle = NULL;

void taskA(void *parameter) {
  for (;;) {
    Serial.print("Task A is printing slowly... ");

    xTaskNotifyGive(taskBHandle);

    Serial.println("Task A continues and finishes.");

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void taskB(void *parameter) {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    Serial.println(">>> [Task B IS RUNNING!] <<<");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(taskB, "Task B", 2048, NULL, 2, &taskBHandle, PROCESSOR_CORE);
  xTaskCreatePinnedToCore(taskA, "Task A", 2048, NULL, 1, NULL, PROCESSOR_CORE);
}

void loop() {
}
