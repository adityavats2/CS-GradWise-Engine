#ifndef PLANNEDSCHEDULESTORAGE_H
#define PLANNEDSCHEDULESTORAGE_H

#include <string>
#include "PlannedSchedule.h"

/**
 * @brief The File I/O helpers for reading and writing planned schedules.
 */
class PlannedScheduleStorage {
public:
    /**
     * @brief Loads a planned schedule from disk.
     *
     * @param path The file path to read.
     * @param schedule The destination in-memory schedule.
     * @return True on successful parse/load.
     */
    static bool loadFromFile(const std::string& path, PlannedSchedule& schedule);
    /**
     * @brief Saves a planned schedule to disk.
     *
     * @param path The file path to write.
     * @param schedule The source in-memory schedule.
     * @return True if the write succeeds.
     */
    static bool saveToFile(const std::string& path, const PlannedSchedule& schedule);
};

#endif
