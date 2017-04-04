#include <stdio.h>

typedef enum {
  INGOS_ERR_OK,
  INGOS_ERR_INVALID_HANDLER,
  INGOS_ERR_INVALID_MESSAGE,
  INGOS_ERR_INVALID_TIMER,
  INGOS_ERR_INVALID_TIME,
  INGOS_ERR_INVALID_TASK_ID,
} INGOS_RET;

typedef enum {
  INGOS_MESSAGE_PLAIN,
  INGOS_MESSAGE_TIMER,
} INGOS_MESSAGE_TYPE;

typedef struct {
  INGOS_MESSAGE_TYPE message_type;
  uint16_t dst_task_id;
  uint16_t src_task_id;
  uint32_t command;
  void* data;

  //Ingos internals
  void* __next;

} ingos_message;

typedef void (*ingos_handler)(ingos_message*);

typedef struct {
  uint16_t ID;
  ingos_handler handler;

  //Ingos internals
  void* __next;
} ingos_task;

typedef struct {
  ingos_message* message;
  uint32_t time;
  uint8_t recurring;

  //Inogs internals
  uint32_t __start_time;
  void* __prev;
  void* __next;
} ingos_timer;

void init_ingos();
void ingos_start();
void ingos_error();
uint16_t ingos_add_task(ingos_task* task);
uint16_t ingos_add_timer(ingos_timer* timer);
uint16_t ingos_post_message(ingos_message* message);
