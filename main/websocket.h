#ifndef WEBSOCKET_H_   /* Include guard */
#define WEBSOCKET_H_

#include "esp_event.h"
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void websocket_app_start(void);
void web_socket_task();

#endif