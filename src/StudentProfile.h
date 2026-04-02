/**
 * @file StudentProfile.h
 * @brief Serializable student academic profile (year, specialization, completed courses).
 */

#ifndef STUDENTPROFILE_H
#define STUDENTPROFILE_H

#include <string>
#include <vector>
#include "AcademicData.h"

/**
 * @class StudentProfile
 * @brief Academic history and goals: year, specialization id, and completed course codes.
 *
 * Text file format: @c year=..., @c specialization=..., @c course=CODE per line. Used for persistence
 * and prerequisite checks elsewhere in the app.
 */
class StudentProfile {
public:
    StudentProfile() = default;

    std::string currentYear;              /*!< e.g. "Year 1" */
    std::string specializationId;         /*!< e.g. "CS" */
    std::vector<std::string> completedCourseIds; /*!< Catalog course codes */

    /**
     * @brief Required courses for current specialization not yet in completedCourseIds.
     * @return List of missing course codes (titles resolved by UI via catalog).
     */
    std::vector<std::string> GetMissingGraduationRequirements() const;

    /**
     * @brief Parses profile file; replaces all fields on success.
     * @param path File path to read.
     * @return True if opened and read; false if file missing (fields may be unchanged).
     */
    bool LoadFromFile(const std::string& path);

    /**
     * @brief Writes year, specialization, and completed courses to disk.
     * @param path Output file path.
     * @return True on successful write.
     */
    bool SaveToFile(const std::string& path) const;
};

#endif
