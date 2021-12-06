// Library Includes
#include <string>
// IDF Includes
#include "esp_log.h"
// Local Includes
#include "motor/motor.h"

namespace motor_ctrl
{
    static const char * TAG = "MOTOR";
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

    log::ROVER_EC motor::Init(mcpwm_unit_t unit, pwm_channel_t channel, mcpwm_timer_t timer)
    {
        m_MCPWM_Unit = unit;
        switch(channel)
        {
            case PWM_CHANNEL_0:
                m_MCPWM_A = MCPWM0A;
                m_MCPWM_B = MCPWM0B;
                break;
            case PWM_CHANNEL_1:
                m_MCPWM_A = MCPWM1A;
                m_MCPWM_B = MCPWM1B;
                break;
            case PWM_CHANNEL_2:
                m_MCPWM_A = MCPWM2A;
                m_MCPWM_B = MCPWM2B;
                break;
        }
        m_MCPWM_Timer = timer;
        // Initial GPIO configuration
        if(log::ROVER_OK != log::return_err_if_esp_err(TAG, mcpwm_gpio_init(m_MCPWM_Unit, m_MCPWM_A, pwm_pin.mcpwm_A)))
        {
            return log::ROVER_ERR;
        }
        if(log::ROVER_OK != log::return_err_if_esp_err(TAG, mcpwm_gpio_init(m_MCPWM_Unit, m_MCPWM_B, pwm_pin.mcpwm_B)))
        {
            return log::ROVER_ERR;
        }
        // Initial mcpwm configuration
        ESP_LOGI(TAG,"Configuring Initial Parameters of mcpwm...");
        m_MCPWM_Config.frequency = 1000;    //frequency = 1kHz,
        m_MCPWM_Config.cmpr_a = duty_cycle.mcpwm_A_duty;          //duty cycle of PWMxA = 0
        m_MCPWM_Config.cmpr_b = duty_cycle.mcpwm_B_duty;          //duty cycle of PWMxb = 0
        m_MCPWM_Config.counter_mode = MCPWM_UP_COUNTER;
        m_MCPWM_Config.duty_mode = MCPWM_DUTY_MODE_0;
        if(log::ROVER_OK != log::return_err_if_esp_err(TAG, mcpwm_init(m_MCPWM_Unit, m_MCPWM_Timer, &m_MCPWM_Config)))
        {
            return log::ROVER_ERR;
        }
        return log::ROVER_OK;
    }

    log::ROVER_EC motor::Start()
    {
        switch(direction)
        {
            case MOTOR_DIR_BWD:
                mcpwm_set_signal_low(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_A);
                mcpwm_set_duty(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_B, duty_cycle.mcpwm_B_duty);
                mcpwm_set_duty_type(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_B, m_MCPWM_Config.duty_mode);
                break;
            case MOTOR_DIR_FWD:
                mcpwm_set_signal_low(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_B);
                mcpwm_set_duty(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_A, duty_cycle.mcpwm_A_duty);
                mcpwm_set_duty_type(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_A, m_MCPWM_Config.duty_mode);
                break;
            case MOTOR_DIR_MAX:
            default:
                return log::ROVER_ERR;
        }
        return log::ROVER_OK;
    }

    log::ROVER_EC motor::Stop()
    {
        mcpwm_set_signal_low(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_A);
        mcpwm_set_signal_low(m_MCPWM_Unit, m_MCPWM_Timer, MCPWM_GEN_B);
        return log::ROVER_OK;
    }

    log::ROVER_EC motor::SetDirection(motor_dir_t dir)
    {
        switch(dir)
        {
            case MOTOR_DIR_BWD:
                ESP_LOGI(TAG, "Setting Direction : %s", "Backward");
                direction = dir;
                break;
            case MOTOR_DIR_FWD:
                ESP_LOGI(TAG, "Setting Direction : %s", "Forward");
                direction = dir;
                break;
            case MOTOR_DIR_MAX:
            default:
                ESP_LOGE(TAG, "Motor Direction Invalid : %d", dir);
                return log::ROVER_ERR;

        }
        return log::ROVER_OK;
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
