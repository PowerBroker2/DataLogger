#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "Utils.h"
#include "sdutils.h"

/**
 * @brief Base class for all loggers.
 * 
 * Provides common buffer management, message formatting,
 * and virtual methods for flushing and ending logging.
 */
class LoggerBase
{
public:
             LoggerBase() = default;  /**< Default constructor */
    virtual ~LoggerBase() = default;     /**< Virtual default destructor */

    /**
     * @brief End the logging session.
     * 
     * Must be implemented by derived classes.
     */
    virtual void end() = 0;

    /**
     * @brief Set the log format to CSV or binary.
     * @param _csv true for CSV, false for binary
     */
    void setLogType(bool _csv);

    /**
     * @brief Get the current log format.
     * @return true if CSV, false if binary
     */
    bool getLogType();

    /**
     * @brief Check if the logger is connected to the target device/storage.
     * @return true if connected
     */
    bool isConnected();

    /**
     * @brief Reset the message index to start a new message.
     */
    void     resetMessageIndex();

    /**
     * @brief Calculate checksum of the current message buffer.
     * @return 16-bit checksum
     */
    uint16_t calculateChecksum();

    /**
     * @brief Copy the message buffer into the main buffer starting at a given index.
     * @param startIndex Starting index in the message buffer (default 0)
     * @return Number of bytes copied
     */
    int16_t  cpyMessageToBuff(int startIndex = 0);

    /**
     * @brief Flush the buffer to storage.
     * 
     * Must be implemented by derived classes.
     * @return Number of bytes written
     */
    virtual int      flush() = 0;

    /**
     * @brief Add a double value formatted as a string to the message.
     * @param val Value to add
     * @param numAfterDecimal Number of decimal digits (default 6)
     * @param minStringWidth Minimum width of the string (default 1)
     * @return true if added successfully
     */
    bool addDoubleStrMessageField(const double& val,
                                  const int16_t& numAfterDecimal = 6,
                                  const int16_t& minStringWidth  = 1);

    /**
     * @brief Add a signed integer formatted as a string to the message.
     * @param val Value to add
     * @param radix Number base (default 10)
     * @return true if added successfully
     */
    bool addIntStrMessageField(const int32_t& val,
                               const int16_t& radix = 10);

    /**
     * @brief Add an unsigned integer formatted as a string to the message.
     * @param val Value to add
     * @param radix Number base (default 10)
     * @return true if added successfully
     */
    bool addUIntStrMessageField(const uint32_t& val,
                                const int16_t&  radix = 10);

    /**
     * @brief Add a delimiter (comma) to the message.
     * @return true if added successfully
     */
    bool addDelimiter();

    /**
     * @brief Add a newline character to the message.
     * @return true if added successfully
     */
    bool addNewline();

    /**
     * @brief Add the checksum of the current message to the message buffer.
     * @return true if added successfully
     */
    bool addChecksumMessageField();

    /**
     * @brief Finish writing the current message into the main buffer, flushing if necessary.
     * @return true if flush occurred
     */
    bool finishUpMessage();

    /**
     * @brief Add a generic data field to the message buffer.
     * @tparam T Type of the data
     * @param val Value to add
     * @param len Number of bytes to add (default sizeof(T))
     * @return true if added successfully
     */
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

    /**
     * @brief Add raw byte pointer to the message buffer.
     * @param val Pointer to data
     * @param len Number of bytes to add
     * @return true if added successfully
     */
    bool addMessageFieldPtr(uint8_t* val, const uint16_t& len);

    /**
     * @brief Add raw C-string to the message buffer.
     * @param val Pointer to string
     * @param len Number of characters to add
     * @return true if added successfully
     */
    bool addMessageFieldPtr(const char* val, const uint16_t& len);

protected:
    bool _isConnected = false;  /**< Connection status */

    bool csv = true;            /**< CSV mode flag */

    char filePath[log_space::MAX_FILE_PATH_LEN + 1] = {0}; /**< Log file path */
    char dirPath [log_space::MAX_FILE_PATH_LEN + 1] = {0}; /**< Directory path */

    char     buff[log_space::BUFF_SIZE] = {0};             /**< Output buffer */
    uint32_t buffIndex = 0;                                /**< Current index in buffer */

    char     message[log_space::MAX_MESSAGE_LEN] = {0};    /**< Current message buffer */
    uint32_t messageIndex = 0;                             /**< Current message index */

    char field[log_space::MAX_FIELD_LEN] = {0};            /**< Temporary field buffer */
};
