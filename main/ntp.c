//Orginal license below

/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "ntp.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_sntp.h"
#include "main.h"


static const char *TAG = "NTP";


void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "sync.symphony");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}


void ntp_sync_task()
{
    ESP_LOGI(TAG, "NTP SYNC TASK STARTING");
    
    initialize_sntp();
    
    while(true){
        
        ESP_LOGI(TAG, "NTP sleep start");
        const int time_sync_delay = 60000 / portTICK_PERIOD_MS;
        vTaskDelay(time_sync_delay);
        ESP_LOGI(TAG, "NTP sleep end");
        
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);


        obtain_time();
        time(&now);


        char strftime_buf[64];

        // Set timezone to Eastern Standard Time and print local time
        setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
        tzset();
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);


    } 
}

static void obtain_time(void)
{

    
    // wait for time to be set
    time_t now;
    struct tm timeinfo = { 0 };

    sntp_request(NULL); //start time sync

    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}

