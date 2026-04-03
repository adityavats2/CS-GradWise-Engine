#include "DelayCourseAcceptanceTest.h"
#include "AcceptanceTestCommon.h"
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"
#include "StudentProfile.h"
#include "Term.h"
#include <iostream>

bool DelayCourseAcceptanceTest::Run(const std::string& catalogFilePath) {
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
    profile.completedCourseIds.push_back("CS1026");

    std::vector<std::string> onTimeContext = profile.completedCourseIds;
    AcceptanceAddIfMissing(onTimeContext, "MATH2155");

    GeneratedMultiTermPathwayResult onTimePathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            profile,
            Term(Season::Winter, 2027),
            6,
            5,
            nullptr,
            onTimeContext
        );

    GeneratedMultiTermPathwayResult delayedPathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            profile,
            Term(Season::Winter, 2027),
            6,
            5,
            nullptr,
            profile.completedCourseIds
        );

    std::cout << "\n--- Delay Course Acceptance Test ---\n";
    std::cout << "Timeline when MATH2155 is already done:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), onTimePathway);
    std::cout << "Timeline when MATH2155 is delayed:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), delayedPathway);

    int onTimeIndex = AcceptanceGetCourseTermIndex(onTimePathway, "MATH2156");
    int delayedIndex = AcceptanceGetCourseTermIndex(delayedPathway, "MATH2156");

    return onTimeIndex >= 0 && delayedIndex > onTimeIndex;
}