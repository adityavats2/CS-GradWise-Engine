#ifndef SCHEDULEPREFERENCES_H
#define SCHEDULEPREFERENCES_H

/**
 * @brief User preference inputs for schedule ranking and hard filtering.
 */
struct SchedulePreferences {
    bool preferMorning = false;
    bool preferCompactDays = false;
    double morningWeight = 1.0;
    double compactDayWeight = 1.0;
    bool enforceEarliestStart = false;
    int earliestStartMinutes = 0;
};

#endif
