/**
 * @file StudentProfile.cpp
 * @brief Implementation of StudentProfile.
 */

#include "StudentProfile.h"
#include <fstream>
#include <algorithm>
#include <sstream>

std::vector<std::string> StudentProfile::GetMissingGraduationRequirements() const {
    std::vector<std::string> missing;
    const Academic::Specialization* spec = Academic::FindSpecializationById(specializationId);
    if (!spec) return missing;
    for (const std::string& reqId : spec->requiredCourseIds) {
        bool found = std::find(completedCourseIds.begin(), completedCourseIds.end(), reqId) != completedCourseIds.end();
        if (!found)
            missing.push_back(reqId);
    }
    return missing;
}

bool StudentProfile::LoadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    currentYear.clear();
    specializationId.clear();
    completedCourseIds.clear();
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (line.compare(0, 5, "year=") == 0)
            currentYear = line.substr(5);
        else if (line.compare(0, 15, "specialization=") == 0)
            specializationId = line.substr(15);
        else if (line.compare(0, 7, "course=") == 0)
            completedCourseIds.push_back(line.substr(7));
    }
    return true;
}

bool StudentProfile::SaveToFile(const std::string& path) const {
    std::ofstream f(path);
    if (!f) return false;
    if (!currentYear.empty())
        f << "year=" << currentYear << "\n";
    if (!specializationId.empty())
        f << "specialization=" << specializationId << "\n";
    for (const std::string& id : completedCourseIds)
        f << "course=" << id << "\n";
    return true;
}
