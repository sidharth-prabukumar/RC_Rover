// Library Includes
#include <string>
// IDF Includes
#include "esp_log.h"
// Local Includes
#include "motor/motor.h"

static const char * TAG = "MOTOR";
namespace motor_ctrl
{
    motor::motor(pwm_gpio_t pin, motor_dir_t dir, pwm_duty_t duty)
    {
        direction                   = dir;
        duty_cycle                  = duty;
        pwm_pin.mcpwm_A             = pin.mcpwm_A;
        pwm_pin.mcpwm_B             = pin.mcpwm_B;
        SetDuty(duty);
    }

    motor::~motor()
    {
        // Nothing to destroy
    }

    log::ROVER_EC motor::Init()
    {
        // Initial GPIO configuration
        if(log::ROVER_OK != log::return_err_if_esp_err(TAG, mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, pwm_pin.mcpwm_A)))
        {
            return log::ROVER_ERR;
        }
        if(log::ROVER_OK != log::return_err_if_esp_err(TAG, mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, pwm_pin.mcpwm_B)))
        {
            return log::ROVER_ERR;
        }
        // Initial mcpwm configuration
        ESP_LOGI(TAG,"Configuring Initial Parameters of mcpwm...");
        mcpwm_config_t pwm_config;
        pwm_config.frequency = 1000;    //frequency = 1kHz,
        pwm_config.cmpr_a = duty_cycle.mcpwm_A_duty;          //duty cycle of PWMxA = 0
        pwm_config.cmpr_b = duty_cycle.mcpwm_B_duty;          //duty cycle of PWMxb = 0
        pwm_config.counter_mode = MCPWM_UP_COUNTER;
        pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
        if(log::ROVER_OK != log::return_err_if_esp_err(TAG, mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config)))
        {
            return log::ROVER_ERR;
        }
        return log::ROVER_OK;
    }

    log::ROVER_EC motor::Start()
    {
        if(direction == MOTOR_DIR_FWD)
        {
            mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B);
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty_cycle.mcpwm_A_duty);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);
        }
        else if(direction == MOTOR_DIR_BWD)
        {
            mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A);
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty_cycle.mcpwm_B_duty);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_0);
        }
        return log::ROVER_OK;
    }

    log::ROVER_EC motor::Stop()
    {
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A);
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B);
        ESP_LOGI(TAG, "Motor Stopped");
        return log::ROVER_OK;
    }

    log::ROVER_EC motor::SetDirection(motor_dir_t dir)
    {
        if(dir >= MOTOR_DIR_BWD && dir < MOTOR_DIR_MAX)
        {
            ESP_LOGI(TAG, "Setting Direction : %s", (dir == MOTOR_DIR_FWD) ? "Forward" : "Backward");
            direction = dir;
            return log::ROVER_OK;
        }
        ESP_LOGE(TAG, "Motor Direction Invalid : %d", dir);
        return log::ROVER_ERR;
    }

    log::ROVER_EC motor::SetDuty(pwm_duty_t duty)
    {
        if(duty.mcpwm_A_duty >= 0 && duty.mcpwm_A_duty <= 100 &&
            duty.mcpwm_B_duty >= 0 && duty.mcpwm_B_duty <= 100)
        {
            ESP_LOGI(TAG, "Setting Duty Cycle : PWMA = %f, PWMB = %f", duty.mcpwm_A_duty, duty.mcpwm_B_duty);
            duty_cycle.mcpwm_A_duty     = duty.mcpwm_A_duty;
            duty_cycle.mcpwm_B_duty     = duty.mcpwm_B_duty;
            return log::ROVER_OK;
        }
        ESP_LOGE(TAG, "Duty Cycle Invalid : %f %f", duty.mcpwm_A_duty, duty.mcpwm_B_duty);
        return log::ROVER_ERR;
    }
}
