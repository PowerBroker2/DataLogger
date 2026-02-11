#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "Base.h"
#include "Utils.h"
#include "sdutils.h"

class SdLogger : public LoggerBase
{
public:
     SdLogger() = default;
    ~SdLogger() override;

    void begin(const SdioConfig& _sdConfig,
               const char*       _filePath);
    void end() override;

    void  setLogFilePath(const char* _filePath);
    char* getLogFilePath();

    int flush() override;

protected:
    SdFs    sd;
    FsFile  logFile;

    bool append = true;
};
