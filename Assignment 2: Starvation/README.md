# Starvation and Prevention in FreeRTOS (ESP32-S3)

## Part A – Understanding the Problem

**1. Which tasks may experience starvation? Explain why.**  
Both `Task_Display` (Priority 2) and `Task_Log` (Priority 1) will experience starvation. Because `Task_Control` has the highest priority (3) and runs continuously in a loop without blocking, the FreeRTOS scheduler always keeps it in the `Running` state on that core, preventing lower-priority tasks from ever getting CPU time.

**2. Why is it dangerous for a high-priority task to run continuously without blocking?**  
In FreeRTOS, the scheduler is preemptive and strictly prioritizes the highest-priority ready task (with no built-in priority aging). If a high-priority task never blocks, lower-priority tasks are completely starved, which can freeze UI updates, stop logging, or even trigger watchdog resets.

**3. Give two ways to allow lower-priority tasks to get CPU time without changing task priorities.**  
* Call `vTaskDelay()` (or `vTaskDelayUntil()`) periodically inside the high-priority loop so it temporarily enters the `Blocked` state.
* Make the high-priority task event-driven by blocking on a queue, semaphore, or task notification until new sensor data actually arrives.

---

## Part B – Running and Modifying the ESP32 Demo

### Step 1 – Observe Starvation (`STARVATION_FIX 0`)

**Serial Monitor Output:**
```text
[ВСТАВЬ СЮДА РЕАЛЬНЫЙ ВЫВОД ИЗ SERIAL MONITOR ДЛЯ STARVATION_FIX 0 (ГДЕ ВИДНЫ И BUSY, И SLEEP)]
