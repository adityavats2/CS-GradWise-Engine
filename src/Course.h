#ifndef COURSE_H
#define COURSE_H
#include <string>
#include <vector>
#include "CourseOffering.h"

/** @brief Represents one course in the catalog */
class Course {
private:
    std::string code;
    std::string title;
    double credits;
    std::vector<Course*> prerequisites;
    std::vector<Course*> exclusions;
    std::vector<CourseOffering> offerings;

public:
    Course();
    /** @brief Creates a course
     *  @param code Course code
     *  @param title Course title
     *  @param credits Course credits
     */
    Course(const std::string& code, const std::string& title, double credits);
    const std::string& getCode() const;
    const std::string& getTitle() const;
    double getCredits() const;
    const std::vector<Course*>& getPrerequisites() const;
    const std::vector<Course*>& getExclusions() const;
    const std::vector<CourseOffering>& getOfferings() const;
    /** @brief Adds prerequisite
     *  @param prerequisite Prerequisite course
     */
    void addPrerequisite(Course* prerequisite);
    /** @brief Adds exclusion
     *  @param exclusion Exclusion course
     */
    void addExclusion(Course* exclusion);
    /** @brief Adds offering
     *  @param offering Course offering
     */
    void addOffering(const CourseOffering& offering);
    /** @brief Checks whether the course is offered in a term
     *  @param term Term to check
     *  @return True if the course is offered in the term
     */
    bool isOfferedIn(const Term& term) const;
};

#endif