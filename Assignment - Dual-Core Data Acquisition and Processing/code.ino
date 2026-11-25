#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// ------------------------------------------------------------
// 1. Структура данных для очереди (добавлена влажность)
// ------------------------------------------------------------
struct SensorReading {
  uint32_t sequence;
  float temperature;
  float humidity;       // Расширение: второе измерение
  TickType_t producedAt;
};

// ------------------------------------------------------------
// 2. Общая статистика (добавлены влажность и счётчик потерь)
// ------------------------------------------------------------
struct SharedStatistics {
  uint32_t sampleCount;
  uint32_t droppedCount; // Расширение: счётчик отброшенных пакетов
  float tempSum;
  float tempAvg;
  float tempMin;
  float tempMax;
  float humAvg;          // Расширение: средняя влажность
  float humSum;
};

// ------------------------------------------------------------
// Конфигурация
// ------------------------------------------------------------
constexpr UBaseType_t QUEUE_LENGTH = 8;
constexpr uint32_t PRODUCER_PERIOD_MS = 500;
constexpr uint32_t QUEUE_WAIT_MS = 100;
constexpr uint32_t MUTEX_WAIT_MS = 50;
constexpr uint32_t MONITOR_PERIOD_MS = 3000;
constexpr uint32_t PRODUCER_STACK = 4096;
constexpr uint32_t CONSUMER_STACK = 4096;
constexpr float TEMP_ALARM_THRESHOLD = 24.2f; // Расширение: порог тревоги

QueueHandle_t readingQueue = nullptr;
SemaphoreHandle_t statisticsMutex = nullptr;

SharedStatistics statistics = {0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

void fatalError(const char *message) {
  Serial.printf("FATAL: %s\n", message);
  Serial.flush();
  while (true) {
    delay(1000);
  }
}

// Симуляция температуры (22.0 ... 24.8)
float makeSimulatedTemp(uint32_t sequence) {
  return 22.0f + static_cast<float>(sequence % 8U) * 0.4f;
}

// Симуляция влажности (45.0 ... 60.0)
float makeSimulatedHumidity(uint32_t sequence) {
  return 45.0f + static_cast<float>((sequence * 3U) % 16U);
}

// ============================================================
// TASK 1: SENSOR PRODUCER (Core 0)
// ============================================================
void sensorProducerTask(void *parameter) {
  (void)parameter;
  uint32_t sequence = 0;
  TickType_t lastWake = xTaskGetTickCount();

  for (;;) {
    SensorReading reading;
    reading.sequence = sequence++;
    reading.temperature = makeSimulatedTemp(reading.sequence);
    reading.humidity = makeSimulatedHumidity(reading.sequence);
    reading.producedAt = xTaskGetTickCount();

    if (xQueueSend(readingQueue, &reading, pdMS_TO_TICKS(QUEUE_WAIT_MS)) == pdPASS) {
      Serial.printf(
          "[Producer | Core %d] seq=%lu temp=%.2fC hum=%.1f%% queued=%u\n",
          xPortGetCoreID(),
          static_cast<unsigned long>(reading.sequence),
          reading.temperature,
          reading.humidity,
          static_cast<unsigned>(uxQueueMessagesWaiting(readingQueue)));
    } else {
      // Если очередь полна, безопасно увеличиваем счётчик потерь через мьютекс
      if (xSemaphoreTake(statisticsMutex, pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {
        statistics.droppedCount++;
        xSemaphoreGive(statisticsMutex);
      }
      Serial.printf(
          "[Producer | Core %d] QUEUE FULL! Dropped seq=%lu\n",
          xPortGetCoreID(),
          static_cast<unsigned long>(reading.sequence));
    }

    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(PRODUCER_PERIOD_MS));
  }
}

// ============================================================
// TASK 2: DATA CONSUMER (Core 1)
// ============================================================
void dataConsumerTask(void *parameter) {
  (void)parameter;
  SensorReading reading;

  for (;;) {
    if (xQueueReceive(readingQueue, &reading, portMAX_DELAY) == pdPASS) {
      SharedStatistics snapshot;
      bool updated = false;

      // Захват мьютекса для обновления общей статистики
      if (xSemaphoreTake(statisticsMutex, pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {
        if (statistics.sampleCount == 0) {
          statistics.tempMin = reading.temperature;
          statistics.tempMax = reading.temperature;
        } else {
          if (reading.temperature < statistics.tempMin) statistics.tempMin = reading.temperature;
          if (reading.temperature > statistics.tempMax) statistics.tempMax = reading.temperature;
        }

        statistics.sampleCount++;
        statistics.tempSum += reading.temperature;
        statistics.tempAvg = statistics.tempSum / statistics.sampleCount;

        statistics.humSum += reading.humidity;
        statistics.humAvg = statistics.humSum / statistics.sampleCount;

        snapshot = statistics;
        updated = true;

        // Освобождаем мьютекс ДО вывода в Serial
        xSemaphoreGive(statisticsMutex);
      }

      if (updated) {
        // Проверка порога тревоги (вне мьютекса)
        bool isAlarm = (reading.temperature > TEMP_ALARM_THRESHOLD);

        Serial.printf(
            "[Consumer | Core %d] seq=%lu avgT=%.2fC (min=%.2f max=%.2f) avgH=%.1f%% count=%lu %s\n",
            xPortGetCoreID(),
            static_cast<unsigned long>(reading.sequence),
            snapshot.tempAvg,
            snapshot.tempMin,
            snapshot.tempMax,
            snapshot.humAvg,
            static_cast<unsigned long>(snapshot.sampleCount),
            isAlarm ? "[ALARM: HIGH TEMP!]" : "");
      } else {
        Serial.printf("[Consumer | Core %d] mutex timeout for seq=%lu\n",
                      xPortGetCoreID(), static_cast<unsigned long>(reading.sequence));
      }
    }
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nLecture 12 - Extended Dual-Core Queue + Mutex Demo");
  Serial.printf("setup() running on core %d\n", xPortGetCoreID());

  readingQueue = xQueueCreate(QUEUE_LENGTH, sizeof(SensorReading));
  if (readingQueue == nullptr) fatalError("Could not create reading queue");

  statisticsMutex = xSemaphoreCreateMutex();
  if (statisticsMutex == nullptr) {
    vQueueDelete(readingQueue);
    fatalError("Could not create statistics mutex");
  }

  if (xTaskCreatePinnedToCore(sensorProducerTask, "SensorProducer", PRODUCER_STACK, nullptr, 2, nullptr, 0) != pdPASS) {
    fatalError("Could not create producer task on Core 0");
  }

  if (xTaskCreatePinnedToCore(dataConsumerTask, "DataConsumer", CONSUMER_STACK, nullptr, 2, nullptr, 1) != pdPASS) {
    fatalError("Could not create consumer task on Core 1");
  }

  Serial.println("Tasks created successfully.");
}

// ============================================================
// MONITOR (Arduino loop)
// ============================================================
void loop() {
  static uint32_t lastMonitorTime = 0;

  if (millis() - lastMonitorTime >= MONITOR_PERIOD_MS) {
    lastMonitorTime = millis();
    SharedStatistics snapshot;
    bool copied = false;

    if (xSemaphoreTake(statisticsMutex, pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {
      snapshot = statistics;
      copied = true;
      xSemaphoreGive(statisticsMutex);
    }

    if (copied) {
      Serial.printf(
          "[Monitor  | Core %d] count=%lu dropped=%lu | Temp avg=%.2fC min=%.2fC max=%.2fC | Hum avg=%.1f%%\n",
          xPortGetCoreID(),
          static_cast<unsigned long>(snapshot.sampleCount),
          static_cast<unsigned long>(snapshot.droppedCount),
          snapshot.tempAvg,
          snapshot.tempMin,
          snapshot.tempMax,
          snapshot.humAvg);
    } else {
      Serial.printf("[Monitor  | Core %d] mutex timeout\n", xPortGetCoreID());
    }
  }

  delay(10);
}
