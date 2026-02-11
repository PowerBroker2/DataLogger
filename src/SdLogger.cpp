#include "Arduino.h"
#include "SdFat.h"
#include "SdLogger.h"
#include "sdutils.h"

SdLogger::~SdLogger()
{
    end();
}

void SdLogger::begin(const SdioConfig& _sdConfig,
                     const char*       _filePath)
{
    end();

    if (!sd.begin(_sdConfig))
        return;

    setLogFilePath(_filePath);
}

void SdLogger::end()
{
    buffIndex    = 0;
    messageIndex = 0;

    if (logFile.isOpen())
    {
        logFile.sync();
        logFile.close();
    }

    _isConnected = false;
}

void SdLogger::setLogFilePath(const char* _filePath)
{
    end();

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

    if (append)
        _isConnected = logFile.open(filePath, O_WRONLY | O_CREAT | O_AT_END);
    else
        _isConnected = logFile.open(filePath, O_WRONLY | O_CREAT | O_TRUNC);
}

char* SdLogger::getLogFilePath()
{
    return filePath;
}

int SdLogger::flush()
{
    if (buffIndex == 0)
        return 0;

    int output = buffIndex;

    if (!_isConnected || !logFile.isOpen())
        return 0;

    logFile.write(buff, buffIndex);
    logFile.sync();

    buffIndex = 0;
    return output;
}
