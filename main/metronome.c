#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/ledc.h"


#include "main.h"

#define BUZZER_GPIO 19
#define PWM_SPEED_MODE LEDC_HIGH_SPEED_MODE
const int INTENSITY = (1<<LEDC_TIMER_13_BIT)*0.5; //half the most intense of 2^(# bits)
const int FIRST_BEAT_INTENSITY = (1<<LEDC_TIMER_13_BIT)*0.75; //75% the most intense of 2^(# bits)

void metronome_task(){
    int timeSig;
    int tempo;
    int startTime;

    xQueueReceive(DataQueueHandle, &timeSig, 1);
    xQueueReceive(DataQueueHandle, &tempo, 1);
    xQueueReceive(DataQueueHandle, &startTime, 1);
    printf("\nDATA:\n timeSig: %d \n tempo: %d\n startTime:%d\n", timeSig, tempo,startTime);

    const long msPerBeat = 60000 / tempo; 
    //60,000 ms in a minute over beats in a minute, min cancles out
    
    const long regBeat = msPerBeat * 0.5;

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = PWM_SPEED_MODE,           // timer mode
        .timer_num = LEDC_TIMER_0,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
            .channel    = LEDC_CHANNEL_0,
            .duty       = 0,
            .gpio_num   = BUZZER_GPIO,
            .speed_mode = PWM_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
    };

    ledc_channel_config(&ledc_channel);

    ledc_set_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0); 

    time_t now;
    time(&now);

    double delay = difftime((time_t) startTime,now);
    printf("%f",delay);
    


    vTaskDelay((delay * 1000) / portTICK_PERIOD_MS);
    

    while (true){
        
        printf("beat: 1\n");
	    ledc_set_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0, FIRST_BEAT_INTENSITY);
        ledc_update_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(regBeat / portTICK_PERIOD_MS);

        ledc_set_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay((msPerBeat * 0.5) / portTICK_PERIOD_MS); //delay for the rest of the beat

        for(int beats=2;beats<=timeSig;beats++){
            printf("beat: %d\n",beats);
            
            ledc_set_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0, INTENSITY);
            ledc_update_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(regBeat / portTICK_PERIOD_MS);

            ledc_set_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(PWM_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay((msPerBeat * 0.5) / portTICK_PERIOD_MS); //delay for the rest of the beat
        }
    }
}