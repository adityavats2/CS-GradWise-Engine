#ifndef PLANNEDSCHEDULE_H
#define PLANNEDSCHEDULE_H

#include <string>
#include <vector>
#include "Term.h"

/**
 * @brief Saves courses associated with one academic term.
 */
struct PlannedTerm {
    Term term;
    std::vector<std::string> courseIds;
};

/**
 * @brief In-memory container for saved planned terms and course IDs.
 */
class PlannedSchedule {
public:
    /**
     * @brief Constructs an empty planned schedule.
     */
    PlannedSchedule();

    /**
     * @brief Removes all the planned terms from memory.
     */
    void clear();

    /**
     * @brief Inserts or replaces planned courses for a term.
     *
     * @param term Term to update.
     * @param courseIds Planned course IDs for that term.
     */
    void setCoursesForTerm(const Term& term, const std::vector<std::string>& courseIds);

    /**
     * @brief Gets planned courses for one term.
     *
     * @param term Term to query.
     * @return Planned course IDs for the requested term.
     */
    std::vector<std::string> getCoursesForTerm(const Term& term) const;

    /**
     * @brief Gets unique planned courses from terms earlier than the input term.
     *
     * @param term Cutoff term.
     * @return Unique planned course IDs from earlier terms.
     */
    std::vector<std::string> getCoursesBeforeTerm(const Term& term) const;

    /**
     * @brief Accesses all planned terms in insertion order.
     *
     * @return Read-only list of planned terms.
     */
    const std::vector<PlannedTerm>& getAllPlannedTerms() const;

private:
    std::vector<PlannedTerm> plannedTerms;
};

#endif
