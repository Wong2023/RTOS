# Lab Report: ESP32-S3 Onboard RGB LED Control

## Overview
This laboratory work focuses on setting up the Arduino IDE environment for the ESP32-S3 board and controlling its onboard addressable RGB LED using the `neopixelWrite()` function.

## Hardware & Environment
* **Board:** ESP32-S3-DevKitC-1 (Onboard RGB LED GPIO 38)
* **Software:** Arduino IDE 2.x
* **Core:** esp32 by Espressif Systems

---

## Serial Monitor Check
Below is the screenshot confirming successful serial communication at 115200 baud rate:

---

## Tasks & Observations

### Task A: Change the Colour
* **Change made:** Modified the RGB channel values inside `neopixelWrite(RGB_BUILTIN, R, G, B)`.
* **Effect:** Setting `(0, 50, 0)` lights up the LED in green, `(0, 0, 50)` in blue, and `(50, 50, 50)` combines all three colors to produce white light.

### Task B: Change the Blink Rate
* **Change made:** Decreased the `delay()` parameters from `1000` ms to `250` ms.
* **Effect:** The LED blinks four times faster compared to the default setup, creating a rapid flashing effect.

### Task C: Create an RGB Cycle
* **Change made:** Created a sequential loop switching through Red $\rightarrow$ Green $\rightarrow$ Blue $\rightarrow$ Off with a `500` ms delay between each state.
* **Effect:** The LED smoothly transitions through primary colors before resetting.

### Task D: Custom Pattern & Brightness Tuning
* **Change made:** Implemented a custom 5-color animation sequence using individual RGB brightness adjustments for each frame:
  1. **Dim Red:** `(20, 0, 0)` — soft low-power red.
  2. **Max Green:** `(0, 255, 0)` — full brightness green.
  3. **Low Blue:** `(0, 0, 30)` — subtle blue shade.
  4. **Custom White:** `(16, 15, 17)` — balanced low-brightness neutral white.
  5. **Amber / Orange:** `(60, 20, 0)` — warm custom color tone.
  6. **Off:** `(0, 0, 0)` — complete shutdown for 500 ms.
* **Effect:** Demonstrates precise color blending and independent brightness control per step without relying on global limits.
