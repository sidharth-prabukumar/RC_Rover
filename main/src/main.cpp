// Library Includes
#include <stdio.h>
// IDF Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <esp_bt_main.h>
#include <esp_bt_defs.h>
// Local Includes
#include "motor/motor.h"
extern "C" {
#include  "esp_bt_device.h"
#include "include/ps3.h"
}

#define ESP_BD_ADDR_HEX_STR        "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"
#define ESP_BD_ADDR_HEX_ARR(addr)   addr[0],  addr[1],  addr[2],  addr[3],  addr[4],  addr[5]
#define ESP_BD_ADDR_HEX_PTR(addr)  &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]

typedef enum
{
    DIRECTION_STOP = 0,
    DIRECTION_FWD  = 1,
    DIRECTION_BWD  = 2,
    DIRECTION_LFT  = 3,
    DIRECTION_RGT  = 4,
}DirectionType_t;

static DirectionType_t RoverDirection = DIRECTION_STOP;


void controller_event_cb( ps3_t ps3, ps3_event_t event )
{
    if(ps3.button.up)
    {
        RoverDirection = DIRECTION_FWD;
        return;
    }
    if(ps3.button.down)
    {
        RoverDirection = DIRECTION_BWD;
        return;
    }
    if(ps3.button.left)
    {
        RoverDirection = DIRECTION_LFT;
        return;
    }
    if(ps3.button.right)
    {
        RoverDirection = DIRECTION_RGT;
        return;
    }
    RoverDirection = DIRECTION_STOP;
}


static const char * TAG = "APP_MAIN";
TaskHandle_t xInitAndRunMotorHandle = NULL;
TaskHandle_t xGamepadReceiverHandle = NULL;

extern "C"
{
    void app_main();
}

void GamepadReceiver(void * params)
{
    while (!ps3IsConnected()){
    vTaskDelay(10 / portTICK_PERIOD_MS);
    }
   while(1)
   {
        vTaskDelay(100 / portTICK_PERIOD_MS);
   }
}


void InitAndRunMotor(void * params)
{
    motor_ctrl::pwm_gpio_t Motor_Right_PWM = {
        .mcpwm_A = 13,
        .mcpwm_B = 12
    };
    motor_ctrl::pwm_duty_t Motor_Right_DutyCycle = {
        .mcpwm_A_duty = 0.0,
        .mcpwm_B_duty = 0.0
    };
    motor_ctrl::pwm_gpio_t Motor_Left_PWM = {
        .mcpwm_A = 4,
        .mcpwm_B = 2
    };
    motor_ctrl::pwm_duty_t Motor_Left_DutyCycle = {
        .mcpwm_A_duty = 0.0,
        .mcpwm_B_duty = 0.0
    };
    motor_ctrl::motor Motor_Left(Motor_Left_PWM, motor_ctrl::MOTOR_DIR_FWD, Motor_Left_DutyCycle);
    motor_ctrl::motor Motor_Right(Motor_Right_PWM, motor_ctrl::MOTOR_DIR_FWD, Motor_Right_DutyCycle);
    Motor_Left.Init(MCPWM_UNIT_0, motor_ctrl::PWM_CHANNEL_0, MCPWM_TIMER_0);
    Motor_Right.Init(MCPWM_UNIT_0, motor_ctrl::PWM_CHANNEL_1, MCPWM_TIMER_1);
    DirectionType_t LocalDirection = DIRECTION_STOP;
    while(1)
    {
        if(LocalDirection != RoverDirection)
        {
            switch(RoverDirection)
            {
                case DIRECTION_STOP:
                    Motor_Left.Stop();
                    Motor_Right.Stop();
                    break;
                case DIRECTION_FWD:
                    Motor_Left_DutyCycle.mcpwm_A_duty = 80;
                    Motor_Right_DutyCycle.mcpwm_A_duty = 80;
                    Motor_Left_DutyCycle.mcpwm_B_duty = 0;
                    Motor_Right_DutyCycle.mcpwm_B_duty = 0;
                    Motor_Left.SetDirection(motor_ctrl::MOTOR_DIR_FWD);
                    Motor_Right.SetDirection(motor_ctrl::MOTOR_DIR_FWD);
                    Motor_Left.SetDuty(Motor_Left_DutyCycle);
                    Motor_Right.SetDuty(Motor_Right_DutyCycle);
                    Motor_Left.Start();
                    Motor_Right.Start();
                    break;
                case DIRECTION_BWD:
                    Motor_Left_DutyCycle.mcpwm_A_duty = 0;
                    Motor_Right_DutyCycle.mcpwm_A_duty = 0;
                    Motor_Left_DutyCycle.mcpwm_B_duty = 80;
                    Motor_Right_DutyCycle.mcpwm_B_duty = 80;
                    Motor_Left.SetDirection(motor_ctrl::MOTOR_DIR_BWD);
                    Motor_Right.SetDirection(motor_ctrl::MOTOR_DIR_BWD);
                    Motor_Left.SetDuty(Motor_Left_DutyCycle);
                    Motor_Right.SetDuty(Motor_Right_DutyCycle);
                    Motor_Left.Start();
                    Motor_Right.Start();
                    break;
                case DIRECTION_RGT:
                    Motor_Left_DutyCycle.mcpwm_A_duty = 50;
                    Motor_Right_DutyCycle.mcpwm_A_duty = 0;
                    Motor_Left_DutyCycle.mcpwm_B_duty = 0;
                    Motor_Right_DutyCycle.mcpwm_B_duty = 0;
                    Motor_Left.SetDirection(motor_ctrl::MOTOR_DIR_FWD);
                    Motor_Left.SetDuty(Motor_Left_DutyCycle);
                    Motor_Left.Start();
                    break;
                case DIRECTION_LFT:
                    Motor_Left_DutyCycle.mcpwm_A_duty = 0;
                    Motor_Right_DutyCycle.mcpwm_A_duty = 50;
                    Motor_Left_DutyCycle.mcpwm_B_duty = 0;
                    Motor_Right_DutyCycle.mcpwm_B_duty = 0;
                    Motor_Right.SetDirection(motor_ctrl::MOTOR_DIR_FWD);
                    Motor_Right.SetDuty(Motor_Right_DutyCycle);
                    Motor_Right.Start();
                    break;
                default:
                    ESP_LOGE(TAG, "Invalid Direction Set");
            }
            LocalDirection = RoverDirection;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void onConnect(uint8_t is_connected){
    if(is_connected)
    {
    printf("Connected.\n");
    ps3SetLed(1);
    }
    else
    printf("Not Connected.\n");
}

void app_main(void)
{
    if(pdPASS != xTaskCreate(&InitAndRunMotor, "Init and Run Motor", 4092, NULL, 2, &xInitAndRunMotorHandle))
    {
        ESP_LOGE(TAG,"InitAndRunMotor Task Creation Failure");
    }
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ps3Init();
    ps3SetEventCallback(controller_event_cb);
    ps3SetConnectionCallback(onConnect);

    if(pdPASS != xTaskCreate(&GamepadReceiver, "Testing Controller", 4092, NULL, 2, &xGamepadReceiverHandle))
    {
            ESP_LOGE(TAG,"GamepadReceiver Task Creation Failure");
    }
}
