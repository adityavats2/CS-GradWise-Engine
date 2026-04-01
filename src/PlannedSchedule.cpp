#include "PlannedSchedule.h"

#include <unordered_set>

namespace {
/**
 * @brief Convert season to an ordering rank for comparisons.
 *
 * @param season Season enum value.
 * @return Integer rank used for within-year ordering.
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
 * @brief Check whether term @p a is strictly earlier than term @p b.
 *
 * @param a Candidate earlier term.
 * @param b Candidate later term.
 * @return True when @p a occurs before @p b.
 */
bool isEarlierThan(const Term& a, const Term& b) {
    if (a.getYear() != b.getYear()) {
        return a.getYear() < b.getYear();
    }
    return seasonRank(a.getSeason()) < seasonRank(b.getSeason());
}
}  // namespace

/**
 * @brief Construct an empty planned schedule container.
 */
PlannedSchedule::PlannedSchedule() : plannedTerms() {}

/**
 * @brief Remove all saved planned terms from memory.
 */
void PlannedSchedule::clear() {
    plannedTerms.clear();
}

/**
 * @brief Insert or replace course IDs for a given term.
 *
 * @param term Term to update.
 * @param courseIds Planned course IDs for that term.
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
 * @brief Get course IDs saved for one term.
 *
 * @param term Term to query.
 * @return Course IDs for the term, or empty if missing.
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
 * @brief Get unique course IDs from terms strictly before @p term.
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
 * @brief Access all planned terms stored in memory.
 *
 * @return Read-only list of planned terms.
 */
const std::vector<PlannedTerm>& PlannedSchedule::getAllPlannedTerms() const {
    return plannedTerms;
}
