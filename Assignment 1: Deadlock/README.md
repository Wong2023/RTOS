# Deadlock and Prevention in FreeRTOS (ESP32-S3)

## Part A – Understanding the Problem

**1. Which two tasks can cause a deadlock? Explain how they can end up waiting for each other.**  
`Task_Sensor` and `Task_Logger` can cause a deadlock because they acquire the mutexes in opposite orders. If `Task_Sensor` locks `i2cMutex` and `Task_Logger` locks `uartMutex` at the same time, `Task_Sensor` will wait forever for `uartMutex` while `Task_Logger` waits forever for `i2cMutex`.

**2. Can Task_Display create a circular wait by itself? Explain why or why not.**  
No, `Task_Display` cannot create a circular wait because it only uses a single resource (`i2cMutex`). Since it never holds one mutex while waiting for another, it does not satisfy the "hold and wait" condition required for a cycle.

**3. Which Coffman condition is removed when all tasks acquire mutexes in the same order?**  
Acquiring mutexes in a consistent global order eliminates the **Circular Wait** condition.

---

## Part B – Running and Modifying the ESP32 Demo

### Step 1 – Observe the Deadlock (`DEMO_MODE 0`)

**Serial Monitor Output:**
```text
[ВСТАВЬТЕ СЮДА ~8 СТРОК ВЫВОДА ИЗ SERIAL MONITOR ДЛЯ DEMO_MODE 0]
