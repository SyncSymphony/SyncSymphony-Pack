/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>

#include "esp_event_loop.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_event_loop.h"

static const char *TAG = "WEBSOCKET";

#include "metronome.h"
#include "main.h"

TaskHandle_t metronomeTaskHandle = NULL;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
            ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
            ESP_LOGW(TAG, "Received=%.*s\r\n", data->data_len, (char*)data->data_ptr);
            char * strdata = data->data_ptr;
            strdata[data->data_len] = '\0';   /* null character manually added */
            
            char * timeSigStr = strtok(strdata," ");
            char * tempoStr = strtok(NULL," ");
            char * startTimeStr = strtok(NULL," ");

            if (tempoStr == NULL || timeSigStr == NULL || startTimeStr == NULL) {
                printf("ERROR: missing tempo, time signature or start time");
                break; //quit if it didn't find the needed info
            }

            //Process timeSig
            int timeSig = atoi(timeSigStr);

            if (timeSig <= 0){
                printf("ERROR: invalid time signature");
                break;
            }

            //Process tempo
            int tempo = atoi(tempoStr);

            if(tempo < 30 || tempo > 240){
                printf("ERROR: invalid tempo");
                break;
            }

            //process the start time
            int startTime = atoi(startTimeStr); 
            //NOTE MAY BECOME AN ISSUE ON 2038 WITH 32-bit timestamp
            //TODO: CHANGE BEFORE THEN

            if (startTime <= 0){
                printf("ERROR: Invalid Timestamp");
                break;
            }

           
            //printf("TimeSig: %s/%s \nTempo: %s \nStartTime: %s\n", timeSigTopStr, timeSigBottomStr, tempoStr, startTimeStr);

            if (metronomeTaskHandle != NULL) {
                vTaskDelete(metronomeTaskHandle); //delete the task if it's already running
            }

            xTaskCreate(metronome_task, "metronome", 4096, NULL, 5, &metronomeTaskHandle);

            
            xQueueSendToBack(DataQueueHandle, &timeSig, 0);
            xQueueSendToBack(DataQueueHandle, &tempo, 0);
            xQueueSendToBack(DataQueueHandle, &startTime, 0);


            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
            break;
    }
}

static void websocket_app_start(void)
{
    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.uri = "ws://sync.symphony/websocket";


    ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

    esp_websocket_client_start(client); // will auto reconnect
}

void web_socket_task()
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("WEBSOCKET_CLIENT", ESP_LOG_DEBUG);
    esp_log_level_set("TRANS_TCP", ESP_LOG_DEBUG);


    websocket_app_start();
    
    vTaskSuspend(NULL); 
}
