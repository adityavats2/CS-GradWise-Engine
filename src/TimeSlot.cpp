#include "TimeSlot.h"

TimeSlot::TimeSlot() : day("Monday"), startTime(0), endTime(60) {}

TimeSlot::TimeSlot(const std::string& day, int startTime, int endTime) : day(day), startTime(startTime), endTime(endTime) {}

std::string TimeSlot::getDay() const {
    return day;
}

int TimeSlot::getStartTime() const {
    return startTime;
}

int TimeSlot::getEndTime() const {
    return endTime;
}

bool TimeSlot::conflictsWith(const TimeSlot& other) const {
    int otherStart = other.getStartTime();
    int otherEnd = other.getEndTime();
    if (day == other.getDay() && startTime < otherEnd && otherStart < endTime) {
        return true;
    }
    return false;
}