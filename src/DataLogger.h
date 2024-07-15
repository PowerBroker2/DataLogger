#pragma once
#include "Arduino.h"
#include "TimeLib.h"
#include "SdFat.h"




namespace log_space
{
    const bool CSV    = true;
    const bool BINARY = false;

    const char ver[] = "0.0.1";

    const uint16_t PREAMBLE = 0xA0D;
    
    const float INVALID_NUM   = -999999.0;
    const char  INVALID_STR[] = "-";

    const int MAX_MESSAGE_LEN = 500;
    const int MAX_FIELD_LEN   = 100;

    const int DEFAULT_BUFF_SIZE = 512;
};




uint8_t numOccur(char* input, const char& target);
char*   findOccur(char input[], char target[], uint16_t ithOccur);
char*   findSubStr(char input[], uint8_t place, const char* delim);
char*   join(char path[], char add[]);
char*   basename(char path[]);
char*   dirname(char path[]);
bool    mkdir(SdFs& _sd, char path[]);




class logger
{
public: // <<---------------------------------------//public
    void     begin(const bool& _csv          = true,
                   const uint32_t& _buffSize = log_space::DEFAULT_BUFF_SIZE);
    void     setStream(Stream& _stream);
    void     setSD(SdFs&       _sd,
                   FsFile&     _logFile,
                   const char* _filePath);
    void     setLogType(const bool& _csv);
    bool     getLogType();
    bool     isConnected();
    void     resetMessageIndex();
    uint16_t calculateChecksum();
    int16_t  cpyMessageToBuff(const int& startIndex = 0);
    int      flush();
    bool     addDoubleStrMessageField(const double&  val,
                                      const int16_t& numAfterDecimal = 6,
                                      const int16_t& minStringWidth  = 1);
    bool     addIntStrMessageField(const int32_t& val,
                                   const int16_t& radix = 10);
    bool     addChecksumMessageField();
    bool     finishUpMessage();

    template <typename T>
    bool addMessageField(const T& val, const uint16_t& len = sizeof(T))
    {
        uint8_t* ptr = (uint8_t*)&val;

        uint32_t newMessageIndex = messageIndex + len;

        if (newMessageIndex <= log_space::MAX_MESSAGE_LEN)
        {
            for (int i=0; i<len; i++)
            {
                message[messageIndex + i] = *ptr;
                ptr++;
            }

            messageIndex = newMessageIndex;
            return true;
        }

        return false;
    };

    bool addMessageFieldPtr(uint8_t* val, const uint16_t& len)
    {
        uint8_t* ptr = val;

        uint32_t newMessageIndex = messageIndex + len;

        if (newMessageIndex <= log_space::MAX_MESSAGE_LEN)
        {
            for (int i=0; i<len; i++)
            {
                message[messageIndex + i] = *ptr;
                ptr++;
            }

            messageIndex = newMessageIndex;
            return true;
        }

        return false;
    };

    bool addMessageFieldPtr(const char* val, const uint16_t& len)
    {
        const char* ptr = val;

        uint32_t newMessageIndex = messageIndex + len;

        if (newMessageIndex <= log_space::MAX_MESSAGE_LEN)
        {
            for (int i=0; i<len; i++)
                message[messageIndex + i] = *(ptr + i);

            messageIndex = newMessageIndex;
            return true;
        }

        return false;
    };




protected: // <<---------------------------------------//protected
    char* buff = NULL;




private: // <<---------------------------------------//private
    bool     _isConnected = false;
    Stream*  stream;
    SdFs*    sd;
    FsFile*  logFile;
    char*    filePath;
    bool     useStream;
    bool     csv;
    uint32_t buffSize;
    uint32_t buffIndex;
    char     message[log_space::MAX_MESSAGE_LEN];
    uint32_t messageIndex;
    char     field[log_space::MAX_FIELD_LEN];
    int      fieldLen;
};
