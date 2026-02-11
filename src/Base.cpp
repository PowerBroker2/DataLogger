#include "Arduino.h"
#include "SdFat.h"
#include "Base.h"
#include "sdutils.h"

void LoggerBase::setLogType(bool _csv)
{
    csv = _csv;
}

bool LoggerBase::getLogType()
{
    return csv;
}

bool LoggerBase::isConnected()
{
    return _isConnected;
}

void LoggerBase::resetMessageIndex()
{
    messageIndex = 0;
}

uint16_t LoggerBase::calculateChecksum()
{
    uint16_t sum = 0;
    for (uint32_t i = 0; i < messageIndex; i++)
        sum += (uint8_t)message[i];

    return ~sum;
}

int16_t LoggerBase::cpyMessageToBuff(int startIndex)
{
    int16_t available = sizeof(buff) - buffIndex;
    int16_t toWrite   = messageIndex - startIndex;
    int16_t bytes     = min(available, toWrite);

    memcpy(&buff[buffIndex], &message[startIndex], bytes);
    buffIndex += bytes;

    return bytes;
}

bool LoggerBase::finishUpMessage()
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

bool LoggerBase::addDoubleStrMessageField(const double& val,
                                      const int16_t& numAfterDecimal,
                                      const int16_t& minStringWidth)
{
    dtostrf(val, minStringWidth, numAfterDecimal, field);
    return addMessageFieldPtr(field, strlen(field));
}

bool LoggerBase::addIntStrMessageField(const int32_t& val,
                                       const int16_t& radix)
{
    itoa(val, field, radix);
    return addMessageFieldPtr(field, strlen(field));
}

bool LoggerBase::addUIntStrMessageField(const uint32_t& val,
                                        const int16_t&  radix)
{
    utoa(val, field, radix);
    return addMessageFieldPtr(field, strlen(field));
}

bool LoggerBase::addDelimiter()
{
    return addMessageField(',');
}

bool LoggerBase::addNewline()
{
    return addMessageField('\n');
}

bool LoggerBase::addChecksumMessageField()
{
    if (csv)
    {
        sprintf(field, "%hu", calculateChecksum());
        return addMessageFieldPtr(field, strlen(field));
    }
    return addMessageField(calculateChecksum());
}

bool LoggerBase::addMessageFieldPtr(uint8_t* val, const uint16_t& len)
{
    uint32_t newIndex = messageIndex + len;
    if (newIndex > log_space::MAX_MESSAGE_LEN)
        return false;

    memcpy(&message[messageIndex], val, len);
    messageIndex = newIndex;
    return true;
}

bool LoggerBase::addMessageFieldPtr(const char* val, const uint16_t& len)
{
    uint32_t newIndex = messageIndex + len;
    if (newIndex > log_space::MAX_MESSAGE_LEN)
        return false;

    memcpy(&message[messageIndex], val, len);
    messageIndex = newIndex;
    return true;
}
