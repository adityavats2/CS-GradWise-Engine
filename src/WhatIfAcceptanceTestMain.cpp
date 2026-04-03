#include <iostream>

#include "AcceptanceTestCommon.h"
#include "FailCourseAcceptanceTest.h"
#include "ChangeCourseAcceptanceTest.h"
#include "DelayCourseAcceptanceTest.h"
#include "SpecializationAcceptanceTest.h"
#include "TimelineAccuracyAcceptanceTest.h"

int main() {
    int passedCount = 0;
    int failedCount = 0;

    std::string catalogFile = "what_if_acceptance_catalog.txt";

    PrintAcceptanceResult(
        "Create acceptance catalog file",
        AcceptanceCreateCatalogFile(catalogFile),
        passedCount,
        failedCount
    );

    FailCourseAcceptanceTest failCourseTest;
    ChangeCourseAcceptanceTest changeCourseTest;
    DelayCourseAcceptanceTest delayCourseTest;
    SpecializationAcceptanceTest specializationTest;
    TimelineAccuracyAcceptanceTest timelineAccuracyTest;

    PrintAcceptanceResult(
        "Acceptance test: user fails a course",
        failCourseTest.Run(catalogFile),
        passedCount,
        failedCount
    );

    PrintAcceptanceResult(
        "Acceptance test: user changes a course",
        changeCourseTest.Run(catalogFile),
        passedCount,
        failedCount
    );

    PrintAcceptanceResult(
        "Acceptance test: user delays a course",
        delayCourseTest.Run(catalogFile),
        passedCount,
        failedCount
    );

    PrintAcceptanceResult(
        "Acceptance test: different specializations update timeline",
        specializationTest.Run(catalogFile),
        passedCount,
        failedCount
    );

    PrintAcceptanceResult(
        "Acceptance test: timeline is clear and accurate",
        timelineAccuracyTest.Run(catalogFile),
        passedCount,
        failedCount
    );

    std::cout << "\nAcceptance Testing Summary\n";
    std::cout << "Passed: " << passedCount << "\n";
    std::cout << "Failed: " << failedCount << "\n";

    if (failedCount == 0) {
        std::cout << "All acceptance tests passed.\n";
        return 0;
    }

    std::cout << "Some acceptance tests failed.\n";
    return 1;
}