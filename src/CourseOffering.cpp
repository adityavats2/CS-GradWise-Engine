#include "CourseOffering.h"

CourseOffering::CourseOffering() : term(Term()), timeSlots() {}

CourseOffering::CourseOffering(const Term& term, const std::vector<TimeSlot>& timeSlots) : term(term), timeSlots(timeSlots) {}

const Term& CourseOffering::getTerm() const {
    return term;
}

const std::vector<TimeSlot>& CourseOffering::getTimeSlots() const {
    return timeSlots;
}

void CourseOffering::addTimeSlot(const TimeSlot& timeSlot) {
    timeSlots.push_back(timeSlot);
}

bool CourseOffering::isOfferedIn(const Term& otherTerm) const {
    return term.equal(otherTerm);
}