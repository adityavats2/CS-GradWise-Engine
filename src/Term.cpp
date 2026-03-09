#include "Term.h"

Term::Term() : season(Season::Fall), year(0) {}

Term::Term(Season season, int year) : season(season), year(year) {}

Season Term::getSeason() const {
    return season;
}

int Term::getYear() const {
    return year;
}

bool Term::equal(const Term& other) const {
    return season == other.season && year == other.year;
}

std::string Term::toString() const {
    std::string seasonString;
    switch (season){
        case Season::Fall:
            seasonString = "Fall";
            break;
        case Season::Winter:
            seasonString = "Winter";
            break;
        case Season::Summer:
            seasonString = "Summer";
            break;
    }
    return seasonString + " " + std::to_string(year);
}