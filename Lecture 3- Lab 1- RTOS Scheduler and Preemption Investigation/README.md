# Lecture 02 - Lab 2: Preemption and Task Notification

## 1. Purpose and Setup
This assignment investigates how the FreeRTOS scheduler handles task states, priority-based scheduling, and preemption on an ESP32-S3 microcontroller.

* **Board:** ESP32-S3-DevKitC-1 v1.1
* **Core:** Core 1 (both tasks pinned)
* **Serial Monitor:** 115200 baud

## 2. Preemption Demonstration Output
When Task A sends a notification to Task B via `xTaskNotifyGive()`, Task B immediately preempts Task A (when Task B has a higher priority) and prints its message right after the notification call.

![Serial Monitor Screenshot](images/serial-monitor.png)

## 3. Prediction and Observation Table

| Scenario | Prediction before test | Actual Observation | Match? Why? |
| :--- | :--- | :--- | :--- |
| **A: Task A = 1, Task B = 2** | Task B will immediately preempt Task A when notified because Task B has a higher priority. | Task B interrupted Task A instantly upon receiving the notification. | **Yes.** Higher-priority Ready tasks preempt lower-priority Running tasks. |
| **B: Task A = 1, Task B = 1** | Task B will not preempt Task A immediately. Task A will finish its message before Task B runs. | Task A completed its entire printing output before Task B executed. | **Yes.** Equal-priority tasks do not preempt automatically unless time-slicing or explicit yield occurs when Task A enters Blocked state. |
| **C: Task A = 1, Task B = 3** | Task B will immediately preempt Task A just like in Scenario A. | Task B interrupted Task A instantly upon notification. | **Yes.** Any priority higher than the currently running task triggers preemption. |

## 4. Explanation of Scheduler Behaviour

### Task States
FreeRTOS tasks transition between three main states:
1. **Running:** The task currently executing code on the assigned CPU core.
2. **Ready:** A task that is prepared to run and waiting for CPU time.
3. **Blocked:** A task waiting for an event, timer (`vTaskDelay`), or notification (`ulTaskNotifyTake`).

### Scheduler & Notification Analysis
Initially, **Task B is in the Blocked state** because it calls `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)` and waits indefinitely for a notification.

When Task A calls `xTaskNotifyGive(taskBHandle)`:
1. The notification unblocks Task B, transitioning it from **Blocked** to **Ready**.
2. In **Scenario A and C**, Task B has a higher priority than Task A. The FreeRTOS scheduler immediately halts Task A (moving Task A to the **Ready** state) and puts Task B into the **Running** state. This phenomenon is called **preemption**.
3. In **Scenario B**, because both tasks share equal priority (1), Task B enters the **Ready** state but does not preempt Task A. Task A continues in the **Running** state until it blocks on `vTaskDelay()`, at which point the scheduler transfers execution to Task B.
4. Once Task B finishes printing, it blocks again on `ulTaskNotifyTake()`. The scheduler then selects Task A to return to the **Running** state. Task A resumes execution precisely from where its program counter was suspended.

## 5. File Structure
* `SchedulerDemo.ino` - The complete Arduino/FreeRTOS source code.
* `README.md` - Assignment documentation and state analysis.
* `images/serial-monitor.png` - Screenshot of Serial Monitor preemption.
