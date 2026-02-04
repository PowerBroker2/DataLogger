#pragma once
#include "Arduino.h"
#include "SdFat.h"

/**
 * @namespace log_space
 * @brief Library-wide constants and settings
 */
namespace log_space
{
    const bool CSV    = true;   /**< CSV mode flag */
    const bool BINARY = false;  /**< Binary mode flag */

    const char ver[] = "0.0.1"; /**< Library version string */

    const uint16_t PREAMBLE = 0xA0D; /**< Binary packet preamble */

    const float INVALID_NUM   = -999999.0; /**< Default invalid numeric value */
    const char  INVALID_STR[] = "-";       /**< Default invalid string */

    const int MAX_MESSAGE_LEN  = 500; /**< Maximum allowed message length */
    const int MAX_FIELD_LEN    = 100; /**< Maximum allowed field length */
    const int DEFAULT_BUFF_SIZE = 512; /**< Default internal buffer size */
    const int MAX_FILE_PATH_LEN = 100; /**< Maximum file path length */
};

/**
 * @brief Count occurrences of a character in a string
 * @param input Input string
 * @param target Character to count
 * @return Number of occurrences of target in input
 */
uint8_t numOccur(char* input, const char& target);

/**
 * @brief Find the nth occurrence of a substring in a string
 * @param input Input string
 * @param target Substring to search for
 * @param ithOccur Occurrence index (1-based)
 * @return Pointer to substring start, or NULL if not found
 */
char* findOccur(char input[], char target[], uint16_t ithOccur);

/**
 * @brief Extract the substring between delimiters
 * @param input Input string
 * @param place Index of delimiter occurrence (1-based)
 * @param delim Delimiter string
 * @return Pointer to allocated substring (caller must free)
 */
char* findSubStr(char input[], uint8_t place, const char* delim);

/**
 * @brief Join two paths safely with '/' separator
 * @param path Base path
 * @param add Path to append
 * @return Pointer to newly allocated string containing joined path
 */
char* join(char path[], char add[]);

/**
 * @brief Extract basename (filename) from path
 * @param path Input path
 * @return Allocated string containing basename
 */
char* basename(char path[]);

/**
 * @brief Extract directory name from path
 * @param path Input path
 * @return Allocated string containing directory name
 */
char* dirname(char path[]);

/**
 * @brief Create a directory on SD card if it does not exist
 * @param _sd SdFs instance
 * @param path Directory path
 * @return true if directory created successfully
 */
bool mkdir(SdFs& _sd, char path[]);

/**
 * @brief Generate a unique log filename to prevent overwriting
 * @param _sd SdFs instance
 * @param _logFile FsFile instance
 * @param _filePath Input file path, updated to unique name
 * @return true if unique name successfully generated
 */
bool getUniqueLogName(SdFs&   _sd,
                      FsFile& _logFile,
                      char*   _filePath);

/**
 * @class logger
 * @brief Main logging class for writing to Serial or SD card
 */
class logger
{
public: // <<---------------------------------------//public

    /**
     * @brief Initialize logger
     * @param _csv true = CSV mode, false = binary mode
     * @param _buffSize Size of internal buffer
     */
    void begin(const bool& _csv          = true,
               const uint32_t& _buffSize = log_space::DEFAULT_BUFF_SIZE);

    /**
     * @brief Set output to a Stream (Serial, HardwareSerial, etc.)
     * @param _stream Reference to output stream
     */
    void setOutput(Stream& _stream);

    /**
     * @brief Set output to SD card file
     * @param _sd SdFs instance
     * @param _logFile FsFile instance
     * @param _filePath Path to log file
     */
    void setOutput(      SdFs&   _sd,
                         FsFile& _logFile,
                   const char*   _filePath);

    /** @brief Set CSV or binary mode */
    void setLogType(const bool& _csv);

    /** @brief Get current log type */
    bool getLogType();

    /** @brief Check if logger is connected (SD card) */
    bool isConnected();

    /** @brief Reset current message buffer index */
    void resetMessageIndex();

    /** @brief Calculate checksum of current message buffer */
    uint16_t calculateChecksum();

    /**
     * @brief Copy message buffer into internal buffer starting at index
     * @param startIndex Index to start copying from
     * @return Number of bytes copied
     */
    int16_t cpyMessageToBuff(const int& startIndex = 0);

    /** @brief Flush internal buffer to output (Stream or SD card) */
    virtual int flush();

    /**
     * @brief Add double value as formatted string field
     * @param val Double value to add
     * @param numAfterDecimal Number of decimal places
     * @param minStringWidth Minimum width of string
     */
    bool addDoubleStrMessageField(const double&  val,
                                  const int16_t& numAfterDecimal = 6,
                                  const int16_t& minStringWidth  = 1);

    /**
     * @brief Add integer value as formatted string field
     * @param val Integer value to add
     * @param radix Base for number conversion (default = 10)
     */
    bool addIntStrMessageField(const int32_t& val,
                               const int16_t& radix = 10);

    /** @brief Add CSV delimiter ',' */
    bool addDelimiter();

    /** @brief Add newline '\n' */
    bool addNewline();

    /** @brief Add checksum field to message */
    bool addChecksumMessageField();

    /** @brief Finish current message and copy to buffer if needed */
    bool finishUpMessage();

    /**
     * @brief Add raw message field (templated)
     * @tparam T Type of data
     * @param val Value to add
     * @param len Number of bytes to copy (default = sizeof(T))
     * @return true if added successfully
     */
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

    /**
     * @brief Add a pointer to raw byte data
     * @param val Pointer to data
     * @param len Number of bytes to copy
     * @return true if added successfully
     */
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

    /**
     * @brief Add a pointer to raw char data
     * @param val Pointer to char data
     * @param len Number of characters to copy
     * @return true if added successfully
     */
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

    bool     _isConnected = false;                  /**< SD card connected status */
    Stream*  stream;                                /**< Pointer to output stream */
    SdFs*    sd;                                     /**< Pointer to SD instance */
    FsFile*  logFile;                                /**< Pointer to SD file */
    char*    filePath;                               /**< Path to log file */
    bool     useStream;                              /**< True = Stream output, false = SD card */
    bool     csv;                                    /**< CSV or binary mode flag */
    char*    buff = NULL;                            /**< Internal buffer pointer */
    uint32_t buffSize;                               /**< Size of internal buffer */
    uint32_t buffIndex;                              /**< Current write index in buffer */
    char     message[log_space::MAX_MESSAGE_LEN];    /**< Current message buffer */
    uint32_t messageIndex;                           /**< Current message index */
    char     field[log_space::MAX_FIELD_LEN];       /**< Temporary field buffer */
    int      fieldLen;                               /**< Length of current field */
};
