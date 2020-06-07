#include "websocket.h"
#include "ntp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

QueueHandle_t DataQueueHandle; //queue between metronome and websocket

void app_main(){
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    
    //queue between metronome and websocket 
    DataQueueHandle = xQueueCreate(4, sizeof(int));
    // 4 vals - toptimesig, bottomtimesig, tempo, starttime


    xTaskCreate(ntp_sync_task, "ntp_sync", 4096, NULL, 5, NULL);
    xTaskCreate(web_socket_task, "web_socket", 4096, NULL, 5, NULL);
    //Metronome task created in web_socket when a request is recieved
}
