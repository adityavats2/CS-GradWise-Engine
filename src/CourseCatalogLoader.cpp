#include "CourseCatalogLoader.h"
#include <cctype>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

namespace {

struct PendingOffering {
    Term term;
    std::vector<TimeSlot> timeSlots;
};

struct PendingCourse {
    std::string code;
    std::string title;
    double credits = 0.0;
    bool hasCredits = false;
    std::vector<std::string> prerequisiteCodes;
    std::vector<std::string> exclusionCodes;
    std::vector<PendingOffering> offerings;
};

std::string trim(const std::string& text) {
    std::size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        start++;
    }
    std::size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        end--;
    }
    return text.substr(start, end - start);
}

bool startsWith(const std::string& text, const std::string& prefix) {
    return text.rfind(prefix, 0) == 0;
}

std::vector<std::string> splitCommaSeparatedList(const std::string& text) {
    std::vector<std::string> items;
    std::stringstream stream(text);
    std::string item;
    while (std::getline(stream, item, ',')) {
        std::string trimmedItem = trim(item);
        if (!trimmedItem.empty()) {
            items.push_back(trimmedItem);
        }
    }
    return items;
}

bool parseSeason(const std::string& seasonText, Season& season) {
    if (seasonText == "Fall") {
        season = Season::Fall;
        return true;
    }
    if (seasonText == "Winter") {
        season = Season::Winter;
        return true;
    }
    if (seasonText == "Summer") {
        season = Season::Summer;
        return true;
    }
    return false;
}
} 

bool CourseCatalogLoader::loadFromFile(const std::string& filePath, CourseCatalog& catalog) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    std::vector<PendingCourse> pendingCourses;
    PendingCourse currentCourse;
    bool inCourseBlock = false;
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line == "COURSE") {
            if (inCourseBlock) {
                return false;
            }
            currentCourse = PendingCourse();
            inCourseBlock = true;
            continue;
        }
        if (!inCourseBlock) {
            return false;
        }
        if (line == "END") {
            if (currentCourse.code.empty() || currentCourse.title.empty() || !currentCourse.hasCredits) {
                return false;
            }
            pendingCourses.push_back(currentCourse);
            inCourseBlock = false;
            continue;
        }
        if (startsWith(line, "CODE:")) {
            currentCourse.code = trim(line.substr(5));
            continue;
        }
        if (startsWith(line, "TITLE:")) {
            currentCourse.title = trim(line.substr(6));
            continue;
        }
        if (startsWith(line, "CREDITS:")) {
            std::string creditsText = trim(line.substr(8));
            try {
                currentCourse.credits = std::stod(creditsText);
            } catch (const std::exception& e) {
                return false;
            }
            currentCourse.hasCredits = true;
            continue;
        }
        if (startsWith(line, "PREREQUISITES:")) {
            currentCourse.prerequisiteCodes = splitCommaSeparatedList(line.substr(14));
            continue;
        }
        if (startsWith(line, "EXCLUSIONS:")) {
            currentCourse.exclusionCodes = splitCommaSeparatedList(line.substr(11));
            continue;
        }
        if (startsWith(line, "OFFERING:")) {
            std::vector<std::string> offeringParts = splitCommaSeparatedList(line.substr(9));
            if (offeringParts.size() != 2) {
                return false;
            }
            Season season;
            if (!parseSeason(offeringParts[0], season)) {
                return false;
            }
            int year;
            try {
                year = std::stoi(offeringParts[1]);
            } catch (const std::exception& e){
                return false;
            }
            PendingOffering offering;
            offering.term = Term(season, year);
            currentCourse.offerings.push_back(offering);
            continue;
        }
        if (startsWith(line, "TIMESLOT:")) {
            if (currentCourse.offerings.empty()) {
                return false;
            }
            std::vector<std::string> timeSlotParts = splitCommaSeparatedList(line.substr(9));
            if (timeSlotParts.size() != 3) {
                return false;
            }
            int startTime;
            int endTime;
            try {
                startTime = std::stoi(timeSlotParts[1]);
                endTime = std::stoi(timeSlotParts[2]);
            } catch (const std::exception& e) {
                return false;
            }
            if (startTime >= endTime) {
                return false;
            }
            currentCourse.offerings.back().timeSlots.push_back(
                TimeSlot(timeSlotParts[0], startTime, endTime)
            );
            continue;
        }
        return false;
    }
    if (inCourseBlock) {
        return false;
    }
    for (const PendingCourse& pendingCourse : pendingCourses) {
        std::unique_ptr<Course> course = std::make_unique<Course>(pendingCourse.code, pendingCourse.title, pendingCourse.credits);
        if (!catalog.addCourse(std::move(course))) {
            return false;
        }
    }
    for (const PendingCourse& pendingCourse : pendingCourses) {
        Course* course = catalog.getCourse(pendingCourse.code);
        if (course == nullptr) {
            return false;
        }
        for (const std::string& prerequisiteCode : pendingCourse.prerequisiteCodes) {
            Course* prerequisite = catalog.getCourse(prerequisiteCode);
            if (prerequisite == nullptr) {
                return false;
            }
            course->addPrerequisite(prerequisite);
        }
        for (const std::string& exclusionCode : pendingCourse.exclusionCodes) {
            Course* exclusion = catalog.getCourse(exclusionCode);
            if (exclusion == nullptr) {
                return false;
            }
            course->addExclusion(exclusion);
        }
        for (const PendingOffering& pendingOffering : pendingCourse.offerings) {
            CourseOffering offering(pendingOffering.term, {});
            for (const TimeSlot& timeSlot : pendingOffering.timeSlots) {
                offering.addTimeSlot(timeSlot);
            }
            course->addOffering(offering);
        }
    }
    return true;
}