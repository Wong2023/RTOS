# ESP32-S3 Dual-Core Producer-Consumer-Monitor Pipeline

This project implements a multicore embedded data pipeline on the **ESP32-S3** using **FreeRTOS**. It demonstrates core affinity, thread-safe inter-task communication via a queue, and shared resource protection using a mutex.

## 1. Application Architecture

The system is structured around three main components running across both CPU cores:

```text
[Producer Task | Core 0] ---> (FreeRTOS Queue) ---> [Consumer Task | Core 1]
                                                               |
                                                            writes
                                                               v
                                                      [Shared Statistics] <--- Protected by Mutex
                                                               ^
                                                             reads
                                                               |
                                                      [Monitor | loop()]
```

* **Producer Task (Pinned to Core 0):** Generates simulated temperature and humidity readings every 500 ms, attaches a sequence number and timestamp, and sends the struct to a FreeRTOS queue. If the queue is full, it acquires the statistics mutex and increments the dropped-packet counter.
* **Consumer Task (Pinned to Core 1):** Blocks waiting for incoming readings from the queue. When data arrives, it acquires the mutex, updates the shared statistics (sample count, running averages, min/max temperature), releases the mutex, and prints the processed results along with a threshold alarm if triggered.
* **Monitor (`loop()`):** Runs every 3000 ms. It locks the same mutex, copies a consistent snapshot of the shared statistics into a local struct, immediately releases the mutex, and prints the summary to the Serial Monitor.

## 2. Design & Synchronization Choices

* **Why separate cores (Core 0 and Core 1)?**
  Pinning the Producer to Core 0 and the Consumer to Core 1 allows data generation and data processing/logging to run in true hardware parallel without blocking each other.
* **Why use a FreeRTOS Queue?**
  The queue acts as a thread-safe FIFO buffer between tasks on different cores. It decouples the Producer's sampling rate from the Consumer's processing speed without requiring manual locks between the two tasks.
* **Why use a Mutex for Shared Statistics?**
  The Consumer writes to `SharedStatistics` while the Monitor reads from it asynchronously. Using a mutex prevents race conditions and torn reads (such as reading an updated sample count paired with an outdated average). Critical sections are kept minimal—all `Serial.printf()` calls happen strictly after releasing the mutex.

## 3. Extensions Beyond the Lecture Demonstration

Three extensions were added to the base lecture example:
1. **Second Sensor Measurement (Humidity):** Added simulated relative humidity (`45.0% – 60.0%`) to `SensorReading` and tracked its running average in `SharedStatistics`.
2. **Threshold Alarm:** Added a high-temperature threshold (`24.2°C`). When exceeded, the Consumer appends `[ALARM: HIGH TEMP!]` to its log output.
3. **Dropped-Sample Counter:** Added `droppedCount` to `SharedStatistics` so any samples dropped due to a full queue are safely counted and displayed by the Monitor.

## 4. How to Compile, Upload, and Test

1. Open `Dual_Core_Data_Pipeline.ino` in **Arduino IDE 2.x** (or paste the code into the Wokwi ESP32-S3 simulator).
2. Select the target board: **ESP32S3 Dev Module** (`ESP32-S3 DevKitC-1`).
3. Compile and upload the firmware to the board.
4. Open the **Serial Monitor** at **115200 baud**.
5. Let the application run for at least 12–15 seconds to verify that more than 20 sequential samples are processed.

### Observations from the Output:
* **Core Verification:** Logs confirm `[Producer | Core 0]` runs on Core 0, while `[Consumer | Core 1]` and `[Monitor | Core 1]` run on Core 1.
* **Queue & Mutex Integrity:** Over 20 readings (`count >= 20`) are processed cleanly with zero dropped samples (`dropped=0`). Running averages, min/max values, and periodic Monitor summaries remain consistent, and `[ALARM: HIGH TEMP!]` triggers whenever temperature exceeds `24.20°C`.
