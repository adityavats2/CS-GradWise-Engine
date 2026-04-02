/**
 * @file CourseCatalogPanel.h
 * @brief wxWidgets panel to load and browse the course catalog.
 */

#ifndef COURSECATALOGPANEL_H
#define COURSECATALOGPANEL_H
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "CourseCatalog.h"

/**
 * @class CourseCatalogPanel
 * @brief Loads courses from a user-specified file path and shows a table plus detailed read-only metadata.
 *
 * Displays code, title, credits, prerequisites, exclusions, and offerings with time slots. Does not own
 * the catalog; MainFrame (or similar) provides a shared CourseCatalog pointer.
 */
class CourseCatalogPanel : public wxPanel {
private:
    wxTextCtrl* filePathInput;   /*!< Default path to courses file (e.g. ../data/courses.txt) */
    wxButton* loadButton;
    wxListCtrl* courseList;      /*!< Report: Code, Title, Credits */
    wxStaticText* detailsLabel;
    wxTextCtrl* detailsText;     /*!< Read-only details for selected course */
    wxStaticText* statusText;
    CourseCatalog* catalog;      /*!< Non-owning; must outlive this panel */

    /**
     * @brief Clears catalog, loads from filePathInput, refreshes list and details.
     * @param event Load button click.
     */
    void OnLoadCatalog(wxCommandEvent& event);

    /**
     * @brief Updates details pane for the selected list row.
     * @param event List selection event.
     */
    void OnCourseSelected(wxListEvent& event);

    /**
     * @brief Formats and shows course metadata in detailsText.
     * @param course Course to display; nullptr shows empty message.
     */
    void ShowCourseDetails(const Course* course);

    /** @brief Rebuilds courseList from catalog->getAllCourses(). */
    void RefreshCourseList();

public:
    /**
     * @brief Constructs layout and bindings; if catalog is pre-filled, shows first course.
     * @param parent Parent window (e.g. notebook page).
     * @param catalog Shared CourseCatalog; not owned by this panel.
     */
    CourseCatalogPanel(wxWindow* parent, CourseCatalog* catalog);
};

#endif
