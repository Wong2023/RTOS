# FreeRTOS Memory Management Demo (ESP32)

This project demonstrates how FreeRTOS allocates memory for tasks dynamically from the Heap on an ESP32 microcontroller, and how changing the task stack size directly impacts overall system memory.

---

## 1. Measurement Tables

### Test 1: Stack Size = 4096 bytes
| Measurement | Free Heap (bytes) | Decrease / Change (bytes) |
| :--- | :--- | :--- |
| **Free heap before tasks** | 349,356 | — |
| **Free heap after Task A** | 344,628 | 4,728 |
| **Free heap after Task B** | 339,900 | 4,728 |

### Test 2: Stack Size = 8192 bytes
| Measurement | Free Heap (bytes) | Decrease / Change (bytes) |
| :--- | :--- | :--- |
| **Free heap before tasks** | 349,356 | — |
| **Free heap after Task A** | 340,532 | 8,824 |
| **Free heap after Task B** | 331,708 | 8,824 |

---

## 2. Assignment Questions & Answers

### a. Did the free heap change when you increased the task stack size?
**Yes.** Increasing the task stack size caused a significantly larger drop in free heap memory when creating each task.

### b. What happened to the free heap?
When doubling the task stack size from `4096` to `8192` bytes, the heap consumption per task increased by exactly 4096 additional bytes (from 4,728 bytes per task down to 8,824 bytes per task).

### c. Why does a task need stack memory?
A task needs stack memory as its private working space to store local variables, keep track of function call hierarchies, and save CPU register states during context switches.

### d. In your own words, explain: Why does creating a FreeRTOS task use RAM?
Creating a FreeRTOS task dynamically allocates memory from the system **Heap** for two essential components:
1. The **Task Stack** (the task's reserved private working area).
2. The **Task Control Block (TCB)**, which stores metadata required by the FreeRTOS scheduler (such as task priority, current state, and stack pointers).

---

## 3. Serial Monitor Output

```text
=== RTOS Memory Demo ===
Free heap before tasks: 349356 bytes

Creating Task A...
Task A created.
Free heap after Task A: 344628 bytes

Creating Task B...
Task B created.
Free heap after Task B: 339900 bytes

=== Demo finished ===
Task A is running
Task B is running
