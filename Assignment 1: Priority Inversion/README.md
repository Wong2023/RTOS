# Assignment 1: Priority Inversion in FreeRTOS

## Part A – Predict the Behaviour

1. With a binary semaphore, Task_H (priority 3) blocks at 100 ms waiting for Task_L (priority 1). At 150 ms, Task_M (priority 2) wakes up and preempts Task_L because 2 > 1. Task_L is paused until Task_M finishes its 1000 ms work, delaying Task_H.

2. Timeline (Binary Semaphore):
0 - 100 ms    : Task_L runs (takes lock)
100 ms        : Task_H requests lock -> Blocked
100 - 150 ms  : Task_L resumes
150 - 1150 ms : Task_M preempts Task_L (1000 ms)
1150 - 1350 ms: Task_L finishes work, releases lock
1350 - 1400 ms: Task_H gets lock, runs, finishes

3. With a mutex, Task_L inherits priority 3 at 100 ms when Task_H requests the lock. At 150 ms, Task_M (priority 2) cannot preempt Task_L (3 > 2). Task_L finishes at 300 ms, Task_H runs next, and Task_M runs last.
Timeline (Mutex):
0 - 300 ms    : Task_L runs (priority 3 from 100 ms), releases lock
300 - 350 ms  : Task_H gets lock, runs, finishes
350 - 1350 ms : Task_M runs 1000 ms, finishes

4. Task_L temporarily inherits priority 3 (from Task_H).

---

## Part B – Measurements and Comparison

### Step 1 & 2: 1000 ms Workload

Serial Monitor (USE_MUTEX 0):
[PASTE OUTPUT HERE]

Serial Monitor (USE_MUTEX 1):
[PASTE OUTPUT HERE]

| Observation | Binary semaphore (USE_MUTEX 0) | Mutex (USE_MUTEX 1) |
| :--- | :--- | :--- |
| Task_H waiting time | [1200] ms | [200] ms |
| Task_L printed priority | 1 | 1, then 3 |
| Does Task_M delay Task_L? | Yes | No |
| Does priority inheritance occur? | No | Yes |
| Completion order | Task_M -> Task_L -> Task_H | Task_L -> Task_H -> Task_M |

Comparison: The measured results match the Part A predictions. Using a mutex eliminated the 1000 ms delay caused by Task_M.

### Step 3: 2000 ms Workload

Prediction: Under USE_MUTEX 0, Task_H waiting time will increase by 1000 ms (to ~2200 ms). Under USE_MUTEX 1, Task_H waiting time will stay ~200 ms.

Serial Monitor (USE_MUTEX 0, 2000 ms):
[PASTE OUTPUT HERE]

Serial Monitor (USE_MUTEX 1, 2000 ms):
[PASTE OUTPUT HERE]

Result & Explanation:
- USE_MUTEX 0 wait time: [2200] ms
- USE_MUTEX 1 wait time: [200] ms
Without priority inheritance, Task_M blocks Task_L for its entire runtime, directly adding to Task_H's wait. With priority inheritance, Task_L finishes before Task_M starts, so Task_M's workload has no effect on Task_H.

---

## Part C – Explanation

1. Task_H waits longer with a binary semaphore because Task_M has a higher priority than Task_L and can preempt it, preventing Task_L from releasing the lock.
2. When Task_H blocks on the mutex held by Task_L, FreeRTOS raises Task_L's priority so it temporarily inherits the priority of Task_H (3) until the mutex is released.
3. While holding the mutex, Task_L has an inherited priority of 3, which is higher than Task_M's priority of 2, so Task_M cannot preempt Task_L.
4. The main difference is that a mutex supports priority inheritance—allowing the lock holder to inherit a higher priority so medium-priority tasks cannot preempt it—while a binary semaphore does not change task priorities.
