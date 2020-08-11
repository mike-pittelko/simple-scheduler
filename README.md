# simple-scheduler
Extremely simple fixed time based task scheduler.

I have in many lifetimes needed a simple way (usually in an embedded application with non-existant thread capability) to run short snippets of code on a fixed time base, usually 10's of ms to 10's of seconds.  Most such systems have a simple system tick available.  Coming from this background of very limited resources, very few cycles to waste (2us cycle times were the norm!), I came up with this.

Using the system tick counter, assuming it is generating ticks at 1 per ms (1khz), count the zeros from the right side of the counter.  The number of zero's is the power of 2 of milliseconds elapsed.  This code implements a neat trick for doing that on a 32 bit value.

As implemented here, I use a 32 bit counter, and only use the lower 24 bits.

Simple usage:
Include the task-schedule .c/.h files in your project.  Call the TasksOnPowerOfTwo function every ms.
In a STM32Cube project generated with CubeMX, put "TasksOnPowerOfTwo(HAL_GetTick());" into the "USER CODE BEGIN" section of SysTick_Handler in stm32f1xx_it.c

Define the tasks to run by including them in the appropriate defines in task-schedule.h.  A "task" is defined as a void func(void), wher func will be called on the timebase specified by the define it's placed in.  You may put at most one function in each define.  For example: you define a function for every 256ms, and one for 1s.  At 1 second time slot, the 256ms task, and the 1s task will both be called.

The example code demonstrates doing this to toggle the heartbeat led on a blue-pill every 256ms (a 2hz, 50% duty cycle blink)

# Slighly Less Simple Scheduler:
This is a somehwat more complex implementation that runs tasks in the main code loop.  Using this is a much safer and general purpose method of using the same basic scheduling algorithm.  It's not complicated, but has a little finesse. Call "MainLoopTaskProcessor" from your main loop.

# CAUTION - Simpler Scheduler, does not apply to the slightly less simple scheduler:
This is a very simple method to make things occur on a synchronized time base that's generally useful for UI purposes, and great for keeping things like sets of blinking LED's in time with each other.  It's also great for kicking off background tasks that need to happen.  It's NOT great for long running executions that would need to occur in the calling context of the "task" (which in this case executes in the context of the system tick interrupt) - in those cases, it's much better to use the "task" to set a flag/semaphore to enable a background task via your main loop.  I'll add an example of this at some point.
