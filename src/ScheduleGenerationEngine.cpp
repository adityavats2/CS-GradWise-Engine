#include "ScheduleGenerationEngine.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <unordered_set>

namespace {
/**
 * @brief Checks whether or not two courses exclude each other.
 *
 * @param a The First course.
 * @param b The Second course.
 * @return True if either course excludes the other.
 */
bool isExcludedPair(const Course* a, const Course* b) {
    for (const Course* excluded : a->getExclusions()) {
        if (excluded == b) {
            return true;
        }
    }
    for (const Course* excluded : b->getExclusions()) {
        if (excluded == a) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Collects the offerings of a course in the target term.
 *
 * @param course Courses to inspect.
 * @param term The target term.
 * @return Offering pointers that match the term.
 */
std::vector<const CourseOffering*> getOfferingsInTerm(const Course* course, const Term& term) {
    std::vector<const CourseOffering*> offerings;
    for (const CourseOffering& offering : course->getOfferings()) {
        if (offering.getTerm().equal(term)) {
            offerings.push_back(&offering);
        }
    }
    return offerings;
}

/**
 * @brief Checks whether or not two offerings have any slot conflict.
 *
 * @param a The ffirst offering.
 * @param b The Second offering.
 * @return True if any pair of slots overlaps.
 */
bool offeringPairHasConflict(const CourseOffering* a, const CourseOffering* b) {
    for (const TimeSlot& slotA : a->getTimeSlots()) {
        for (const TimeSlot& slotB : b->getTimeSlots()) {
            if (slotA.conflictsWith(slotB)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Determines if two courses are unavoidably conflicting in a term.
 *
 * @param a First course.
 * @param b Second course.
 * @param term Target term.
 * @return True when all offering combinations conflict.
 */
bool coursesConflictInTerm(const Course* a, const Course* b, const Term& term) {
    const std::vector<const CourseOffering*> offeringsA = getOfferingsInTerm(a, term);
    const std::vector<const CourseOffering*> offeringsB = getOfferingsInTerm(b, term);

    if (offeringsA.empty() || offeringsB.empty()) {
        return false;
    }

    // Report conflict only if every offering combination conflicts.
    for (const CourseOffering* offA : offeringsA) {
        for (const CourseOffering* offB : offeringsB) {
            if (!offeringPairHasConflict(offA, offB)) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Evaluates all prerequisite rules for one course.
 *
 * @param course Course to being evaluated.
 * @param prereqContextCourseIds IDs available as prerequisite context.
 * @return True if every prerequisite rule is satisfied.
 */
bool prerequisitesSatisfied(const Course* course, const std::vector<std::string>& prereqContextCourseIds) {
    for (const Prerequisite& rule : course->getPrerequisiteRules()) {
        if (!rule.isSatisfied(prereqContextCourseIds)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Applies hard preference filters to a generated schedule.
 *
 * @param schedule Generates term schedule candidate.
 * @param catalog Course catalog for offering lookup.
 * @param term Targets the term for schedule offerings.
 * @param preferences User preference configuration.
 * @return True if the schedule satisfies hard constraints.
 */
bool satisfiesHardPreferences(
    const GeneratedTermScheduleResult& schedule,
    const CourseCatalog& catalog,
    const Term& term,
    const SchedulePreferences& preferences
) {
    if (!preferences.enforceEarliestStart) {
        return true;
    }

    for (const std::string& courseId : schedule.selectedCourseIds) {
        Course* course = catalog.getCourse(courseId);
        if (course == nullptr) {
            continue;
        }
        for (const CourseOffering& offering : course->getOfferings()) {
            if (!offering.getTerm().equal(term)) {
                continue;
            }
            for (const TimeSlot& slot : offering.getTimeSlots()) {
                if (slot.getStartTime() < preferences.earliestStartMinutes) {
                    return false;
                }
            }
            break;
        }
    }
    return true;
}

/**
 * @brief Computes a preference score for ranking schedule options.
 *
 * @param schedule Generates a schedule to score.
 * @param catalog Course catalog for offering lookup.
 * @param term Targets term for schedule offerings.
 * @param preferences User preference configuration.
 * @return Higher score indicates a better match to preferences.
 */
double scoreSchedule(
    const GeneratedTermScheduleResult& schedule,
    const CourseCatalog& catalog,
    const Term& term,
    const SchedulePreferences& preferences
) {
    double score = 0.0;

    if (preferences.preferMorning) {
        double morningSum = 0.0;
        std::size_t morningCount = 0;
        for (const std::string& courseId : schedule.selectedCourseIds) {
            Course* course = catalog.getCourse(courseId);
            if (course == nullptr) {
                continue;
            }
            for (const CourseOffering& offering : course->getOfferings()) {
                if (!offering.getTerm().equal(term)) {
                    continue;
                }
                for (const TimeSlot& slot : offering.getTimeSlots()) {
                    // Earlier starts -> larger contribution.
                    morningSum += (24 * 60 - slot.getStartTime());
                    morningCount++;
                }
                break;
            }
        }
        if (morningCount > 0) {
            score += preferences.morningWeight * (morningSum / static_cast<double>(morningCount));
        }
    }

    if (preferences.preferCompactDays) {
        std::map<std::string, std::pair<int, int>> daySpans;
        for (const std::string& courseId : schedule.selectedCourseIds) {
            Course* course = catalog.getCourse(courseId);
            if (course == nullptr) {
                continue;
            }
            for (const CourseOffering& offering : course->getOfferings()) {
                if (!offering.getTerm().equal(term)) {
                    continue;
                }
                for (const TimeSlot& slot : offering.getTimeSlots()) {
                    auto it = daySpans.find(slot.getDay());
                    if (it == daySpans.end()) {
                        daySpans[slot.getDay()] = {slot.getStartTime(), slot.getEndTime()};
                    } else {
                        it->second.first = std::min(it->second.first, slot.getStartTime());
                        it->second.second = std::max(it->second.second, slot.getEndTime());
                    }
                }
                break;
            }
        }
        double totalSpan = 0.0;
        for (const auto& entry : daySpans) {
            totalSpan += static_cast<double>(entry.second.second - entry.second.first);
        }
        // Smaller span -> larger contribution.
        score += preferences.compactDayWeight * (24 * 60 * 7 - totalSpan);
    }

    return score;
}

/**
 * @brief Advances to the next logical academic term.
 *
 * @param term The current term.
 * @return Next chronological term.
 */
Term nextTerm(const Term& term) {
    const int year = term.getYear();
    switch (term.getSeason()) {
        case Season::Winter:
            return Term(Season::Summer, year);
        case Season::Summer:
            return Term(Season::Fall, year);
        case Season::Fall:
            return Term(Season::Winter, year + 1);
    }
    return Term(Season::Winter, year + 1);
}

/**
 * @brief Normalizes a course ID for robust comparisons.
 *
 * @param id The Raw course ID.
 * @return Trimmed uppercase course ID.
 */
std::string normalizeCourseId(const std::string& id) {
    std::size_t start = 0;
    while (start < id.size() && std::isspace(static_cast<unsigned char>(id[start]))) {
        start++;
    }
    std::size_t end = id.size();
    while (end > start && std::isspace(static_cast<unsigned char>(id[end - 1]))) {
        end--;
    }
    std::string normalized = id.substr(start, end - start);
    for (char& ch : normalized) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return normalized;
}

/**
 * @brief Extracts the first integer found in text.
 *
 * @param text The Input text.
 * @return Parsed integer, or -1 if none found.
 */
int extractFirstInteger(const std::string& text) {
    for (std::size_t i = 0; i < text.size(); i++) {
        if (std::isdigit(static_cast<unsigned char>(text[i]))) {
            int value = 0;
            std::size_t j = i;
            while (j < text.size() && std::isdigit(static_cast<unsigned char>(text[j]))) {
                value = value * 10 + (text[j] - '0');
                j++;
            }
            return value;
        }
    }
    return -1;
}

/**
 * @brief Infers course level from the first numeric digit in a course code.
 *
 * @param courseCode Course code text.
 * @return Level 1-4 when available, otherwise 0.
 */
int inferCourseLevel(const std::string& courseCode) {
    for (std::size_t i = 0; i < courseCode.size(); i++) {
        if (std::isdigit(static_cast<unsigned char>(courseCode[i]))) {
            const int level = courseCode[i] - '0';
            if (level >= 1 && level <= 4) {
                return level;
            }
            return 0;
        }
    }
    return 0;
}

/**
 * @brief Gets the highest inferred level from a set of course IDs.
 *
 * @param courseIds Course IDs to inspect.
 * @return Maximum inferred course level, or 0 if none.
 */
int inferHighestCourseLevel(const std::vector<std::string>& courseIds) {
    int highest = 0;
    for (const std::string& id : courseIds) {
        highest = std::max(highest, inferCourseLevel(id));
    }
    return highest;
}
}  // namespace

/**
 * @brief Generates one valid schedule for a term via greedy selection.
 *
 * @param catalog Course catalog used for lookup and constraints.
 * @param term Target term for offerings.
 * @param candidateCourseIds Candidate IDs in selection priority order.
 * @param prereqContextCourseIds IDs available to satisfy prerequisites.
 * @param maxCoursesPerTerm Maximum number of selected courses.
 * @return Generated term schedule result.
 */
GeneratedTermScheduleResult ScheduleGenerationEngine::GenerateOneValidTermSchedule(
    const CourseCatalog& catalog,
    const Term& term,
    const std::vector<std::string>& candidateCourseIds,
    const std::vector<std::string>& prereqContextCourseIds,
    std::size_t maxCoursesPerTerm
) const {
    GeneratedTermScheduleResult result;
    std::vector<const Course*> selectedCourses;

    for (const std::string& candidateId : candidateCourseIds) {
        if (selectedCourses.size() >= maxCoursesPerTerm) {
            break;
        }

        Course* course = catalog.getCourse(candidateId);
        if (course == nullptr) {
            result.errors.push_back("Unknown course ID: " + candidateId);
            continue;
        }

        bool alreadySelected = false;
        for (const Course* selected : selectedCourses) {
            if (selected == course) {
                alreadySelected = true;
                break;
            }
        }
        if (alreadySelected) {
            continue;
        }

        if (!course->isOfferedIn(term)) {
            continue;
        }

        if (!prerequisitesSatisfied(course, prereqContextCourseIds)) {
            continue;
        }

        bool excludedBySelected = false;
        for (const Course* selected : selectedCourses) {
            if (isExcludedPair(course, selected)) {
                excludedBySelected = true;
                break;
            }
        }
        if (excludedBySelected) {
            continue;
        }

        bool conflictsWithSelected = false;
        for (const Course* selected : selectedCourses) {
            if (coursesConflictInTerm(course, selected, term)) {
                conflictsWithSelected = true;
                break;
            }
        }
        if (conflictsWithSelected) {
            continue;
        }

        selectedCourses.push_back(course);
        result.selectedCourseIds.push_back(course->getCode());
    }

    result.success = !result.selectedCourseIds.empty();
    return result;
}

/**
 * @brief Generates multiple distinct valid schedules for one term.
 *
 * @param catalog Course catalog used for lookup and constraints.
 * @param term Target term for offerings.
 * @param candidateCourseIds Candidate IDs in selection priority order.
 * @param prereqContextCourseIds IDs available to satisfy prerequisites.
 * @param maxCoursesPerTerm Maximum number of selected courses.
 * @param maxSchedules Maximum number of distinct schedules to return.
 * @param preferences Optional hard filter/ranking preferences.
 * @return Distinct valid schedule options.
 */
std::vector<GeneratedTermScheduleResult> ScheduleGenerationEngine::GenerateMultipleValidTermSchedules(
    const CourseCatalog& catalog,
    const Term& term,
    const std::vector<std::string>& candidateCourseIds,
    const std::vector<std::string>& prereqContextCourseIds,
    std::size_t maxCoursesPerTerm,
    std::size_t maxSchedules,
    const SchedulePreferences* preferences
) const {
    std::vector<GeneratedTermScheduleResult> results;
    if (maxSchedules == 0) {
        return results;
    }

    std::vector<std::vector<std::string>> orderings;
    orderings.push_back(candidateCourseIds);

    std::vector<std::string> reversed = candidateCourseIds;
    std::reverse(reversed.begin(), reversed.end());
    orderings.push_back(reversed);

    std::vector<std::string> rotated = candidateCourseIds;
    if (!rotated.empty()) {
        std::rotate(rotated.begin(), rotated.begin() + 1, rotated.end());
    }
    orderings.push_back(rotated);

    std::set<std::string> seenKeys;
    for (const std::vector<std::string>& ordering : orderings) {
        GeneratedTermScheduleResult generated = GenerateOneValidTermSchedule(
            catalog,
            term,
            ordering,
            prereqContextCourseIds,
            maxCoursesPerTerm
        );

        if (!generated.success) {
            continue;
        }
        if (preferences != nullptr && !satisfiesHardPreferences(generated, catalog, term, *preferences)) {
            continue;
        }

        std::vector<std::string> canonicalIds = generated.selectedCourseIds;
        std::sort(canonicalIds.begin(), canonicalIds.end());
        std::string key;
        for (const std::string& id : canonicalIds) {
            if (!key.empty()) {
                key += "|";
            }
            key += id;
        }

        if (seenKeys.insert(key).second) {
            results.push_back(generated);
            if (results.size() >= maxSchedules) {
                break;
            }
        }
    }

    if (preferences != nullptr) {
        std::stable_sort(
            results.begin(),
            results.end(),
            [&](const GeneratedTermScheduleResult& a, const GeneratedTermScheduleResult& b) {
                const double scoreA = scoreSchedule(a, catalog, term, *preferences);
                const double scoreB = scoreSchedule(b, catalog, term, *preferences);
                return scoreA > scoreB;
            }
        );
    }

    return results;
}

/**
 * @brief Generates a best-effort required-course pathway across terms.
 *
 * @param catalog Course catalog used for lookup and constraints.
 * @param profile Student profile used for specialization and context.
 * @param startTerm First term to attempt generation.
 * @param maxTerms Maximum number of terms to iterate.
 * @param maxCoursesPerTerm Maximum courses selected per term.
 * @param preferences Optional hard preference filter.
 * @param startingContextCourseIds Additional satisfied/planned context IDs.
 * @return Multi-term pathway generation result.
 */
GeneratedMultiTermPathwayResult ScheduleGenerationEngine::GenerateBestEffortMultiTermPathway(
    const CourseCatalog& catalog,
    const StudentProfile& profile,
    const Term& startTerm,
    std::size_t maxTerms,
    std::size_t maxCoursesPerTerm,
    const SchedulePreferences* preferences,
    const std::vector<std::string>& startingContextCourseIds
) const {
    GeneratedMultiTermPathwayResult result;

    const Academic::Specialization* spec = Academic::FindSpecializationById(profile.specializationId);
    if (spec == nullptr) {
        result.errors.push_back("No valid specialization selected for pathway generation.");
        return result;
    }

    std::unordered_set<std::string> satisfied;
    for (const std::string& id : profile.completedCourseIds) {
        satisfied.insert(normalizeCourseId(id));
    }
    for (const std::string& id : startingContextCourseIds) {
        satisfied.insert(normalizeCourseId(id));
    }
    int startYear = extractFirstInteger(profile.currentYear);
    if (startYear < 1 || startYear > 4) {
        startYear = 4;
    }
    const int completedLevel = inferHighestCourseLevel(profile.completedCourseIds);
    const int startingContextLevel = inferHighestCourseLevel(startingContextCourseIds);
    const int baseAllowedLevel = std::min(4, std::max(startYear, std::max(completedLevel, startingContextLevel)));
    Term currentTerm = startTerm;

    for (std::size_t termIndex = 0; termIndex < maxTerms; termIndex++) {
        std::vector<std::string> remainingRequired;
        for (const std::string& requiredId : spec->requiredCourseIds) {
            if (satisfied.find(normalizeCourseId(requiredId)) == satisfied.end()) {
                remainingRequired.push_back(requiredId);
            }
        }
        if (remainingRequired.empty()) {
            result.success = true;
            result.remainingRequiredCourseIds.clear();
            return result;
        }

        std::vector<std::string> prereqContextIds(satisfied.begin(), satisfied.end());

        const int generatedPathwayIndex = static_cast<int>(result.termPlans.size());
        const int allowedLevel = std::min(4, baseAllowedLevel + (generatedPathwayIndex / 3));
        std::vector<std::string> candidateCourseIds;
        candidateCourseIds.reserve(remainingRequired.size());
        int skippedByLevel = 0;
        for (const std::string& requiredId : remainingRequired) {
            const int courseLevel = inferCourseLevel(requiredId);
            if (courseLevel == 0 || courseLevel <= allowedLevel) {
                candidateCourseIds.push_back(requiredId);
            } else {
                skippedByLevel++;
            }
        }
        if (skippedByLevel > 0) {
            result.errors.push_back(
                "Level gating skipped " + std::to_string(skippedByLevel) +
                " course(s) in " + currentTerm.toString() +
                " (allowed level <= " + std::to_string(allowedLevel) + ")."
            );
        }

        GeneratedTermScheduleResult generated = GenerateOneValidTermSchedule(
            catalog,
            currentTerm,
            candidateCourseIds,
            prereqContextIds,
            maxCoursesPerTerm
        );

        if (generated.success && preferences != nullptr && !satisfiesHardPreferences(generated, catalog, currentTerm, *preferences)) {
            generated.success = false;
            generated.errors.push_back("No generated schedule in this term satisfies the hard preference constraints.");
        }

        std::vector<std::string> newSelections;
        for (const std::string& selectedId : generated.selectedCourseIds) {
            const std::string normalizedSelectedId = normalizeCourseId(selectedId);
            if (satisfied.find(normalizedSelectedId) == satisfied.end()) {
                newSelections.push_back(selectedId);
            }
        }

        if (!generated.success || newSelections.empty()) {
            result.errors.push_back("No valid progress in " + currentTerm.toString() + "; term skipped.");
            currentTerm = nextTerm(currentTerm);
            continue;
        }

        GeneratedPathwayTermPlan termPlan;
        termPlan.term = currentTerm;
        termPlan.selectedCourseIds = newSelections;
        result.termPlans.push_back(termPlan);
        for (const std::string& selectedId : newSelections) {
            satisfied.insert(normalizeCourseId(selectedId));
        }

        currentTerm = nextTerm(currentTerm);
    }

    result.remainingRequiredCourseIds.clear();
    for (const std::string& requiredId : spec->requiredCourseIds) {
        if (satisfied.find(normalizeCourseId(requiredId)) == satisfied.end()) {
            result.remainingRequiredCourseIds.push_back(requiredId);
        }
    }
    result.success = result.remainingRequiredCourseIds.empty();
    if (!result.success && result.errors.empty()) {
        result.errors.push_back("Reached term limit before satisfying all required courses.");
    }

    return result;
}
