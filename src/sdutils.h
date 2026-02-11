#pragma once
#include <Arduino.h>
#include <SdFat.h>

/**
 * @brief Join two path components with a '/' if needed.
 *
 * Safely concatenates a base path and an additional path component.
 * If the base path already ends with '/', no extra slash is inserted.
 * The result is always null-terminated (subject to outSize).
 *
 * @param path     Base path (e.g. "/logs")
 * @param add      Path component to append (e.g. "data.csv")
 * @param out      Output buffer to receive the joined path
 * @param outSize  Size of the output buffer in bytes
 */
void join(const char   path[],
          const char   add[],
                char   out[],
                size_t outSize);

/**
 * @brief Extract the filename portion of a path.
 *
 * Copies the last path component (after the final '/') into @p out.
 * Trailing slashes are ignored.
 *
 * @param path     Full path string
 * @param out      Output buffer to receive the basename
 * @param outSize  Size of the output buffer in bytes
 */
void basename(const char   path[],
                    char   out[],
                    size_t outSize);

/**
 * @brief Extract the directory portion of a path.
 *
 * Copies the directory portion of @p path into @p out.
 * If no directory separator is present, "." is returned.
 *
 * @param path     Full path string
 * @param out      Output buffer to receive the directory name
 * @param outSize  Size of the output buffer in bytes
 */
void dirname(const char   path[],
                   char   out[],
                   size_t outSize);

/**
 * @brief Create a directory on the SD card if it does not already exist.
 *
 * Checks whether the directory exists and attempts to create it if not.
 *
 * @param _sd   Reference to an initialized SdFs object
 * @param path  Directory path to create
 *
 * @return true if the directory exists or was successfully created,
 *         false otherwise
 */
bool mkdir(SdFs& _sd,
           const char path[]);

/**
 * @brief Generate a unique filename on the SD card.
 *
 * If the provided file path already exists, a numeric suffix of the
 * form "(N)" is inserted before the file extension until a non-existent
 * filename is found.
 *
 * Example:
 *   - "log.csv"   → "log.csv"
 *   - "log.csv"   → "log(1).csv"
 *   - "log.csv"   → "log(2).csv"
 *
 * @param _sd        Reference to an initialized SdFs object
 * @param _logFile   Reference to an FsFile object (not opened by this function)
 * @param _filePath  Desired file path
 * @param out        Output buffer to receive the unique file path
 * @param outSize    Size of the output buffer in bytes
 */
void uniqueFileName(      SdFs&   _sd,
                          FsFile& _logFile,
                    const char    _filePath[],
                          char    out[],
                          size_t  outSize);