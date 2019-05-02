//*****************************************************************************
//
// File: scheduler.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// A time-triggered scheduler module which can run a series of background
// tasks at different frequencies.
//
//*****************************************************************************

#ifndef SCHEDULER_H_
#define SCHEDULER_H_


//*****************************************************************************
// Allocates an array which can hold up to numberOfTasks tasks.
//*****************************************************************************
void initScheduler(uint16_t numberOfTasks);

//*****************************************************************************
// Creates a new task with the given callback and number of ticks per
// task execution and adds it to the array of tasks.
//*****************************************************************************
void schedulerRegisterTask(void (*runTask)(), uint16_t ticksPerRun);

//*****************************************************************************
// Updates the ticks of each task, setting the task to ready if necessary.
// Should be called frequently, e.g. from a SysTick interrupt handler.
//*****************************************************************************
void schedulerUpdateTicks(void);

//*****************************************************************************
// Enters an infinite loop, repeatedly checking whether each task is ready,
// and executing the task if so.
//*****************************************************************************
void schedulerStart(void);


#endif  // SCHEDULER_H_
