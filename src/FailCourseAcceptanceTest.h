#ifndef FAILCOURSEACCEPTANCETEST_H
#define FAILCOURSEACCEPTANCETEST_H

#include <string>

/** @brief Acceptance test for timeline update when a user fails a course */
/**
 * @class FailCourseAcceptanceTest
 * This class simulates a what-if scenario where a student fails a required
 * course. It checks whether the generated graduation timeline updates correctly
 * by pushing dependent courses into later terms.
 * @author Matthew Cheong
 */
class FailCourseAcceptanceTest {
public:
    bool Run(const std::string& catalogFilePath);
};

#endif
