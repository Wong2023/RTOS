#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
SemaphoreHandle_t mutex;

//*****************************************************************************
// Tasks

void blinkLED(void *parameters) {

  xSemaphoreTake(mutex, portMAX_DELAY);

  // Копируем значение
  int num = *(int *)parameters;

  Serial.print("Received: ");
  Serial.println(num);

  xSemaphoreGive(mutex);

  pinMode(led_pin, OUTPUT);

  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(num / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}



void setup() {

  int delay_arg;

  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println();
  Serial.println("---FreeRTOS Mutex Challenge---");
  Serial.println("Enter a number for delay (milliseconds)");

  while (Serial.available() <= 0);

  delay_arg = Serial.parseInt();
  Serial.print("Sending: ");
  Serial.println(delay_arg);

  mutex = xSemaphoreCreateMutex();

  xSemaphoreTake(mutex, portMAX_DELAY);

  xTaskCreatePinnedToCore(blinkLED,
                          "Blink LED",
                          2048,
                          (void *)&delay_arg,
                          1,
                          NULL,
                          app_cpu);

  xSemaphoreGive(mutex);

  vTaskDelay(10 / portTICK_PERIOD_MS); // Даем квант времени задаче blinkLED
  xSemaphoreTake(mutex, portMAX_DELAY);

  Serial.println("Done!");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
