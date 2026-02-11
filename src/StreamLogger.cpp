#include "Arduino.h"
#include "SdFat.h"
#include "StreamLogger.h"
#include "sdutils.h"

StreamLogger::~StreamLogger()
{
    end();
}

void StreamLogger::begin(Stream& _stream)
{
    end();

    stream       = &_stream;
    _isConnected = true;
}

void StreamLogger::end()
{
    buffIndex    = 0;
    messageIndex = 0;
    stream       = nullptr;
    _isConnected = false;
}

int StreamLogger::flush()
{
    if (!stream || buffIndex == 0)
        return 0;

    int output = buffIndex;

    stream->write(buff, buffIndex);

    buffIndex = 0;
    return output;
}
