#include "Arduino.h"
#include "SdFat.h"
#include "DataLogger.h"




void logger::begin(const bool& _csv, const uint32_t& _buffSize)
{
    useStream = true;
    stream    = &Serial;
    
    setLogType(_csv);
    buffSize  = (_buffSize / 16) * 16; // Force buffer size to be multiple of 16 bytes (128 bits) to make encryption easier if used
    buff      = new char[buffSize];
    buffIndex = 0;
    memset(buff, ' ', buffSize);

    messageIndex = 0;
}




void logger::setStream(Stream& _stream)
{
    useStream = true;
    stream    = &_stream;
}




void logger::setSD(SdFs& _sd, FsFile& _logFile, const char* _filePath)
{
    useStream = false;
    sd        = &_sd;
    logFile   = &_logFile;

    if (_filePath != NULL)
    {
        filePath = _filePath;

        if (!sd->exists(dirname(filePath)))
            mkdir(*sd, dirname(filePath));

        logFile->open(filePath, FILE_WRITE);
    }
}




void logger::setLogType(const bool& _csv)
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
    uint16_t output = 0;

    for (uint32_t i=0; i<messageIndex; i++)
        output += message[i];
    
    return ~output;
}




int16_t logger::cpyMessageToBuff(const int& startIndex)
{
    int16_t available    = buffSize - buffIndex;
    int16_t toWrite      = messageIndex - startIndex;
    int16_t bytesWritten = 0;

    if (toWrite > available)
        bytesWritten = available;
    else
        bytesWritten = toWrite;

    for (int i=0; i<bytesWritten; i++)
        buff[buffIndex + i] = message[startIndex + i];

    buffIndex += bytesWritten;

    return bytesWritten;
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
            if (!sd->exists(dirname(filePath)))
                mkdir(*sd, dirname(filePath));
            
            logFile->open(filePath, FILE_WRITE);
        }

        int numWritten = logFile->write(buff, buffIndex);

        if (!numWritten)
        {
            _isConnected = false;
            return 0;
        }
        else
            _isConnected = true;

        logFile->close();
        logFile->open(filePath, FILE_WRITE);
    }
        
    buffIndex = 0;
    memset(buff, ' ', buffSize);
    
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




bool logger::addDoubleStrMessageField(const double&  val,
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




bool logger::addChecksumMessageField()
{
    if (csv)
    {
        sprintf(field, "%hu", calculateChecksum());
        return addMessageFieldPtr(field, strlen(field));
    }
    else
        return addMessageField(calculateChecksum());
}
