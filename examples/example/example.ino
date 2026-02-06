#include "DataLogger.h"
#include "SdFat.h"

#ifdef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif

#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdioConfig(FIFO_SDIO)

using namespace log_space;

logger myLog;

/* Fixed buffers (avoid malloc on embedded) */
char filePath[MAX_FILE_PATH_LEN + 1] = {0};
char tempPath[MAX_FILE_PATH_LEN + 1] = {0};

/* Example helper */
bool writeComment(uint32_t sysUsCounter, const char* comment)
{
    myLog.resetMessageIndex();

    if (myLog.getLogType() == CSV)
    {
        myLog.addIntStrMessageField(0);
        myLog.addDelimiter();
        myLog.addIntStrMessageField(sysUsCounter);
        myLog.addDelimiter();
        myLog.addMessageFieldPtr(comment, strlen(comment));
        myLog.addDelimiter();
        myLog.addChecksumMessageField();
        myLog.addNewline();
    }
    else
    {
        myLog.addMessageField(PREAMBLE);
        myLog.addMessageField((uint8_t)0);
        myLog.addMessageField(sysUsCounter);
        myLog.addMessageFieldPtr(comment, strlen(comment));
        myLog.addChecksumMessageField();
    }

    return myLog.finishUpMessage();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("Logger example starting");

    myLog.begin();

    /* Build file path: /logs/test/test.txt */
    join("logs", "test", tempPath, sizeof(tempPath));
    join(tempPath, "test.txt", filePath, sizeof(filePath));

    Serial.print("Log path: ");
    Serial.println(filePath);

    /* Initialize SD output */
    myLog.setOutput(SD_CONFIG, filePath);

    if (!myLog.isConnected())
        Serial.println("SD logging not connected");
    else
        Serial.println("SD logging ready");
}

void loop()
{
    /* CSV → SD */
    myLog.setLogType(CSV);
    myLog.setOutput(SD_CONFIG, filePath);
    writeComment(micros(), "CSV to SD");
    myLog.flush();

    /* Binary → SD */
    myLog.setLogType(BINARY);
    writeComment(micros(), "Binary to SD");
    myLog.flush();

    /* CSV → Serial */
    myLog.setLogType(CSV);
    myLog.setOutput(Serial);
    writeComment(micros(), "CSV to Serial");
    myLog.flush();

    /* Binary → Serial */
    myLog.setLogType(BINARY);
    writeComment(micros(), "Binary to Serial");
    myLog.flush();

    Serial.println("\nLogged");
    delay(1000);
}
