#include "ingos.h"
#include "stm32f4xx_hal.h"

#include <string.h>

ingos_message* messages;
ingos_task* tasks;
ingos_timer* timer_head;

static int running = 0;


uint16_t __get_task(uint16_t ID, ingos_task** task) {
  ingos_task* ptr = tasks;

  while (ptr) {
    if ((ptr->ID == ID)) {
      *task = ptr;
      return INGOS_ERR_OK;
    }

    ptr = ptr->__next;
  }

  return INGOS_ERR_INVALID_TASK_ID;
}

uint8_t __task_exists(uint16_t ID) {
  ingos_task* tmp;
  if (__get_task(ID, &tmp) != INGOS_ERR_OK) return 1;

  return 0;
}

uint16_t __call_handler (ingos_message* message) {
  ingos_task* ptr;
  uint16_t ret = __get_task(message->dst_task_id, &ptr);
  if (ret != INGOS_ERR_OK) return ret;

  ptr->handler(message);
  return INGOS_ERR_OK;
}

void init_ingos() {
  if (running) return;

  int i;
  messages = NULL;
  timer_head = NULL;
  tasks = NULL;
}

void ingos_error() {
  while (1) {
    //Eternal loop ....
  }
}

void ingos_start() {
  if (running) return;

  while (1) {

    /*Timers*/

    ingos_timer* ptr = timer_head;
    while (ptr) {
      if ((HAL_GetTick() - ptr->__start_time) >= ptr->time) {
        uint32_t t0 = HAL_GetTick();
        //Call message handler
        uint16_t ret = __call_handler(ptr->message);
        if (ret != INGOS_ERR_OK) ingos_error();

        if (!ptr->recurring) {
          //This timer shall be removed
          if (!ptr->__prev && !ptr->__next) {
            //Head, no more timers
            timer_head = NULL;
          } else if (ptr->__prev && ptr->__next) {
            //In the middle of the chain
            ingos_timer* p = ptr->__prev;
            ingos_timer* n = ptr->__next;
            p->__next = n;
            n->__prev = p;
          } else if(ptr->__prev && ! ptr->__next) {
            //Last element
            ingos_timer* p = ptr->__prev;
            p->__next = NULL;
          } else {
            ingos_error();
          }
        } else {
          ptr->__start_time = HAL_GetTick();
        }
      }

      ptr = ptr->__next;
    }

    /*Messages*/
    while (messages) {
      //Call message handler
      uint16_t ret = __call_handler(messages);
      if (ret != INGOS_ERR_OK) ingos_error();

      //Go to next message
      messages = messages->__next;
    }
  }
}

uint16_t ingos_add_task(ingos_task* task) {
  if (!task->handler) return INGOS_ERR_INVALID_HANDLER;

  if (!tasks) {
    tasks = task;
    task->ID = 0;
    task->__next = NULL;
  } else {
    ingos_task* ptr = tasks;
    //Fast forward to last element
    while (ptr->__next) {}

    ptr->__next = task;
    task->ID = ptr->ID + 1;
    task->__next = NULL;
  }

  return INGOS_ERR_OK;
}


uint16_t ingos_add_timer(ingos_timer* timer) {

  if (!timer) return INGOS_ERR_INVALID_TIMER;
  if (!timer->message) return INGOS_ERR_INVALID_MESSAGE;

  timer->__start_time = HAL_GetTick();
  timer->message->message_type = INGOS_MESSAGE_TIMER;
  timer->__next = NULL;
  timer->__prev = NULL;

  if(!timer_head) {
    timer_head = timer;
    return INGOS_ERR_OK;
  }

  ingos_timer* ptr = timer_head;
  while (1) {
    if (!ptr->__next) {
      ptr->__next = timer;
      timer->__prev = ptr;
      break;
    } else {
      ptr = ptr->__next;
    }
  }

  return INGOS_ERR_OK;
}

uint16_t ingos_post_message(ingos_message* message) {
  if (!message) return INGOS_ERR_INVALID_MESSAGE;
  if (!message->dst_task_id) return INGOS_ERR_INVALID_TASK_ID;
  if (!__task_exists(message->dst_task_id)) return INGOS_ERR_INVALID_TASK_ID;

  message->__next = NULL;
  ingos_message* ptr = messages;

  if (!ptr) {
    messages = message;
  } else {
    //Forward until we get to last element
    while (ptr->__next) {
      ptr = ptr->__next;
    }
    ptr->__next = message;
  }
  return INGOS_ERR_OK;
}
