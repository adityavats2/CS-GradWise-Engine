#ifndef COURSE_H
#define COURSE_H
#include <string>
#include <vector>
#include "CourseOffering.h"
#include "Prerequisite.h"

/** @brief Represents one course in the catalog */
class Course {
private:
    std::string code;
    std::string title;
    double credits;
    std::vector<Prerequisite> prerequisites;
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
    /** @brief Returns course code
     *  @return Course code
     */
    const std::string& getCode() const;
    /** @brief Returns course title
     * @return Course title
     */
    const std::string& getTitle() const;
    /** @brief Returns course credits
     *  @return Number of course credits
     */
    double getCredits() const;
    /** @brief Returns list of courses from all prerequisite rules
     *  @return All prerequisite courses
     */
    std::vector<Course*> getPrerequisites() const;
    /** @brief Returns the prerequisite rules with types
     *  @return All prerequisite rules
     */
    const std::vector<Prerequisite>& getPrerequisiteRules() const;
    /** @brief Returns list of courses that are exclusions
     *  @return All exclusion courses
     */
    const std::vector<Course*>& getExclusions() const;
    /** @brief Returns list of courses offerings
     *  @return List of course offerings
     */
    const std::vector<CourseOffering>& getOfferings() const;
    /** @brief Adds a prerequisite rule
     *  @param prerequisite The prerequisite to add
     */
    void addPrerequisite(const Prerequisite& prerequisite);
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