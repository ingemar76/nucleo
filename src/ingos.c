#include "ingos.h"
#include "stm32f4xx_hal.h"

#include <string.h>

#define MAX_MESSAGES 20
#define MAX_TASKS 20
#define MAX_TIMERS 50

#define NULL_ID 9999

typedef struct {
  ingos_task_id dst_task_id;
  uint32_t timer;
  uint8_t recurring;
  void* data;

  uint32_t start_time;
} ingos_timer;

ingos_message messages[MAX_MESSAGES];
ingos_task tasks[MAX_TASKS];
ingos_timer timers[MAX_TIMERS];

ingos_message msg;

void init_ingos() {
  int i;
  memset(messages, 0, sizeof(messages));
  memset(tasks, 0, sizeof(tasks));
  memset(timers, 0, sizeof(timers));

  for (i = 0; i < MAX_MESSAGES; i++) {
    messages[i].message_type = INGOS_MESSAGE_NULL;
  }

  for (i = 0; i < MAX_TASKS; i++) {
    tasks[i] = NULL;
  }

  for (i = 0; i < MAX_TIMERS; i++) {
    timers[i].dst_task_id = NULL_ID;
  }
}

void __error() {

}

void start_ingos() {
  int i;
  while (1) {

    /*Timers*/
    for (i = 0; i < MAX_TIMERS; i++) {
      if (timers[i].dst_task_id != NULL_ID) {
        if ((HAL_GetTick() - timers[i].start_time) >= timers[i].timer) {
          if (timers[i].dst_task_id >= MAX_TASKS) {
            __error();
          }
          msg.message_type = INGOS_MESSAGE_TIMER;
          msg.data = timers[i].data;
          //Call message handler
          tasks[timers[i].dst_task_id](&msg);

          if (timers[i].recurring) {
            timers[i].start_time += timers[i].timer;
          } else {
            //Delete the timer
            timers[i].dst_task_id = NULL_ID;
          }
        }
      }
    }

  }
}

uint16_t add_ingo_task(ingos_task task, ingos_task_id* task_id) {
  int i;
  for (i = 0; i < MAX_TASKS; i++) {
    if (!tasks[i]) {
      tasks[i] = task;
      *task_id = i;
      return INGOS_ERR_OK;
    }
  }

  return INGOS_ERR_MAX_TASKS;
}

uint16_t add_ingo_timer(ingos_task_id dst_task_id, uint32_t timer, uint8_t recurring, void* data) {
  int i;

  if (timer == 0) return INGOS_ERR_INVALID_TIME;
  if (dst_task_id >= MAX_TASKS || !tasks[dst_task_id]) return INGOS_ERR_INVALID_TASK_ID;

  for (i = 0; i < MAX_TIMERS; i++) {
    if (timers[i].dst_task_id == NULL_ID) {
      timers[i].dst_task_id = dst_task_id;
      timers[i].timer = timer;
      timers[i].recurring = recurring;
      timers[i].data = data;
      timers[i].start_time = HAL_GetTick();

      return INGOS_ERR_OK;
    }
  }
  return INGOS_ERR_MAX_TIMERS;
}

void post_message(ingos_message message) {

}
