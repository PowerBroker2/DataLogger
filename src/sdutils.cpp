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
