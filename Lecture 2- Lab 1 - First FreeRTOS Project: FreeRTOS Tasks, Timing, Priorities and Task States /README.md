# Lecture 02 - Lab 1: First FreeRTOS Project

## 1. Hardware and Software
* **Board:** ESP32-S3-DevKitC-1 v1.1
* **Onboard Output:** RGB LED (GPIO 38)
* **Software:** Arduino IDE 2.x (ESP32 Board Package)
* **Serial Monitor:** 115200 baud

## 2. Baseline Implementation
Both Task A (Serial heartbeat every 1000 ms) and Task B (RGB LED toggle every 500 ms) are pinned to Core 1 and execute periodically using `vTaskDelay(pdMS_TO_TICKS(...))`.

![Serial Monitor Screenshot](images/serial-monitor.png)

## 3. Prediction and Observation Table

| Scenario | Prediction before test | Actual observation | Did it match? Why? |
| :--- | :--- | :--- | :--- |
| **A. Task A: 1, Task B: 1** | Both tasks will run periodically because they use `vTaskDelay()`. | Both tasks executed normally (Task A every 1000ms, Task B every 500ms). | **Yes.** Equal priorities allow time-slicing and voluntary yield via delay. |
| **B. Task A: 2, Task B: 1** | Both tasks will still run because Task A blocks during `vTaskDelay()`. | Both tasks executed normally without any visible timing changes. | **Yes.** Higher priority does not dominate if the task enters the Blocked state regularly. |
| **C. Task A: 1, Task B: 2** | Both tasks will continue running periodically without issues. | Both tasks executed normally without any visible timing changes. | **Yes.** Task B yields execution to Task A whenever Task B enters the Blocked state. |

## 4. Priority Experiments
Varying the task priority values between 1 and 2 did not visibly alter execution periods. This occurs because both tasks spend the majority of their time in the **Blocked** state while waiting for `vTaskDelay()`. A higher-priority task only occupies the CPU when it is in the **Ready** or **Running** state.

## 5. Starvation Experiment
When Task A priority was set to 2, Task B to 1, and `vTaskDelay()` was removed from Task A:
* **Observation:** Serial output completely froze, and the RGB LED remained stuck ON without blinking.
* **Explanation:** Without `vTaskDelay()`, Task A never enters the Blocked state and continuously occupies Core 1 in the Running state. Because Task A has a higher priority (2) than Task B (1), the FreeRTOS scheduler never grants CPU time to Task B. Task B remains trapped in the Ready state, causing complete task starvation.

> **Confirmation:** The starvation experiment was temporary. All `vTaskDelay()` calls have been fully restored and verified in the submitted codebase.

## 6. Ready, Running and Blocked Explanation
FreeRTOS tasks transition between three primary states:
1. **Running:** The task currently executing instructions on the CPU core.
2. **Blocked:** A task waiting for a temporal event (`vTaskDelay()`) or resource.
3. **Ready:** A task prepared to execute, waiting for scheduler selection.

When a task executes `vTaskDelay()`, it transitions from **Running** to **Blocked**, relinquishing control of the core. When the delay timer expires, the tick interrupt transitions the task back to the **Ready** state.

The FreeRTOS scheduler always selects the highest-priority **Ready** task to enter the **Running** state. During normal operation, when a higher-priority task blocks, the scheduler picks the lower-priority Ready task. However, if the higher-priority task never enters the Blocked state (as demonstrated in the starvation test), it stays continuously Ready/Running, preventing lower-priority tasks from ever reaching the Running state.

## 7. Final Restored Configuration
* **Task A Priority:** 1
* **Task B Priority:** 1
* **Core Assignment:** Core 1 (both tasks)
* **Status:** All delays active and verified.
