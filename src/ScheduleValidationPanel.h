#ifndef SCHEDULEVALIDATIONPANEL_H
#define SCHEDULEVALIDATIONPANEL_H

#include <wx/wx.h>
#include <wx/checklst.h>
#include "CourseCatalog.h"

class ScheduleValidationPanel : public wxPanel {
public:
    explicit ScheduleValidationPanel(wxWindow* parent);

private:
    // Course catalog
    CourseCatalog catalog;

    // UI components
    wxComboBox* seasonCombo;
    wxTextCtrl* yearInput;
    wxCheckListBox* coursesCheckList;
    wxButton* validateButton;
    wxTextCtrl* resultsText;

    // Internal methods
    void LoadCatalog();
    void PopulateCoursesList();

    // Event handlers
    void OnValidate(wxCommandEvent& event);
};

#endif