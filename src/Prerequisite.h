#ifndef PREREQUISITE_H
#define PREREQUISITE_H
#include <string>
#include <vector>

class Course;

/** @brief Type of prerequisite rule */
enum class PrerequisiteType {
    SIMPLE,
    ONE_OF,
    CREDITS_FROM
};

/**
 * @brief Represents prerequisite rule attached to a course
 */
struct Prerequisite {
    PrerequisiteType type;
    std::vector<Course*> courses;
    double requiredCredits = 0.0;
    /**
     * @brief Checks whether this prerequisite is satisfied
     * @param completedCodes Course codes student has completed
     * @return True if rule is satisfied
     */
    bool isSatisfied(const std::vector<std::string>& completedCodes) const;
    /**
     * @brief Returns string description of this prerequisite
     * @return String description of prerequisite
     */
    std::string toString() const;
};

#endif
