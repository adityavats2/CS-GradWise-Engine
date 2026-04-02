#ifndef OUTPUTEXPORTPANEL_H
#define OUTPUTEXPORTPANEL_H
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "CourseCatalog.h"
#include "ScheduleGenerationEngine.h"

/**
 * @brief Panel for displaying generated schedule options and exporting to CSV.
 *
 * Reads the saved planned schedule from disk and lets the user pick which option to view and export.
 */
class OutputExportPanel : public wxPanel {
public:
    /**
     * @brief Creates the output and export panel.
     * @param parent Parent window.
     * @param catalog Course catalog.
     */
    OutputExportPanel(wxWindow* parent, CourseCatalog* catalog);

    /**
     * @brief Sets schedule options from ScheduleValidationPanel.
     * @param term Term the options apply to.
     * @param generatedOptions Generated schedule options.
     */
    void SetOptions(const Term& term, const std::vector<GeneratedTermScheduleResult>& generatedOptions);

    /**
     * @brief Reloads planned schedule from disk, regenerates schedule options, refreshes display.
     */
    void RefreshView();

private:
    CourseCatalog* catalog;
    wxStaticText* termLabel;
    wxChoice* optionChoice;
    wxListCtrl* scheduleList;
    wxButton* exportButton;
    wxStaticText* statusText;
    /** Generated schedule options for the current planned term. */
    std::vector<GeneratedTermScheduleResult> options;
    /** The term that the current options apply to. */
    Term currentTerm;
    /** @brief Clears display and resets to empty state. */
    void showEmpty();
    /** @brief Populates the table from given option index. */
    void populateTable(int optionIndex);
    /** @brief Handles option-dropdown selection changes. */
    void OnOptionChanged(wxCommandEvent& event);
    /** @brief Handles Export to CSV button click. */
    void OnExport(wxCommandEvent& event);
};

#endif