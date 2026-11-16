ESP32-S3 Hardware Timer ISR to FreeRTOS QueueBoard & Wiring SetupBoard: ESP32-S3 (ESP32-S3 Dev Module)   Wiring: No external wiring required. Uses the internal hardware timer (1 MHz counter, auto-reload alarm every 100,000 ticks / 100 ms) and USB-Serial interface at 115200 baud.Queue Configuration: 4-slot FreeRTOS queue for uint32_t values.Run A: 20 ms Consumer Task DelayOutput Excerpt1
2
3
4
5
6
7
8
9
10
11
12ObservationWith a 20 ms task delay, the consumer processes each item five times faster than the 100 ms timer interrupt interval. The queue never fills up, and all sequence numbers arrive consecutively without any dropped events.   Run B: 500 ms Consumer Task DelayPrediction Before Run BThe queue will fill up. The timer ISR generates events every 100 ms (10 Hz), while the consumer task processes only one item every 500 ms (2 Hz). Because the queue has a capacity of only 4 slots, it will saturate within the first second, causing subsequent ISR events to be dropped.Output Excerpt1
2
3
4
5
6
11
16
21
26
31
36Explanation of Queue Capacity & Sequence GapsFirst observed gap: Occurs immediately after 6, jumping to 11 (events 7, 8, 9, and 10 were dropped). After that, every 5th interrupt is captured (11, 16, 21, 26...) because one 500 ms task delay spans five 100 ms timer intervals.   Why older numbers appear before a gap: When item 1 arrives at t = 100 ms, the consumer prints it and enters a 500 ms delay. During this delay, interrupts at 200, 300, 400, and 500 ms fill all 4 slots of the FIFO queue with 2, 3, 4, 5. At t = 600 ms, the consumer wakes up and reads 2, freeing one slot just as interrupt 6 fires and takes that slot. Because xQueueSendFromISR() drops new items rather than overwriting existing ones when the queue is full, the buffered older numbers (2 through 6) are printed sequentially before the first gap (7–10 dropped) becomes visible.   
