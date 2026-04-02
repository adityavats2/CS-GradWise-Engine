#include "MainFrame.h"
#include "CourseCatalogLoader.h"

/** @brief Creates the main window */
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600)) {
    if (!CourseCatalogLoader::loadFromFile("data/courses.txt", catalog)) {
        CourseCatalogLoader::loadFromFile("../data/courses.txt", catalog);
    }
    wxPanel* mainPanel = new wxPanel(this);
    studentProfileButton = new wxButton(mainPanel, wxID_ANY, "Student Profile");
    courseCatalogButton = new wxButton(mainPanel, wxID_ANY, "Course Catalog");
    scheduleValidationButton = new wxButton(mainPanel, wxID_ANY, "Schedule Validation");
    outputExportButton = new wxButton(mainPanel, wxID_ANY, "Output and Export");
    pageBook = new wxSimplebook(mainPanel, wxID_ANY);
    studentProfilePanel = new StudentProfilePanel(pageBook);
    courseCatalogPanel = new CourseCatalogPanel(pageBook, &catalog);
    scheduleValidationPanel = new ScheduleValidationPanel(pageBook);
    outputExportPanel = new OutputExportPanel(pageBook, &catalog);
    pageBook->AddPage(studentProfilePanel, "Student Profile");
    pageBook->AddPage(courseCatalogPanel, "Course Catalog");
    pageBook->AddPage(scheduleValidationPanel, "Schedule Validation");
    pageBook->AddPage(outputExportPanel, "Output and Export");
    wxBoxSizer* navSizer = new wxBoxSizer(wxHORIZONTAL);
    navSizer->Add(studentProfileButton, 1, wxEXPAND | wxALL, 0);
    navSizer->Add(courseCatalogButton, 1, wxEXPAND | wxALL, 0);
    navSizer->Add(scheduleValidationButton, 1, wxEXPAND | wxALL, 0);
    navSizer->Add(outputExportButton, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(navSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(pageBook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    mainPanel->SetSizer(mainSizer);
    studentProfileButton->Bind(wxEVT_BUTTON, &MainFrame::OnStudentProfile, this);
    courseCatalogButton->Bind(wxEVT_BUTTON, &MainFrame::OnCourseCatalog, this);
    scheduleValidationButton->Bind(wxEVT_BUTTON, &MainFrame::OnScheduleValidation, this);
    outputExportButton->Bind(wxEVT_BUTTON, &MainFrame::OnOutputExport, this);
    ShowPage(0);
}

/** @brief Shows the selected page */
void MainFrame::ShowPage(int pageIndex) {
    pageBook->SetSelection(pageIndex);
    studentProfileButton->Enable(pageIndex != 0);
    courseCatalogButton->Enable(pageIndex != 1);
    scheduleValidationButton->Enable(pageIndex != 2);
    outputExportButton->Enable(pageIndex != 3);
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

/** @brief Opens the output and export page */
void MainFrame::OnOutputExport(wxCommandEvent& event) {
    const auto& opts = scheduleValidationPanel->getLastGeneratedOptions();
    if (!opts.empty()) {
        outputExportPanel->SetOptions(
            scheduleValidationPanel->getLastGeneratedTerm(), opts);
    } else {
        outputExportPanel->RefreshView();
    }
    ShowPage(3);
}
