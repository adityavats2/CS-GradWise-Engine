/**
 * @file CourseCatalogLoader.h
 * @brief Declares the parser that fills a CourseCatalog from a text file.
 */

#ifndef COURSECATALOGLOADER_H
#define COURSECATALOGLOADER_H
#include <string>
#include "CourseCatalog.h"

/**
 * @class CourseCatalogLoader
 * @brief Static parser for the project COURSE/END block format (CODE, TITLE, CREDITS, prerequisites, etc.).
 *
 * On parse failure, behavior depends on where the error occurred; callers should treat false return as failure.
 */
class CourseCatalogLoader {
public:
    /**
     * @brief Opens and parses filePath into catalog.
     * @param filePath Path to courses file (often relative to process working directory).
     * @param catalog Destination; caller typically clears before load.
     * @return True if the file opened and parsing completed successfully.
     */
    static bool loadFromFile(const std::string& filePath, CourseCatalog& catalog);
};

#endif
