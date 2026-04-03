#ifndef CHANGECOURSEACCEPTANCETEST_H
#define CHANGECOURSEACCEPTANCETEST_H

#include <string>

/** @class ChangeCourseAcceptanceTest
 * 
 * @brief Acceptance test for timeline update when a user changes a course */
/**
 * 
 *
 * @author Matthew Cheong
 */
class ChangeCourseAcceptanceTest {
public:
    bool Run(const std::string& catalogFilePath);
};

#endif