#ifndef SCHEDULEVALIDATIONPANEL_H
#define SCHEDULEVALIDATIONPANEL_H

#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <vector>
#include "CourseCatalog.h"
#include "PlannedSchedule.h"
#include "ScheduleGenerationEngine.h"

/**
 * @brief Panel that validates term selections and shows generated planning output.
 */
class ScheduleValidationPanel : public wxPanel {
public:
    /**
     * @brief Construct the schedule validation panel UI.
     *
     * @param parent Parent wxWidgets window.
     */
    explicit ScheduleValidationPanel(wxWindow* parent);

private:
    // Course catalog
    CourseCatalog catalog;
    PlannedSchedule plannedSchedule;
    wxString plannedSchedulePath;

    // UI components
    wxComboBox* seasonCombo;
    wxTextCtrl* yearInput;
    wxCheckBox* preferMorningCheckBox;
    wxCheckBox* preferCompactDaysCheckBox;
    wxCheckBox* enforceEarliestStartCheckBox;
    wxCheckBox* includeSavedFuturePlansInPathwayCheckBox;
    wxSpinCtrl* morningWeightSpinCtrl;
    wxSpinCtrl* compactDaysWeightSpinCtrl;
    wxSpinCtrl* earliestStartHourSpinCtrl;
    wxCheckListBox* coursesCheckList;
    wxButton* validateButton;
    wxButton* clearPlannedScheduleButton;
    wxTextCtrl* resultsText;
    wxChoice* generatedOptionsChoice;
    wxTextCtrl* generatedOptionDetailsTextCtrl;
    wxTextCtrl* savedEarlierPlansTextCtrl;
    std::vector<GeneratedTermScheduleResult> lastGeneratedOptions;

    // Internal methods
    /**
     * @brief Resolve and create the planned schedule file path.
     */
    void InitPlannedSchedulePath();
    /**
     * @brief Load persisted planned schedule data into memory.
     */
    void LoadPlannedSchedule();
    /**
     * @brief Persist the in-memory planned schedule to disk.
     *
     * @return True if the save succeeds.
     */
    bool SavePlannedSchedule();
    /**
     * @brief Load the course catalog from available data paths.
     */
    void LoadCatalog();
    /**
     * @brief Populate the checklist with catalog courses.
     */
    void PopulateCoursesList();
    /**
     * @brief Refresh the read-only display of earlier saved terms.
     */
    void RefreshSavedEarlierPlansDisplay();
    /**
     * @brief Refresh detail text for the currently selected generated option.
     */
    void RefreshGeneratedOptionDisplay();
    /**
     * @brief Rebuild the generated option selector entries.
     */
    void PopulateGeneratedOptionsChoice();

    // Event handlers
    /**
     * @brief Validate selected courses and render generation/pathway output.
     *
     * @param event Button click event.
     */
    void OnValidate(wxCommandEvent& event);
    /**
     * @brief Handle generated-option selector changes.
     *
     * @param event Choice selection event.
     */
    void OnGeneratedOptionChanged(wxCommandEvent& event);
    /**
     * @brief Clear persisted planned schedule state and refresh the panel.
     *
     * @param event Button click event.
     */
    void OnClearPlannedSchedule(wxCommandEvent& event);
};

#endif
