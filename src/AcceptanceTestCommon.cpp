#include "AcceptanceTestCommon.h"
#include <iostream>
#include <fstream>

void PrintAcceptanceResult(const std::string& testName, bool passed, int& passedCount, int& failedCount) {
    if (passed) {
        std::cout << "[PASS] " << testName << "\n";
        passedCount++;
    } else {
        std::cout << "[FAIL] " << testName << "\n";
        failedCount++;
    }
}

bool AcceptanceContains(const std::vector<std::string>& values, const std::string& target) {
    for (std::size_t i = 0; i < values.size(); i++) {
        if (values[i] == target) {
            return true;
        }
    }
    return false;
}

void AcceptanceAddIfMissing(std::vector<std::string>& values, const std::string& value) {
    if (!AcceptanceContains(values, value)) {
        values.push_back(value);
    }
}

std::string AcceptanceJoinCourseIds(const std::vector<std::string>& courseIds) {
    std::string text;
    for (std::size_t i = 0; i < courseIds.size(); i++) {
        if (i > 0) {
            text += ", ";
        }
        text += courseIds[i];
    }
    return text;
}

std::string AcceptanceBuildTimelineText(
    const std::vector<PlannedTerm>& lockedTerms,
    const GeneratedMultiTermPathwayResult& pathway
) {
    std::string text;

    for (std::size_t i = 0; i < lockedTerms.size(); i++) {
        text += lockedTerms[i].term.toString();
        text += ": ";
        text += AcceptanceJoinCourseIds(lockedTerms[i].courseIds);
        text += "\n";
    }

    for (std::size_t i = 0; i < pathway.termPlans.size(); i++) {
        text += pathway.termPlans[i].term.toString();
        text += ": ";
        text += AcceptanceJoinCourseIds(pathway.termPlans[i].selectedCourseIds);
        text += "\n";
    }

    if (!pathway.remainingRequiredCourseIds.empty()) {
        text += "Remaining required courses: ";
        text += AcceptanceJoinCourseIds(pathway.remainingRequiredCourseIds);
        text += "\n";
    }

    return text;
}

std::string AcceptanceGetFirstLine(const std::string& text) {
    std::size_t pos = text.find('\n');
    if (pos == std::string::npos) {
        return text;
    }
    return text.substr(0, pos);
}

int AcceptanceGetCourseTermIndex(const GeneratedMultiTermPathwayResult& pathway, const std::string& courseId) {
    for (std::size_t i = 0; i < pathway.termPlans.size(); i++) {
        if (AcceptanceContains(pathway.termPlans[i].selectedCourseIds, courseId)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool AcceptancePathwayHasCourse(const GeneratedMultiTermPathwayResult& pathway, const std::string& courseId) {
    for (std::size_t i = 0; i < pathway.termPlans.size(); i++) {
        if (AcceptanceContains(pathway.termPlans[i].selectedCourseIds, courseId)) {
            return true;
        }
    }
    return false;
}

bool AcceptanceCreateCatalogFile(const std::string& filePath) {
    std::ofstream file(filePath.c_str());
    if (!file.is_open()) {
        return false;
    }

    file << "COURSE\n";
    file << "CODE: CS1020\n";
    file << "TITLE: Intro to CS\n";
    file << "CREDITS: 0.5\n";
    file << "OFFERING: Fall, 2026\n";
    file << "TIMESLOT: Monday, 540, 630\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS1025\n";
    file << "TITLE: Intro to Programming Tools\n";
    file << "CREDITS: 0.5\n";
    file << "OFFERING: Fall, 2026\n";
    file << "TIMESLOT: Tuesday, 540, 630\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS1026\n";
    file << "TITLE: Foundations of Programming\n";
    file << "CREDITS: 0.5\n";
    file << "OFFERING: Fall, 2026\n";
    file << "TIMESLOT: Wednesday, 540, 630\n";
    file << "OFFERING: Winter, 2027\n";
    file << "TIMESLOT: Wednesday, 540, 630\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS1027\n";
    file << "TITLE: Fundamentals II\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS1026\n";
    file << "OFFERING: Winter, 2027\n";
    file << "TIMESLOT: Thursday, 540, 630\n";
    file << "OFFERING: Fall, 2027\n";
    file << "TIMESLOT: Thursday, 540, 630\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS2210\n";
    file << "TITLE: Algorithms\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS1027\n";
    file << "OFFERING: Fall, 2027\n";
    file << "TIMESLOT: Monday, 600, 690\n";
    file << "OFFERING: Winter, 2028\n";
    file << "TIMESLOT: Monday, 600, 690\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS2211\n";
    file << "TITLE: Systems Programming\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS2210\n";
    file << "OFFERING: Winter, 2028\n";
    file << "TIMESLOT: Tuesday, 600, 690\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS2208\n";
    file << "TITLE: Logic\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS1026\n";
    file << "OFFERING: Fall, 2027\n";
    file << "TIMESLOT: Wednesday, 600, 690\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS2209\n";
    file << "TITLE: Software Engineering Intro\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS1026\n";
    file << "OFFERING: Fall, 2027\n";
    file << "TIMESLOT: Thursday, 600, 690\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS3305\n";
    file << "TITLE: Operating Systems\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS2210\n";
    file << "OFFERING: Fall, 2028\n";
    file << "TIMESLOT: Monday, 660, 750\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS3307\n";
    file << "TITLE: Networks\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS2210\n";
    file << "OFFERING: Fall, 2028\n";
    file << "TIMESLOT: Tuesday, 660, 750\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: CS2214\n";
    file << "TITLE: Discrete Structures\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS2210\n";
    file << "OFFERING: Fall, 2028\n";
    file << "TIMESLOT: Wednesday, 660, 750\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: MATH2155\n";
    file << "TITLE: Math for CS\n";
    file << "CREDITS: 0.5\n";
    file << "OFFERING: Fall, 2026\n";
    file << "TIMESLOT: Friday, 540, 630\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: MATH2156\n";
    file << "TITLE: Advanced Math\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: MATH2155\n";
    file << "OFFERING: Winter, 2027\n";
    file << "TIMESLOT: Monday, 720, 810\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: DATA3000\n";
    file << "TITLE: Data Science Methods\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: CS1026\n";
    file << "OFFERING: Winter, 2027\n";
    file << "TIMESLOT: Tuesday, 720, 810\n";
    file << "END\n\n";

    file << "COURSE\n";
    file << "CODE: MATH3159\n";
    file << "TITLE: Statistics for Data Science\n";
    file << "CREDITS: 0.5\n";
    file << "PREREQUISITES: MATH2156\n";
    file << "OFFERING: Fall, 2028\n";
    file << "TIMESLOT: Thursday, 720, 810\n";
    file << "END\n";

    file.close();
    return true;
}

bool AcceptanceLoadCatalog(CourseCatalog& catalog, const std::string& filePath) {
    catalog.clear();
    return CourseCatalogLoader::loadFromFile(filePath, catalog);
}
