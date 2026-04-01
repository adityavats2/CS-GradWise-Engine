#include "CourseCatalogPanel.h"
#include "CourseCatalogLoader.h"
#include "Prerequisite.h"
#include <sstream>

namespace {
std::string formatTime(int minutesFromMidnight) {
    int hours = minutesFromMidnight / 60;
    int minutes = minutesFromMidnight % 60;
    std::ostringstream stream;
    if (hours < 10) {
        stream << '0';
    }
    stream << hours << ':';
    if (minutes < 10) {
        stream << '0';
    }
    stream << minutes;
    return stream.str();
}
}

CourseCatalogPanel::CourseCatalogPanel(wxWindow* parent, CourseCatalog* catalog)
    : wxPanel(parent, wxID_ANY), catalog(catalog) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Course Catalog");
    mainSizer->Add(title, 0, wxALL, 10);
    wxBoxSizer* loadSizer = new wxBoxSizer(wxHORIZONTAL);
    filePathInput = new wxTextCtrl(this, wxID_ANY, "../data/courses.txt");
    loadButton = new wxButton(this, wxID_ANY, "Load Catalog");
    loadSizer->Add(filePathInput, 1, wxRIGHT | wxEXPAND, 10);
    loadSizer->Add(loadButton, 0);
    mainSizer->Add(loadSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    statusText = new wxStaticText(this, wxID_ANY, "No catalog loaded.");
    mainSizer->Add(statusText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    courseList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    courseList->AppendColumn("Code", wxLIST_FORMAT_LEFT, 120);
    courseList->AppendColumn("Title", wxLIST_FORMAT_LEFT, 300);
    courseList->AppendColumn("Credits", wxLIST_FORMAT_LEFT, 100);
    mainSizer->Add(courseList, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    detailsLabel = new wxStaticText(this, wxID_ANY, "Course Details");
    mainSizer->Add(detailsLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    detailsText = new wxTextCtrl(this, wxID_ANY, "Select a course to view its details.", wxDefaultPosition, wxSize(-1, 220), wxTE_MULTILINE | wxTE_READONLY);
    mainSizer->Add(detailsText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    SetSizer(mainSizer);
    loadButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnLoadCatalog, this);
    courseList->Bind(wxEVT_LIST_ITEM_SELECTED, &CourseCatalogPanel::OnCourseSelected, this);
    if(!catalog->getAllCourses().empty()){
        RefreshCourseList();
        ShowCourseDetails(catalog->getAllCourses()[0].get());
        courseList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        statusText->SetLabel("Catalog loaded successfully.");
    }
}

void CourseCatalogPanel::OnLoadCatalog(wxCommandEvent& event) {
    std::string filePath = filePathInput->GetValue().ToStdString();
    catalog->clear();
    if (!CourseCatalogLoader::loadFromFile(filePath, *catalog)) {
        statusText->SetLabel("Failed to load catalog.");
        courseList->DeleteAllItems();
        detailsText->SetValue("No course details available.");
        return;
    }
    RefreshCourseList();
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    if (!courses.empty()) {
        courseList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        ShowCourseDetails(courses[0].get());
    } else {
        detailsText->SetValue("No course details available.");
    }
    statusText->SetLabel("Catalog loaded successfully.");
}

void CourseCatalogPanel::OnCourseSelected(wxListEvent& event) {
    long selectedRow = event.GetIndex();
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    if (selectedRow < 0 || static_cast<std::size_t>(selectedRow) >= courses.size()) {
        detailsText->SetValue("No course details available.");
        return;
    }
    ShowCourseDetails(courses[static_cast<std::size_t>(selectedRow)].get());
}

void CourseCatalogPanel::ShowCourseDetails(const Course* course) {
    if(course == nullptr) {
        detailsText->SetValue("No course details available.");
        return;
    }
    std::ostringstream details;
    details << "Code: " << course->getCode() << "\n";
    details << "Title: " << course->getTitle() << "\n";
    details << "Credits: " << course->getCredits() << "\n\n";
    details << "Prerequisites: ";
    const std::vector<Prerequisite>& prerequisites = course->getPrerequisiteRules();
    if (prerequisites.empty()) {
        details << "None";
    } else {
        for (std::size_t i = 0; i < prerequisites.size(); i++) {
            if (i > 0) {
                details << "; ";
            }
            details << prerequisites[i].toString();
        }
    }
    details << "\n";
    details << "Exclusions: ";
    const std::vector<Course*>& exclusions = course->getExclusions();
    if (exclusions.empty()) {
        details << "None";
    } else {
        for (std::size_t i = 0; i < exclusions.size(); i++) {
            if (i > 0) {
                details << ", ";
            }
            details << exclusions[i]->getCode();
        }
    }
    details << "\n\n";
    details << "Offerings:\n";
    const std::vector<CourseOffering>& offerings = course->getOfferings();
    if (offerings.empty()) {
        details << "None";
    } else {
        for (const CourseOffering& offering : offerings) {
            details << "- " << offering.getTerm().toString() << "\n";
            const std::vector<TimeSlot>& timeSlots = offering.getTimeSlots();
            if (timeSlots.empty()) {
                details << "  No time slots\n";
            } else {
                for (const TimeSlot& timeSlot : timeSlots) {
                    details << "  "
                            << timeSlot.getDay()
                            << " "
                            << formatTime(timeSlot.getStartTime())
                            << " - "
                            << formatTime(timeSlot.getEndTime())
                            << "\n";
                }
            }
        }
    }
    detailsText->SetValue(details.str());
}

void CourseCatalogPanel::RefreshCourseList() {
    courseList->DeleteAllItems();
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    for (std::size_t i = 0; i < courses.size(); i++) {
        const Course* course = courses[i].get();
        long row = courseList->InsertItem(static_cast<long>(i), course->getCode());
        courseList->SetItem(row, 1, course->getTitle());
        courseList->SetItem(row, 2, wxString::Format("%.1f", course->getCredits()));
    }
}