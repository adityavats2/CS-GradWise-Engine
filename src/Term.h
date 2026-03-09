#ifndef TERM_H
#define TERM_H
#include <string>

/** @brief Represents a term season */
enum class Season {
    Fall,
    Winter,
    Summer
};

/** @brief Represents one academic term */
class Term {
private:
    Season season;
    int year;

public:
    Term();
    /** @brief Creates new term
     *  @param season Term season: Fall, Winter or Summer
     *  @param year Term year
     */
    Term(Season season, int year);
    Season getSeason() const;
    int getYear() const;
    /** @brief Compares two terms
     *  @param other The other term
     *  @return True if the terms are equal
     */
    bool equal(const Term& other) const;
    /** @brief Converts the term to text
     *  @return The term as a string
     */
    std::string toString() const;
};

#endif