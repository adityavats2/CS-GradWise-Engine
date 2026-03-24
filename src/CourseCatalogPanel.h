#ifndef COURSECATALOGPANEL_H
#define COURSECATALOGPANEL_H
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "CourseCatalog.h"

/** @brief Panel for loading and viewing course catalog */
class CourseCatalogPanel : public wxPanel {
private:
    wxTextCtrl* filePathInput;
    wxButton* loadButton;
    wxListCtrl* courseList;
    wxStaticText* detailsLabel;
    wxTextCtrl* detailsText;
    wxStaticText* statusText;
    CourseCatalog* catalog;
    /** @brief Handles catalog loading
     *  @param event Button event
     */
    void OnLoadCatalog(wxCommandEvent& event);
    /** @brief Handles course selection
     *  @param event List event
     */
    void OnCourseSelected(wxListEvent& event);
    /** @brief Shows details for one course
     *  @param course Selected course
     */
    void ShowCourseDetails(const Course* course);
    /** @brief Refreshes course list */
    void RefreshCourseList();

public:
    /** @brief Creates course catalog panel
     *  @param parent Parent window
     *  @param catalog Shared catalog owned by MainFrame
     */
    CourseCatalogPanel(wxWindow* parent, CourseCatalog* catalog);
};

#endif