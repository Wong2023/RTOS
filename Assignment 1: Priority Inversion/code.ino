/*
  Real Time Systems (4_EFS8050) - Lecture 10
  Demo 3 - Priority Inversion and Priority Inheritance

  OBJECTIVE
    Show how a medium-priority task can delay a high-priority task that waits
    for a lock held by a low-priority task, and how a FreeRTOS mutex
    (priority inheritance) prevents it. A binary semaphore does NOT do this.

  HARDWARE
    ESP32-S3 DevKitC-1 + USB cable. No other parts.

  HOW TO RUN
    Board: "ESP32S3 Dev Module". Serial Monitor: 115200 baud.
    If you use the board's native USB port, set Tools > USB CDC On Boot > Enabled.
    Run once with USE_MUTEX 0, then set USE_MUTEX 1, upload, press RESET, compare.

  SCENARIO (all tasks pinned to core 1, so priorities decide who runs)
    Task_L priority 1: takes the lock at t=0, works 300 ms inside it
    Task_H priority 3: wakes at t=100 ms and needs the same lock
    Task_M priority 2: wakes at t=150 ms, works 1000 ms, does NOT use the lock

  NOTE: USE_MUTEX 0 shows the problem on purpose. Use a mutex in real code.
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define USE_MUTEX 1          // 0 = binary semaphore (inversion), 1 = mutex (inheritance)

SemaphoreHandle_t lockHandle;
uint32_t t0;

// Burns CPU time in 10 ms slices, so time spent preempted is not counted as work.
void busyWorkMs(uint32_t ms) {
  for (uint32_t done = 0; done < ms; done += 10) {
    uint32_t s = millis();
    while (millis() - s < 10) { }
  }
}

void taskL(void* pv) {
  Serial.printf("[%5lu ms] L: taking lock\n", (unsigned long)(millis() - t0));
  xSemaphoreTake(lockHandle, portMAX_DELAY);
  for (int i = 1; i <= 3; i++) {
    busyWorkMs(100);
    Serial.printf("[%5lu ms] L: critical section %d/3, my priority = %u\n",
                  (unsigned long)(millis() - t0), i, (unsigned)uxTaskPriorityGet(NULL));
  }
  xSemaphoreGive(lockHandle);
  vTaskDelete(NULL);
}

void taskM(void* pv) {
  vTaskDelay(pdMS_TO_TICKS(150));
  Serial.printf("[%5lu ms] M: starts 1000 ms of work (does not use the lock)\n",
                (unsigned long)(millis() - t0));
  busyWorkMs(2000);
  Serial.printf("[%5lu ms] M: finished\n", (unsigned long)(millis() - t0));
  vTaskDelete(NULL);
}

void taskH(void* pv) {
  vTaskDelay(pdMS_TO_TICKS(100));
  uint32_t requested = millis();
  Serial.printf("[%5lu ms] H: needs the lock\n", (unsigned long)(requested - t0));
  xSemaphoreTake(lockHandle, portMAX_DELAY);
  Serial.printf("[%5lu ms] H: got the lock after waiting %lu ms\n",
                (unsigned long)(millis() - t0), (unsigned long)(millis() - requested));
  busyWorkMs(50);
  xSemaphoreGive(lockHandle);
  Serial.printf("[%5lu ms] H: done\n", (unsigned long)(millis() - t0));
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
#if USE_MUTEX
  Serial.println("\n=== Demo 3: Priority Inversion | USE_MUTEX 1 (mutex, priority inheritance) ===");
  lockHandle = xSemaphoreCreateMutex();            // starts "free"
#else
  Serial.println("\n=== Demo 3: Priority Inversion | USE_MUTEX 0 (binary semaphore, NO inheritance) ===");
  lockHandle = xSemaphoreCreateBinary();           // starts "empty"
  xSemaphoreGive(lockHandle);                      // make it available
#endif

  t0 = millis();
  xTaskCreatePinnedToCore(taskL, "Task_L", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskM, "Task_M", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(taskH, "Task_H", 4096, NULL, 3, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);    // loop() is not needed; remove its task
}
