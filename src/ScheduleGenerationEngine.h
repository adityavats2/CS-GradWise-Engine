#ifndef SCHEDULEGENERATIONENGINE_H
#define SCHEDULEGENERATIONENGINE_H

#include <cstddef>
#include <string>
#include <vector>
#include "CourseCatalog.h"
#include "SchedulePreferences.h"
#include "StudentProfile.h"
#include "Term.h"

/**
 * @brief Result of generating one valid term schedule.
 */
struct GeneratedTermScheduleResult {
    bool success = false;
    std::vector<std::string> selectedCourseIds;
    std::vector<std::string> errors;
};

/**
 * @brief One term entry in a multi-term generated pathway.
 */
struct GeneratedPathwayTermPlan {
    Term term;
    std::vector<std::string> selectedCourseIds;
};

/**
 * @brief Result of generating a best-effort multi-term pathway.
 */
struct GeneratedMultiTermPathwayResult {
    bool success = false;
    std::vector<GeneratedPathwayTermPlan> termPlans;
    std::vector<std::string> remainingRequiredCourseIds;
    std::vector<std::string> errors;
};

/**
 * @brief Generates valid schedules and best-effort pathways from catalog/profile data.
 */
class ScheduleGenerationEngine {
public:
    /**
     * @brief Generate one valid schedule for a target term using greedy selection.
     *
     * @param catalog Course catalog used for lookup and constraints.
     * @param term Target term for generated offerings.
     * @param candidateCourseIds Candidate course IDs in priority order.
     * @param prereqContextCourseIds IDs that satisfy prerequisite context.
     * @param maxCoursesPerTerm Maximum number of selected courses.
     * @return Generated result containing selected courses and warnings.
     */
    GeneratedTermScheduleResult GenerateOneValidTermSchedule(
        const CourseCatalog& catalog,
        const Term& term,
        const std::vector<std::string>& candidateCourseIds,
        const std::vector<std::string>& prereqContextCourseIds,
        std::size_t maxCoursesPerTerm = 5
    ) const;

    /**
     * @brief Generate multiple distinct valid schedules for a term.
     *
     * @param catalog Course catalog used for lookup and constraints.
     * @param term Target term for generated offerings.
     * @param candidateCourseIds Candidate course IDs in priority order.
     * @param prereqContextCourseIds IDs that satisfy prerequisite context.
     * @param maxCoursesPerTerm Maximum number of selected courses.
     * @param maxSchedules Maximum number of distinct schedules to return.
     * @param preferences Optional preferences for hard filters and ranking.
     * @return Distinct generated schedules, optionally preference-ranked.
     */
    std::vector<GeneratedTermScheduleResult> GenerateMultipleValidTermSchedules(
        const CourseCatalog& catalog,
        const Term& term,
        const std::vector<std::string>& candidateCourseIds,
        const std::vector<std::string>& prereqContextCourseIds,
        std::size_t maxCoursesPerTerm = 5,
        std::size_t maxSchedules = 3,
        const SchedulePreferences* preferences = nullptr
    ) const;

    /**
     * @brief Generate a best-effort required-course pathway across multiple terms.
     *
     * @param catalog Course catalog used for lookup and constraints.
     * @param profile Student profile with specialization and completed courses.
     * @param startTerm First term to generate in the pathway.
     * @param maxTerms Maximum number of terms to attempt.
     * @param maxCoursesPerTerm Maximum number of selected courses per term.
     * @param preferences Optional preferences for hard filtering.
     * @param startingContextCourseIds Course IDs treated as already satisfied/planned.
     * @return Multi-term pathway result with remaining required courses.
     */
    GeneratedMultiTermPathwayResult GenerateBestEffortMultiTermPathway(
        const CourseCatalog& catalog,
        const StudentProfile& profile,
        const Term& startTerm,
        std::size_t maxTerms,
        std::size_t maxCoursesPerTerm = 5,
        const SchedulePreferences* preferences = nullptr,
        const std::vector<std::string>& startingContextCourseIds = {}
    ) const;
};

#endif
