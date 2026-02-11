#pragma once
#include "Arduino.h"

/**
 * @namespace log_space
 * @brief Library-wide constants and settings
 */
namespace log_space
{
    const bool CSV    = true;
    const bool BINARY = false;

    const char ver[] = "0.0.1";

    const uint16_t PREAMBLE = 0xA0D;

    const float INVALID_NUM   = -999999.0;
    const char  INVALID_STR[] = "-";

    const int MAX_MESSAGE_LEN   = 500;
    const int MAX_FIELD_LEN     = 100;
    const int BUFF_SIZE         = 512;
    const int MAX_FILE_PATH_LEN = 100;
};