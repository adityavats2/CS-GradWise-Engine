#include "Prerequisite.h"
#include "Course.h"
#include <sstream>

bool Prerequisite::isSatisfied(const std::vector<std::string>& completedCodes) const {
    auto isCompleted = [&](const std::string& code) {
        for (const std::string& c : completedCodes)
            if (c == code) return true;
        return false;
    };
    switch (type) {
        case PrerequisiteType::SIMPLE:
            return !courses.empty() && isCompleted(courses[0]->getCode());
        case PrerequisiteType::ONE_OF:
            for (const Course* c : courses)
                if (isCompleted(c->getCode())) return true;
            return false;
        case PrerequisiteType::CREDITS_FROM: {
            double total = 0.0;
            for (const Course* c : courses)
                if (isCompleted(c->getCode())) total += c->getCredits();
            return total >= requiredCredits;
        }
    }
    return false;
}

std::string Prerequisite::toString() const {
    std::ostringstream out;
    switch (type) {
        case PrerequisiteType::SIMPLE:
            if (!courses.empty()) out << courses[0]->getCode();
            break;
        case PrerequisiteType::ONE_OF:
            out << "One of: ";
            for (std::size_t i = 0; i < courses.size(); i++) {
                if (i > 0) out << ", ";
                out << courses[i]->getCode();
            }
            break;
        case PrerequisiteType::CREDITS_FROM:
            out << requiredCredits << " credits from: ";
            for (std::size_t i = 0; i < courses.size(); i++) {
                if (i > 0) out << ", ";
                out << courses[i]->getCode();
            }
            break;
    }
    return out.str();
}
