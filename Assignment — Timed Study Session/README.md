### Answers to Questions

1. **Why does this system need both a one-shot and an auto-reload timer?**
   The auto-reload timer (1000 ms) provides periodic updates to trigger the second-by-second screen countdown. The one-shot timer (10,000 ms) precisely measures the total session duration and triggers the final completion event.

2. **Which task executes the timer callbacks?**
   The FreeRTOS Timer Service Task (also known as the Timer Daemon Task) executes all software timer callbacks.

3. **Why do OLED updates belong in the application task?**
   OLED updates involve slow I²C transmission and blocking calls. Executing them inside a timer callback would block the shared Timer Daemon Task, delaying other timers and violating FreeRTOS real-time principles.

4. **When are the timers dormant?**
   Both timers are dormant right after initialization (before the first session starts) and return to the dormant state after the 10-second session finishes (one-shot expires automatically, auto-reload is explicitly stopped).

5. **How does your program prevent extra s commands from extending the session?**
   The application task tracks session state using a boolean flag (`isSessionActive`). Incoming `'s'` characters are processed only when `isSessionActive == false`; any extra commands during an active session are ignored.
