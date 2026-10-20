1. Why can passing the address of a local variable to a task be unsafe?
Local variables reside on the stack of the function where they are created (in this case, setup()). Once setup() exits, its stack frame is freed and can be overwritten by other operations. If a background task attempts to dereference that memory address after setup() completes, it reads invalid or corrupted data (dangling pointer issue).

2. When does blinkLED copy the parameter?
blinkLED copies the parameter at the very beginning of its execution, immediately after successfully acquiring the mutex (xSemaphoreTake) and before releasing it (xSemaphoreGive).

3. How does the mutex make setup() wait?

setup() creates the mutex and acquires it first via xSemaphoreTake.

When xTaskCreatePinnedToCore spawns blinkLED, setup() executes a second xSemaphoreTake(mutex, portMAX_DELAY). Since the mutex is already locked, setup() enters a blocked state.

Once blinkLED runs, it tries to take the mutex (blocks if setup() hasn't reached its second take), copies the variable, and calls xSemaphoreGive.

Unlocking the mutex unblocks setup(), allowing it to safely complete execution.

4. Why is this described as a demonstration hack? What would be a better design in a real application?
It is considered a hack because mutexes are designed for mutual exclusion when accessing shared resources, not for task synchronization or passing arguments. Using a mutex here forces a synchronous wait on task creation, defeating the asynchronous nature of task scheduling.

Better designs in real applications:

Pass value directly by value (casting): Cast the integer value directly into a void* pointer (e.g., (void*)delay_arg), avoiding pointers to local memory altogether.

Dynamic memory allocation: Allocate memory on the heap using pvPortMalloc(), pass the pointer to the task, and let the task free the memory (vPortFree()) when done.

Global/Static variable: Use a static or global variable if the value persists across the execution lifecycle.

Queues: Use FreeRTOS Queues (xQueueSend / xQueueReceive) for safe inter-task communication.
