#include "CourseCatalog.h"

CourseCatalog::CourseCatalog() : courses(), courseMap() {}

bool CourseCatalog::addCourse(std::unique_ptr<Course> course) {
    if (course == nullptr) {
        return false;
    }
    std::string code = course->getCode();
    if (containsCourse(code)) {
        return false;
    }
    Course* coursePtr = course.get();
    courseMap[code] = coursePtr;
    courses.push_back(std::move(course));
    return true;
}

Course* CourseCatalog::getCourse(const std::string& code) const {
    auto course = courseMap.find(code);
    if (course == courseMap.end()) {
        return nullptr;
    }
    return course->second;
}

bool CourseCatalog::containsCourse(const std::string& code) const {
    if (courseMap.find(code) != courseMap.end()) {
        return true;
    }
    return false;
}

const std::vector<std::unique_ptr<Course>>& CourseCatalog::getAllCourses() const {
    return courses;
}

void CourseCatalog::clear() {
    courses.clear();
    courseMap.clear();
}