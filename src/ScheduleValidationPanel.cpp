#include "ScheduleValidationPanel.h"
#include "CourseCatalogLoader.h"
#include "StudentProfile.h"

ScheduleValidationPanel::ScheduleValidationPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Schedule Validation");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    title->SetFont(titleFont);
    mainSizer->Add(title, 0, wxALL, 10);

    // Term selection
    wxBoxSizer* termSizer = new wxBoxSizer(wxHORIZONTAL);

    termSizer->Add(new wxStaticText(this, wxID_ANY, "Season:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    seasonCombo = new wxComboBox(
        this,
        wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        0,
        nullptr,
        wxCB_READONLY
    );

    seasonCombo->Append("Fall");
    seasonCombo->Append("Winter");
    seasonCombo->Append("Summer");

    termSizer->Add(seasonCombo, 0, wxRIGHT, 10);

    termSizer->Add(new wxStaticText(this, wxID_ANY, "Year:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    yearInput = new wxTextCtrl(this, wxID_ANY, "2026");
    termSizer->Add(yearInput, 0);

    mainSizer->Add(termSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // Courses list
    mainSizer->Add(
        new wxStaticText(this, wxID_ANY, "Select planned courses:"),
        0,
        wxLEFT | wxRIGHT | wxTOP,
        10
    );

    coursesCheckList = new wxCheckListBox(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxSize(400,200)
    );

    mainSizer->Add(coursesCheckList, 1, wxEXPAND | wxALL, 10);

    // Validate button
    validateButton = new wxButton(this, wxID_ANY, "Validate Schedule");
    mainSizer->Add(validateButton, 0, wxALL, 10);

    // Results output
    resultsText = new wxTextCtrl(
        this,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxSize(-1,150),
        wxTE_MULTILINE | wxTE_READONLY
    );

    mainSizer->Add(resultsText, 0, wxEXPAND | wxALL, 10);

    SetSizer(mainSizer);

    LoadCatalog();
    PopulateCoursesList();

    validateButton->Bind(wxEVT_BUTTON, &ScheduleValidationPanel::OnValidate, this);
}

void ScheduleValidationPanel::LoadCatalog() {
    std::string path = "data/courses.txt";

    if (!CourseCatalogLoader::loadFromFile(path, catalog)) {
        path = "../data/courses.txt";
        CourseCatalogLoader::loadFromFile(path, catalog);
    }
}

void ScheduleValidationPanel::PopulateCoursesList() {
    coursesCheckList->Clear();

    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();

    for (const auto& ptr : courses) {
        const Course* c = ptr.get();

        if (c) {
            coursesCheckList->Append(
                wxString::FromUTF8(c->getCode() + " - " + c->getTitle())
            );
        }
    }
}

void ScheduleValidationPanel::OnValidate(wxCommandEvent&) {

    resultsText->Clear();
    bool hasErrors = false;

    StudentProfile profile;
    if (!profile.LoadFromFile("data/student_profile.txt")) {
    resultsText->AppendText(" Prerequisite checks may be incomplete.\n");
}

    int selectedCount = 0;
    std::vector<const Course*> selectedCourses;

    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();

    for (unsigned int i = 0; i < coursesCheckList->GetCount(); i++) {

        if (coursesCheckList->IsChecked(i)) {

            selectedCount++;

            if (i < courses.size()) {
                selectedCourses.push_back(courses[i].get());
            }
        }
    }

    if (selectedCount == 0) {
        resultsText->AppendText("No courses selected.\n");
        return;
    }

    if (selectedCount > 5) {
        resultsText->AppendText("Error: Maximum course load is 5 courses per term.\n");
        hasErrors = true;
    } else {
        resultsText->AppendText("Course load valid.\n");
    }

    wxString season = seasonCombo->GetValue();
    wxString yearStr = yearInput->GetValue();

    std::string termSeason = season.ToStdString();
    int year = std::stoi(yearStr.ToStdString());

    Season selectedSeason;

    if (termSeason == "Fall") selectedSeason = Season::Fall;
    else if (termSeason == "Winter") selectedSeason = Season::Winter;
    else selectedSeason = Season::Summer;

    bool offeringErrors = false;

    for (const Course* course : selectedCourses) {

        // prerequisite validation
        for (const Course* prereq : course->getPrerequisites())
        {
            bool satisfied = false;

            for (const std::string& completed : profile.completedCourseIds)
            {
                if (completed == prereq->getCode())
                {
                    satisfied = true;
                    break;
                }
            }

            if (!satisfied)
            {
                hasErrors = true;

                resultsText->AppendText(
                    "Error: " +
                    wxString::FromUTF8(course->getCode()) +
                    " missing prerequisite " +
                    wxString::FromUTF8(prereq->getCode()) +
                    "\n"
                );
                resultsText->AppendText(
                    wxString::FromUTF8(course->getCode()) + " prerequisites satisfied.\n"
                );

            }
        }

        bool offered = false;

        for (const auto& offering : course->getOfferings()) {

            const Term& term = offering.getTerm();

            if (term.getSeason() == selectedSeason && term.getYear() == year) {
                offered = true;
                break;
            }
        }

        if (!offered) {
            hasErrors = true;

            resultsText->AppendText(
                "Error: " +
                wxString::FromUTF8(course->getCode()) +
                " is not offered in " +
                season +
                " " +
                yearStr +
                ".\n"
            );

            offeringErrors = true;
        }
    }
    // time conflict validation
for (size_t i = 0; i < selectedCourses.size(); i++)
{
    for (size_t j = i + 1; j < selectedCourses.size(); j++)
    {
        const Course* c1 = selectedCourses[i];
        const Course* c2 = selectedCourses[j];

        for (const auto& off1 : c1->getOfferings())
        {
            for (const auto& off2 : c2->getOfferings())
            {
                const Term& t1 = off1.getTerm();
                const Term& t2 = off2.getTerm();

                if (t1.getSeason() == selectedSeason &&
                    t2.getSeason() == selectedSeason &&
                    t1.getYear() == year &&
                    t2.getYear() == year)
                {
                    const std::vector<TimeSlot>& slots1 = off1.getTimeSlots();
                    const std::vector<TimeSlot>& slots2 = off2.getTimeSlots();

                    for (const TimeSlot& s1 : slots1)
                    {
                        for (const TimeSlot& s2 : slots2)
                        {
                            if (s1.conflictsWith(s2))
                            {
                                hasErrors = true;

                                resultsText->AppendText(
                                    "Error: Time conflict between " +
                                    wxString::FromUTF8(c1->getCode()) +
                                    " and " +
                                    wxString::FromUTF8(c2->getCode()) +
                                    "\n"
                                );
                            }
                        }
                    }
                }
            }
        }
    }
}
    if (!offeringErrors) {
        resultsText->AppendText("All selected courses are offered in the selected term.\n");
    }

    if (!hasErrors) {
        resultsText->AppendText("Schedule passes validation.\n");
    }
}