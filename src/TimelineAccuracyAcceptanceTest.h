#ifndef TIMELINEACCURACYACCEPTANCETEST_H
#define TIMELINEACCURACYACCEPTANCETEST_H

#include <string>

/**
 *@class TimelineAccuracyAcceptanceTest
 * 
 * @brief Tests whether the timeline is clear and accurate after a scenario change. It gives the system some already locked earlier terms, then compares future timelines after a change.

What it looks for:
It checks 2 things:

old locked timeline parts stay the same
future parts update properly
 */
/**
 * @author Matthew Cheong
 */
 *  @brief Acceptance test for timeline clarity and accuracy */
class TimelineAccuracyAcceptanceTest {
public:
    bool Run(const std::string& catalogFilePath);
};

#endif