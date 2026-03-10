#ifndef STUDENTPROFILE_H
#define STUDENTPROFILE_H

#include <string>
#include <vector>
#include "AcademicData.h"

/** @brief Student profile: academic history and goals; persists to file and remains editable */
class StudentProfile {
public:
    StudentProfile() = default;

    /** Current year (e.g. "Year 1") */
    std::string currentYear;
    /** Specialization id (e.g. "CS") */
    std::string specializationId;
    /** IDs of completed courses */
    std::vector<std::string> completedCourseIds;

    /** @brief Returns course IDs that are required for graduation in the selected specialization but not yet completed */
    std::vector<std::string> GetMissingGraduationRequirements() const;

    /** @brief Load profile from file; returns true on success */
    bool LoadFromFile(const std::string& path);
    /** @brief Save profile to file; returns true on success */
    bool SaveToFile(const std::string& path) const;
};

#endif
