#include "FailCourseAcceptanceTest.h"
#include "AcceptanceTestCommon.h"
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"
#include "StudentProfile.h"
#include "Term.h"
#include <iostream>

bool FailCourseAcceptanceTest::Run(const std::string& catalogFilePath) {
    CourseCatalog catalog;
    if (!AcceptanceLoadCatalog(catalog, catalogFilePath)) {
        return false;
    }

    ScheduleGenerationEngine engine;

    StudentProfile profile;
    profile.currentYear = "Year 3";
    profile.specializationId = "DATA";
    profile.completedCourseIds.push_back("CS1020");
    profile.completedCourseIds.push_back("CS1025");

    std::vector<std::string> normalContext = profile.completedCourseIds;
    AcceptanceAddIfMissing(normalContext, "CS1026");
    AcceptanceAddIfMissing(normalContext, "MATH2155");

    GeneratedMultiTermPathwayResult normalPathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            profile,
            Term(Season::Winter, 2027),
            6,
            5,
            nullptr,
            normalContext
        );

    std::vector<std::string> failedContext = profile.completedCourseIds;
    AcceptanceAddIfMissing(failedContext, "MATH2155");

    GeneratedMultiTermPathwayResult failedPathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            profile,
            Term(Season::Winter, 2027),
            6,
            5,
            nullptr,
            failedContext
        );

    std::cout << "\n--- Fail Course Acceptance Test ---\n";
    std::cout << "Normal timeline:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), normalPathway);
    std::cout << "Timeline after failing CS1026:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), failedPathway);

    int normalIndex = AcceptanceGetCourseTermIndex(normalPathway, "DATA3000");
    int failedIndex = AcceptanceGetCourseTermIndex(failedPathway, "DATA3000");

    return normalIndex >= 0 && failedIndex > normalIndex;
}