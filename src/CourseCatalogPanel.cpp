/**
 * @file CourseCatalogPanel.cpp
 * @brief Implementation of CourseCatalogPanel.
 */

#include "CourseCatalogPanel.h"
#include "CourseCatalogLoader.h"
#include "CourseCatalogSaver.h"
#include "Prerequisite.h"
#include "Course.h"
#include "Term.h"

#include <sstream>
#include <algorithm>
#include <cctype>

namespace {
/** @brief Formats minutes since midnight as HH:MM for offering display. */
std::string formatTime(int minutesFromMidnight) {
    std::string trim(const std::string& text) {
    std::size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        start++;
    }

    std::size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        end--;
    }

    return text.substr(start, end - start);
}

std::vector<std::string> splitCommaSeparatedList(const std::string& text) {
    std::vector<std::string> items;
    std::stringstream stream(text);
    std::string item;

    while (std::getline(stream, item, ',')) {
        std::string cleaned = trim(item);
        if (!cleaned.empty()) {
            items.push_back(cleaned);
        }
    }

    return items;
}

std::vector<std::string> splitNonEmptyLines(const std::string& text) {
    std::vector<std::string> lines;
    std::stringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {
        std::string cleaned = trim(line);
        if (!cleaned.empty()) {
            lines.push_back(cleaned);
        }
    }

    return lines;
}

std::string formatTime24(int minutesFromMidnight) {
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

/**
 * @brief Builds UI, binds events, and optionally displays an already-loaded catalog.
 */
CourseCatalogPanel::CourseCatalogPanel(wxWindow* parent, CourseCatalog* catalog)
    : wxPanel(parent, wxID_ANY), 
    catalog(catalog),
    filePathInput(nullptr),
    loadButton(nullptr),
    saveCatalogButton(nullptr),
    statusText(nullptr),
    courseList(nullptr),
    detailsLabel(nullptr),
    detailsText(nullptr),
    newCourseCodeInput(nullptr),
    newCourseTitleInput(nullptr),
    newCourseCreditsInput(nullptr),
    newCourseBreadthInput(nullptr),
    newCourseSimplePrereqInput(nullptr),
    newCourseExclusionsInput(nullptr),
    offeringSeasonChoice(nullptr),
    offeringYearSpinCtrl(nullptr),
    timeSlotDayChoice(nullptr),
    startHourSpinCtrl(nullptr),
    startMinuteChoice(nullptr),
    startAmPmChoice(nullptr),
    endHourSpinCtrl(nullptr),
    endMinuteChoice(nullptr),
    endAmPmChoice(nullptr),
    addTimeSlotButton(nullptr),
    removeTimeSlotButton(nullptr),
    currentTimeSlotsList(nullptr),
    addOfferingButton(nullptr),
    removeOfferingButton(nullptr),
    offeringsList(nullptr),
    addCourseButton(nullptr) {

    wxScrolledWindow* scroll = new wxScrolledWindow(this, wxID_ANY);
    scroll->SetScrollRate(0, 20);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* title = new wxStaticText(scroll, wxID_ANY, "Course Catalog");
    mainSizer->Add(title, 0, wxALL, 10);

    wxBoxSizer* loadSizer = new wxBoxSizer(wxHORIZONTAL);
    filePathInput = new wxTextCtrl(scroll, wxID_ANY, "data/courses.txt");
    loadButton = new wxButton(scroll, wxID_ANY, "Load Catalog");
    saveCatalogButton = new wxButton(scroll, wxID_ANY, "Save Catalog");

    loadSizer->Add(filePathInput, 1, wxRIGHT | wxEXPAND, 10);
    loadSizer->Add(loadButton, 0, wxRIGHT, 10);
    loadSizer->Add(saveCatalogButton, 0);
    mainSizer->Add(loadSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

    statusText = new wxStaticText(scroll, wxID_ANY, "No catalog loaded.");
    mainSizer->Add(statusText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    courseList = new wxListCtrl(scroll, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    courseList->AppendColumn("Code", wxLIST_FORMAT_LEFT, 120);
    courseList->AppendColumn("Title", wxLIST_FORMAT_LEFT, 300);
    courseList->AppendColumn("Credits", wxLIST_FORMAT_LEFT, 100);
    courseList->AppendColumn("Breadth", wxLIST_FORMAT_LEFT, 150);
    courseList->AppendColumn("Elective", wxLIST_FORMAT_LEFT, 100);
    mainSizer->Add(courseList, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

    wxStaticBoxSizer* addCourseSizer = new wxStaticBoxSizer(wxVERTICAL, scroll, "Add New Course");
    wxStaticBoxSizer* basicInfoSizer = new wxStaticBoxSizer(wxVERTICAL, scroll, "Basic Information");

    wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
    row1->Add(new wxStaticText(scroll, wxID_ANY, "Code:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    newCourseCodeInput = new wxTextCtrl(scroll, wxID_ANY);
    newCourseCodeInput->SetMinSize(wxSize(140, -1));
    row1->Add(newCourseCodeInput, 0, wxRIGHT, 20);

    row1->Add(new wxStaticText(scroll, wxID_ANY, "Title:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    newCourseTitleInput = new wxTextCtrl(scroll, wxID_ANY);
    newCourseTitleInput->SetMinSize(wxSize(320, -1));
    row1->Add(newCourseTitleInput, 1, wxEXPAND);
    basicInfoSizer->Add(row1, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
    row2->Add(new wxStaticText(scroll, wxID_ANY, "Credits:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    newCourseCreditsInput = new wxTextCtrl(scroll, wxID_ANY, "0.5");
    newCourseCreditsInput->SetMinSize(wxSize(80, -1));
    row2->Add(newCourseCreditsInput, 0, wxRIGHT, 20);

    row2->Add(new wxStaticText(scroll, wxID_ANY, "Breadth Category (optional):"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    newCourseBreadthInput = new wxTextCtrl(scroll, wxID_ANY);
    newCourseBreadthInput->SetMinSize(wxSize(260, -1));
    row2->Add(newCourseBreadthInput, 1, wxEXPAND);
    basicInfoSizer->Add(row2, 0, wxEXPAND | wxALL, 5);

    newCourseElectiveCheckBox = new wxCheckBox(scroll, wxID_ANY, "This course is an elective");
    basicInfoSizer->Add(newCourseElectiveCheckBox, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

    addCourseSizer->Add(basicInfoSizer, 0, wxEXPAND | wxALL, 5);
    wxStaticBoxSizer* requisitesSizer = new wxStaticBoxSizer(wxVERTICAL, scroll, "Prerequisites and Exclusions");
    wxFlexGridSizer* reqForm = new wxFlexGridSizer(2, 5, 5);

    reqForm->Add(new wxStaticText(scroll, wxID_ANY, "Simple prerequisites (comma-separated):"), 0, wxALIGN_TOP);
    newCourseSimplePrereqInput = new wxTextCtrl(scroll, wxID_ANY);
    reqForm->Add(newCourseSimplePrereqInput, 1, wxEXPAND);

    reqForm->Add(new wxStaticText(scroll, wxID_ANY, "Exclusions (comma-separated):"), 0, wxALIGN_TOP);
    newCourseExclusionsInput = new wxTextCtrl(scroll, wxID_ANY);
    reqForm->Add(newCourseExclusionsInput, 1, wxEXPAND);

    reqForm->AddGrowableCol(1, 1);
    requisitesSizer->Add(reqForm, 1, wxEXPAND | wxALL, 5);
    addCourseSizer->Add(requisitesSizer, 0, wxEXPAND | wxALL, 5);

    // Offerings / timeslots builder
    wxStaticBoxSizer* offeringBuilderSizer = new wxStaticBoxSizer(wxVERTICAL, scroll, "Offerings and Timeslots");

    wxBoxSizer* offeringTermSizer = new wxBoxSizer(wxHORIZONTAL);
    offeringTermSizer->Add(new wxStaticText(scroll, wxID_ANY, "Season:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    offeringSeasonChoice = new wxChoice(scroll, wxID_ANY);
    offeringSeasonChoice->Append("Fall");
    offeringSeasonChoice->Append("Winter");
    offeringSeasonChoice->Append("Summer");
    offeringSeasonChoice->SetSelection(0);
    offeringTermSizer->Add(offeringSeasonChoice, 0, wxRIGHT, 15);

    offeringTermSizer->Add(new wxStaticText(scroll, wxID_ANY, "Year:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    offeringYearSpinCtrl = new wxSpinCtrl(scroll, wxID_ANY);
    offeringYearSpinCtrl->SetRange(2000, 2100);
    offeringYearSpinCtrl->SetValue(2026);
    offeringTermSizer->Add(offeringYearSpinCtrl, 0);

    offeringBuilderSizer->Add(offeringTermSizer, 0, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* timeSlotBuilderSizer = new wxStaticBoxSizer(wxVERTICAL, scroll, "Current Offering Timeslots");

    wxFlexGridSizer* timeForm = new wxFlexGridSizer(4, 7, 7);

    timeForm->Add(new wxStaticText(scroll, wxID_ANY, "Day:"), 0, wxALIGN_CENTER_VERTICAL);
    timeSlotDayChoice = new wxChoice(scroll, wxID_ANY);
    timeSlotDayChoice->Append("Monday");
    timeSlotDayChoice->Append("Tuesday");
    timeSlotDayChoice->Append("Wednesday");
    timeSlotDayChoice->Append("Thursday");
    timeSlotDayChoice->Append("Friday");
    timeSlotDayChoice->SetSelection(0);
    timeForm->Add(timeSlotDayChoice, 1, wxEXPAND);

    timeForm->Add(new wxStaticText(scroll, wxID_ANY, "Start:"), 0, wxALIGN_CENTER_VERTICAL);
    wxBoxSizer* startSizer = new wxBoxSizer(wxHORIZONTAL);
    startHourSpinCtrl = new wxSpinCtrl(scroll, wxID_ANY);
    startHourSpinCtrl->SetRange(1, 12);
    startHourSpinCtrl->SetValue(9);
    startSizer->Add(startHourSpinCtrl, 0, wxRIGHT, 5);

    startMinuteChoice = new wxChoice(scroll, wxID_ANY);
    startMinuteChoice->Append("00");
    startMinuteChoice->Append("30");
    startMinuteChoice->SetSelection(0);
    startSizer->Add(startMinuteChoice, 0, wxRIGHT, 5);

    startAmPmChoice = new wxChoice(scroll, wxID_ANY);
    startAmPmChoice->Append("AM");
    startAmPmChoice->Append("PM");
    startAmPmChoice->SetSelection(0);
    startSizer->Add(startAmPmChoice, 0);
    timeForm->Add(startSizer, 1, wxEXPAND);

    timeForm->Add(new wxStaticText(scroll, wxID_ANY, "End:"), 0, wxALIGN_CENTER_VERTICAL);
    wxBoxSizer* endSizer = new wxBoxSizer(wxHORIZONTAL);
    endHourSpinCtrl = new wxSpinCtrl(scroll, wxID_ANY);
    endHourSpinCtrl->SetRange(1, 12);
    endHourSpinCtrl->SetValue(10);
    endSizer->Add(endHourSpinCtrl, 0, wxRIGHT, 5);

    endMinuteChoice = new wxChoice(scroll, wxID_ANY);
    endMinuteChoice->Append("00");
    endMinuteChoice->Append("30");
    endMinuteChoice->SetSelection(1);
    endSizer->Add(endMinuteChoice, 0, wxRIGHT, 5);

    endAmPmChoice = new wxChoice(scroll, wxID_ANY);
    endAmPmChoice->Append("AM");
    endAmPmChoice->Append("PM");
    endAmPmChoice->SetSelection(0);
    endSizer->Add(endAmPmChoice, 0);
    timeForm->Add(endSizer, 1, wxEXPAND);

    timeForm->AddGrowableCol(1, 1);
    timeForm->AddGrowableCol(3, 1);
    timeSlotBuilderSizer->Add(timeForm, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* timeButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    addTimeSlotButton = new wxButton(scroll, wxID_ANY, "Add Timeslot to Current Offering");
    removeTimeSlotButton = new wxButton(scroll, wxID_ANY, "Remove Selected Timeslot");
    timeButtonsSizer->Add(addTimeSlotButton, 0, wxRIGHT, 10);
    timeButtonsSizer->Add(removeTimeSlotButton, 0);
    timeSlotBuilderSizer->Add(timeButtonsSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

    currentTimeSlotsList = new wxListBox(scroll, wxID_ANY, wxDefaultPosition, wxSize(-1, 100));
    timeSlotBuilderSizer->Add(currentTimeSlotsList, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    offeringBuilderSizer->Add(timeSlotBuilderSizer, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* offeringButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    addOfferingButton = new wxButton(scroll, wxID_ANY, "Add Offering to Course");
    removeOfferingButton = new wxButton(scroll, wxID_ANY, "Remove Selected Offering");
    offeringButtonsSizer->Add(addOfferingButton, 0, wxRIGHT, 10);
    offeringButtonsSizer->Add(removeOfferingButton, 0);
    offeringBuilderSizer->Add(offeringButtonsSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

    offeringsList = new wxListBox(scroll, wxID_ANY, wxDefaultPosition, wxSize(-1, 110));
    offeringBuilderSizer->Add(offeringsList, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    addCourseSizer->Add(offeringBuilderSizer, 0, wxEXPAND | wxALL, 5);


    addCourseButton = new wxButton(scroll, wxID_ANY, "Add Course");
    addCourseSizer->Add(addCourseButton, 0, wxALL, 5);
    mainSizer->Add(addCourseSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    detailsLabel = new wxStaticText(scroll, wxID_ANY, "Course Details");
    mainSizer->Add(detailsLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    detailsText = new wxTextCtrl(scroll, wxID_ANY, "Select a course to view its details.", wxDefaultPosition, wxSize(-1, 220), wxTE_MULTILINE | wxTE_READONLY);
    mainSizer->Add(detailsText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    
    scroll->SetSizer(mainSizer);
    scroll->FitInside();

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(scroll, 1, wxEXPAND);
    SetSizer(panelSizer);

    addCourseButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnAddCourse, this);
    loadButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnLoadCatalog, this);
    saveCatalogButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnSaveCatalog, this);
    courseList->Bind(wxEVT_LIST_ITEM_SELECTED, &CourseCatalogPanel::OnCourseSelected, this);

    addTimeSlotButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnAddTimeSlot, this);
    removeTimeSlotButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnRemoveTimeSlot, this);
    addOfferingButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnAddOffering, this);
    removeOfferingButton->Bind(wxEVT_BUTTON, &CourseCatalogPanel::OnRemoveOffering, this);

    if(!catalog->getAllCourses().empty()){
        RefreshCourseList();
        ShowCourseDetails(catalog->getAllCourses()[0].get());
        courseList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        statusText->SetLabel("Catalog loaded successfully.");
    }
}

/**
 * @brief Loads catalog from the path in filePathInput; updates status and list on success or failure.
 */
void CourseCatalogPanel::OnLoadCatalog(wxCommandEvent& event) {
    std::string filePath = filePathInput->GetValue().ToStdString();
    catalog->clear();
    pendingOfferings.clear();
    currentOfferingTimeSlots.clear();
    RefreshPendingOfferingsList();
    RefreshCurrentTimeSlotsList();
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


void CourseCatalogPanel::OnSaveCatalog(wxCommandEvent& event) {
    std::string filePath = filePathInput->GetValue().ToStdString();
    if (filePath.empty()) {
        statusText->SetLabel("Please enter a file path before saving.");
        return;
    }

    if (CourseCatalogSaver::saveToFile(filePath, *catalog)) {
        statusText->SetLabel("Catalog saved successfully.");
    } else {
        statusText->SetLabel("Failed to save catalog.");
    }
}

void CourseCatalogPanel::OnAddTimeSlot(wxCommandEvent&) {
    const wxString selectedDay = timeSlotDayChoice->GetStringSelection();
    if (selectedDay.IsEmpty()) {
        statusText->SetLabel("Please select a day for the timeslot.");
        return;
    }

    const int startMinutes = ConvertToMinutesFromMidnight(
        startHourSpinCtrl->GetValue(),
        wxAtoi(startMinuteChoice->GetStringSelection()),
        startAmPmChoice->GetStringSelection()
    );

    const int endMinutes = ConvertToMinutesFromMidnight(
        endHourSpinCtrl->GetValue(),
        wxAtoi(endMinuteChoice->GetStringSelection()),
        endAmPmChoice->GetStringSelection()
    );

    if (endMinutes <= startMinutes) {
        statusText->SetLabel("Timeslot end time must be later than start time.");
        return;
    }

    TimeSlot newSlot(selectedDay.ToStdString(), startMinutes, endMinutes);

    for (const TimeSlot& existing : currentOfferingTimeSlots) {
        if (existing.getDay() == newSlot.getDay() &&
            existing.getStartTime() == newSlot.getStartTime() &&
            existing.getEndTime() == newSlot.getEndTime()) {
            statusText->SetLabel("That timeslot is already in the current offering.");
            return;
        }
    }

    currentOfferingTimeSlots.push_back(newSlot);
    RefreshCurrentTimeSlotsList();
    statusText->SetLabel("Timeslot added to current offering.");
}

void CourseCatalogPanel::OnRemoveTimeSlot(wxCommandEvent&) {
    int selectedIndex = currentTimeSlotsList->GetSelection();
    if (selectedIndex == wxNOT_FOUND) {
        statusText->SetLabel("Please select a timeslot to remove.");
        return;
    }

    if (selectedIndex < 0 || static_cast<std::size_t>(selectedIndex) >= currentOfferingTimeSlots.size()) {
        statusText->SetLabel("Invalid timeslot selection.");
        return;
    }

    currentOfferingTimeSlots.erase(currentOfferingTimeSlots.begin() + selectedIndex);
    RefreshCurrentTimeSlotsList();
    statusText->SetLabel("Timeslot removed.");
}

void CourseCatalogPanel::OnAddOffering(wxCommandEvent&) {
    if (currentOfferingTimeSlots.empty()) {
        statusText->SetLabel("Please add at least one timeslot before adding an offering.");
        return;
    }

    Season season;
    if (!TryParseSeason(offeringSeasonChoice->GetStringSelection(), season)) {
        statusText->SetLabel("Please select a valid season.");
        return;
    }

    const int year = offeringYearSpinCtrl->GetValue();
    CourseOffering offering(Term(season, year), currentOfferingTimeSlots);

    for (const CourseOffering& existingOffering : pendingOfferings) {
        if (!existingOffering.getTerm().equal(offering.getTerm())) {
            continue;
        }

        const std::vector<TimeSlot>& existingSlots = existingOffering.getTimeSlots();
        const std::vector<TimeSlot>& newSlots = offering.getTimeSlots();

        if (existingSlots.size() != newSlots.size()) {
            continue;
        }

        bool identical = true;
        for (std::size_t i = 0; i < existingSlots.size(); i++) {
            if (existingSlots[i].getDay() != newSlots[i].getDay() ||
                existingSlots[i].getStartTime() != newSlots[i].getStartTime() ||
                existingSlots[i].getEndTime() != newSlots[i].getEndTime()) {
                identical = false;
                break;
            }
        }

        if (identical) {
            statusText->SetLabel("That offering is already in the pending list.");
            return;
        }
    }

    pendingOfferings.push_back(offering);
    currentOfferingTimeSlots.clear();

    RefreshPendingOfferingsList();
    RefreshCurrentTimeSlotsList();

    statusText->SetLabel("Offering added to course.");
}

void CourseCatalogPanel::OnRemoveOffering(wxCommandEvent&) {
    int selectedIndex = offeringsList->GetSelection();
    if (selectedIndex == wxNOT_FOUND) {
        statusText->SetLabel("Please select an offering to remove.");
        return;
    }

    if (selectedIndex < 0 || static_cast<std::size_t>(selectedIndex) >= pendingOfferings.size()) {
        statusText->SetLabel("Invalid offering selection.");
        return;
    }

    pendingOfferings.erase(pendingOfferings.begin() + selectedIndex);
    RefreshPendingOfferingsList();
    statusText->SetLabel("Offering removed.");
}


void CourseCatalogPanel::OnAddCourse(wxCommandEvent& event) {
    std::string code = trim(newCourseCodeInput->GetValue().ToStdString());
    std::string title = trim(newCourseTitleInput->GetValue().ToStdString());
    std::string creditsText = trim(newCourseCreditsInput->GetValue().ToStdString());
    std::string breadth = trim(newCourseBreadthInput->GetValue().ToStdString());
    std::string simplePrereqText = newCourseSimplePrereqInput->GetValue().ToStdString();
    std::string exclusionsText = newCourseExclusionsInput->GetValue().ToStdString();


    if (code.empty() || title.empty() || creditsText.empty()) {
        statusText->SetLabel("Please enter code, title, and credits.");
        return;
    }

    double credits = 0.0;
    try {
        credits = std::stod(creditsText);
    } catch (...) {
        statusText->SetLabel("Credits must be a valid number.");
        return;
    }

    if (credits <= 0.0) {
        statusText->SetLabel("Credits must be greater than zero.");
        return;
    }

    if (catalog->containsCourse(code)) {
        statusText->SetLabel("A course with that code already exists.");
        return;
    }

    std::unique_ptr<Course> newCourse = std::make_unique<Course>(code, title, credits);
    if (!breadth.empty()) {
        newCourse->setBreadthCategory(breadth);
    }

    if (!catalog->addCourse(std::move(newCourse))) {
        statusText->SetLabel("Failed to add course.");
        return;
    }
    newCourse->setElective(newCourseElectiveCheckBox->GetValue());

    RefreshCourseList();

    Course* addedCourse = catalog->getCourse(code);
    if (addedCourse == nullptr) {
        statusText->SetLabel("Course was added, but could not be retrieved for linking.");
        return;
    }

    for (const std::string& exclusionCode : splitCommaSeparatedList(exclusionsText)) {
        Course* exclusionCourse = catalog->getCourse(exclusionCode);
        if (exclusionCourse == nullptr) {
            statusText->SetLabel("Unknown exclusion course: " + exclusionCode);
            return;
        }
        addedCourse->addExclusion(exclusionCourse);
    }

    for (const CourseOffering& offering : pendingOfferings) {
        addedCourse->addOffering(offering);
    }

    RefreshCourseList();

    long newRow = static_cast<long>(catalog->getAllCourses().size() - 1);
    if (newRow >= 0) {
        courseList->SetItemState(newRow, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        ShowCourseDetails(catalog->getAllCourses()[static_cast<std::size_t>(newRow)].get());
    }

    statusText->SetLabel("Course added to catalog. Click Save Catalog to persist the change.");
}

/**
 * @brief Shows details for the course at the selected row index.
 */
void CourseCatalogPanel::OnCourseSelected(wxListEvent& event) {
    long selectedRow = event.GetIndex();
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    if (selectedRow < 0 || static_cast<std::size_t>(selectedRow) >= courses.size()) {
        detailsText->SetValue("No course details available.");
        return;
    }
    ShowCourseDetails(courses[static_cast<std::size_t>(selectedRow)].get());
}

/**
 * @brief Builds a multi-line description: prereqs, exclusions, offerings and time slots.
 */
void CourseCatalogPanel::ShowCourseDetails(const Course* course) {
    if(course == nullptr) {
        detailsText->SetValue("No course details available.");
        return;
    }
    std::ostringstream details;
    details << "Code: " << course->getCode() << "\n";
    details << "Title: " << course->getTitle() << "\n";
    details << "Credits: " << course->getCredits() << "\n";
    details << "Breadth: " << course->getBreadthCategory() << "\n";
    details << "Elective: " << (course->isElective() ? "Yes" : "No") << "\n\n";
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
                            << formatTime24(timeSlot.getStartTime())
                            << " - "
                            << formatTime24(timeSlot.getEndTime())
                            << "\n";
                }
            }
        }
    }
    detailsText->SetValue(details.str());
}

/**
 * @brief Clears and repopulates the list control from catalog.
 */
void CourseCatalogPanel::RefreshCourseList() {
    courseList->DeleteAllItems();
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    for (std::size_t i = 0; i < courses.size(); i++) {
        const Course* course = courses[i].get();
        long row = courseList->InsertItem(static_cast<long>(i), course->getCode());
        courseList->SetItem(row, 1, course->getTitle());
        courseList->SetItem(row, 2, wxString::Format("%.1f", course->getCredits()));
        courseList->SetItem(row, 3, course->getBreadthCategory());
        courseList->SetItem(row, 4, course->isElective() ? "Yes" : "No");
    }
}

void CourseCatalogPanel::RefreshCurrentTimeSlotsList() {
    currentTimeSlotsList->Clear();

    for (const TimeSlot& slot : currentOfferingTimeSlots) {
        currentTimeSlotsList->Append(FormatTimeSlotForDisplay(slot));
    }
}

void CourseCatalogPanel::RefreshPendingOfferingsList() {
    offeringsList->Clear();

    for (const CourseOffering& offering : pendingOfferings) {
        offeringsList->Append(FormatOfferingForDisplay(offering));
    }
}

void CourseCatalogPanel::ClearCourseEntryForm() {
    newCourseCodeInput->Clear();
    newCourseTitleInput->Clear();
    newCourseCreditsInput->SetValue("0.5");
    newCourseBreadthInput->Clear();
    newCourseElectiveCheckBox->SetValue(false);

    newCourseSimplePrereqInput->Clear();
    newCourseExclusionsInput->Clear();

    offeringSeasonChoice->SetSelection(0);
    offeringYearSpinCtrl->SetValue(2026);

    timeSlotDayChoice->SetSelection(0);
    startHourSpinCtrl->SetValue(9);
    startMinuteChoice->SetSelection(0);
    startAmPmChoice->SetSelection(0);
    endHourSpinCtrl->SetValue(10);
    endMinuteChoice->SetSelection(1);
    endAmPmChoice->SetSelection(0);

    currentOfferingTimeSlots.clear();
    pendingOfferings.clear();
    RefreshCurrentTimeSlotsList();
    RefreshPendingOfferingsList();
}

int CourseCatalogPanel::ConvertToMinutesFromMidnight(int hour12, int minute, const wxString& amPm) const {
    int hour24 = hour12 % 12;
    if (amPm == "PM") {
        hour24 += 12;
    }
    return hour24 * 60 + minute;
}

bool CourseCatalogPanel::TryParseSeason(const wxString& text, Season& season) const {
    if (text == "Fall") {
        season = Season::Fall;
        return true;
    }
    if (text == "Winter") {
        season = Season::Winter;
        return true;
    }
    if (text == "Summer") {
        season = Season::Summer;
        return true;
    }
    return false;
}

wxString CourseCatalogPanel::FormatTime12Hour(int minutesFromMidnight) const {
    int hour24 = minutesFromMidnight / 60;
    int minute = minutesFromMidnight % 60;

    int hour12 = hour24 % 12;
    if (hour12 == 0) {
        hour12 = 12;
    }

    wxString amPm = (hour24 < 12) ? "AM" : "PM";
    return wxString::Format("%d:%02d %s", hour12, minute, amPm);
}

wxString CourseCatalogPanel::FormatTimeSlotForDisplay(const TimeSlot& slot) const {
    return wxString::FromUTF8(slot.getDay()) + " "
         + FormatTime12Hour(slot.getStartTime())
         + " - "
         + FormatTime12Hour(slot.getEndTime());
}

wxString CourseCatalogPanel::FormatOfferingForDisplay(const CourseOffering& offering) const {
    wxString text = wxString::FromUTF8(offering.getTerm().toString()) + " — ";

    const std::vector<TimeSlot>& slots = offering.getTimeSlots();
    for (std::size_t i = 0; i < slots.size(); i++) {
        if (i > 0) {
            text += "; ";
        }
        text += FormatTimeSlotForDisplay(slots[i]);
    }

    return text;
}