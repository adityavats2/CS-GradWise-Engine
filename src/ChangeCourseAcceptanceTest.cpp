#include "ChangeCourseAcceptanceTest.h"
#include "AcceptanceTestCommon.h"
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"
#include "StudentProfile.h"
#include "Term.h"
#include <iostream>

bool ChangeCourseAcceptanceTest::Run(const std::string& catalogFilePath) {
    CourseCatalog catalog;
    if (!AcceptanceLoadCatalog(catalog, catalogFilePath)) {
        return false;
    }

    ScheduleGenerationEngine engine;

    StudentProfile profile;
    profile.currentYear = "Year 3";
    profile.specializationId = "SE";
    profile.completedCourseIds.push_back("CS1020");
    profile.completedCourseIds.push_back("CS1025");
    profile.completedCourseIds.push_back("CS1026");
    profile.completedCourseIds.push_back("CS1027");
    profile.completedCourseIds.push_back("MATH2155");

    std::vector<std::string> contextA = profile.completedCourseIds;
    AcceptanceAddIfMissing(contextA, "CS2210");
    AcceptanceAddIfMissing(contextA, "CS2208");

    GeneratedMultiTermPathwayResult pathwayA =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            profile,
            Term(Season::Winter, 2028),
            6,
            5,
            nullptr,
            contextA
        );

    std::vector<std::string> contextB = profile.completedCourseIds;
    AcceptanceAddIfMissing(contextB, "CS2210");
    AcceptanceAddIfMissing(contextB, "CS2209");

    GeneratedMultiTermPathwayResult pathwayB =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            profile,
            Term(Season::Winter, 2028),
            6,
            5,
            nullptr,
            contextB
        );

    std::cout << "\n--- Change Course Acceptance Test ---\n";
    std::cout << "Timeline with CS2208 already planned:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), pathwayA);
    std::cout << "Timeline after changing CS2208 to CS2209:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), pathwayB);

    return !AcceptancePathwayHasCourse(pathwayA, "CS2208") &&
           AcceptancePathwayHasCourse(pathwayB, "CS2208");
}