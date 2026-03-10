#ifndef ACADEMICDATA_H
#define ACADEMICDATA_H

#include <string>
#include <vector>

namespace Academic {

/** @brief Represents a specialization/major with its graduation requirements (course codes) */
struct Specialization {
    std::string id;
    std::string name;
    std::vector<std::string> requiredCourseIds;  // target graduation requirements (codes from catalog)
};

/** @brief Predefined academic years */
inline const std::vector<std::string>& GetYearOptions() {
    static const std::vector<std::string> years = { "Year 1", "Year 2", "Year 3", "Year 4" };
    return years;
}

/** @brief All specializations with their required course codes for graduation (codes match CourseCatalog) */
inline const std::vector<Specialization>& GetSpecializations() {
    static const std::vector<Specialization> specs = {
        {"CS", "Computer Science",
         {"CS1020", "CS1025", "CS1026", "CS1027", "CS2210", "CS2211", "CS2208", "CS2209", "CS2212",
          "CS3305", "CS3307", "CS2214", "MATH2155", "MATH2156"}},
        {"SE", "Software Engineering",
         {"CS1020", "CS1025", "CS1026", "CS1027", "CS2210", "CS2211", "CS2208", "CS3305", "CS3307",
          "CS2214", "MATH2155"}},
        {"DATA", "Data Science",
         {"CS1020", "CS1025", "CS1026", "DATA3000", "MATH2155", "MATH2156", "MATH3159"}},
    };
    return specs;
}

/** @brief Get specialization by id */
inline const Specialization* FindSpecializationById(const std::string& id) {
    const std::vector<Specialization>& specs = GetSpecializations();
    for (const auto& s : specs)
        if (s.id == id) return &s;
    return nullptr;
}

} // namespace Academic

#endif
