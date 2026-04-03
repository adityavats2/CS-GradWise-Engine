/**
 * @file StudentProfilePanel.cpp
 * @brief Implementation of StudentProfilePanel.
 */

#include "StudentProfilePanel.h"
#include "AcademicData.h"
#include "CourseCatalogLoader.h"
#include <algorithm>
#include <sstream>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/scrolwin.h>

/** @brief Ensures user data dir exists; sets profile path; builds UI, catalog, profile. */
StudentProfilePanel::StudentProfilePanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY) {
StudentProfilePanel::StudentProfilePanel(wxWindow* parent, CourseCatalog* catalog)
    : wxPanel(parent, wxID_ANY), catalog(catalog) {
    wxStandardPaths& stdPaths = wxStandardPaths::Get();
    wxString userData = stdPaths.GetUserDataDir();
    if (!wxFileName::DirExists(userData))
        wxFileName::Mkdir(userData, 0755, wxPATH_MKDIR_FULL);
    profilePath = wxFileName(userData, "profile.txt").GetFullPath();
    BuildUI();
    LoadCatalog();
    LoadProfile();
}

/** @brief Lays out controls inside a wxScrolledWindow and binds wx events. */
void StudentProfilePanel::BuildUI() {
    wxScrolledWindow* scroll = new wxScrolledWindow(this, wxID_ANY);
    scroll->SetScrollRate(0, 20);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* title = new wxStaticText(scroll, wxID_ANY, "Student Profile Input Page");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    title->SetFont(titleFont);
    mainSizer->Add(title, 0, wxALL, 10);

    wxFlexGridSizer* formSizer = new wxFlexGridSizer(2, 5, 5);
    formSizer->Add(new wxStaticText(scroll, wxID_ANY, "Current year:"), 0, wxALIGN_CENTER_VERTICAL);
    yearCombo = new wxComboBox(scroll, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0, nullptr, wxCB_READONLY);
    PopulateYearCombo();
    formSizer->Add(yearCombo, 1, wxEXPAND);
    formSizer->Add(new wxStaticText(scroll, wxID_ANY, "Specialization:"), 0, wxALIGN_CENTER_VERTICAL);
    specializationCombo = new wxComboBox(scroll, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0, nullptr, wxCB_READONLY);
    PopulateSpecializationCombo();
    formSizer->Add(specializationCombo, 1, wxEXPAND);
    mainSizer->Add(formSizer, 0, wxEXPAND | wxALL, 10);

    catalogStatusText = new wxStaticText(scroll, wxID_ANY, "Loading catalog...");
    mainSizer->Add(catalogStatusText, 0, wxLEFT | wxRIGHT | wxTOP, 10);

    mainSizer->Add(new wxStaticText(scroll, wxID_ANY, "Check completed courses and click Add, or double-click a course to add it:"), 0, wxLEFT | wxRIGHT | wxTOP, 10);
    coursesCheckList = new wxCheckListBox(scroll, wxID_ANY, wxDefaultPosition, wxSize(420, 220), 0, nullptr, wxLB_SINGLE | wxLB_ALWAYS_SB);
    coursesCheckList->SetMinSize(wxSize(420, 220));
    mainSizer->Add(coursesCheckList, 0, wxEXPAND | wxALL, 10);

    wxBoxSizer* addRemoveSizer = new wxBoxSizer(wxHORIZONTAL);
    addButton = new wxButton(scroll, wxID_ANY, "Add checked courses to completed");
    removeButton = new wxButton(scroll, wxID_ANY, "Remove selected from completed");
    addRemoveSizer->Add(addButton, 0, wxRIGHT, 10);
    addRemoveSizer->Add(removeButton, 0);
    mainSizer->Add(addRemoveSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    mainSizer->Add(new wxStaticText(scroll, wxID_ANY, "Your completed courses (click Save Profile to save):"), 0, wxLEFT | wxRIGHT | wxTOP, 10);
    lastCompletedListSelection = -1;
    completedCoursesList = new wxListBox(scroll, wxID_ANY, wxDefaultPosition, wxSize(420, 120), 0, nullptr, wxLB_SINGLE | wxLB_ALWAYS_SB);
    completedCoursesList->SetMinSize(wxSize(420, 120));
    mainSizer->Add(completedCoursesList, 0, wxEXPAND | wxALL, 10);

    selectionStatusText = new wxStaticText(scroll, wxID_ANY, "0 courses in completed list. Click Save Profile to save to your profile.");
    selectionStatusText->SetForegroundColour(wxColour(0, 100, 0));
    mainSizer->Add(selectionStatusText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    saveButton = new wxButton(scroll, wxID_ANY, "Save Profile");
    mainSizer->Add(saveButton, 0, wxALL, 10);

    missingLabel = new wxStaticText(scroll, wxID_ANY, "Missing graduation requirements (based on your specialization):");
    mainSizer->Add(missingLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    missingList = new wxListBox(scroll, wxID_ANY, wxDefaultPosition, wxSize(420, 100), 0, nullptr, wxLB_SINGLE | wxLB_ALWAYS_SB);
    missingList->SetMinSize(wxSize(420, 100));
    scienceBreadthLabel = new wxStaticText(scroll, wxID_ANY, "Science breadth: Missing");
    mainSizer->Add(scienceBreadthLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    humanitiesBreadthLabel = new wxStaticText(scroll, wxID_ANY, "Humanities breadth: Missing");
    mainSizer->Add(humanitiesBreadthLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    socialScienceBreadthLabel = new wxStaticText(scroll, wxID_ANY, "Social Science breadth: Missing");
    mainSizer->Add(socialScienceBreadthLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    mainSizer->Add(missingList, 0, wxEXPAND | wxALL, 10);

    scroll->SetSizer(mainSizer);
    scroll->FitInside();

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(scroll, 1, wxEXPAND);
    SetSizer(panelSizer);

    saveButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnSave, this);
    addButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnAddToCompleted, this);
    removeButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnRemoveFromCompleted, this);
    completedCoursesList->Bind(wxEVT_LISTBOX, &StudentProfilePanel::OnCompletedListSelected, this);
    completedCoursesList->Bind(wxEVT_LISTBOX_DCLICK, &StudentProfilePanel::OnCompletedCoursesDoubleClick, this);
    coursesCheckList->Bind(wxEVT_LISTBOX_DCLICK, &StudentProfilePanel::OnCoursesCheckListDoubleClick, this);
    yearCombo->Bind(wxEVT_COMBOBOX, &StudentProfilePanel::OnSelectionChanged, this);
    specializationCombo->Bind(wxEVT_COMBOBOX, &StudentProfilePanel::OnSelectionChanged, this);
}

/** @brief Tries executable-relative and cwd paths for data/courses.txt; fills checklist on success. */
void StudentProfilePanel::LoadCatalog() {
    if (catalog == nullptr) {
        catalogStatusText->SetLabel("Catalog pointer is null.");
        return;
    }

    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    if (courses.empty()) {
        catalogStatusText->SetLabel("Catalog not loaded.");
        return;
    }

    catalogStatusText->SetLabel(wxString::Format("Catalog loaded: %zu courses.", courses.size()));
    PopulateCoursesCheckList();
}

/** @brief Appends Academic::GetYearOptions() to yearCombo. */
void StudentProfilePanel::PopulateYearCombo() {
    const auto& years = Academic::GetYearOptions();
    for (const auto& y : years)
        yearCombo->Append(wxString::FromUTF8(y));
}

/** @brief Appends specializations (id + name) to specializationCombo. */
void StudentProfilePanel::PopulateSpecializationCombo() {
    const auto& specs = Academic::GetSpecializations();
    for (const auto& s : specs)
        specializationCombo->Append(wxString::FromUTF8(s.id + " - " + s.name));
}

/** @brief Rebuilds the checklist from catalog (code - title per row). */
void StudentProfilePanel::PopulateCoursesCheckList() {
    coursesCheckList->Clear();
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    for (const auto& ptr : courses) {
        const Course* c = ptr.get();
        if (c)
            coursesCheckList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
    }
}

/** @brief Rebuilds completed list box from profile.completedCourseIds. */
void StudentProfilePanel::RefreshCompletedCoursesList() {
    completedCoursesList->Clear();
    for (const std::string& code : profile.completedCourseIds) {
        Course* c = catalog->getCourse(code);
        if (c)
            completedCoursesList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
        else
            completedCoursesList->Append(wxString::FromUTF8(code));
    }
}

/** @brief Sets combo selections from profile and refreshes completed + missing lists. */
void StudentProfilePanel::SyncUIFromProfile() {
    yearCombo->SetSelection(wxNOT_FOUND);
    const auto& years = Academic::GetYearOptions();
    for (size_t i = 0; i < years.size(); i++) {
        if (years[i] == profile.currentYear) {
            yearCombo->SetSelection(static_cast<int>(i));
            break;
        }
    }
    specializationCombo->SetSelection(wxNOT_FOUND);
    const auto& specs = Academic::GetSpecializations();
    for (size_t i = 0; i < specs.size(); i++) {
        if (specs[i].id == profile.specializationId) {
            specializationCombo->SetSelection(static_cast<int>(i));
            break;
        }
    }
    RefreshCompletedCoursesList();
    RefreshMissingRequirements();
}

/** @brief Writes selected year and specialization into profile only. */
void StudentProfilePanel::SyncYearSpecFromUI() {
    int yearIdx = yearCombo->GetSelection();
    if (yearIdx >= 0) {
        const auto& years = Academic::GetYearOptions();
        profile.currentYear = years[static_cast<size_t>(yearIdx)];
    }
    int specIdx = specializationCombo->GetSelection();
    if (specIdx >= 0) {
        const auto& specs = Academic::GetSpecializations();
        profile.specializationId = specs[static_cast<size_t>(specIdx)].id;
    }
}

/** @brief Updates year/spec from UI, recomputes missing graduation courses, refreshes missingList. */
void StudentProfilePanel::RefreshMissingRequirements() {
    SyncYearSpecFromUI();

    std::vector<std::string> missing = profile.GetMissingGraduationRequirements();
    missingList->Clear();
    for (const std::string& code : missing) {
        Course* c = catalog->getCourse(code);
        if (c)
            missingList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
        else
            missingList->Append(wxString::FromUTF8(code));
    }

    auto formatBreadthCategoryDisplay = [this](const std::string& category) -> wxString {
        std::vector<std::string> ids = profile.GetCompletedBreadthCourseIdsForCategory(*catalog, category);
        double credits = profile.GetCompletedBreadthCreditsForCategory(*catalog, category);
        bool complete = profile.HasCompletedBreadthCredits(*catalog, category, 1.0);

        std::ostringstream out;
        out << category << " breadth: "
            << (complete ? "Complete" : "Missing")
            << " (" << credits << "/1.0 credits";

        if (!ids.empty()) {
            out << " — ";
            for (std::size_t i = 0; i < ids.size(); i++) {
                Course* c = catalog->getCourse(ids[i]);
                if (i > 0) {
                    out << ", ";
                }
                if (c != nullptr) {
                    out << c->getCode() << " - " << c->getTitle();
                } else {
                    out << ids[i];
                }
            }
        }

        out << ")";
        return wxString::FromUTF8(out.str());
    };

    const bool hasScience = profile.HasCompletedBreadthCredits(*catalog, "Science", 1.0);
    const bool hasHumanities = profile.HasCompletedBreadthCredits(*catalog, "Humanities", 1.0);
    const bool hasSocialScience = profile.HasCompletedBreadthCredits(*catalog, "Social Science", 1.0);

    scienceBreadthLabel->SetLabel(formatBreadthCategoryDisplay("Science"));
    scienceBreadthLabel->SetForegroundColour(
        hasScience ? wxColour(0, 100, 0) : wxColour(180, 0, 0)
    );

    humanitiesBreadthLabel->SetLabel(formatBreadthCategoryDisplay("Humanities"));
    humanitiesBreadthLabel->SetForegroundColour(
        hasHumanities ? wxColour(0, 100, 0) : wxColour(180, 0, 0)
    );

    socialScienceBreadthLabel->SetLabel(formatBreadthCategoryDisplay("Social Science"));
    socialScienceBreadthLabel->SetForegroundColour(
        hasSocialScience ? wxColour(0, 100, 0) : wxColour(180, 0, 0)
    );

    UpdateSelectionStatus();
}

/** @brief Sets the status line from the count of completed courses. */
void StudentProfilePanel::UpdateSelectionStatus() {
    size_t count = profile.completedCourseIds.size();
    wxString msg = wxString::Format("%zu course%s in completed list. Click Save Profile to save to your profile.", count, count == 1 ? "" : "s");
    selectionStatusText->SetLabel(msg);
}

/** @brief Loads profile.txt from user data dir and applies to UI. */
void StudentProfilePanel::LoadProfile() {
    profile.LoadFromFile(profilePath.ToStdString());
    SyncUIFromProfile();
}

/** @brief Persists profile (year, spec, completed) after syncing year/spec from combos. */
void StudentProfilePanel::RefreshCatalogView() {
    if (catalog == nullptr) {
        return;
    }
    PopulateCoursesCheckList();
    RefreshCompletedCoursesList();
    RefreshMissingRequirements();
}

void StudentProfilePanel::OnSave(wxCommandEvent&) {
    SyncYearSpecFromUI();
    if (profile.SaveToFile(profilePath.ToStdString())) {
        RefreshMissingRequirements();
        size_t n = profile.completedCourseIds.size();
        selectionStatusText->SetLabel(wxString::Format("Saved. %zu course%s saved to your profile.", n, n == 1 ? "" : "s"));
        selectionStatusText->SetForegroundColour(wxColour(0, 100, 0));
        wxMessageBox("Profile saved successfully. You can edit and save again at any time.", "Profile Saved", wxOK | wxICON_INFORMATION, this);
    } else {
        wxMessageBox("Could not save profile to file.", "Save Error", wxOK | wxICON_ERROR, this);
    }
}

/** @brief Appends checked catalog rows to profile.completedCourseIds and clears those checks. */
void StudentProfilePanel::OnAddToCompleted(wxCommandEvent&) {
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    if (courses.empty() || coursesCheckList->GetCount() == 0) {
        wxMessageBox("No courses to add. Is the catalog loaded? (Check the message at the top.)", "Catalog empty", wxOK | wxICON_WARNING, this);
        return;
    }
    int addedCount = 0;
    for (unsigned int i = 0; i < coursesCheckList->GetCount(); i++) {
        if (!coursesCheckList->IsChecked(i))
            continue;
        if (i >= courses.size())
            continue;
        const Course* c = courses[i].get();
        if (!c)
            continue;
        const std::string& code = c->getCode();
        if (std::find(profile.completedCourseIds.begin(), profile.completedCourseIds.end(), code) != profile.completedCourseIds.end())
            continue;
        profile.completedCourseIds.push_back(code);
        coursesCheckList->Check(i, false);
        addedCount++;
    }
    RefreshCompletedCoursesList();
    RefreshMissingRequirements();
    if (addedCount > 0)
        wxMessageBox(wxString::Format("Added %d course%s to your completed list.", addedCount, addedCount == 1 ? "" : "s"), "Courses added", wxOK | wxICON_INFORMATION, this);
    else
        wxMessageBox("Check one or more courses in the list (use the checkbox), or double-click a course to add it. Already-completed courses are skipped.", "Nothing to add", wxOK | wxICON_INFORMATION, this);
}

/** @brief Adds the double-clicked row’s course code if not already completed. */
void StudentProfilePanel::OnCoursesCheckListDoubleClick(wxCommandEvent& event) {
    const std::vector<std::unique_ptr<Course>>& courses = catalog->getAllCourses();
    int i = event.GetInt();
    if (i < 0 || static_cast<size_t>(i) >= courses.size())
        return;
    const Course* c = courses[static_cast<size_t>(i)].get();
    if (!c)
        return;
    const std::string& code = c->getCode();
    if (std::find(profile.completedCourseIds.begin(), profile.completedCourseIds.end(), code) != profile.completedCourseIds.end())
        return;
    profile.completedCourseIds.push_back(code);
    coursesCheckList->Check(static_cast<unsigned int>(i), false);
    RefreshCompletedCoursesList();
    RefreshMissingRequirements();
    selectionStatusText->SetLabel(wxString::FromUTF8(code + " added to completed list."));
}

/** @brief Removes the double-clicked course from completed (same as Remove button). */
void StudentProfilePanel::OnCompletedCoursesDoubleClick(wxCommandEvent& event) {
    int sel = completedCoursesList->GetSelection();
    if (sel < 0)
        sel = event.GetInt();
    if (sel < 0)
        sel = lastCompletedListSelection;
    if (sel < 0) return;
    wxString item = completedCoursesList->GetString(static_cast<unsigned int>(sel));
    std::string s = item.ToStdString();
    size_t pos = s.find(" - ");
    std::string code = (pos != std::string::npos) ? s.substr(0, pos) : s;
    auto it = std::find(profile.completedCourseIds.begin(), profile.completedCourseIds.end(), code);
    if (it != profile.completedCourseIds.end()) {
        profile.completedCourseIds.erase(it);
        RefreshCompletedCoursesList();
        RefreshMissingRequirements();
    }
}

/** @brief Remembers selection so Remove works after focus moves to the button (macOS). */
void StudentProfilePanel::OnCompletedListSelected(wxCommandEvent& event) {
    lastCompletedListSelection = event.GetInt();
}

/** @brief Erases selected (or last selected) course code from profile. */
void StudentProfilePanel::OnRemoveFromCompleted(wxCommandEvent&) {
    int sel = completedCoursesList->GetSelection();
    if (sel < 0)
        sel = lastCompletedListSelection;
    if (sel < 0) {
        wxMessageBox("Select a course in the completed list first, then click Remove.", "No selection", wxOK | wxICON_INFORMATION, this);
        return;
    }
    wxString item = completedCoursesList->GetString(static_cast<unsigned int>(sel));
    std::string s = item.ToStdString();
    size_t pos = s.find(" - ");
    std::string code = (pos != std::string::npos) ? s.substr(0, pos) : s;
    auto it = std::find(profile.completedCourseIds.begin(), profile.completedCourseIds.end(), code);
    if (it != profile.completedCourseIds.end()) {
        profile.completedCourseIds.erase(it);
        RefreshCompletedCoursesList();
        RefreshMissingRequirements();
    }
}

/** @brief Refreshes missing requirements when year or specialization changes. */
void StudentProfilePanel::OnSelectionChanged(wxCommandEvent&) {
    RefreshMissingRequirements();
}
