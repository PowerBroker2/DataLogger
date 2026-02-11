#include "DataLogger.h"

using namespace log_space;

/* Stream logger instead of SD logger */
StreamLogger myStreamLog;

/* Example helper */
bool writeComment(uint32_t sysUsCounter, const char* comment)
{
    myStreamLog.resetMessageIndex();

    if (myStreamLog.getLogType() == CSV)
    {
        myStreamLog.addIntStrMessageField(0);
        myStreamLog.addDelimiter();
        myStreamLog.addUIntStrMessageField(sysUsCounter);
        myStreamLog.addDelimiter();
        myStreamLog.addMessageFieldPtr(comment, strlen(comment));
        myStreamLog.addDelimiter();
        myStreamLog.addChecksumMessageField();
        myStreamLog.addNewline();
    }
    else
    {
        myStreamLog.addMessageField(PREAMBLE);
        myStreamLog.addMessageField((uint8_t)0);
        myStreamLog.addMessageField(sysUsCounter);
        myStreamLog.addMessageFieldPtr(comment, strlen(comment));
        myStreamLog.addChecksumMessageField();
    }

    return myStreamLog.finishUpMessage();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("Stream Logger example starting");

    /* Initialize Stream output (Serial in this example) */
    myStreamLog.begin(Serial);

    if (!myStreamLog.isConnected())
        Serial.println("Stream logging not connected");
    else
        Serial.println("Stream logging ready");
}

void loop()
{
    /* CSV → Stream */
    myStreamLog.setLogType(CSV);
    writeComment(micros(), "CSV to Stream");
    Serial.println("Logged CSV Line to Stream");
    myStreamLog.flush();

    /* Binary → Stream */
    // myStreamLog.setLogType(BINARY);
    // writeComment(micros(), "Binary to Stream");
    // myStreamLog.flush();

    delay(1000);
}
