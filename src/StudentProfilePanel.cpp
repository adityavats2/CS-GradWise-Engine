#include "StudentProfilePanel.h"
#include "AcademicData.h"
#include "CourseCatalogLoader.h"
#include <algorithm>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/scrolwin.h>

StudentProfilePanel::StudentProfilePanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY) {
    wxStandardPaths& stdPaths = wxStandardPaths::Get();
    wxString userData = stdPaths.GetUserDataDir();
    if (!wxFileName::DirExists(userData))
        wxFileName::Mkdir(userData, 0755, wxPATH_MKDIR_FULL);
    profilePath = wxFileName(userData, "profile.txt").GetFullPath();
    BuildUI();
    LoadCatalog();
    LoadProfile();
}

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

    mainSizer->Add(new wxStaticText(scroll, wxID_ANY, "Check each course you have completed, then click Add:"), 0, wxLEFT | wxRIGHT | wxTOP, 10);
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
    mainSizer->Add(missingList, 0, wxEXPAND | wxALL, 10);

    scroll->SetSizer(mainSizer);
    scroll->FitInside();

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(scroll, 1, wxEXPAND);
    SetSizer(panelSizer);

    saveButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnSave, this);
    addButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnAddToCompleted, this);
    removeButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnRemoveFromCompleted, this);
    completedCoursesList->Bind(wxEVT_LISTBOX_DCLICK, &StudentProfilePanel::OnCompletedCoursesDoubleClick, this);
    yearCombo->Bind(wxEVT_COMBOBOX, &StudentProfilePanel::OnSelectionChanged, this);
    specializationCombo->Bind(wxEVT_COMBOBOX, &StudentProfilePanel::OnSelectionChanged, this);
}

void StudentProfilePanel::LoadCatalog() {
    std::vector<std::string> pathsToTry;
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxString exeDir = wxFileName(exePath).GetPath();
    pathsToTry.push_back((exeDir + "/../data/courses.txt").ToStdString());
    pathsToTry.push_back((exeDir + "/data/courses.txt").ToStdString());
    pathsToTry.push_back("data/courses.txt");
    pathsToTry.push_back("../data/courses.txt");

    bool loaded = false;
    for (const std::string& path : pathsToTry) {
        if (CourseCatalogLoader::loadFromFile(path, catalog)) {
            loaded = true;
            break;
        }
    }
    if (!loaded) {
        catalogStatusText->SetLabel("Catalog not loaded. Put data/courses.txt next to the app or in project root.");
        return;
    }
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    catalogStatusText->SetLabel(wxString::Format("Catalog loaded: %zu courses.", courses.size()));
    PopulateCoursesCheckList();
}

void StudentProfilePanel::PopulateYearCombo() {
    const auto& years = Academic::GetYearOptions();
    for (const auto& y : years)
        yearCombo->Append(wxString::FromUTF8(y));
}

void StudentProfilePanel::PopulateSpecializationCombo() {
    const auto& specs = Academic::GetSpecializations();
    for (const auto& s : specs)
        specializationCombo->Append(wxString::FromUTF8(s.id + " - " + s.name));
}

void StudentProfilePanel::PopulateCoursesCheckList() {
    coursesCheckList->Clear();
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    for (const auto& ptr : courses) {
        const Course* c = ptr.get();
        if (c)
            coursesCheckList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
    }
}

void StudentProfilePanel::RefreshCompletedCoursesList() {
    completedCoursesList->Clear();
    for (const std::string& code : profile.completedCourseIds) {
        Course* c = catalog.getCourse(code);
        if (c)
            completedCoursesList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
        else
            completedCoursesList->Append(wxString::FromUTF8(code));
    }
}

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

void StudentProfilePanel::SyncProfileFromUI() {
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
    profile.completedCourseIds.clear();
    for (int i = 0; i < completedCoursesList->GetCount(); i++) {
        wxString item = completedCoursesList->GetString(static_cast<unsigned int>(i));
        std::string s = item.ToStdString();
        size_t pos = s.find(" - ");
        if (pos != std::string::npos)
            profile.completedCourseIds.push_back(s.substr(0, pos));
        else
            profile.completedCourseIds.push_back(s);
    }
}

void StudentProfilePanel::RefreshMissingRequirements() {
    SyncProfileFromUI();
    std::vector<std::string> missing = profile.GetMissingGraduationRequirements();
    missingList->Clear();
    for (const std::string& code : missing) {
        Course* c = catalog.getCourse(code);
        if (c)
            missingList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
        else
            missingList->Append(wxString::FromUTF8(code));
    }
    UpdateSelectionStatus();
}

void StudentProfilePanel::UpdateSelectionStatus() {
    size_t count = profile.completedCourseIds.size();
    wxString msg = wxString::Format("%zu course%s in completed list. Click Save Profile to save to your profile.", count, count == 1 ? "" : "s");
    selectionStatusText->SetLabel(msg);
}

void StudentProfilePanel::LoadProfile() {
    profile.LoadFromFile(profilePath.ToStdString());
    SyncUIFromProfile();
}

void StudentProfilePanel::OnSave(wxCommandEvent&) {
    SyncProfileFromUI();
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

void StudentProfilePanel::OnAddToCompleted(wxCommandEvent&) {
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
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
        wxMessageBox("No checked courses to add, or they are already in your completed list.", "Nothing to add", wxOK | wxICON_INFORMATION, this);
}

void StudentProfilePanel::OnCompletedCoursesDoubleClick(wxCommandEvent&) {
    int sel = completedCoursesList->GetSelection();
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

void StudentProfilePanel::OnRemoveFromCompleted(wxCommandEvent&) {
    int sel = completedCoursesList->GetSelection();
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

void StudentProfilePanel::OnSelectionChanged(wxCommandEvent&) {
    RefreshMissingRequirements();
}
