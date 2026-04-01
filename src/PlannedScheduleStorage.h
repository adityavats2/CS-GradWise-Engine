#ifndef PLANNEDSCHEDULESTORAGE_H
#define PLANNEDSCHEDULESTORAGE_H

#include <string>
#include "PlannedSchedule.h"

/**
 * @brief File I/O helpers for reading and writing planned schedules.
 */
class PlannedScheduleStorage {
public:
    /**
     * @brief Load a planned schedule from disk.
     *
     * @param path File path to read.
     * @param schedule Destination in-memory schedule.
     * @return True on successful parse/load.
     */
    static bool loadFromFile(const std::string& path, PlannedSchedule& schedule);
    /**
     * @brief Save a planned schedule to disk.
     *
     * @param path File path to write.
     * @param schedule Source in-memory schedule.
     * @return True if the write succeeds.
     */
    static bool saveToFile(const std::string& path, const PlannedSchedule& schedule);
};

#endif
