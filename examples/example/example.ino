#include "DataLogger.h"
#include "SdFat.h"




#ifdef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif // SDCARD_SS_PIN
#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdioConfig(FIFO_SDIO)




using namespace log_space;




logger myLog;
SdFs   sd;
FsFile logFile;

char* filePath = (char*)malloc(MAX_FILE_PATH_LEN);
auto  temp     = join(join("logs", "test"), "test.txt"); // Results in file path of: /logs/test/test.txt



bool writeComment(const long& sysUsCounter,
                  const char  comment[])
{
    myLog.resetMessageIndex();

    if (myLog.getLogType())
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
        myLog.addMessageField(log_space::PREAMBLE);
        myLog.addMessageField(0);
        myLog.addMessageField(sysUsCounter);
        myLog.addMessageFieldPtr(comment, strlen(comment));
        myLog.addChecksumMessageField();
    }

    return myLog.finishUpMessage();
}




void setup()
{
  Serial.begin(115200);

  if (!sd.begin(SD_CONFIG))
    Serial.println("<<<<<<<<<<<<<<<<<<<< SD initialization failed >>>>>>>>>>>>>>>>>>>>");
  else
    Serial.println("SD initialization succeeded");

  myLog.begin();

  memset(filePath, '\0', MAX_FILE_PATH_LEN);
  strncpy(filePath, temp, strlen(temp)); // Results in file path of: /logs/test/test.txt
  
  Serial.print("File path before: ");
  Serial.println(filePath);
  getUniqueLogName(sd,
                   logFile,
                   filePath);
  Serial.print("File path final: ");
  Serial.println(filePath);
  delay(500);
}




void loop()
{
  // CSV strings to SD card
  myLog.setLogType(CSV);
  myLog.setOutput(sd, logFile, filePath);
  writeComment(micros(), "hiiiiiiiii");
  myLog.flush();
  
  // Binary packets to SD card
  myLog.setLogType(BINARY);
  writeComment(micros(), "hiiiiiiiii");
  myLog.flush();
  
  // CSV strings to Serial port
  myLog.setLogType(CSV);
  myLog.setOutput(Serial);
  writeComment(micros(), "hiiiiiiiii");
  myLog.flush();
  
  // Binary packets to Serial port
  myLog.setLogType(BINARY);
  writeComment(micros(), "hiiiiiiiii");
  myLog.flush();
  
  Serial.println("\nLogged");
  delay(1000);
}