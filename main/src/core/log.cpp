#include "core/log.h"
namespace log
{
    ROVER_EC return_err_if_esp_err(const char * tag, esp_err_t err_code)
    {
        if(err_code != ESP_OK)
        {
            ESP_LOGE(tag, "ERROR : %s", esp_err_to_name(err_code));
            return ROVER_ERR;
        }
        return ROVER_OK;
    }
}
