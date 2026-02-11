#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "Base.h"
#include "Utils.h"
#include "sdutils.h"

/**
 * @brief SD card logger implementation of LoggerBase.
 * 
 * Handles writing logs to an SD card using SdFat.
 */
class SdLogger : public LoggerBase
{
public:
    /** Default constructor */
     SdLogger() = default;

    /** Destructor; ensures logging is properly ended */
    ~SdLogger() override;

    /**
     * @brief Initialize the SD card and open the log file.
     * 
     * Ends any existing logging session before starting.
     * 
     * @param _sdConfig SD card configuration (SdioConfig)
     * @param _filePath Path of the log file
     */
    void begin(const SdioConfig& _sdConfig,
               const char*       _filePath);

    /**
     * @brief End the logging session.
     * 
     * Flushes buffers, closes the log file, and resets state.
     */
    void end() override;

    /**
     * @brief Set the log file path for this logger.
     * 
     * Ends any existing logging session before changing the file.
     * Creates the directory if it does not exist.
     * 
     * @param _filePath Path of the log file
     */
    void  setLogFilePath(const char* _filePath);

    /**
     * @brief Get the currently set log file path.
     * @return Pointer to a null-terminated string containing the log file path
     */
    char* getLogFilePath();

    /**
     * @brief Flush the internal buffer to the SD card.
     * @return Number of bytes written
     */
    int flush() override;

protected:
    SdFs    sd;       /**< SD card filesystem object */
    FsFile  logFile;  /**< File handle for the log file */

    bool append = true; /**< Whether to append to existing log file */
};
