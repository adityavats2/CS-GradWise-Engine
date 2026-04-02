cmake_minimum_required(VERSION 3.20)
project(group04 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
add_executable(group04
    src/main.cpp
    src/MainFrame.cpp
    src/StudentProfile.cpp
    src/StudentProfilePanel.cpp
    src/CourseCatalogPanel.cpp
    src/ScheduleValidationPanel.cpp
    src/Term.cpp
    src/TimeSlot.cpp
    src/CourseOffering.cpp
    src/Prerequisite.cpp
    src/OutputExportPanel.cpp
    src/Course.cpp
    src/CourseCatalog.cpp
    src/CourseCatalogLoader.cpp
    src/ScheduleGenerationEngine.cpp
    src/PlannedSchedule.cpp
    src/PlannedScheduleStorage.cpp
)
target_include_directories(group04 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(group04 ${wxWidgets_LIBRARIES})
