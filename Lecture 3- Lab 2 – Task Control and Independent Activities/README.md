# Lecture 03 - Lab 2: Task Control and Independent Activities

## 1. Setup
* **Board:** ESP32-S3-DevKitC-1 v1.1
* **Output:** RGB LED (GPIO 38)
* **IDE & Serial:** Arduino IDE 2.x | 115200 baud | Core 1

## 2. Task Design & Handles
The application runs two independent tasks pinned to Core 1:
* **Serial Task (Priority 1):** Validates input (`250`, `500`, `1000`, `suspend`, `resume`), updates `blinkInterval`, and manages the LED task state.
* **RGB LED Task (Priority 1):** Toggles the onboard RGB LED using `neopixelWrite()` and delays using `blinkInterval`.

3. Demonstration OutputsRunning: Normal blinking and interval changes (250, 500, 1000).Suspended: LED task paused via vTaskSuspend(), Serial Task remains fully active.Resumed: LED task resumed via vTaskResume().
4. Prediction & Observation TableSituationPrediction before testActual ObservationMatch? Why?LED task runningBoth tasks execute periodically using vTaskDelay().LED blinks normally; Serial accepts commands.Yes. Both tasks yield CPU time when entering the Blocked state.LED task suspendedLED stops blinking; Serial Task continues working.LED froze instantly; Serial Task remained responsive.Yes. vTaskSuspend() removes the LED Task from the Ready list.LED task resumedLED starts blinking again at the current interval.LED resumed blinking at the last set speed.Yes. vTaskResume() transitions the LED Task back to the Ready state.
5. Task State AnalysisSituation 1 (Normal Operation):Serial Task: Blocked / Running (Transitions to Blocked during vTaskDelay(100)).RGB LED Task: Blocked / Running (Transitions to Blocked during vTaskDelay(blinkInterval)).Situation 2 (LED Task Suspended):Serial Task: Blocked / Running (Continues normal periodic execution).RGB LED Task: Suspended (Completely ignored by the scheduler).Situation 3 (LED Task Resumed):Serial Task: Blocked / Running.RGB LED Task: Ready $\rightarrow$ Running (Calling vTaskResume() returns the task to Ready, then Running when CPU time is granted).
6. ReflectionTask handles: References/pointers used to directly target and control specific task instances in memory.Suspend: vTaskSuspend(ledTaskHandle) transitions the LED task to the Suspended state, removing it from scheduler evaluation without consuming CPU cycles.Resume: vTaskResume(ledTaskHandle) transitions the task back to the Ready state so the scheduler can run it again.Independent tasks & states: The Serial Task continues working because FreeRTOS manages task states independently. A task in the Suspended state consumes zero execution time and does not block other Ready or Running tasks.
