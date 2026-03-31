#ifndef COURSECATALOG_H
#define COURSECATALOG_H
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Course.h"

/** @brief Stores and looks up courses */
class CourseCatalog {
private:
    std::vector<std::unique_ptr<Course>> courses;
    std::unordered_map<std::string, Course*> courseMap;

public:
    CourseCatalog();
    /** @brief Adds a course to the catalog
     *  @param course The course to add
     *  @return True if the course was added
     */
    bool addCourse(std::unique_ptr<Course> course);
    /** @brief Gets a course by code
     *  @param code Course code
     *  @return Matching course or nullptr
     */
    Course* getCourse(const std::string& code) const;
    /** @brief Checks whether a course exists
     *  @param code Course code
     *  @return True if the course exists
     */
    bool containsCourse(const std::string& code) const;
    /** @brief Gets all courses
     *  @return All courses in the catalog
     */
    const std::vector<std::unique_ptr<Course>>& getAllCourses() const;
    /** @brief Removes all courses from the catalog */
    void clear();
};

#endif