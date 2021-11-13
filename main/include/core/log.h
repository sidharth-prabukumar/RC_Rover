#pragma once
#include "esp_log.h"
#include "esp_err.h"
/**
 * @brief Core logging header file
 */

/* Global Definitions */

/**
 * @brief Rover return error code enum
 * 
 */

namespace log
{
    typedef enum
    {
        ROVER_OK  = 0,
        ROVER_ERR = 1,
    }ROVER_EC;
    ROVER_EC return_err_if_esp_err(const char * tag, esp_err_t err_code);
}
