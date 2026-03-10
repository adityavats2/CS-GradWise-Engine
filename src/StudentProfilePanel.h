#ifndef STUDENTPROFILEPANEL_H
#define STUDENTPROFILEPANEL_H

#include <wx/wx.h>
#include <wx/checklst.h>
#include "StudentProfile.h"
#include "CourseCatalog.h"

class StudentProfilePanel : public wxPanel {
public:
    explicit StudentProfilePanel(wxWindow* parent);

    /** @brief Load profile from file and refresh UI (e.g. when returning to this page) */
    void LoadProfile();

private:
    StudentProfile profile;
    wxString profilePath;
    CourseCatalog catalog;

    wxComboBox* yearCombo;
    wxComboBox* specializationCombo;
    wxCheckListBox* coursesCheckList;
    wxButton* saveButton;
    wxListBox* missingList;
    wxStaticText* missingLabel;
    wxStaticText* catalogStatusText;

    void BuildUI();
    void LoadCatalog();
    void PopulateYearCombo();
    void PopulateSpecializationCombo();
    void PopulateCoursesList();
    void SyncUIFromProfile();
    void SyncProfileFromUI();
    void RefreshMissingRequirements();

    void OnSave(wxCommandEvent& event);
    void OnSelectionChanged(wxCommandEvent& event);
};

#endif
