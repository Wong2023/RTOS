# RTOS Queue Assignment

## 1. Purpose
This project demonstrates how to safely pass data between two FreeRTOS tasks on an ESP32 using a queue. It illustrates the basic Producer–Queue–Consumer design pattern.

## 2. How It Works
1. **Sensor Task (Producer):** Simulates a temperature sensor starting at 20 °C. Every second, it increments the temperature value and sends a copy to the queue using `xQueueSend()`.
2. **Queue:** Acts as a thread-safe FIFO buffer with a capacity of 5 integers.
3. **Display Task (Consumer):** Periodically checks the queue using `xQueueReceive()`. When a new temperature value is available, it retrieves it and prints `Temperature: XX C` to the Serial Monitor.

## 3. Serial Monitor
Serial Monitor Output in the assignment (repo) but different folder

## 4. Reflection
Using a queue is useful because it provides thread-safe communication between tasks without risking race conditions or lost data. It decouples the Sensor Task from the Display Task, allowing them to run independently at different rates. FreeRTOS automatically manages the data synchronization without requiring global shared variables.
