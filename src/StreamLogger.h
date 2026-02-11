#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "Base.h"
#include "Utils.h"
#include "sdutils.h"

/**
 * @brief Logger that writes to any Arduino Stream (e.g., Serial, HardwareSerial, SoftwareSerial).
 * 
 * Implements LoggerBase interface for logging messages to a Stream instead of a file.
 */
class StreamLogger : public LoggerBase
{
public:
    /** Default constructor */
     StreamLogger() = default;

    /** Destructor; ensures logging session is ended */
    ~StreamLogger() override;

    /**
     * @brief Begin logging to the specified stream.
     * 
     * Ends any previous logging session.
     * 
     * @param _stream Reference to the Stream object to log to
     */
    void begin(Stream& _stream);

    /**
     * @brief End the logging session.
     * 
     * Resets buffers and internal state.
     */
    void end() override;

    /**
     * @brief Flush the internal buffer to the Stream.
     * 
     * @return Number of bytes written
     */
    int flush() override;

protected:
    Stream* stream; /**< Pointer to the target Stream for logging */
};
