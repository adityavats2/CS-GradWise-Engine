#include "StudentProfilePanel.h"
#include "AcademicData.h"
#include "CourseCatalogLoader.h"
#include <algorithm>
#include <wx/filename.h>
#include <wx/stdpaths.h>

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
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Student Profile Input Page");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    title->SetFont(titleFont);
    mainSizer->Add(title, 0, wxALL, 10);

    wxFlexGridSizer* formSizer = new wxFlexGridSizer(2, 5, 5);
    formSizer->Add(new wxStaticText(this, wxID_ANY, "Current year:"), 0, wxALIGN_CENTER_VERTICAL);
    yearCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
    PopulateYearCombo();
    formSizer->Add(yearCombo, 1, wxEXPAND);
    formSizer->Add(new wxStaticText(this, wxID_ANY, "Specialization:"), 0, wxALIGN_CENTER_VERTICAL);
    specializationCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
    PopulateSpecializationCombo();
    formSizer->Add(specializationCombo, 1, wxEXPAND);
    mainSizer->Add(formSizer, 0, wxEXPAND | wxALL, 10);

    catalogStatusText = new wxStaticText(this, wxID_ANY, "Loading catalog...");
    mainSizer->Add(catalogStatusText, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Completed courses (check all that apply):"), 0, wxLEFT | wxRIGHT | wxTOP, 10);
    coursesCheckList = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(400, 180));
    PopulateCoursesList();
    mainSizer->Add(coursesCheckList, 1, wxEXPAND | wxALL, 10);

    selectionStatusText = new wxStaticText(this, wxID_ANY, "0 courses selected. Click Save Profile to save to your profile.");
    selectionStatusText->SetForegroundColour(wxColour(0, 100, 0));
    mainSizer->Add(selectionStatusText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    saveButton = new wxButton(this, wxID_ANY, "Save Profile");
    mainSizer->Add(saveButton, 0, wxALL, 10);

    missingLabel = new wxStaticText(this, wxID_ANY, "Missing graduation requirements (based on your specialization):");
    mainSizer->Add(missingLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    missingList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(400, 120), 0, nullptr, wxLB_SINGLE);
    mainSizer->Add(missingList, 0, wxEXPAND | wxALL, 10);

    SetSizer(mainSizer);

    saveButton->Bind(wxEVT_BUTTON, &StudentProfilePanel::OnSave, this);
    yearCombo->Bind(wxEVT_COMBOBOX, &StudentProfilePanel::OnSelectionChanged, this);
    specializationCombo->Bind(wxEVT_COMBOBOX, &StudentProfilePanel::OnSelectionChanged, this);
    coursesCheckList->Bind(wxEVT_CHECKLISTBOX, &StudentProfilePanel::OnSelectionChanged, this);
}

void StudentProfilePanel::LoadCatalog() {
    std::string path = "data/courses.txt";
    if (!CourseCatalogLoader::loadFromFile(path, catalog)) {
        path = "../data/courses.txt";
        if (!CourseCatalogLoader::loadFromFile(path, catalog)) {
            catalogStatusText->SetLabel("Catalog not loaded. Place data/courses.txt next to the app or in project root.");
            return;
        }
    }
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    catalogStatusText->SetLabel(wxString::Format("Catalog loaded: %zu courses.", courses.size()));
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

void StudentProfilePanel::PopulateCoursesList() {
    coursesCheckList->Clear();
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    for (const auto& ptr : courses) {
        const Course* c = ptr.get();
        if (c)
            coursesCheckList->Append(wxString::FromUTF8(c->getCode() + " - " + c->getTitle()));
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
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    for (size_t i = 0; i < courses.size(); i++) {
        const Course* c = courses[i].get();
        if (!c) continue;
        bool completed = std::find(profile.completedCourseIds.begin(), profile.completedCourseIds.end(), c->getCode()) != profile.completedCourseIds.end();
        coursesCheckList->Check(static_cast<unsigned int>(i), completed);
    }
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
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    for (size_t i = 0; i < courses.size(); i++) {
        if (coursesCheckList->IsChecked(static_cast<unsigned int>(i))) {
            const Course* c = courses[i].get();
            if (c)
                profile.completedCourseIds.push_back(c->getCode());
        }
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
    UpdateSelectionStatus(-1);
}

void StudentProfilePanel::UpdateSelectionStatus(int toggledIndex) {
    unsigned int count = 0;
    const std::vector<std::unique_ptr<Course>>& courses = catalog.getAllCourses();
    for (unsigned int i = 0; i < coursesCheckList->GetCount(); i++) {
        if (coursesCheckList->IsChecked(i))
            count++;
    }
    wxString msg;
    if (toggledIndex >= 0 && static_cast<size_t>(toggledIndex) < courses.size()) {
        const Course* c = courses[static_cast<size_t>(toggledIndex)].get();
        if (c) {
            std::string codeTitle = c->getCode() + " - " + c->getTitle();
            if (coursesCheckList->IsChecked(static_cast<unsigned int>(toggledIndex)))
                msg = wxString::FromUTF8(codeTitle + " marked as completed. ");
            else
                msg = wxString::FromUTF8(codeTitle + " removed from completed. ");
        }
    }
    msg += wxString::Format("%u course%s selected. Click Save Profile to save to your profile.", count, count == 1 ? "" : "s");
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

void StudentProfilePanel::OnSelectionChanged(wxCommandEvent& event) {
    int toggledIndex = (event.GetEventType() == wxEVT_CHECKLISTBOX) ? event.GetInt() : -1;
    RefreshMissingRequirements();
    UpdateSelectionStatus(toggledIndex);
}
