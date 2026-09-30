#define RGB_BUILTIN 38
#define RGB_BRIGHTNESS 50
#define PROCESSOR_CORE 1

volatile uint32_t blinkInterval = 500;

TaskHandle_t serialTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;

void serialTask(void *parameter) {
  String inputString = "";
  for (;;) {
    if (Serial.available() > 0) {
      inputString = Serial.readStringUntil('\n');
      inputString.trim(); 

      if (inputString == "250" || inputString == "500" || inputString == "1000") {
        blinkInterval = inputString.toInt();
        Serial.print("SUCCESS: Blink interval updated to ");
        Serial.print(blinkInterval);
        Serial.println(" ms");
      } 
      else if (inputString == "suspend") {
        if (ledTaskHandle != NULL) {
          vTaskSuspend(ledTaskHandle);
          Serial.println("SUCCESS: LED Task SUSPENDED.");
        }
      } 
      else if (inputString == "resume") {
        if (ledTaskHandle != NULL) {
          vTaskResume(ledTaskHandle);
          Serial.println("SUCCESS: LED Task RESUMED.");
        }
      } 
      else {
        Serial.print("ERROR: Invalid command '");
        Serial.print(inputString);
        Serial.println("'. Allowed values: 250, 500, 1000, suspend, resume");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100)); 
  }
}

void ledTask(void *parameter) {
  for (;;) {
    neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkInterval));

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkInterval));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("==================================================");
  Serial.println("FreeRTOS Task Control & Shared Variable App");
  Serial.println("Commands: 250, 500, 1000, suspend, resume");
  Serial.println("==================================================");

  xTaskCreatePinnedToCore(serialTask, "Serial Task", 2048, NULL, 1, &serialTaskHandle, PROCESSOR_CORE);
  xTaskCreatePinnedToCore(ledTask, "LED Task", 2048, NULL, 1, &ledTaskHandle, PROCESSOR_CORE);
}

void loop() {
}
