Requirements:
- CMake
- wxWidgets

Build and run:
cmake -S . -B build
cmake --build build
./build/group04.\build\Debug\group04.exe

The course catalog is loaded from data/courses.txt.
Each course is stored in a block that starts with COURSE and ends with END.
Supported fields:
- CODE
- TITLE
- CREDITS
- PREREQUISITES
- PREREQ_ONE_OF
- PREREQ_CREDITS_FROM
- EXCLUSIONS
- OFFERING
- TIMESLOT