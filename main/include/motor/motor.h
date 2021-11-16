#pragma once

// Library Includes
#include <stdint.h>
// IDF Includes
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
// Local Includes
#include "core/log.h"

namespace motor_ctrl
{
    typedef enum
    {
        MOTOR_DIR_BWD = 0,
        MOTOR_DIR_FWD = 1,
        MOTOR_DIR_MAX = 2,  // Not a real state. Use for iterators
    }motor_dir_t;

    typedef enum
    {
        PWM_CHANNEL_0 = 0,
        PWM_CHANNEL_1 = 1,
        PWM_CHANNEL_2 = 2,
    }pwm_channel_t;

    typedef struct
    {
        uint8_t mcpwm_A;
        uint8_t mcpwm_B;
    }pwm_gpio_t;

    typedef struct
    {
        float mcpwm_A_duty;
        float mcpwm_B_duty;
    }pwm_duty_t;

    class motor
    {
        public:
            motor(pwm_gpio_t, motor_dir_t, pwm_duty_t);
            ~motor();
            log::ROVER_EC Init(mcpwm_unit_t, pwm_channel_t, mcpwm_timer_t);
            log::ROVER_EC Start();
            log::ROVER_EC Stop();
            log::ROVER_EC SetDirection(motor_dir_t);
            log::ROVER_EC SetDuty(pwm_duty_t);
        private:
            pwm_gpio_t  pwm_pin; 
            motor_dir_t direction;
            pwm_duty_t  duty_cycle;
            // PWM Config Specific - Set at Init
            mcpwm_unit_t m_MCPWM_Unit;
            mcpwm_io_signals_t m_MCPWM_A;
            mcpwm_io_signals_t m_MCPWM_B;
            mcpwm_timer_t m_MCPWM_Timer;
            mcpwm_config_t m_MCPWM_Config;
    };
}

