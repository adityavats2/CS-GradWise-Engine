#include "MainFrame.h"

/** @brief Creates the main window */
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600)) {
    wxPanel* mainPanel = new wxPanel(this);
    studentProfileButton = new wxButton(mainPanel, wxID_ANY, "Student Profile");
    courseCatalogButton = new wxButton(mainPanel, wxID_ANY, "Course Catalog");
    scheduleValidationButton = new wxButton(mainPanel, wxID_ANY, "Schedule Validation");
    pageBook = new wxSimplebook(mainPanel, wxID_ANY);
    studentProfilePanel = new StudentProfilePanel(pageBook);
    courseCatalogPanel = new CourseCatalogPanel(pageBook);
    scheduleValidationPanel = new ScheduleValidationPanel(pageBook);
    pageBook->AddPage(studentProfilePanel, "Student Profile");
    pageBook->AddPage(courseCatalogPanel, "Course Catalog");
    pageBook->AddPage(scheduleValidationPanel, "Schedule Validation");
    wxBoxSizer* navSizer = new wxBoxSizer(wxHORIZONTAL);
    navSizer->Add(studentProfileButton, 1, wxEXPAND | wxALL, 0);
    navSizer->Add(courseCatalogButton, 1, wxEXPAND | wxALL, 0);
    navSizer->Add(scheduleValidationButton, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(navSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(pageBook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    mainPanel->SetSizer(mainSizer);
    studentProfileButton->Bind(wxEVT_BUTTON, &MainFrame::OnStudentProfile, this);
    courseCatalogButton->Bind(wxEVT_BUTTON, &MainFrame::OnCourseCatalog, this);
    scheduleValidationButton->Bind(wxEVT_BUTTON, &MainFrame::OnScheduleValidation, this);
    ShowPage(0);
}

/** @brief Shows the selected page */
void MainFrame::ShowPage(int pageIndex) {
    pageBook->SetSelection(pageIndex);
    studentProfileButton->Enable(pageIndex != 0);
    courseCatalogButton->Enable(pageIndex != 1);
    scheduleValidationButton->Enable(pageIndex != 2);
}

/** @brief Opens the student profile page */
void MainFrame::OnStudentProfile(wxCommandEvent& event) {
    ShowPage(0);
}

/** @brief Opens the course catalog page */
void MainFrame::OnCourseCatalog(wxCommandEvent& event) {
    ShowPage(1);
}

/** @brief Opens the schedule validation page */
void MainFrame::OnScheduleValidation(wxCommandEvent& event) {
    ShowPage(2);
}