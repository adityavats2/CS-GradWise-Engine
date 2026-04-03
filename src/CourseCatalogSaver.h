#ifndef COURSE_CATALOG_SAVER_H
#define COURSE_CATALOG_SAVER_H

#include "CourseCatalog.h"
#include "Course.h"
#include "CourseOffering.h"
#include "TimeSlot.h"
#include "Prerequisite.h"

#include <fstream>
#include <sstream>

class CourseCatalog;

/**
 * @brief Helper class used to save added courses to the catalog file
 */
class CourseCatalogSaver {
public:
    /**
     * @brief Save an added course to the catalog file
     * @param filePath the path fo the file to write to
     * @param catalog The course catalog to save
     * @return true if everything was saved successfully, false otherwise
     */
    static bool saveToFile(const std::string& filePath, const CourseCatalog& catalog);
};

#endif