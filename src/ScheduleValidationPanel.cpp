#include "ScheduleValidationPanel.h"
#include "AcademicData.h"
#include "CourseCatalogLoader.h"
#include "PlannedScheduleStorage.h"
#include "ScheduleGenerationEngine.h"
#include "StudentProfile.h"
#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <wx/filename.h>
#include <wx/spinctrl.h>
#include <wx/stdpaths.h>

/**
 * @brief Constructs the validation panel UI and bind event handlers.
 *
 * @param parent Parent wxWidgets window.
 */
ScheduleValidationPanel::ScheduleValidationPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    wxScrolledWindow* scroll = new wxScrolledWindow(this, wxID_ANY);
    scroll->SetScrollRate(0, 20);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* title = new wxStaticText(scroll, wxID_ANY, "Schedule Validation");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    title->SetFont(titleFont);
    mainSizer->Add(title, 0, wxALL, 10);

    // Term selection
    wxBoxSizer* termSizer = new wxBoxSizer(wxHORIZONTAL);

    termSizer->Add(new wxStaticText(scroll, wxID_ANY, "Season:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    seasonCombo = new wxComboBox(
        scroll,
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

    termSizer->Add(new wxStaticText(scroll, wxID_ANY, "Year:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    yearInput = new wxTextCtrl(scroll, wxID_ANY, "2026");
    termSizer->Add(yearInput, 0);

    mainSizer->Add(termSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxBoxSizer* preferencesSizer = new wxBoxSizer(wxHORIZONTAL);
    preferMorningCheckBox = new wxCheckBox(scroll, wxID_ANY, "Prefer Morning Classes");
    preferCompactDaysCheckBox = new wxCheckBox(scroll, wxID_ANY, "Prefer Compact Days");
    morningWeightSpinCtrl = new wxSpinCtrl(scroll, wxID_ANY);
    morningWeightSpinCtrl->SetRange(0, 100);
    morningWeightSpinCtrl->SetValue(50);
    compactDaysWeightSpinCtrl = new wxSpinCtrl(scroll, wxID_ANY);
    compactDaysWeightSpinCtrl->SetRange(0, 100);
    compactDaysWeightSpinCtrl->SetValue(50);
    enforceEarliestStartCheckBox = new wxCheckBox(scroll, wxID_ANY, "Enforce no classes before");
    earliestStartHourSpinCtrl = new wxSpinCtrl(scroll, wxID_ANY);
    earliestStartHourSpinCtrl->SetRange(0, 23);
    earliestStartHourSpinCtrl->SetValue(9);
    includeSavedFuturePlansInPathwayCheckBox = new wxCheckBox(
        scroll,
        wxID_ANY,
        "Include saved future planned terms in pathway context"
    );
    includeSavedFuturePlansInPathwayCheckBox->SetValue(false);
    preferencesSizer->Add(preferMorningCheckBox, 0, wxRIGHT, 15);
    preferencesSizer->Add(new wxStaticText(scroll, wxID_ANY, "Morning %:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
    preferencesSizer->Add(morningWeightSpinCtrl, 0, wxRIGHT, 15);
    preferencesSizer->Add(preferCompactDaysCheckBox, 0, wxRIGHT, 15);
    preferencesSizer->Add(new wxStaticText(scroll, wxID_ANY, "Compact %:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
    preferencesSizer->Add(compactDaysWeightSpinCtrl, 0, wxRIGHT, 15);
    preferencesSizer->Add(enforceEarliestStartCheckBox, 0, wxRIGHT, 5);
    preferencesSizer->Add(earliestStartHourSpinCtrl, 0, wxRIGHT, 15);
    preferencesSizer->Add(includeSavedFuturePlansInPathwayCheckBox, 0);
    mainSizer->Add(preferencesSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // Courses list
    mainSizer->Add(
        new wxStaticText(scroll, wxID_ANY, "Select planned courses:"),
        0,
        wxLEFT | wxRIGHT | wxTOP,
        10
    );

    coursesCheckList = new wxCheckListBox(
        scroll,
        wxID_ANY,
        wxDefaultPosition,
        wxSize(400,200)
    );

    coursesCheckList->SetMinSize(wxSize(-1, 160));
    mainSizer->Add(coursesCheckList, 1, wxEXPAND | wxALL, 10);

    // Validate/Clear buttons
    wxBoxSizer* actionsSizer = new wxBoxSizer(wxHORIZONTAL);
    validateButton = new wxButton(scroll, wxID_ANY, "Validate Schedule");
    clearPlannedScheduleButton = new wxButton(scroll, wxID_ANY, "Clear Saved Planned Schedule");
    actionsSizer->Add(validateButton, 0, wxRIGHT, 10);
    actionsSizer->Add(clearPlannedScheduleButton, 0);
    mainSizer->Add(actionsSizer, 0, wxALL, 10);

    // Results output
    resultsText = new wxTextCtrl(
        scroll,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxSize(-1,150),
        wxTE_MULTILINE | wxTE_READONLY
    );
    resultsText->SetMinSize(wxSize(-1, 120));
    mainSizer->Add(resultsText, 1, wxEXPAND | wxALL, 10);

    wxBoxSizer* generatedOptionSizer = new wxBoxSizer(wxHORIZONTAL);
    generatedOptionSizer->Add(new wxStaticText(scroll, wxID_ANY, "Generated Option:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
    generatedOptionsChoice = new wxChoice(scroll, wxID_ANY);
    generatedOptionsChoice->Enable(false);
    generatedOptionSizer->Add(generatedOptionsChoice, 0);
    mainSizer->Add(generatedOptionSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    generatedOptionDetailsTextCtrl = new wxTextCtrl(
        scroll,
        wxID_ANY,
        "No generated option selected.",
        wxDefaultPosition,
        wxSize(-1, 100),
        wxTE_MULTILINE | wxTE_READONLY
    );
    generatedOptionDetailsTextCtrl->SetMinSize(wxSize(-1, 100));
    mainSizer->Add(generatedOptionDetailsTextCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    mainSizer->Add(new wxStaticText(scroll, wxID_ANY, "Saved Earlier-Term Plans"), 0, wxLEFT | wxRIGHT | wxTOP, 10);
    savedEarlierPlansTextCtrl = new wxTextCtrl(
        scroll,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxSize(-1, 140),
        wxTE_MULTILINE | wxTE_READONLY
    );
    savedEarlierPlansTextCtrl->SetMinSize(wxSize(-1, 120));
    mainSizer->Add(savedEarlierPlansTextCtrl, 0, wxEXPAND | wxALL, 10);

    scroll->SetSizer(mainSizer);
    scroll->FitInside();

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(scroll, 1, wxEXPAND);
    SetSizer(panelSizer);

    InitPlannedSchedulePath();
    LoadPlannedSchedule();
    LoadCatalog();
    PopulateCoursesList();

    auto ApplyCurrentTermPlannedSelection = [this]() {
        wxString season = seasonCombo->GetValue();
        wxString yearStr = yearInput->GetValue();
        long yearLong;
        if (yearStr.IsEmpty() || !yearStr.ToLong(&yearLong)) {
            RefreshSavedEarlierPlansDisplay();
            return;
        }

        Season selectedSeason;
        if (season == "Fall") selectedSeason = Season::Fall;
        else if (season == "Winter") selectedSeason = Season::Winter;
        else if (season == "Summer") selectedSeason = Season::Summer;
        else {
            RefreshSavedEarlierPlansDisplay();
            return;
        }

        for (unsigned int i = 0; i < coursesCheckList->GetCount(); i++) {
            coursesCheckList->Check(i, false);
        }

        Term currentTerm(selectedSeason, static_cast<int>(yearLong));
        const std::vector<std::string> savedCourseIds = plannedSchedule.getCoursesForTerm(currentTerm);
        const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();

        for (unsigned int i = 0; i < coursesCheckList->GetCount() && i < courses.size(); i++) {
            const Course* course = courses[i].get();
            if (course == nullptr) {
                continue;
            }
            for (const std::string& savedId : savedCourseIds) {
                if (savedId == course->getCode()) {
                    coursesCheckList->Check(i, true);
                    break;
                }
            }
        }
        RefreshSavedEarlierPlansDisplay();
    };
    ApplyCurrentTermPlannedSelection();
    RefreshSavedEarlierPlansDisplay();

    validateButton->Bind(wxEVT_BUTTON, &ScheduleValidationPanel::OnValidate, this);
    clearPlannedScheduleButton->Bind(wxEVT_BUTTON, &ScheduleValidationPanel::OnClearPlannedSchedule, this);
    seasonCombo->Bind(wxEVT_COMBOBOX, [ApplyCurrentTermPlannedSelection](wxCommandEvent&) {
        ApplyCurrentTermPlannedSelection();
    });
    yearInput->Bind(wxEVT_TEXT, [ApplyCurrentTermPlannedSelection](wxCommandEvent&) {
        ApplyCurrentTermPlannedSelection();
    });
    generatedOptionsChoice->Bind(wxEVT_CHOICE, &ScheduleValidationPanel::OnGeneratedOptionChanged, this);
}

/**
 * @brief Load course catalog data from known relative paths.
 */
void ScheduleValidationPanel::LoadCatalog() {
    std::string path = "data/courses.txt";

    if (!CourseCatalogLoader::loadFromFile(path, catalog)) {
        path = "../data/courses.txt";
        CourseCatalogLoader::loadFromFile(path, catalog);
    }
}

/**
 * @brief Initialize persistent path used for planned schedule storage.
 */
void ScheduleValidationPanel::InitPlannedSchedulePath() {
    wxString userDataDir = wxStandardPaths::Get().GetUserDataDir();
    if (!wxFileName::DirExists(userDataDir)) {
        wxFileName::Mkdir(userDataDir, 0755, wxPATH_MKDIR_FULL);
    }
    plannedSchedulePath = wxFileName(userDataDir, "planned_schedule.txt").GetFullPath();
}

/**
 * @brief Load planned schedule state from persistent storage.
 */
void ScheduleValidationPanel::LoadPlannedSchedule() {
    plannedSchedule.clear();
    if (!PlannedScheduleStorage::loadFromFile(plannedSchedulePath.ToStdString(), plannedSchedule)) {
        wxLogWarning("Failed to load planned schedule from %s", plannedSchedulePath);
    }
}

/**
 * @brief Save in-memory planned schedule state to disk.
 *
 * @return True if saving succeeds.
 */
bool ScheduleValidationPanel::SavePlannedSchedule() {
    return PlannedScheduleStorage::saveToFile(
        plannedSchedulePath.ToStdString(),
        plannedSchedule
    );
}

/**
 * @brief Populate generated-option selector labels from current results.
 */
void ScheduleValidationPanel::PopulateGeneratedOptionsChoice() {
    if (generatedOptionsChoice == nullptr) {
        return;
    }

    generatedOptionsChoice->Clear();
    const char* labels[] = {"Option A", "Option B", "Option C"};
    std::size_t count = lastGeneratedOptions.size();
    if (count > 3) {
        count = 3;
    }
    for (std::size_t i = 0; i < count; i++) {
        generatedOptionsChoice->Append(labels[i]);
    }

    if (generatedOptionsChoice->GetCount() > 0) {
        generatedOptionsChoice->SetSelection(0);
        generatedOptionsChoice->Enable(true);
    } else {
        generatedOptionsChoice->Enable(false);
    }
}

/**
 * @brief Render details for the selected generated option.
 */
void ScheduleValidationPanel::RefreshGeneratedOptionDisplay() {
    if (generatedOptionDetailsTextCtrl == nullptr) {
        return;
    }
    if (lastGeneratedOptions.empty()) {
        generatedOptionDetailsTextCtrl->SetValue("No generated option selected.");
        return;
    }

    int selection = generatedOptionsChoice ? generatedOptionsChoice->GetSelection() : wxNOT_FOUND;
    if (selection == wxNOT_FOUND || selection < 0 || static_cast<std::size_t>(selection) >= lastGeneratedOptions.size()) {
        selection = 0;
    }

    const GeneratedTermScheduleResult& option = lastGeneratedOptions[static_cast<std::size_t>(selection)];
    std::ostringstream out;
    out << "Option " << static_cast<char>('A' + selection) << ":\n";
    for (const std::string& id : option.selectedCourseIds) {
        out << id << "\n";
    }
    if (!option.errors.empty()) {
        out << "\nWarnings:\n";
        for (const std::string& error : option.errors) {
            out << "- " << error << "\n";
        }
    }

    generatedOptionDetailsTextCtrl->SetValue(wxString::FromUTF8(out.str()));
}

/**
 * @brief Handle generated-option selector changes.
 *
 * @param event Choice event.
 */
void ScheduleValidationPanel::OnGeneratedOptionChanged(wxCommandEvent&) {
    RefreshGeneratedOptionDisplay();
}

/**
 * @brief Clear persisted planned schedule data and refresh panel state.
 *
 * @param event Button event.
 */
void ScheduleValidationPanel::OnClearPlannedSchedule(wxCommandEvent&) {
    plannedSchedule.clear();
    if (!SavePlannedSchedule()) {
        if (resultsText != nullptr) {
            resultsText->AppendText("Warning: Could not clear saved planned schedule.\n");
        }
        return;
    }

    for (unsigned int i = 0; i < coursesCheckList->GetCount(); i++) {
        coursesCheckList->Check(i, false);
    }

    lastGeneratedOptions.clear();
    PopulateGeneratedOptionsChoice();
    RefreshGeneratedOptionDisplay();
    RefreshSavedEarlierPlansDisplay();

    if (resultsText != nullptr) {
        resultsText->AppendText("Saved planned schedule cleared.\n");
    }
}

/**
 * @brief Refresh the earlier-term saved-plan display for the selected term.
 */
void ScheduleValidationPanel::RefreshSavedEarlierPlansDisplay() {
    if (savedEarlierPlansTextCtrl == nullptr) {
        return;
    }

    auto seasonRank = [](Season season) {
        switch (season) {
            case Season::Winter: return 0;
            case Season::Summer: return 1;
            case Season::Fall: return 2;
        }
        return 0;
    };
    auto isEarlierThan = [&](const Term& a, const Term& b) {
        if (a.getYear() != b.getYear()) {
            return a.getYear() < b.getYear();
        }
        return seasonRank(a.getSeason()) < seasonRank(b.getSeason());
    };
    auto formatTime12Hour = [](int minutesFromMidnight) -> std::string {
        int hour24 = minutesFromMidnight / 60;
        int minute = minutesFromMidnight % 60;
        int hour12 = hour24 % 12;
        if (hour12 == 0) {
            hour12 = 12;
        }
        std::ostringstream out;
        out << hour12;
        if (minute != 0) {
            out << ":";
            if (minute < 10) {
                out << "0";
            }
            out << minute;
        }
        out << ((hour24 < 12) ? "AM" : "PM");
        return out.str();
    };

    wxString season = seasonCombo->GetValue();
    wxString yearStr = yearInput->GetValue();
    long yearLong;
    if (yearStr.IsEmpty() || !yearStr.ToLong(&yearLong)) {
        savedEarlierPlansTextCtrl->SetValue("No earlier saved plans.");
        return;
    }

    Season selectedSeason;
    if (season == "Fall") selectedSeason = Season::Fall;
    else if (season == "Winter") selectedSeason = Season::Winter;
    else if (season == "Summer") selectedSeason = Season::Summer;
    else {
        savedEarlierPlansTextCtrl->SetValue("No earlier saved plans.");
        return;
    }

    Term currentTerm(selectedSeason, static_cast<int>(yearLong));
    std::ostringstream out;
    bool anyEarlier = false;
    const std::vector<PlannedTerm>& plannedTerms = plannedSchedule.getAllPlannedTerms();
    for (const PlannedTerm& plannedTerm : plannedTerms) {
        if (!isEarlierThan(plannedTerm.term, currentTerm)) {
            continue;
        }
        anyEarlier = true;
        out << plannedTerm.term.toString() << ":\n";
        for (const std::string& courseId : plannedTerm.courseIds) {
            Course* course = catalog.getCourse(courseId);
            if (course == nullptr) {
                out << "  " << courseId << "\n";
                continue;
            }

            out << "  " << course->getCode() << "\n";
            bool foundOffering = false;
            for (const CourseOffering& offering : course->getOfferings()) {
                if (!offering.getTerm().equal(plannedTerm.term)) {
                    continue;
                }
                foundOffering = true;
                for (const TimeSlot& slot : offering.getTimeSlots()) {
                    out << "    " << slot.getDay() << " "
                        << formatTime12Hour(slot.getStartTime()) << "-"
                        << formatTime12Hour(slot.getEndTime()) << "\n";
                }
                break;
            }
            if (!foundOffering) {
                out << "    No time slots found.\n";
            }
        }
        out << "\n";
    }

    if (!anyEarlier) {
        savedEarlierPlansTextCtrl->SetValue("No earlier saved plans.");
        return;
    }
    savedEarlierPlansTextCtrl->SetValue(wxString::FromUTF8(out.str()));
}

/**
 * @brief Populate the course checklist from the loaded catalog.
 */
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

/**
 * @brief Validate current selection and display generation/pathway results.
 *
 * @param event Button event.
 */
void ScheduleValidationPanel::OnValidate(wxCommandEvent&) {

    resultsText->Clear();
    bool hasErrors = false;

    StudentProfile profile;
    wxString profilePath = wxFileName(wxStandardPaths::Get().GetUserDataDir(), "profile.txt").GetFullPath();
    bool profileLoaded = profile.LoadFromFile(profilePath.ToStdString());
    if (!profileLoaded) {
        resultsText->AppendText("Prerequisite checks were skipped because the student profile could not be loaded.\n");
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
        wxString season = seasonCombo->GetValue();
        wxString yearStr = yearInput->GetValue();
        long yearLong;
        Season selectedSeason;
        bool validYear = !yearStr.IsEmpty() && yearStr.ToLong(&yearLong);
        bool validSeason = false;
        if (season == "Fall") {
            selectedSeason = Season::Fall;
            validSeason = true;
        } else if (season == "Winter") {
            selectedSeason = Season::Winter;
            validSeason = true;
        } else if (season == "Summer") {
            selectedSeason = Season::Summer;
            validSeason = true;
        }
        if (validYear && validSeason) {
            plannedSchedule.setCoursesForTerm(Term(selectedSeason, static_cast<int>(yearLong)), {});
            if (!SavePlannedSchedule()) {
                resultsText->AppendText("Warning: Could not save current term plan.\n");
            }
            RefreshSavedEarlierPlansDisplay();
        }
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

    long yearLong;
    if (yearStr.IsEmpty() || !yearStr.ToLong(&yearLong)) {
        resultsText->AppendText("Error: Please enter a valid year.\n");
        return;
    }
    int year = static_cast<int>(yearLong);

    Season selectedSeason;

    if (season == "Fall") selectedSeason = Season::Fall;
    else if (season == "Winter") selectedSeason = Season::Winter;
    else if (season == "Summer") selectedSeason = Season::Summer;
    else {
        resultsText->AppendText("Error: Please select a season.\n");
        return;
    }

    auto SaveCurrentTermSelectionToPlan = [&]() {
        Term currentTerm(selectedSeason, year);
        std::vector<std::string> checkedCourseIds;
        checkedCourseIds.reserve(selectedCourses.size());
        for (const Course* course : selectedCourses) {
            if (course != nullptr) {
                checkedCourseIds.push_back(course->getCode());
            }
        }
        plannedSchedule.setCoursesForTerm(currentTerm, checkedCourseIds);
        if (!SavePlannedSchedule()) {
            resultsText->AppendText("Warning: Could not save current term plan.\n");
        }
    };

    bool offeringErrors = false;
    std::vector<std::string> normalizedCompletedCourseIds;
    normalizedCompletedCourseIds.reserve(profile.completedCourseIds.size());
    std::vector<std::string> normalizedPrereqContextCourseIds;
    auto normalizeCourseId = [](const std::string& id) {
        std::size_t start = 0;
        while (start < id.size() && std::isspace(static_cast<unsigned char>(id[start]))) {
            start++;
        }
        std::size_t end = id.size();
        while (end > start && std::isspace(static_cast<unsigned char>(id[end - 1]))) {
            end--;
        }
        std::string normalized = id.substr(start, end - start);
        for (char& ch : normalized) {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
        return normalized;
    };
    for (const std::string& completedId : profile.completedCourseIds) {
        normalizedCompletedCourseIds.push_back(normalizeCourseId(completedId));
    }
    normalizedPrereqContextCourseIds = normalizedCompletedCourseIds;
    Term currentValidationTerm(selectedSeason, year);
    const std::vector<std::string> earlierPlannedCourseIds = plannedSchedule.getCoursesBeforeTerm(currentValidationTerm);
    for (const std::string& plannedId : earlierPlannedCourseIds) {
        normalizedPrereqContextCourseIds.push_back(normalizeCourseId(plannedId));
    }
    std::vector<std::string> candidateCourseIds;
    candidateCourseIds.reserve(selectedCourses.size());
    for (const Course* course : selectedCourses) {
        if (course != nullptr) {
            candidateCourseIds.push_back(course->getCode());
        }
    }
    {
        const std::vector<std::string> missingRequiredIds = profile.GetMissingGraduationRequirements();
        std::set<std::string> missingRequiredSet(missingRequiredIds.begin(), missingRequiredIds.end());
        std::vector<std::string> reorderedCandidates;
        reorderedCandidates.reserve(candidateCourseIds.size());

        for (const std::string& id : candidateCourseIds) {
            if (missingRequiredSet.find(id) != missingRequiredSet.end()) {
                reorderedCandidates.push_back(id);
            }
        }
        for (const std::string& id : candidateCourseIds) {
            if (missingRequiredSet.find(id) == missingRequiredSet.end()) {
                reorderedCandidates.push_back(id);
            }
        }
        candidateCourseIds = reorderedCandidates;
    }
    ScheduleGenerationEngine generationEngine;
    SchedulePreferences preferences;
    preferences.morningWeight = static_cast<double>(morningWeightSpinCtrl->GetValue()) / 100.0;
    preferences.compactDayWeight = static_cast<double>(compactDaysWeightSpinCtrl->GetValue()) / 100.0;
    preferences.preferMorning = preferMorningCheckBox->GetValue() && preferences.morningWeight > 0.0;
    preferences.preferCompactDays = preferCompactDaysCheckBox->GetValue() && preferences.compactDayWeight > 0.0;
    preferences.enforceEarliestStart = enforceEarliestStartCheckBox->GetValue();
    preferences.earliestStartMinutes = earliestStartHourSpinCtrl->GetValue() * 60;
    std::vector<GeneratedTermScheduleResult> generatedOptions = generationEngine.GenerateMultipleValidTermSchedules(
        catalog,
        currentValidationTerm,
        candidateCourseIds,
        normalizedPrereqContextCourseIds,
        5,
        3,
        &preferences
    );
    lastGeneratedOptions = generatedOptions;
    lastGeneratedTerm = currentValidationTerm;
    if (lastGeneratedOptions.size() > 3) {
        lastGeneratedOptions.resize(3);
    }
    PopulateGeneratedOptionsChoice();
    RefreshGeneratedOptionDisplay();
    if (generatedOptions.empty()) {
        resultsText->AppendText("No valid generated schedule options were found.\n");
        if (preferences.enforceEarliestStart) {
            resultsText->AppendText(
                "No options satisfy the hard preference: no classes before " +
                wxString::Format("%d:00", earliestStartHourSpinCtrl->GetValue()) + ".\n"
            );
        }
    } else {
        for (std::size_t optionIndex = 0; optionIndex < generatedOptions.size(); optionIndex++) {
            const GeneratedTermScheduleResult& option = generatedOptions[optionIndex];
            std::string generatedList;
            for (std::size_t i = 0; i < option.selectedCourseIds.size(); i++) {
                if (i > 0) {
                    generatedList += ", ";
                }
                generatedList += option.selectedCourseIds[i];
            }
            resultsText->AppendText(
                "Option " + wxString::Format("%zu", optionIndex + 1) + ":\n" +
                wxString::FromUTF8(generatedList) + "\n\n"
            );
        }
    }
    std::set<std::string> generationWarnings;
    for (const GeneratedTermScheduleResult& option : generatedOptions) {
        for (const std::string& error : option.errors) {
            generationWarnings.insert(error);
        }
    }
    for (const std::string& warning : generationWarnings) {
        resultsText->AppendText("Generation warning: " + wxString::FromUTF8(warning) + "\n");
    }

    for (const Course* course : selectedCourses) {
        std::vector<std::string> missingPrereqs;
        if (profileLoaded) {
            std::string selectedCodeNormalized = normalizeCourseId(course->getCode());
            for (const std::string& completedId : normalizedCompletedCourseIds) {
                if (completedId == selectedCodeNormalized) {
                    hasErrors = true;
                    resultsText->AppendText(
                        "Error: You have already taken " +
                        wxString::FromUTF8(course->getCode()) +
                        ".\n"
                    );
                    break;
                }
            }
        }

        // prerequisite validation
        if (profileLoaded) {
            for (const auto& prereqRule : course->getPrerequisiteRules())
            {
                if (!prereqRule.isSatisfied(normalizedPrereqContextCourseIds))
                {
                    hasErrors = true;
                    missingPrereqs.push_back(prereqRule.toString());
                }
            }
        }
        if (!missingPrereqs.empty()) {
            std::string joined;
            for (std::size_t i = 0; i < missingPrereqs.size(); i++) {
                if (i > 0) {
                    joined += ", ";
                }
                joined += missingPrereqs[i];
            }
            resultsText->AppendText(
                "Error: " +
                wxString::FromUTF8(course->getCode()) +
                " is missing prerequisites " +
                wxString::FromUTF8(joined) +
                "\n"
            );
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
    auto formatTime12Hour = [](int minutesFromMidnight) -> wxString {
        int hour24 = minutesFromMidnight / 60;
        int minute = minutesFromMidnight % 60;
        int hour12 = hour24 % 12;
        if (hour12 == 0) {
            hour12 = 12;
        }
        wxString amPm = (hour24 < 12) ? "AM" : "PM";
        if (minute == 0) {
            return wxString::Format("%d%s", hour12, amPm);
        }
        return wxString::Format("%d:%02d%s", hour12, minute, amPm);
    };

    // time conflict validation
for (size_t i = 0; i < selectedCourses.size(); i++)
{
    for (size_t j = i + 1; j < selectedCourses.size(); j++)
    {
        const Course* c1 = selectedCourses[i];
        const Course* c2 = selectedCourses[j];
        std::vector<const CourseOffering*> termOfferings1;
        std::vector<const CourseOffering*> termOfferings2;
        for (const auto& off1 : c1->getOfferings()) {
            const Term& t1 = off1.getTerm();
            if (t1.getSeason() == selectedSeason && t1.getYear() == year) {
                termOfferings1.push_back(&off1);
            }
        }
        for (const auto& off2 : c2->getOfferings()) {
            const Term& t2 = off2.getTerm();
            if (t2.getSeason() == selectedSeason && t2.getYear() == year) {
                termOfferings2.push_back(&off2);
            }
        }
        if (termOfferings1.empty() || termOfferings2.empty()) {
            continue;
        }

        bool hasConflictFreeCombination = false;
        const TimeSlot* firstConflictSlot1 = nullptr;
        const TimeSlot* firstConflictSlot2 = nullptr;

        for (const CourseOffering* off1 : termOfferings1)
        {
            for (const CourseOffering* off2 : termOfferings2)
            {
                bool comboHasConflict = false;
                const std::vector<TimeSlot>& slots1 = off1->getTimeSlots();
                const std::vector<TimeSlot>& slots2 = off2->getTimeSlots();

                for (const TimeSlot& s1 : slots1)
                {
                    for (const TimeSlot& s2 : slots2)
                    {
                        if (s1.conflictsWith(s2))
                        {
                            comboHasConflict = true;
                            if (firstConflictSlot1 == nullptr) {
                                firstConflictSlot1 = &s1;
                                firstConflictSlot2 = &s2;
                            }
                        }
                    }
                }

                if (!comboHasConflict) {
                    hasConflictFreeCombination = true;
                    break;
                }
            }
            if (hasConflictFreeCombination) {
                break;
            }
        }

        if (!hasConflictFreeCombination) {
            hasErrors = true;
            wxString c1Code = wxString::FromUTF8(c1->getCode());
            wxString c2Code = wxString::FromUTF8(c2->getCode());

            if (firstConflictSlot1 != nullptr && firstConflictSlot2 != nullptr) {
                wxString slot1 = c1Code + " is on " + wxString::FromUTF8(firstConflictSlot1->getDay()) + " " +
                                 formatTime12Hour(firstConflictSlot1->getStartTime()) + "-" + formatTime12Hour(firstConflictSlot1->getEndTime());
                wxString slot2 = c2Code + " is on " + wxString::FromUTF8(firstConflictSlot2->getDay()) + " " +
                                 formatTime12Hour(firstConflictSlot2->getStartTime()) + "-" + formatTime12Hour(firstConflictSlot2->getEndTime());
                resultsText->AppendText(
                    "Error: " + c1Code + " and " + c2Code +
                    " have a time conflict because " + slot1 +
                    " and " + slot2 + ".\n"
                );
            } else {
                resultsText->AppendText(
                    "Error: Time conflict between " + c1Code + " and " + c2Code + "\n"
                );
            }
        }
    }
}
    if (!offeringErrors) {
        resultsText->AppendText("All selected courses are offered in the selected term.\n");
    }

    if (!hasErrors) {
        SaveCurrentTermSelectionToPlan();
        RefreshSavedEarlierPlansDisplay();
        resultsText->AppendText("Schedule passes validation.\n");
    }

    if (!profileLoaded || profile.specializationId.empty() ||
        Academic::FindSpecializationById(profile.specializationId) == nullptr) {
        resultsText->AppendText(
            "Pathway generation requires a valid saved student profile with a selected specialization.\n"
        );
    }

    std::size_t pathwayMaxTerms = 8;
    {
        int parsedYear = -1;
        const std::string yearText = profile.currentYear;
        for (std::size_t i = 0; i < yearText.size(); i++) {
            if (std::isdigit(static_cast<unsigned char>(yearText[i]))) {
                int value = 0;
                std::size_t j = i;
                while (j < yearText.size() && std::isdigit(static_cast<unsigned char>(yearText[j]))) {
                    value = value * 10 + (yearText[j] - '0');
                    j++;
                }
                parsedYear = value;
                break;
            }
        }

        if (parsedYear == 1) pathwayMaxTerms = 12;
        else if (parsedYear == 2) pathwayMaxTerms = 9;
        else if (parsedYear == 3) pathwayMaxTerms = 6;
        else if (parsedYear >= 4) pathwayMaxTerms = 4;
    }

    auto nextTerm = [](const Term& term) {
        const int year = term.getYear();
        switch (term.getSeason()) {
            case Season::Winter:
                return Term(Season::Summer, year);
            case Season::Summer:
                return Term(Season::Fall, year);
            case Season::Fall:
                return Term(Season::Winter, year + 1);
        }
        return Term(Season::Winter, year + 1);
    };

    auto seasonRank = [](Season season) {
        switch (season) {
            case Season::Winter: return 0;
            case Season::Summer: return 1;
            case Season::Fall: return 2;
        }
        return 0;
    };
    auto isEarlierThan = [&](const Term& a, const Term& b) {
        if (a.getYear() != b.getYear()) {
            return a.getYear() < b.getYear();
        }
        return seasonRank(a.getSeason()) < seasonRank(b.getSeason());
    };

    std::vector<PlannedTerm> lockedSavedTerms;
    for (const PlannedTerm& plannedTerm : plannedSchedule.getAllPlannedTerms()) {
        if (plannedTerm.courseIds.empty()) {
            continue;
        }
        if (!isEarlierThan(plannedTerm.term, currentValidationTerm)) {
            lockedSavedTerms.push_back(plannedTerm);
        }
    }
    std::sort(
        lockedSavedTerms.begin(),
        lockedSavedTerms.end(),
        [&](const PlannedTerm& a, const PlannedTerm& b) { return isEarlierThan(a.term, b.term); }
    );
    const bool hasLockedSavedTerms = !lockedSavedTerms.empty();

    const bool hasLockedFirstTerm = !hasLockedSavedTerms && !generatedOptions.empty();
    const std::vector<std::string> lockedFirstTermCourseIds = hasLockedFirstTerm
        ? generatedOptions[0].selectedCourseIds
        : std::vector<std::string>{};

    std::vector<std::string> pathwayStartingContext;
    std::set<std::string> pathwayContextSeen;
    auto appendUniqueContext = [&](const std::string& courseId) {
        const std::string normalized = normalizeCourseId(courseId);
        if (pathwayContextSeen.insert(normalized).second) {
            pathwayStartingContext.push_back(courseId);
        }
    };
    for (const std::string& id : profile.completedCourseIds) {
        appendUniqueContext(id);
    }
    for (const std::string& id : earlierPlannedCourseIds) {
        appendUniqueContext(id);
    }
    for (const PlannedTerm& plannedTerm : lockedSavedTerms) {
        for (const std::string& id : plannedTerm.courseIds) {
            appendUniqueContext(id);
        }
    }
    if (hasLockedFirstTerm) {
        for (const std::string& id : lockedFirstTermCourseIds) {
            appendUniqueContext(id);
        }
    }

    Term pathwayStartTerm = currentValidationTerm;
    if (hasLockedSavedTerms) {
        pathwayStartTerm = nextTerm(lockedSavedTerms.back().term);
    } else if (hasLockedFirstTerm) {
        pathwayStartTerm = nextTerm(currentValidationTerm);
    }

    GeneratedMultiTermPathwayResult pathway = generationEngine.GenerateBestEffortMultiTermPathway(
        catalog,
        profile,
        pathwayStartTerm,
        pathwayMaxTerms,
        5,
        &preferences,
        pathwayStartingContext
    );
    resultsText->AppendText("\nPathway:\n");
    resultsText->AppendText(
        "Note: This is a best-effort required-course completion pathway based on specialization required courses.\n"
        "Completion here includes completed profile courses, earlier saved planned terms, the locked first term (if any), and generated future terms.\n"
        "It is not a full institutional graduation guarantee unless full credit and elective rules are modeled.\n\n"
    );
    if (hasLockedSavedTerms) {
        resultsText->AppendText("Locked saved terms:\n");
        for (const PlannedTerm& plannedTerm : lockedSavedTerms) {
            std::string lockedCourseList;
            for (std::size_t i = 0; i < plannedTerm.courseIds.size(); i++) {
                if (i > 0) {
                    lockedCourseList += ", ";
                }
                lockedCourseList += plannedTerm.courseIds[i];
            }
            resultsText->AppendText(
                wxString::FromUTF8(plannedTerm.term.toString()) + ":\n" +
                wxString::FromUTF8(lockedCourseList) + "\n\n"
            );
        }
    } else if (hasLockedFirstTerm) {
        std::string lockedCourseList;
        for (std::size_t i = 0; i < lockedFirstTermCourseIds.size(); i++) {
            if (i > 0) {
                lockedCourseList += ", ";
            }
            lockedCourseList += lockedFirstTermCourseIds[i];
        }
        resultsText->AppendText(
            wxString::FromUTF8(currentValidationTerm.toString()) + " (Locked from Option 1):\n" +
            wxString::FromUTF8(lockedCourseList) + "\n\n"
        );
    }

    {
        const Academic::Specialization* spec = Academic::FindSpecializationById(profile.specializationId);
        if (spec != nullptr) {
            std::set<std::string> contextSatisfied;
            for (const std::string& id : pathwayStartingContext) {
                contextSatisfied.insert(normalizeCourseId(id));
            }

            std::string alreadySatisfiedRequired;
            for (const std::string& reqId : spec->requiredCourseIds) {
                if (contextSatisfied.find(normalizeCourseId(reqId)) != contextSatisfied.end()) {
                    if (!alreadySatisfiedRequired.empty()) {
                        alreadySatisfiedRequired += ", ";
                    }
                    alreadySatisfiedRequired += reqId;
                }
            }

            if (alreadySatisfiedRequired.empty()) {
                resultsText->AppendText("Already satisfied from completed/planned context:\nNone\n\n");
            } else {
                resultsText->AppendText(
                    "Already satisfied from completed/planned context:\n" +
                    wxString::FromUTF8(alreadySatisfiedRequired) + "\n\n"
                );
            }
        }
    }

    resultsText->AppendText("Generated future terms:\n");
    if (pathway.termPlans.empty()) {
        resultsText->AppendText("No future terms could be generated.\n");
    } else {
        for (const GeneratedPathwayTermPlan& termPlan : pathway.termPlans) {
            std::string courseList;
            for (std::size_t i = 0; i < termPlan.selectedCourseIds.size(); i++) {
                if (i > 0) {
                    courseList += ", ";
                }
                courseList += termPlan.selectedCourseIds[i];
            }
            resultsText->AppendText(
                wxString::FromUTF8(termPlan.term.toString()) + ":\n" +
                wxString::FromUTF8(courseList) + "\n\n"
            );
        }
    }
    if (!pathway.success) {
        resultsText->AppendText("Pathway incomplete.\n");
        bool hardPrefBlockedPathway = false;
        if (preferences.enforceEarliestStart) {
            for (const std::string& error : pathway.errors) {
                if (error.find("hard preference") != std::string::npos) {
                    hardPrefBlockedPathway = true;
                    break;
                }
            }
        }
        if (hardPrefBlockedPathway) {
            resultsText->AppendText(
                "Pathway blocked by hard preference: no classes before " +
                wxString::Format("%d:00", earliestStartHourSpinCtrl->GetValue()) + ".\n"
            );
        }
        if (!pathway.remainingRequiredCourseIds.empty()) {
            std::string remaining;
            for (std::size_t i = 0; i < pathway.remainingRequiredCourseIds.size(); i++) {
                if (i > 0) {
                    remaining += ", ";
                }
                remaining += pathway.remainingRequiredCourseIds[i];
            }
            resultsText->AppendText("Remaining required courses: " + wxString::FromUTF8(remaining) + "\n");
        }
    } else {
        resultsText->AppendText("Pathway satisfies required-course completion for the specialization requirement set.\n");
    }
    if (!pathway.errors.empty()) {
        std::vector<std::string> skippedTerms;
        std::vector<std::string> levelGatingNotes;
        std::vector<std::string> hardPreferenceNotes;
        std::set<std::string> otherNotes;

        for (const std::string& error : pathway.errors) {
            if (error.rfind("No valid progress in ", 0) == 0 && error.find("; term skipped.") != std::string::npos) {
                const std::size_t start = std::string("No valid progress in ").size();
                const std::size_t end = error.find("; term skipped.");
                if (end != std::string::npos && end > start) {
                    skippedTerms.push_back(error.substr(start, end - start));
                } else {
                    skippedTerms.push_back(error);
                }
            } else if (error.rfind("Level gating skipped ", 0) == 0) {
                levelGatingNotes.push_back(error);
            } else if (error.find("hard preference") != std::string::npos) {
                hardPreferenceNotes.push_back(error);
            } else {
                otherNotes.insert(error);
            }
        }

        resultsText->AppendText("Pathway notes summary:\n");
        if (!skippedTerms.empty()) {
            std::string skippedJoined;
            for (std::size_t i = 0; i < skippedTerms.size(); i++) {
                if (i > 0) {
                    skippedJoined += ", ";
                }
                skippedJoined += skippedTerms[i];
            }
            resultsText->AppendText(
                "Skipped terms (" + wxString::Format("%zu", skippedTerms.size()) + "): " +
                wxString::FromUTF8(skippedJoined) + "\n"
            );
        }
        if (!levelGatingNotes.empty()) {
            resultsText->AppendText(
                "Level gating: " + wxString::Format("%zu", levelGatingNotes.size()) +
                " occurrence(s). " + wxString::FromUTF8(levelGatingNotes.front()) + "\n"
            );
        }
        if (!hardPreferenceNotes.empty()) {
            std::set<std::string> uniqueHardPreferenceNotes(hardPreferenceNotes.begin(), hardPreferenceNotes.end());
            for (const std::string& note : uniqueHardPreferenceNotes) {
                resultsText->AppendText("Hard preference blocks: " + wxString::FromUTF8(note) + "\n");
            }
        }
        if (!otherNotes.empty()) {
            for (const std::string& note : otherNotes) {
                resultsText->AppendText("Other notes: " + wxString::FromUTF8(note) + "\n");
            }
        }
    }
}
