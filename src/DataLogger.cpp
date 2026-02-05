#include "Arduino.h"
#include "SdFat.h"
#include "DataLogger.h"

void logger::begin()
{
    _isConnected = false;

    buffIndex = 0;
    memset(buff, '\0', sizeof(buff));

    messageIndex = 0;
}

void logger::setOutput(Stream& _stream)
{
    useStream = true;
    stream    = &_stream;
}

void logger::setOutput(const SdSpiConfig& _sdConfig,
                       const char*        _filePath)
{
    _isConnected = false;
    useStream    = false;

    if (!sd.begin(_sdConfig))
        return;

    if (!_filePath)
        return;

    size_t len = strlen(_filePath);
    if (len > log_space::MAX_FILE_PATH_LEN)
        return;

    strncpy(filePath, _filePath, sizeof(filePath));
    filePath[sizeof(filePath) - 1] = '\0';

    dirname(filePath, dirPath, sizeof(dirPath));

    if (!sd.exists(dirPath))
        mkdir(sd, dirPath);

    if (logFile.open(filePath, FILE_WRITE))
        _isConnected = true;
}

void logger::setLogType(bool _csv)
{
    csv = _csv;
}

bool logger::getLogType()
{
    return csv;
}

bool logger::isConnected()
{
    return _isConnected;
}

void logger::resetMessageIndex()
{
    messageIndex = 0;
}

uint16_t logger::calculateChecksum()
{
    uint16_t sum = 0;
    for (uint32_t i = 0; i < messageIndex; i++)
        sum += message[i];

    return ~sum;
}

int16_t logger::cpyMessageToBuff(const int& startIndex)
{
    int16_t available = buffSize - buffIndex;
    int16_t toWrite   = messageIndex - startIndex;
    int16_t bytes     = min(available, toWrite);

    memcpy(&buff[buffIndex], &message[startIndex], bytes);
    buffIndex += bytes;

    return bytes;
}

int logger::flush()
{
    uint32_t output = buffIndex;

    if (useStream)
    {
        stream->write(buff, buffIndex);
    }
    else
    {
        if (!_isConnected)
        {
            if (!sd.exists(dirPath))
                mkdir(sd, dirPath);

            logFile.open(filePath, FILE_WRITE);
        }

        int written = logFile.write(buff, buffIndex);
        _isConnected = (written > 0);

        logFile.close();
        logFile.open(filePath, FILE_WRITE);
    }

    buffIndex = 0;
    memset(buff, '\0', sizeof(buff));

    return output;
}

bool logger::finishUpMessage()
{
    uint32_t wrote = cpyMessageToBuff();
    if (wrote < messageIndex)
    {
        flush();
        cpyMessageToBuff(wrote);
        return true;
    }
    return false;
}

bool logger::addDoubleStrMessageField(const double& val,
                                      const int16_t& numAfterDecimal,
                                      const int16_t& minStringWidth)
{
    dtostrf(val, minStringWidth, numAfterDecimal, field);
    return addMessageFieldPtr(field, strlen(field));
}

bool logger::addIntStrMessageField(const int32_t& val,
                                   const int16_t& radix)
{
    itoa(val, field, radix);
    return addMessageFieldPtr(field, strlen(field));
}

bool logger::addDelimiter()
{
    return addMessageField(',');
}

bool logger::addNewline()
{
    return addMessageField('\n');
}

bool logger::addChecksumMessageField()
{
    if (csv)
    {
        sprintf(field, "%hu", calculateChecksum());
        return addMessageFieldPtr(field, strlen(field));
    }
    return addMessageField(calculateChecksum());
}

bool logger::addMessageFieldPtr(uint8_t* val, const uint16_t& len)
{
    uint32_t newIndex = messageIndex + len;
    if (newIndex > log_space::MAX_MESSAGE_LEN)
        return false;

    memcpy(&message[messageIndex], val, len);
    messageIndex = newIndex;
    return true;
}

bool logger::addMessageFieldPtr(const char* val, const uint16_t& len)
{
    uint32_t newIndex = messageIndex + len;
    if (newIndex > log_space::MAX_MESSAGE_LEN)
        return false;

    memcpy(&message[messageIndex], val, len);
    messageIndex = newIndex;
    return true;
}
