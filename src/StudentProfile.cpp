/**
 * @file StudentProfile.cpp
 * @brief Implementation of StudentProfile.
 */

#include "StudentProfile.h"
#include "CourseCatalog.h"
#include "Course.h"
#include <fstream>
#include <algorithm>
#include <sstream>

namespace {
    std::string normalizeBreadthCategory(const std::string& text) {
        std::string s = text;
        for (char& ch : s) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        return s;
    }
}

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

std::vector<std::string> StudentProfile::GetCompletedBreadthCourseIdsForCategory(
    const CourseCatalog& catalog,
    const std::string& category
) const {
    std::vector<std::string> result;
    for (const std::string& id : completedCourseIds) {
        Course* course = catalog.getCourse(id);
        if (course != nullptr &&
            course->countsForBreadth() &&
            normalizeBreadthCategory(course->getBreadthCategory()) == normalizeBreadthCategory(category)) {
            result.push_back(id);
        }
    }
    return result;
}

std::vector<std::string> StudentProfile::GetMissingBreadthCategories(const CourseCatalog& catalog) const {
    std::vector<std::string> missing;

    if (!HasCompletedBreadthCredits(catalog, "Science", 1.0)) {
        missing.push_back("Science");
    }
    if (!HasCompletedBreadthCredits(catalog, "Humanities", 1.0)) {
        missing.push_back("Humanities");
    }
    if (!HasCompletedBreadthCredits(catalog, "Social Science", 1.0)) {
        missing.push_back("Social Science");
    }

    return missing;
}

double StudentProfile::GetCompletedBreadthCreditsForCategory(
    const CourseCatalog& catalog,
    const std::string& category
) const {
    double total = 0.0;

    for (const std::string& id : completedCourseIds) {
        Course* course = catalog.getCourse(id);
        if (course != nullptr &&
            course->countsForBreadth() &&
            normalizeBreadthCategory(course->getBreadthCategory()) == normalizeBreadthCategory(category)) {
            total += course->getCredits();
        }
    }

    return total;
}

bool StudentProfile::HasCompletedBreadthCredits(
    const CourseCatalog& catalog,
    const std::string& category,
    double requiredCredits
) const {
    return GetCompletedBreadthCreditsForCategory(catalog, category) >= requiredCredits;
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
