/**
 * @file StudentProfilePanel.h
 * @brief wxWidgets panel for student profile input and graduation gap display.
 */

#ifndef STUDENTPROFILEPANEL_H
#define STUDENTPROFILEPANEL_H

#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/listbox.h>
#include <vector>
#include <string>
#include "StudentProfile.h"
#include "CourseCatalog.h"

/**
 * @class StudentProfilePanel
 * @brief Edits year, specialization, and completed courses; saves profile; shows missing requirements.
 *
 * Loads its own CourseCatalog from several candidate paths. Completed courses live only in
 * StudentProfile::completedCourseIds; the completed list box is a mirror for display and remove UX.
 */
class StudentProfilePanel : public wxPanel {
public:
    /**
     * @brief Creates UI, loads catalog, loads profile from user data directory.
     * @param parent Parent window.
     */
    explicit StudentProfilePanel(wxWindow* parent);

    /** @brief Reloads profile from disk and refreshes all dependent controls. */
    StudentProfilePanel(wxWindow* parent, CourseCatalog* catalog);
    void RefreshCatalogView();

private:
    StudentProfile profile;
    CourseCatalog* catalog;
    wxString profilePath;

    wxComboBox* yearCombo;
    wxComboBox* specializationCombo;
    wxCheckListBox* coursesCheckList;
    wxListBox* completedCoursesList;
    wxListBox* missingList;
    wxButton* addButton;
    wxButton* removeButton;
    wxButton* saveButton;
    wxStaticText* catalogStatusText;
    wxStaticText* selectionStatusText;
    wxStaticText* missingLabel;
    wxStaticText* breadthStatusLabel;
    wxStaticText* breadthCompletedByLabel;
    wxStaticText* scienceBreadthLabel;
    wxStaticText* humanitiesBreadthLabel;
    wxStaticText* socialScienceBreadthLabel;
    int lastCompletedListSelection;
    
    void BuildUI();
    void LoadCatalog();
    void LoadProfile();
    void PopulateYearCombo();
    void PopulateSpecializationCombo();
    void PopulateCoursesCheckList();
    void RefreshCompletedCoursesList();
    void SyncUIFromProfile();
    /** @brief Copies year and specialization from combos only; does not alter completedCourseIds. */
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
