//*****************************************************************************
//
// File: scheduler.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// A time-triggered scheduler module which can run a series of background
// tasks at different frequencies. Supports task priorities, by registering
// higher priority tasks first.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "scheduler.h"


//*****************************************************************************
// Task structure, containing a callback to run the task, the number of ticks
// after which the task should be run, and a counter to keep track of the
// current number of ticks.
//*****************************************************************************
typedef struct {
    void (*runTask)(void);
    uint16_t ticksPerRun;
    uint16_t tick;
    bool ready;
} task_t;


//*****************************************************************************
// Static variables
//*****************************************************************************
static task_t* tasks;
static uint16_t numTasks;


//*****************************************************************************
// Allocates an array which can hold up to numberOfTasks tasks.
//*****************************************************************************
void initScheduler(uint16_t numberOfTasks) {
    numTasks = numberOfTasks;
    tasks = malloc(numTasks * sizeof(task_t));
}

//*****************************************************************************
// Creates a new task with the given callback and number of ticks per
// task execution and adds it to the array of tasks. Tasks with higher
// priorities should be registered first.
//*****************************************************************************
void schedulerRegisterTask(void (*runTask)(void), uint16_t ticksPerRun) {
    static uint16_t taskIndex = 0;

    if (taskIndex >= numTasks) {
        // Error: too many tasks registered.
        return;
    }

    task_t* newTask = &tasks[taskIndex];
    newTask->runTask = runTask;
    newTask->ticksPerRun = ticksPerRun;
    newTask->tick = 0;
    newTask->ready = false;

    taskIndex++;
}

//*****************************************************************************
// Updates the ticks of each task, setting the task to ready if necessary.
// Should be called frequently, e.g. from a SysTick interrupt handler.
//*****************************************************************************
void schedulerUpdateTicks(void) {
    uint16_t i = 0;
    for (i = 0; i < numTasks; i++) {
        task_t* task = &tasks[i];
        task->tick++;
        if (task->tick >= task->ticksPerRun) {
            task->tick = 0;
            task->ready = true;
        }
    }
}

//*****************************************************************************
// Enters an infinite loop, repeatedly checking whether each task is ready,
// and executing the task if so. After executing a task, starts checking
// tasks for readiness from the start of the array, implementing
// task priorities.
//*****************************************************************************
void schedulerStart(void) {
    while (true) {
        uint16_t i = 0;
        for (i = 0; i < numTasks; i++) {
            task_t* task = &tasks[i];
            if (task->ready) {
                task->ready = false;
                task->runTask();
                break;
            }
        }
    }
}

