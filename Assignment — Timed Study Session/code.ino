#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// --- Hardware Definitions ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// Standard I2C pins for ESP32-S3 (can vary by specific board model, usually 8/9 or 4/5)
#define I2C_SDA 13
#define I2C_SCL 14

// ESP32-S3 DevKitC-1 Onboard RGB LED (WS2812)
#define PIN_NEOPIXEL 38
#define NUMPIXELS 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// --- FreeRTOS Definitions ---
TimerHandle_t timerUpdate;
TimerHandle_t timerComplete;
TaskHandle_t appTaskHandle;

// Notification bits
#define BIT_UPDATE   (1 << 0)
#define BIT_COMPLETE (1 << 1)

// --- Timer Callbacks (Keep them short!) ---
void vTimerUpdateCallback(TimerHandle_t xTimer) {
  // Notify application task to update display
  xTaskNotify(appTaskHandle, BIT_UPDATE, eSetBits);
}

void vTimerCompleteCallback(TimerHandle_t xTimer) {
  // Notify application task that session is done
  xTaskNotify(appTaskHandle, BIT_COMPLETE, eSetBits);
}

// --- Display Helper ---
void updateDisplay(const char* status, int remainingSeconds) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(status);
  
  if (remainingSeconds >= 0) {
    display.setCursor(0, 30);
    display.print("Remaining: ");
    display.print(remainingSeconds);
    display.println(" s");
  }
  display.display();
}

// --- Application Task ---
void appTaskCode(void *pvParameters) {
  uint32_t startTime = 0;
  bool isRunning = false;
  
  // Max block time of 50ms ensures we poll Serial input frequently
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(50); 

  while (1) {
    // 1. Check Serial Input
    if (Serial.available() > 0) {
      char c = Serial.read();
      // Ignore characters if running, only respond to 's' when idle
      if (c == 's' && !isRunning) {
        isRunning = true;
        startTime = millis();
        
        // Turn ON RGB LED (Green)
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.show();
        
        updateDisplay("Session running", 10);
        
        // Start timers
        xTimerStart(timerUpdate, 0);
        xTimerStart(timerComplete, 0);
      }
    }

    // 2. Check Task Notifications from Timers
    uint32_t notifiedValue = 0;
    if (xTaskNotifyWait(0x00, ULONG_MAX, &notifiedValue, xMaxBlockTime) == pdTRUE) {
      
      // Handle completion FIRST so a late update doesn't overwrite it
      if (notifiedValue & BIT_COMPLETE) {
        isRunning = false;
        
        // Stop the auto-reload update timer
        xTimerStop(timerUpdate, 0);
        
        // Turn OFF RGB LED
        pixels.clear();
        pixels.show();
        
        updateDisplay("Session complete", -1);
      } 
      else if ((notifiedValue & BIT_UPDATE) && isRunning) {
        // Calculate remaining time from elapsed time to prevent drift
        uint32_t elapsed = (millis() - startTime) / 1000;
        int remaining = 10 - elapsed;
        
        if (remaining < 0) remaining = 0;
        
        updateDisplay("Session running", remaining);
      }
    }
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  while(!Serial) { delay(10); } // Wait for serial console

  // Initialize I2C and OLED
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  updateDisplay("Ready", -1);
  display.println("\nSend 's' to start");
  display.display();

  // Initialize RGB LED
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Create Auto-reload timer (1000 ms)
  timerUpdate = xTimerCreate(
    "UpdateTimer", 
    pdMS_TO_TICKS(1000), 
    pdTRUE, // Auto-reload
    (void *)0, 
    vTimerUpdateCallback
  );

  // Create One-shot timer (10000 ms)
  timerComplete = xTimerCreate(
    "CompleteTimer",
pdMS_TO_TICKS(10000), 
    pdFALSE, // One-shot
    (void *)1, 
    vTimerCompleteCallback
  );

  if (timerUpdate == NULL || timerComplete == NULL) {
    Serial.println("Failed to create timers");
    for (;;);
  }

  // Create Application Task
  xTaskCreate(
    appTaskCode,    // Task function
    "AppTask",      // Task name
    4096,           // Stack size
    NULL,           // Parameters
    1,              // Priority
    &appTaskHandle  // Task handle
  );

  // FreeRTOS scheduler starts automatically in Arduino ESP32 core
}

void loop() {
  // Empty. Everything is handled by the FreeRTOS tasks and timers.
  vTaskDelete(NULL); 
}
