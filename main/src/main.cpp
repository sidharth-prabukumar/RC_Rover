#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

extern "C"
{
    void app_main();
}

void app_main(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,                 /*!< LEDC speed speed_mode, high-speed mode or low-speed mode */
        .duty_resolution = LEDC_TIMER_10_BIT,       /*!< LEDC channel duty resolution */ 
        .timer_num = LEDC_TIMER_0,                /*!< The timer source of channel (0 - 3) */
        .freq_hz = 5000,                       /*!< LEDC timer frequency (Hz) */
        .clk_cfg = LEDC_AUTO_CLK                 /*!< Configure LEDC source clock.
                                                        For low speed channels and high speed channels, you can specify the source clock using LEDC_USE_REF_TICK, LEDC_USE_APB_CLK or LEDC_AUTO_CLK.
                                                        For low speed channels, you can also specify the source clock using LEDC_USE_RTC8M_CLK, in this case, all low speed channel's source clock must be RTC8M_CLK*/
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = 4,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel);
    ledc_fade_func_install(0);
    int i;
    //ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024, 0);
    for(i=0; i < 1024; i++)
    {
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i, 0);
        //ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
        //ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        printf("Speed = %d\n", i);
    }
    for(i=1023; i >=0; i--)
    {
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i, 0);
        //ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
        //ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        printf("Speed = %d\n", i);
    }
    
}
