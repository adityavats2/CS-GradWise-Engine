#ifndef COURSECATALOGLOADER_H
#define COURSECATALOGLOADER_H
#include <string>
#include "CourseCatalog.h"

/** @brief Loads courses and course information from txt file */
class CourseCatalogLoader {
public:
    static bool loadFromFile(const std::string& filePath, CourseCatalog& catalog);
};

#endif