/**
 * @file CourseCatalog.h
 * @brief In-memory registry of courses with lookup by course code.
 */

#ifndef COURSECATALOG_H
#define COURSECATALOG_H
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Course.h"

/**
 * @class CourseCatalog
 * @brief Stores courses in insertion order and provides O(1) lookup by code via an internal map.
 *
 * Populated by CourseCatalogLoader; shared across UI panels (catalog browser, student profile, schedule).
 */
class CourseCatalog {
private:
    std::vector<std::unique_ptr<Course>> courses;
    std::unordered_map<std::string, Course*> courseMap;

public:
    CourseCatalog();

    /**
     * @brief Inserts a course if its code is not already present.
     * @param course Ownership transferred to the catalog on success.
     * @return True if added; false if null or duplicate code.
     */
    bool addCourse(std::unique_ptr<Course> course);

    /**
     * @brief Looks up a course by code (e.g. "CS1027").
     * @param code Course code string.
     * @return Pointer to the course, or nullptr if not found.
     */
    Course* getCourse(const std::string& code) const;

    /**
     * @brief Tests whether a course code exists in the catalog.
     * @param code Course code string.
     * @return True if present.
     */
    bool containsCourse(const std::string& code) const;

    /**
     * @brief Returns all courses in load order.
     * @return Const reference to the internal vector of unique_ptr<Course>.
     */
    const std::vector<std::unique_ptr<Course>>& getAllCourses() const;

    /** @brief Removes every course and clears the code map. */
    void clear();
};

#endif
