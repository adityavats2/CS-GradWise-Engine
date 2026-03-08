#include "Course.h"

Course::Course() : code(""), title(""), credits(0.0), prerequisites(), exclusions(), offerings() {}

Course::Course(const std::string& code, const std::string& title, double credits) : code(code), title(title), credits(credits), prerequisites(), exclusions(), offerings() {}

const std::string& Course::getCode() const {
    return code;
}

const std::string& Course::getTitle() const {
    return title;
}

double Course::getCredits() const {
    return credits;
}

const std::vector<Course*>& Course::getPrerequisites() const {
    return prerequisites;
}

const std::vector<Course*>& Course::getExclusions() const {
    return exclusions;
}

const std::vector<CourseOffering>& Course::getOfferings() const {
    return offerings;
}

void Course::addPrerequisite(Course* prerequisite) {
    prerequisites.push_back(prerequisite);
}

void Course::addExclusion(Course* exclusion) {
    exclusions.push_back(exclusion);
}

void Course::addOffering(const CourseOffering& offering) {
    offerings.push_back(offering);
}

bool Course::isOfferedIn(const Term& term) const {
    for (const auto& offering : offerings) {
        if(offering.getTerm().equal(term)) {
            return true;
        }
    }
    return false;
}