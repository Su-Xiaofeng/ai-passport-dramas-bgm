#pragma once
#include "../demo_test_stubs.h"
typedef struct test_queue *QueueHandle_t;
QueueHandle_t xQueueCreate(unsigned depth, size_t item_size);
BaseType_t xQueueSend(QueueHandle_t q, const void *data, TickType_t ticks);
BaseType_t xQueueOverwrite(QueueHandle_t q, const void *data);
BaseType_t xQueueReceive(QueueHandle_t q, void *data, TickType_t ticks);
void vQueueDelete(QueueHandle_t q);
