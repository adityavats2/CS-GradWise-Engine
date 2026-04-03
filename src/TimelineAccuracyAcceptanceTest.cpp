#include "TimelineAccuracyAcceptanceTest.h"
#include "AcceptanceTestCommon.h"
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"
#include "StudentProfile.h"
#include "Term.h"
#include <iostream>

bool TimelineAccuracyAcceptanceTest::Run(const std::string& catalogFilePath) {
    CourseCatalog catalog;
    if (!AcceptanceLoadCatalog(catalog, catalogFilePath)) {
        return false;
    }

    ScheduleGenerationEngine engine;

    StudentProfile dataProfile;
    dataProfile.currentYear = "Year 3";
    dataProfile.specializationId = "DATA";
    dataProfile.completedCourseIds.push_back("CS1020");
    dataProfile.completedCourseIds.push_back("CS1025");

    StudentProfile seProfile;
    seProfile.currentYear = "Year 3";
    seProfile.specializationId = "SE";
    seProfile.completedCourseIds.push_back("CS1020");
    seProfile.completedCourseIds.push_back("CS1025");

    std::vector<PlannedTerm> lockedTerms;
    PlannedTerm lockedTerm;
    lockedTerm.term = Term(Season::Fall, 2026);
    lockedTerm.courseIds.push_back("CS1026");
    lockedTerm.courseIds.push_back("MATH2155");
    lockedTerms.push_back(lockedTerm);

    std::vector<std::string> lockedContext;
    lockedContext.push_back("CS1020");
    lockedContext.push_back("CS1025");
    lockedContext.push_back("CS1026");
    lockedContext.push_back("MATH2155");

    GeneratedMultiTermPathwayResult dataPathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            dataProfile,
            Term(Season::Winter, 2027),
            8,
            5,
            nullptr,
            lockedContext
        );

    GeneratedMultiTermPathwayResult sePathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            seProfile,
            Term(Season::Winter, 2027),
            8,
            5,
            nullptr,
            lockedContext
        );

    std::string dataTimeline = AcceptanceBuildTimelineText(lockedTerms, dataPathway);
    std::string seTimeline = AcceptanceBuildTimelineText(lockedTerms, sePathway);

    std::cout << "\n--- Timeline Clarity and Accuracy Acceptance Test ---\n";
    std::cout << "DATA timeline:\n" << dataTimeline;
    std::cout << "SE timeline:\n" << seTimeline;

    std::string expectedFirstLine = "Fall 2026: CS1026, MATH2155";

    bool lockedPartStayedSame =
        AcceptanceGetFirstLine(dataTimeline) == expectedFirstLine &&
        AcceptanceGetFirstLine(seTimeline) == expectedFirstLine;

    bool futurePartChanged = dataTimeline != seTimeline;

    return lockedPartStayedSame && futurePartChanged;
}