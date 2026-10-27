# FreeRTOS Bounded Buffer Challenge (Producer-Consumer)

## Questions & Answers

1. **What does `emptySlots` count?**  
   `emptySlots` is a counting semaphore that tracks the number of free/available spaces remaining in the shared buffer (starts at 5).

2. **What does `filledSlots` count?**  
   `filledSlots` is a counting semaphore that tracks the number of unread data items currently available in the buffer for consumers to read (starts at 0).

3. **What does the mutex protect?**  
   `bufferMutex` protects the critical section, specifically the shared data buffer array (`buffer`) and the head/tail circular pointers (`head` and `tail`), preventing race conditions when multiple tasks read or write concurrently.

4. **What did you change in the second test, and what happened?**  
   - **Change:** Increased `CONSUMER_DELAY_MS` from `400` to `1000` ms to simulate slower consumers.
   - **Observation:** Because consumers processed data slower than producers were producing it, the buffer quickly reached its full capacity (5 items).
   - **Behavior:** Once the buffer was full, any producer attempting to call `putValue()` blocked at `xSemaphoreTake(emptySlots, portMAX_DELAY)` until a consumer read an item and called `xSemaphoreGive(emptySlots)`. All 15 items were still produced and consumed correctly without data loss or buffer overflow.

## AI Tools Usage Note
AI assistance was used to verify the correct acquisition order of counting semaphores versus mutexes to prevent deadlocks and to format documentation.
