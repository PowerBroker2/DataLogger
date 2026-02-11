#include "DataLogger.h"
#include "SdFat.h"

#ifdef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif

#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdioConfig(FIFO_SDIO)

using namespace log_space;

SdLogger mySdLog;

/* Fixed buffers (avoid malloc on embedded) */
char filePath[MAX_FILE_PATH_LEN + 1] = {0};
char tempPath[MAX_FILE_PATH_LEN + 1] = {0};

/* Example helper */
bool writeComment(uint32_t sysUsCounter, const char* comment)
{
    mySdLog.resetMessageIndex();

    if (mySdLog.getLogType() == CSV)
    {
        mySdLog.addIntStrMessageField(0);
        mySdLog.addDelimiter();
        mySdLog.addUIntStrMessageField(sysUsCounter);
        mySdLog.addDelimiter();
        mySdLog.addMessageFieldPtr(comment, strlen(comment));
        mySdLog.addDelimiter();
        mySdLog.addChecksumMessageField();
        mySdLog.addNewline();
    }
    else
    {
        mySdLog.addMessageField(PREAMBLE);
        mySdLog.addMessageField((uint8_t)0);
        mySdLog.addMessageField(sysUsCounter);
        mySdLog.addMessageFieldPtr(comment, strlen(comment));
        mySdLog.addChecksumMessageField();
    }

    return mySdLog.finishUpMessage();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("Logger example starting");

    /* Build file path: /logs/test/test.txt */
    join("logs", "test", tempPath, sizeof(tempPath));
    join(tempPath, "test.txt", filePath, sizeof(filePath));

    Serial.print("Log path: ");
    Serial.println(filePath);

    /* Initialize SD output */
    mySdLog.begin(SD_CONFIG, filePath);

    if (!mySdLog.isConnected())
        Serial.println("SD logging not connected");
    else
        Serial.println("SD logging ready");
}

void loop()
{
    /* CSV → SD */
    mySdLog.setLogType(CSV);
    writeComment(micros(), "CSV to SD");
    Serial.println("Logged CSV Line to SD");
    mySdLog.flush();

    /* Binary → SD */
    // myLog.setLogType(BINARY);
    // writeComment(micros(), "Binary to SD");
    // myLog.flush();

    delay(1000);
}
