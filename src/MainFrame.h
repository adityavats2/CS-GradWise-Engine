#ifndef MAINFRAME_H
#define MAINFRAME_H
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "CourseCatalog.h"
#include "StudentProfilePanel.h"
#include "CourseCatalogPanel.h"
#include "ScheduleValidationPanel.h"
#include "OutputExportPanel.h"

/** @brief Main application window */
class MainFrame : public wxFrame {
public:
    explicit MainFrame(const wxString& title);

private:
    CourseCatalog catalog;
    wxButton* studentProfileButton;
    wxButton* courseCatalogButton;
    wxButton* scheduleValidationButton;
    wxButton* outputExportButton;
    wxSimplebook* pageBook;
    StudentProfilePanel* studentProfilePanel;
    CourseCatalogPanel* courseCatalogPanel;
    ScheduleValidationPanel* scheduleValidationPanel;
    OutputExportPanel* outputExportPanel;
    /** @brief Shows the selected page */
    void ShowPage(int pageIndex);
    /** @brief Handles Student Profile button clicks */
    void OnStudentProfile(wxCommandEvent& event);
    /** @brief Handles Course catalog button clicks */
    void OnCourseCatalog(wxCommandEvent& event);
    /** @brief Handles Schedule Validation button clicks */
    void OnScheduleValidation(wxCommandEvent& event);
    /** @brief Handles Output and export button clicks */
    void OnOutputExport(wxCommandEvent& event);
};
#endif