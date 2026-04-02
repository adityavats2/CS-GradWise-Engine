#include "PlannedScheduleStorage.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace {
/**
 * @brief Trims leading/trailing ASCII whitespace.
 *
 * @param text The input text.
 * @return The trimmed text.
 */
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

/**
 * @brief Checks whether or not text starts with a prefix.
 *
 * @param text The full text.
 * @param prefix The required prefix.
 * @return True if @p text starts with @p prefix.
 */
bool startsWith(const std::string& text, const std::string& prefix) {
    return text.rfind(prefix, 0) == 0;
}

/**
 * @brief Parses a season label into the season enum.
 *
 * @param seasonText The season token from file.
 * @param season The output season enum.
 * @return True if parsing succeeds.
 */
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

/**
 * @brief Converts the season enum to storage text.
 *
 * @param season The season enum value.
 * @return The serialized season string.
 */
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

/**
 * @brief Loads the planned schedule entries from file.
 *
 * @param path The file path to read.
 * @param schedule Destination schedule to populate.
 * @return True on successful load/parse.
 */
bool PlannedScheduleStorage::loadFromFile(const std::string& path, PlannedSchedule& schedule) {
    std::ifstream file(path);
    schedule.clear();

    if (!file.is_open()) {
        return true;
    }

    bool inTerm = false;
    Season currentSeason = Season::Fall;
    int currentYear = 0;
    std::vector<std::string> currentCourses;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        if (startsWith(line, "TERM=")) {
            if (inTerm) {
                return false;
            }
            std::string value = trim(line.substr(5));
            std::size_t commaPos = value.find(',');
            if (commaPos == std::string::npos) {
                return false;
            }

            std::string seasonText = trim(value.substr(0, commaPos));
            std::string yearText = trim(value.substr(commaPos + 1));
            if (!parseSeason(seasonText, currentSeason)) {
                return false;
            }
            try {
                currentYear = std::stoi(yearText);
            } catch (const std::exception&) {
                return false;
            }

            currentCourses.clear();
            inTerm = true;
            continue;
        }

        if (startsWith(line, "COURSE=")) {
            if (!inTerm) {
                return false;
            }
            std::string code = trim(line.substr(7));
            if (code.empty()) {
                return false;
            }
            if (std::find(currentCourses.begin(), currentCourses.end(), code) == currentCourses.end()) {
                currentCourses.push_back(code);
            }
            continue;
        }

        if (line == "ENDTERM") {
            if (!inTerm) {
                return false;
            }
            schedule.setCoursesForTerm(Term(currentSeason, currentYear), currentCourses);
            inTerm = false;
            continue;
        }

        return false;
    }

    return !inTerm;
}

/**
 * @brief Saves the planned schedule entries to file.
 *
 * @param path The file path to write.
 * @param schedule The source schedule data.
 * @return True if the writing succeeds.
 */
bool PlannedScheduleStorage::saveToFile(const std::string& path, const PlannedSchedule& schedule) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    const std::vector<PlannedTerm>& terms = schedule.getAllPlannedTerms();
    for (std::size_t i = 0; i < terms.size(); i++) {
        const PlannedTerm& plannedTerm = terms[i];
        file << "TERM=" << seasonToString(plannedTerm.term.getSeason()) << "," << plannedTerm.term.getYear() << "\n";
        for (const std::string& courseId : plannedTerm.courseIds) {
            file << "COURSE=" << courseId << "\n";
        }
        file << "ENDTERM\n";
        if (i + 1 < terms.size()) {
            file << "\n";
        }
    }

    return true;
}
