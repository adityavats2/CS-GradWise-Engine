#ifndef TIMESLOT_H
#define TIMESLOT_H
#include <string>

/** @brief Represents one time slot */
class TimeSlot {
private:
    std::string day;
    int startTime;
    int endTime;

public:
    TimeSlot();
    /** @brief Creates a time slot
     *  @param day Weekday of time slot
     *  @param startTime Start time of time slot in minutes from midnight
     *  @param endTime End time of time slot in minutes from midnight
     */
    TimeSlot(const std::string& day, int startTime, int endTime);
    std::string getDay() const;
    int getStartTime() const;
    int getEndTime() const;
    /** @brief Checks whether two time slots conflict
     *  @param other The other time slot
     *  @return True if time slots conflict
     */
    bool conflictsWith(const TimeSlot& other) const;
};

#endif