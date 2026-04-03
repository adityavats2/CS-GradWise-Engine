#include "CourseCatalogSaver.h"

namespace {
std::string seasonToString(Season season) {
    switch (season) {
        case Season::Fall:
            return "Fall";
        case Season::Winter:
            return "Winter";
        case Season::Summer:
            return "Summer";
    }
    return "Fall";
}
}

bool CourseCatalogSaver::saveToFile(const std::string& filePath, const CourseCatalog& catalog) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();

    for (const auto& ptr : courses) {
        const Course* course = ptr.get();
        if (course == nullptr) {
            continue;
        }

        file << "COURSE\n";
        file << "CODE: " << course->getCode() << "\n";
        file << "TITLE: " << course->getTitle() << "\n";
        file << "CREDITS: " << course->getCredits() << "\n";
        file << "ELECTIVE: " << (course->isElective() ? "true" : "false") << "\n";

        const std::vector<Prerequisite>& prereqs = course->getPrerequisiteRules();

        bool wroteSimplePrereq = false;
        for (const Prerequisite& prereq : prereqs) {
            if (prereq.type == PrerequisiteType::SIMPLE) {
                file << "PREREQUISITES: ";
                for (std::size_t i = 0; i < prereq.courses.size(); i++) {
                    if (i > 0) file << ",";
                    file << prereq.courses[i]->getCode();
                }
                file << "\n";
                wroteSimplePrereq = true;
                break;
            }
        }
        if (!wroteSimplePrereq) {
            file << "PREREQUISITES:\n";
        }

        bool wroteOneOf = false;
        for (const Prerequisite& prereq : prereqs) {
            if (prereq.type == PrerequisiteType::ONE_OF) {
                file << "PREREQ_ONE_OF: ";
                for (std::size_t i = 0; i < prereq.courses.size(); i++) {
                    if (i > 0) file << ",";
                    file << prereq.courses[i]->getCode();
                }
                file << "\n";
                wroteOneOf = true;
            }
        }

        bool wroteCreditsFrom = false;
        for (const Prerequisite& prereq : prereqs) {
            if (prereq.type == PrerequisiteType::CREDITS_FROM) {
                file << "PREREQ_CREDITS_FROM: " << prereq.requiredCredits;
                for (const Course* c : prereq.courses) {
                    file << "," << c->getCode();
                }
                file << "\n";
                wroteCreditsFrom = true;
            }
        }

        const std::vector<Course*>& exclusions = course->getExclusions();
        if (exclusions.empty()) {
            file << "EXCLUSIONS:\n";
        } else {
            file << "EXCLUSIONS: ";
            for (std::size_t i = 0; i < exclusions.size(); i++) {
                if (i > 0) file << ",";
                file << exclusions[i]->getCode();
            }
            file << "\n";
        }

        if (course->countsForBreadth()) {
            file << "BREADTH: " << course->getBreadthCategory() << "\n";
        }

        const std::vector<CourseOffering>& offerings = course->getOfferings();
        for (const CourseOffering& offering : offerings) {
            file << "OFFERING: "
                 << seasonToString(offering.getTerm().getSeason())
                 << "," << offering.getTerm().getYear() << "\n";

            for (const TimeSlot& slot : offering.getTimeSlots()) {
                file << "TIMESLOT: "
                     << slot.getDay()
                     << "," << slot.getStartTime()
                     << "," << slot.getEndTime() << "\n";
            }
        }

        file << "END\n\n";
    }

    return true;
}