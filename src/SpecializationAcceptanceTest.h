#ifndef SPECIALIZATIONACCEPTANCETEST_H
#define SPECIALIZATIONACCEPTANCETEST_H

#include <string>

/** @class SpecializationAcceptanceTest
 * 
 * @brief Acceptance test for timeline update when a user changes specialization, One student profile uses one specialization like DATA, and another uses SE
 */
/**
 * @author Matthew Cheong
 */
class SpecializationAcceptanceTest {
public:
    bool Run(const std::string& catalogFilePath);
};

#endif