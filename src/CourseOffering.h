#ifndef COURSEOFFERING_H
#define COURSEOFFERING_H
#include <vector>
#include "Term.h"
#include "TimeSlot.h"

/** @brief Represents a course offering */
class CourseOffering {
private:
    Term term;
    std::vector<TimeSlot> timeSlots;

public:
    CourseOffering();
    /** @brief Creates a course offering
     *  @param term term of the course offering
     *  @param timeSlots Time slot of the course offering
     */
    CourseOffering(const Term& term, const std::vector<TimeSlot>& timeSlots);
    const Term& getTerm() const;
    const std::vector<TimeSlot>& getTimeSlots() const;
    void addTimeSlot(const TimeSlot& timeSlot);
    bool isOfferedIn(const Term& otherTerm) const;
};

#endif