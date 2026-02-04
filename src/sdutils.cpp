#include "Arduino.h"
#include "SdFat.h"
#include "DataLogger.h"




void join(const char path[], const char add[], char out[], size_t outSize)
{
    if (!path || !add || !out || outSize == 0)
        return;  // safety: null pointers or zero-length buffer

    size_t pathLen = strlen(path);

    // Determine if we need a slash
    bool needSlash = (pathLen > 0 && path[pathLen - 1] != '/');

    // snprintf automatically truncates if outSize is too small
    if (needSlash)
        snprintf(out, outSize, "%s/%s", path, add);
    else
        snprintf(out, outSize, "%s%s", path, add);
}




void basename(const char path[], char out[], size_t outSize)
{
    if (!path || !out || outSize == 0)
    {
        if (outSize > 0) out[0] = '\0';
        return;
    }

    size_t len = strlen(path);
    if (len == 0)
    {
        out[0] = '\0';
        return;
    }

    // Skip trailing slashes
    while (len > 0 && path[len - 1] == '/')
        len--;

    if (len == 0)
    {
        out[0] = '\0';
        return;
    }

    // Find last slash
    const char* lastSlash = path;
    for (size_t i = 0; i < len; i++)
    {
        if (path[i] == '/')
            lastSlash = path + i + 1;  // character after last slash
    }

    // Copy basename into out, safely
    snprintf(out, outSize, "%.*s", (int)(len - (lastSlash - path)), lastSlash);
}




void dirname(const char path[], char out[], size_t outSize)
{
    if (!path || !out || outSize == 0)
    {
        if (outSize > 0) out[0] = '\0';
        return;
    }

    size_t len = strlen(path);
    if (len == 0)
    {
        snprintf(out, outSize, ".");
        return;
    }

    // Strip trailing slashes
    while (len > 1 && path[len - 1] == '/')
        len--;

    // Find last slash in trimmed path
    int lastSlash = -1;
    for (size_t i = 0; i < len; i++)
    {
        if (path[i] == '/')
            lastSlash = i;
    }

    if (lastSlash == -1)
    {
        snprintf(out, outSize, ".");
    }
    else if (lastSlash == 0)
    {
        snprintf(out, outSize, "/");
    }
    else
    {
        snprintf(out, outSize, "%.*s", (int)lastSlash, path);
    }
}




bool mkdir(SdFs& _sd, const char path[])
{
    if (!path || strlen(path) == 0)
        return false;

    if (_sd.exists(path))
        return true;

    if (_sd.mkdir(path))
        return true;

    return false;
}





void uniqueFileName(SdFs& _sd,
                    FsFile& _logFile,
                    const char _filePath[],
                    char out[],
                    size_t outSize)
{
    if (!_filePath || !out || outSize == 0)
    {
        if (out && outSize > 0)
            out[0] = '\0';
        return;
    }

    // Find last '.' to separate extension
    const char* dot = strrchr(_filePath, '.');
    size_t baseLen  = dot ? (size_t)(dot - _filePath) : strlen(_filePath);

    unsigned int addonNum = 0;

    while (true)
    {
        if (addonNum == 0)
        {
            // Original filename
            snprintf(out, outSize,
                     "%.*s%s",
                     (int)baseLen,
                     _filePath,
                     dot ? dot : "");
        }
        else
        {
            // Add (N) before extension
            snprintf(out, outSize,
                     "%.*s(%u)%s",
                     (int)baseLen,
                     _filePath,
                     addonNum,
                     dot ? dot : "");
        }

        if (!_sd.exists(out))
            return;

        addonNum++;
    }
}
