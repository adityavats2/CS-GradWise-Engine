#ifndef DELAYCOURSEACCEPTANCETEST_H
#define DELAYCOURSEACCEPTANCETEST_H

#include <string>

/** @class DelayCourseAcceptanceTest
 * 
 * @brief Acceptance test for timeline update when a user delays a required course */
/**
 * 
 *
 * @author Matthew Cheong
 */
class DelayCourseAcceptanceTest {
public:
    bool Run(const std::string& catalogFilePath);
};

#endif