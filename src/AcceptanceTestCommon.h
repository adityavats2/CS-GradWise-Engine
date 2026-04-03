#ifndef ACCEPTANCETESTCOMMON_H
#define ACCEPTANCETESTCOMMON_H

#include <string>
#include <vector>
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"
#include "PlannedSchedule.h"

/**
 * @file AcceptanceTestCommon.h
 * @brief Common helper functions used by the acceptance testing classes.
 *
 * This file contains shared helper functions for the acceptance testing
 * classes. These helpers are used to avoid repeating the same logic in each
 * acceptance test, such as checking if a course exists in a vector, building
 * timeline text, and loading the test catalog.
 *
 * @author Matthew Cheong
 */

/** @brief Prints one PASS/FAIL result */
void PrintAcceptanceResult(const std::string& testName, bool passed, int& passedCount, int& failedCount);

/** @brief Returns true if a vector contains the target string */
bool AcceptanceContains(const std::vector<std::string>& values, const std::string& target);

/** @brief Adds a value only if it is not already in the vector */
void AcceptanceAddIfMissing(std::vector<std::string>& values, const std::string& value);

/** @brief Joins a vector of course ids into one string */
std::string AcceptanceJoinCourseIds(const std::vector<std::string>& courseIds);

/** @brief Builds simple timeline text from locked terms and generated pathway */
std::string AcceptanceBuildTimelineText(
    const std::vector<PlannedTerm>& lockedTerms,
    const GeneratedMultiTermPathwayResult& pathway
);

/** @brief Returns the first line of a text block */
std::string AcceptanceGetFirstLine(const std::string& text);

/** @brief Returns the term index where a course appears in a generated pathway */
int AcceptanceGetCourseTermIndex(const GeneratedMultiTermPathwayResult& pathway, const std::string& courseId);

/** @brief Returns true if a generated pathway contains a course */
bool AcceptancePathwayHasCourse(const GeneratedMultiTermPathwayResult& pathway, const std::string& courseId);

/** @brief Creates a small catalog file used for acceptance testing */
bool AcceptanceCreateCatalogFile(const std::string& filePath);

/** @brief Loads the test catalog */
bool AcceptanceLoadCatalog(CourseCatalog& catalog, const std::string& filePath);

#endif