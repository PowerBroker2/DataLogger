#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "Utils.h"
#include "sdutils.h"

class LoggerBase
{
public:
             LoggerBase() = default;
    virtual ~LoggerBase() = default;

    virtual void end() = 0;

    void setLogType(bool _csv);
    bool getLogType();
    bool isConnected();

            void     resetMessageIndex();
            uint16_t calculateChecksum();
            int16_t  cpyMessageToBuff(int startIndex = 0);
    virtual int      flush() = 0;

    bool addDoubleStrMessageField(const double& val,
                                  const int16_t& numAfterDecimal = 6,
                                  const int16_t& minStringWidth  = 1);

    bool addIntStrMessageField(const int32_t& val,
                               const int16_t& radix = 10);
    bool addUIntStrMessageField(const uint32_t& val,
                                const int16_t&  radix = 10);

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
    bool _isConnected = false;

    bool csv = true;

    char filePath[log_space::MAX_FILE_PATH_LEN + 1] = {0};
    char dirPath [log_space::MAX_FILE_PATH_LEN + 1] = {0};

    char     buff[log_space::BUFF_SIZE] = {0};
    uint32_t buffIndex = 0;

    char     message[log_space::MAX_MESSAGE_LEN] = {0};
    uint32_t messageIndex = 0;

    char field[log_space::MAX_FIELD_LEN] = {0};
};
