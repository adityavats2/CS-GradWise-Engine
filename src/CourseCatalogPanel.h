/**
 * @file CourseCatalogPanel.h
 * @brief wxWidgets panel to load and browse the course catalog.
 */

#ifndef COURSECATALOGPANEL_H
#define COURSECATALOGPANEL_H

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include "CourseCatalog.h"
#include "CourseOffering.h"
#include "TimeSlot.h"

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
    wxButton* saveCatalogButton;
    wxListCtrl* courseList;
    wxStaticText* detailsLabel;
    wxTextCtrl* detailsText;     /*!< Read-only details for selected course */
    wxStaticText* statusText;
    CourseCatalog* catalog;      /*!< Non-owning; must outlive this panel */


    wxTextCtrl* newCourseCodeInput;
    wxTextCtrl* newCourseTitleInput;
    wxTextCtrl* newCourseCreditsInput;
    wxTextCtrl* newCourseBreadthInput;
    wxCheckBox* newCourseElectiveCheckBox;

    wxTextCtrl* newCourseSimplePrereqInput;
    wxTextCtrl* newCourseExclusionsInput;

    wxChoice* offeringSeasonChoice;
    wxSpinCtrl* offeringYearSpinCtrl;

    wxChoice* timeSlotDayChoice;
    wxSpinCtrl* startHourSpinCtrl;
    wxChoice* startMinuteChoice;
    wxChoice* startAmPmChoice;
    wxSpinCtrl* endHourSpinCtrl;
    wxChoice* endMinuteChoice;
    wxChoice* endAmPmChoice;

    wxButton* addTimeSlotButton;
    wxButton* removeTimeSlotButton;
    wxListBox* currentTimeSlotsList;

    wxButton* addOfferingButton;
    wxButton* removeOfferingButton;
    wxListBox* offeringsList;

    wxButton* addCourseButton;

    std::vector<TimeSlot> currentOfferingTimeSlots;
    std::vector<CourseOffering> pendingOfferings;

    CourseCatalog* catalog;
    /** @brief Handles catalog loading
     *  @param event Button event
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

    /**
     * @brief This funtion handles the result when the 'Add Course' action is taken
     * @param event The button that is pressed to trigger the result of 'Add Course'
     */
    void OnAddCourse(wxCommandEvent& event);
    
    /**
     * @brief This funtion handles the result when the 'Save Course in Catalog' action is taken
     * @param event The button that is pressed to trigger the result of 'Save Course in Catalog'
     */
    void OnSaveCatalog(wxCommandEvent& event);

    /**
     * @brief This funtion handles the result of adding a timeslot to a perspective course
     * @param event The button that is pressed to trigger the result of adding a timeslot
     */
    void OnAddTimeSlot(wxCommandEvent& event);

    /**
     * @brief This funtion handles the result of removing a timeslot to a perspective course
     * @param event The button that is pressed to trigger the result of removing a timeslot
     */
    void OnRemoveTimeSlot(wxCommandEvent& event);

    /**
     * @brief This funtion handles the result of adding a time offering to a perspective course
     * @param event The button that is pressed to trigger the result of adding a offering
     */
    void OnAddOffering(wxCommandEvent& event);

    /**
     * @brief This funtion handles the result of removing a time offering to a perspective course
     * @param event The button that is pressed to trigger the result of removing a offering
     */
    void OnRemoveOffering(wxCommandEvent& event);

    /**
     * @brief ensures that the current timeslots for the perspective course are refreshed and shown
     */
    void RefreshCurrentTimeSlotsList();
    
    /**
     * @brief ensures that the current time offerings for the perspective course are refresuehd and shown
     */
    void RefreshPendingOfferingsList();
    
    /**
     * @brief clears out the previously entered contents of the add a course entry form
     */
    void ClearCourseEntryForm();

    /**
     * @brief Converts a 12-hour time into minutes from midnight
     * @param hour12 The hour in 12-hour format.
     * @param minute The minute value.
     * @param amPm The AM/PM designator.
     * @return The number of minutes from midnight.
     */
    int ConvertToMinutesFromMidnight(int hour12, int minute, const wxString& amPm) const;
    
    /**
     * @brief Attempts to parse a season string into a Season enum value.
     * @param text The season text to parse.
     * @param season Output parameter for the parsed season value.
     * @return True if parsing succeeded, false otherwise.
    */
    bool TryParseSeason(const wxString& text, Season& season) const;

    /**
     * @brief formats a time value as a 12-hour clock string
     * @param minutesFromMidnight The time to format, in minutes from midnight.
     * @return The formatted time string.
     */
    wxString FormatTime12Hour(int minutesFromMidnight) const;
    
    /**
     * @brief Creates a timeslot specifically for UI display
     * @param slot the timeslot to format
     * @return The formatted timeslot
     */
    wxString FormatTimeSlotForDisplay(const TimeSlot& slot) const;
    
    /**
     * @brief Creates a time offering specifically for UI display
     * @param slot the offering to format
     * @return The formatted time offering 
     */
    wxString FormatOfferingForDisplay(const CourseOffering& offering) const;

public:
    /**
     * @brief Constructs layout and bindings; if catalog is pre-filled, shows first course.
     * @param parent Parent window (e.g. notebook page).
     * @param catalog Shared CourseCatalog; not owned by this panel.
     */
    CourseCatalogPanel(wxWindow* parent, CourseCatalog* catalog);
};

#endif
