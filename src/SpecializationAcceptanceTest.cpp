#include "SpecializationAcceptanceTest.h"
#include "AcceptanceTestCommon.h"
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"
#include "StudentProfile.h"
#include "Term.h"
#include <iostream>

bool SpecializationAcceptanceTest::Run(const std::string& catalogFilePath) {
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
    dataProfile.completedCourseIds.push_back("CS1026");

    StudentProfile seProfile;
    seProfile.currentYear = "Year 3";
    seProfile.specializationId = "SE";
    seProfile.completedCourseIds.push_back("CS1020");
    seProfile.completedCourseIds.push_back("CS1025");
    seProfile.completedCourseIds.push_back("CS1026");

    GeneratedMultiTermPathwayResult dataPathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            dataProfile,
            Term(Season::Winter, 2027),
            8,
            5,
            nullptr,
            dataProfile.completedCourseIds
        );

    GeneratedMultiTermPathwayResult sePathway =
        engine.GenerateBestEffortMultiTermPathway(
            catalog,
            seProfile,
            Term(Season::Winter, 2027),
            8,
            5,
            nullptr,
            seProfile.completedCourseIds
        );

    std::cout << "\n--- Different Specializations Acceptance Test ---\n";
    std::cout << "DATA timeline:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), dataPathway);
    std::cout << "SE timeline:\n";
    std::cout << AcceptanceBuildTimelineText(std::vector<PlannedTerm>(), sePathway);

    bool dataHasDataCourse = AcceptancePathwayHasCourse(dataPathway, "DATA3000");
    bool seHasDataCourse = AcceptancePathwayHasCourse(sePathway, "DATA3000");
    bool seHasSeCourse = AcceptancePathwayHasCourse(sePathway, "CS2210");

    return dataHasDataCourse && !seHasDataCourse && seHasSeCourse;
}