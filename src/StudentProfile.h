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
class CourseCatalog;

/** @brief Student profile: academic history and goals; persists to file and remains editable */
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
    
    /**
     * @brief Get the course IDs that have completed a breadth category
     * @param catalog the course catalog to reference to check the details of breadth catagory of a certain course
     * @param category the breadth category to check
     * @return a vector of completed course IDS of courses that have completed the breadth requirement
     */
    std::vector<std::string> GetCompletedBreadthCourseIdsForCategory(const CourseCatalog& catalog, const std::string& category) const;
    
    /**
     * @brief returns the breadth requirements that have yet to be completed
     * @param catalog the course catalog to reference to check the details of breadth catagory of a certain course
     * @return a vector of missing breadth requirments
     */
    std::vector<std::string> GetMissingBreadthCategories(const CourseCatalog& catalog) const;
    
    /**
     * @brief calculates exactly how many credits have been put forward towards a certain breadth requirement
     * @param catalog the course catalog to reference to check the details of breadth catagory of a certain course
     * @param category the breadth category to check
     * @return the total amount of credits that have gone towards a certain breadth category
     */
    double GetCompletedBreadthCreditsForCategory(const CourseCatalog& catalog, const std::string& category) const;
    
      /**
     * @brief checks whether or not a certain breadth category has been completed
     * @param catalog the course catalog to reference to check the details of breadth catagory of a certain course
     * @param category the breadth category to check
     * @return true of the breadth category has been completed, false otherwise
     */
    bool HasCompletedBreadthCredits(const CourseCatalog& catalog, const std::string& category, double requiredCredits) const;
    
    /** @brief Load profile from file; returns true on success */
    bool LoadFromFile(const std::string& path);

    /**
     * @brief Writes year, specialization, and completed courses to disk.
     * @param path Output file path.
     * @return True on successful write.
     */
    bool SaveToFile(const std::string& path) const;
};

#endif
