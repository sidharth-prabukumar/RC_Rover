// Library Includes
#include <stdio.h>
// IDF Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
// Local Includes
#include "motor/motor.h"

using namespace motor_ctrl;

extern "C"
{
    void app_main();
}

void app_main(void)
{
    pwm_gpio_t Motor_Left_PWM = {
        .mcpwm_A = 15,
        .mcpwm_B = 13
    };
    pwm_duty_t Motor_Left_DutyCycle = {
        .mcpwm_A_duty = 40.0,
        .mcpwm_B_duty = 40.0
    };
    /*pwm_gpio_t Motor_Right_PWM = {
        .mcpwm_A = 4,
        .mcpwm_B = 5
    };
    pwm_duty_t Motor_Right_DutyCycle = {
        .mcpwm_A_duty = 50.0,
        .mcpwm_B_duty = 50.0
    };*/
    motor Motor_Left(Motor_Left_PWM, MOTOR_DIR_BWD, Motor_Left_DutyCycle);
    //motor Motor_Right(Motor_Right_PWM, MOTOR_DIR_FWD, Motor_Left_DutyCycle);
    Motor_Left.Init();
    Motor_Left.Start();

    // Spin backward for some time
    for(int i=0;i < 100; i++)
    {
        Motor_Left_DutyCycle.mcpwm_B_duty = i;
        Motor_Left.SetDuty(Motor_Left_DutyCycle);
        Motor_Left.Start();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Stop spinning for some time
    Motor_Left.Stop();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Stop forward for some time
    Motor_Left.SetDirection(MOTOR_DIR_FWD);
    for(int i=0;i < 100; i++)
    {
        Motor_Left_DutyCycle.mcpwm_A_duty = i;
        Motor_Left.SetDuty(Motor_Left_DutyCycle);
        Motor_Left.Start();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Stop spinning
    Motor_Left.Stop();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
}
