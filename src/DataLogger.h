#pragma once
#include "Arduino.h"
#include "SdFat.h"

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

// path utilities
void join(const char path[], const char add[], char out[], size_t outSize);
void basename(const char path[], char out[], size_t outSize);
void dirname(const char path[], char out[], size_t outSize);

/**
 * @brief Create a directory on SD card if it does not exist
 */
bool mkdir(SdFs& _sd, const char path[]);

void uniqueFileName(SdFs& _sd,
                    FsFile& _logFile,
                    const char _filePath[],
                    char out[],
                    size_t outSize);

/**
 * @class logger
 * @brief Main logging class for writing to Serial or SD card
 */
class logger
{
public:
    void begin();

    void setOutput(Stream& _stream);

    void setOutput(const SdSpiConfig& _sdConfig,
                   const char*        _filePath);

    void setLogType(bool _csv);
    bool getLogType();
    bool isConnected();

    void resetMessageIndex();
    uint16_t calculateChecksum();
    int16_t cpyMessageToBuff(const int& startIndex = 0);
    virtual int flush();

    bool addDoubleStrMessageField(const double& val,
                                  const int16_t& numAfterDecimal = 6,
                                  const int16_t& minStringWidth  = 1);

    bool addIntStrMessageField(const int32_t& val,
                               const int16_t& radix = 10);

    bool addDelimiter();
    bool addNewline();
    bool addChecksumMessageField();
    bool finishUpMessage();

    template <typename T>
    bool addMessageField(const T& val, const uint16_t& len = sizeof(T))
    {
        uint32_t newMessageIndex = messageIndex + len;
        if (newMessageIndex > log_space::MAX_MESSAGE_LEN)
            return false;

        const uint8_t* ptr = (const uint8_t*)&val;
        for (uint16_t i = 0; i < len; i++)
            message[messageIndex + i] = ptr[i];

        messageIndex = newMessageIndex;
        return true;
    }

    bool addMessageFieldPtr(uint8_t* val, const uint16_t& len);
    bool addMessageFieldPtr(const char* val, const uint16_t& len);

protected:
    bool    _isConnected = false;

    Stream* stream  = nullptr;
    SdFs    sd;
    FsFile  logFile;

    bool     useStream = true;
    bool     csv       = true;

    char     filePath[log_space::MAX_FILE_PATH_LEN + 1] = {0};
    char     dirPath [log_space::MAX_FILE_PATH_LEN + 1] = {0};

    char     buff[log_space::BUFF_SIZE] = {0};
    uint32_t buffIndex = 0;

    char     message[log_space::MAX_MESSAGE_LEN] = {0};
    uint32_t messageIndex = 0;

    char     field[log_space::MAX_FIELD_LEN] = {0};
    int      fieldLen = 0;
};
