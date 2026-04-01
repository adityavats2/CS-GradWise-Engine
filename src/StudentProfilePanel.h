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
    wxListBox* completedCoursesList;
    int lastCompletedListSelection;
    wxButton* addButton;
    wxButton* removeButton;
    wxButton* saveButton;
    wxListBox* missingList;
    wxStaticText* missingLabel;
    wxStaticText* catalogStatusText;
    wxStaticText* selectionStatusText;

    void BuildUI();
    void LoadCatalog();
    void PopulateYearCombo();
    void PopulateSpecializationCombo();
    void PopulateCoursesCheckList();
    void RefreshCompletedCoursesList();
    void SyncUIFromProfile();
    /** Updates year and specialization from combos only (completed courses stay in profile.) */
    void SyncYearSpecFromUI();
    void RefreshMissingRequirements();
    void UpdateSelectionStatus();

    void OnSave(wxCommandEvent& event);
    void OnAddToCompleted(wxCommandEvent& event);
    void OnRemoveFromCompleted(wxCommandEvent& event);
    void OnCompletedCoursesDoubleClick(wxCommandEvent& event);
    void OnCompletedListSelected(wxCommandEvent& event);
    void OnCoursesCheckListDoubleClick(wxCommandEvent& event);
    void OnSelectionChanged(wxCommandEvent& event);
};

#endif
