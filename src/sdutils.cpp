#include "Arduino.h"
#include "SdFat.h"
#include "DataLogger.h"




uint8_t numOccur(char* input, const char& target)
{
    char* p = input;
    uint8_t count = 0;

    while (*p != '\0')
        if (*p++ == target)
            count++;

    return count;
}




char* findOccur(char input[], char target[], uint16_t ithOccur)
{
	char* argP = input;
	char* divP = NULL;

	for (uint8_t i=0; i<ithOccur; i++)
	{
		divP = strstr(argP, target);

		if (!divP)
			return NULL;

		argP = divP + 1;
	}

	return divP;
}




char* findSubStr(char input[], uint8_t place, const char* delim)
{
	char* argP = input;
	char* divP = NULL;
	char* returnStr;

	for (uint8_t i=0; i<place; i++)
	{
		divP = strstr(argP, delim);

		if (!divP)
			return NULL;

		argP = divP + 1;
	}

	divP = strstr(argP, delim);

	if (divP)
	{
		uint32_t argLen = divP - argP + 1;

		returnStr = (char*)malloc(argLen);
		memcpy(returnStr, argP, argLen);
		returnStr[argLen - 1] = '\0';
	}
	else
	{
		returnStr = (char *)malloc(strlen(argP) + 1);
		memcpy(returnStr, argP, strlen(argP) + 1);
	}

	return returnStr;
}




char* join(char path[], char add[])
{
	uint16_t pathLen = strlen(path);
	uint16_t addLen = strlen(add);
	uint16_t joinedPathLen = pathLen + addLen + 1; // add one for joining slash

	char* joinedPath = (char*)malloc(joinedPathLen);

	if (path[pathLen-1] == '/')
		sprintf(joinedPath, "%s%s", path, add);
	else
		sprintf(joinedPath, "%s/%s", path, add);

	return joinedPath;
}




char* basename(char path[])
{
    uint8_t numLevels = numOccur(path, '/');

    if (path[strlen(path) - 1] == '/')
        numLevels--;

    return findSubStr(path, numLevels, "/");
}




char* dirname(char path[])
{
	char* dirName = (char*)malloc(strlen(path) + 1);

	memcpy(dirName, path, strlen(path) + 1);
	uint8_t numLevels = numOccur(dirName, '/');

	if (dirName[strlen(dirName) - 1] == '/')
		numLevels--;

	if (numLevels > 1)
	{
		char rootStr[] = { '/', '\0' };
		dirName[(int)findOccur(dirName, rootStr, numLevels) - (int)dirName] = '\0';
	}
	else
	{
		dirName[0] = '/';
		dirName[1] = '\0';
	}

	return dirName;
}




bool mkdir(SdFs& _sd, char path[])
{
    if (!_sd.exists(path))
    {
        _sd.mkdir(path);

        if (_sd.exists(path))
            return true;
        else
            return false;
    }
    
    return false;
}




bool getUniqueLogName(SdFs&   _sd,
                      FsFile& _logFile,
                      char*   _filePath)
{
    if (strlen(_filePath) <= log_space::MAX_FILE_PATH_LEN)
    {
        // initialize helper variables
        int  _filePathStrLen = strlen(_filePath);
        char basename[90]    = {'\0'};
        int  basenameStrLen  = 0;
        char extension[10]   = {'\0'};
        int  extensionStrLen = 0;
        char addon[5]        = {'\0'};
        int  addonStrLen     = 0;
        int  addonNum        = 0;
        char fnameLen        = log_space::MAX_FILE_PATH_LEN;
        char fname[fnameLen] = {'\0'};

        // Find start of file extension if exists
        char* perPtr = strstr(_filePath, ".");

        if (perPtr == NULL)
        {
            strncpy(basename, _filePath, _filePathStrLen);
        }
        else
        {
            strncpy(basename, _filePath, int(perPtr - _filePath));
            strncpy(extension, perPtr, int((_filePath + _filePathStrLen) - perPtr));
        }

        basenameStrLen  = strlen(basename);
        extensionStrLen = strlen(extension);

        memset(fname, '\0', fnameLen);
        strncpy(fname, basename, basenameStrLen);
        strncpy(fname + basenameStrLen, extension, extensionStrLen);

        // Find fname that doesn't already exist
        while (_sd.exists(fname))
        {
            // Always start with base fname
            memset(fname, '\0', strlen(fname));
            strncpy(fname, basename, basenameStrLen);

            // Construct unique addon
            sprintf(addon, "(%u)", addonNum);
            addonStrLen = strlen(addon);
            addonNum++;

            // Add unique addon to base fname
            strncpy(fname + basenameStrLen, addon, addonStrLen);
            strncpy(fname + basenameStrLen + addonStrLen, extension, extensionStrLen);
        }

        strncpy(_filePath, fname, strlen(fname));

        return true;
    }

    return false;
}
