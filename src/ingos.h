#include <stdio.h>

typedef enum {
  INGOS_ERR_OK,
  INGOS_ERR_MAX_TASKS,
  INGOS_ERR_MAX_TIMERS,
  INGOS_ERR_INVALID_TIME,
  INGOS_ERR_INVALID_TASK_ID,
} INGOS_ERR;

typedef enum {
  INGOS_MESSAGE_NULL,
  INGOS_MESSAGE_PLAIN,
  INGOS_MESSAGE_TIMER,
} INGOS_MESSAGE_TYPE;

typedef uint16_t ingos_task_id;

typedef struct {
  INGOS_MESSAGE_TYPE message_type;
  void* data;
} ingos_message;

typedef void (*ingos_task)(ingos_message*);

void init_ingos();
void start_ingos();
uint16_t add_ingo_task(ingos_task task, ingos_task_id* task_id);
uint16_t add_ingo_timer(ingos_task_id dst_task_id, uint32_t timer, uint8_t recurring, void* data);
void post_message(ingos_message message);
