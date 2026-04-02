#include "PlannedSchedule.h"

#include <unordered_set>

namespace {
/**
 * @brief Converts the season to an orderinng rank for comparisons.
 *
 * @param season The season enum value.
 * @return Integer rank used for within-year ordering is returned.
 */
int seasonRank(Season season) {
    switch (season) {
        case Season::Winter:
            return 0;
        case Season::Summer:
            return 1;
        case Season::Fall:
            return 2;
    }
    return 0;
}

/**
 * @brief Checks whether or not term @p a is strictly earlier than term @p b.
 *
 * @param a Candidate for the earlier term.
 * @param b Candidate for the later term.
 * @return True only when @p a occurs before @p b.
 */
bool isEarlierThan(const Term& a, const Term& b) {
    if (a.getYear() != b.getYear()) {
        return a.getYear() < b.getYear();
    }
    return seasonRank(a.getSeason()) < seasonRank(b.getSeason());
}
}  

/**
 * @brief Constructs an empty planned schedule container.
 */
PlannedSchedule::PlannedSchedule() : plannedTerms() {}

/**
 * @brief Removes all the saved planned terms from memory.
 */
void PlannedSchedule::clear() {
    plannedTerms.clear();
}

/**
 * @brief Inserts or replaces course IDs for a given term.
 *
 * @param term Term to update at the given moment.
 * @param courseIds THe pllanned course IDs for that term.
 */
void PlannedSchedule::setCoursesForTerm(const Term& term, const std::vector<std::string>& courseIds) {
    for (PlannedTerm& plannedTerm : plannedTerms) {
        if (plannedTerm.term.equal(term)) {
            plannedTerm.courseIds = courseIds;
            return;
        }
    }
    plannedTerms.push_back({term, courseIds});
}

/**
 * @brief Gets course IDs saved for one term.
 *
 * @param term Term to query.
 * @return Course IDs for the term or flagged as empty if missing.
 */
std::vector<std::string> PlannedSchedule::getCoursesForTerm(const Term& term) const {
    for (const PlannedTerm& plannedTerm : plannedTerms) {
        if (plannedTerm.term.equal(term)) {
            return plannedTerm.courseIds;
        }
    }
    return {};
}

/**
 * @brief Gets unique course IDs from terms strictly before @p term.
 *
 * @param term Cutoff term.
 * @return Unique course IDs planned in earlier terms.
 */
std::vector<std::string> PlannedSchedule::getCoursesBeforeTerm(const Term& term) const {
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;

    for (const PlannedTerm& plannedTerm : plannedTerms) {
        if (!isEarlierThan(plannedTerm.term, term)) {
            continue;
        }
        for (const std::string& courseId : plannedTerm.courseIds) {
            if (seen.insert(courseId).second) {
                result.push_back(courseId);
            }
        }
    }
    return result;
}

/**
 * @brief Accesses all the planned terms stored in memory.
 *
 * @return Read-only list of planned terms.
 */
const std::vector<PlannedTerm>& PlannedSchedule::getAllPlannedTerms() const {
    return plannedTerms;
}
