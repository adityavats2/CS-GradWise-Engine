#include "OutputExportPanel.h"
#include "StudentProfile.h"
#include <wx/filedlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <fstream>
#include <sstream>

namespace {
/**
 * @brief Format minutes from midnight as HH:MM.
 * @param minutesFromMidnight Time in minutes since 00:00.
 * @return Formatted time string.
 */
std::string formatTime(int minutesFromMidnight) {
    int h = minutesFromMidnight/60, m = minutesFromMidnight%60;
    std::ostringstream s;
    if (h < 10) s << '0'; s << h << ':';
    if (m < 10) s << '0'; s << m;
    return s.str();
}

/**
 * @brief Format all time slots of an offering as a comma separated string.
 * @param offering The course offering.
 * @return Formatted schedule string or "TBA" if no slots.
 */
std::string formatTimeSlots(const CourseOffering& offering) {
    const std::vector<TimeSlot>& slots = offering.getTimeSlots();
    if (slots.empty()) return "TBA";
    std::ostringstream out;
    for (std::size_t i = 0; i < slots.size(); i++) {
        if (i > 0) out << ", ";
        out << slots[i].getDay() << " " << formatTime(slots[i].getStartTime()) << "-" << formatTime(slots[i].getEndTime());
    }
    return out.str();
}
}

/** @brief Constructs the output and export panel UI. */
OutputExportPanel::OutputExportPanel(wxWindow* parent, CourseCatalog* catalog) : wxPanel(parent, wxID_ANY), catalog(catalog), currentTerm(Season::Fall, 0) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* heading = new wxStaticText(this, wxID_ANY, "Schedule Output and Export");
    wxFont f = heading->GetFont();
    f.SetPointSize(14);
    heading->SetFont(f);
    mainSizer->Add(heading, 0, wxALL, 10);
    termLabel = new wxStaticText(this, wxID_ANY, "No schedule generated yet.");
    mainSizer->Add(termLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    wxBoxSizer* optRow = new wxBoxSizer(wxHORIZONTAL);
    optRow->Add(new wxStaticText(this, wxID_ANY, "Schedule Option:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
    optionChoice = new wxChoice(this, wxID_ANY);
    optionChoice->Enable(false);
    optRow->Add(optionChoice, 0);
    mainSizer->Add(optRow, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    statusText = new wxStaticText(this, wxID_ANY, "");
    mainSizer->Add(statusText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    scheduleList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    scheduleList->AppendColumn("Code", wxLIST_FORMAT_LEFT, 100);
    scheduleList->AppendColumn("Title", wxLIST_FORMAT_LEFT, 280);
    scheduleList->AppendColumn("Credits", wxLIST_FORMAT_LEFT, 65);
    scheduleList->AppendColumn("Schedule", wxLIST_FORMAT_LEFT, 500);
    mainSizer->Add(scheduleList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    exportButton = new wxButton(this, wxID_ANY, "Export to CSV");
    exportButton->Enable(false);
    mainSizer->Add(exportButton, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    SetSizer(mainSizer);
    Bind(wxEVT_CHOICE, &OutputExportPanel::OnOptionChanged, this, optionChoice->GetId());
    exportButton->Bind(wxEVT_BUTTON, &OutputExportPanel::OnExport, this);
}

/** @brief Receives generated schedule options and refreshes the display. */
void OutputExportPanel::SetOptions(const Term& term, const std::vector<GeneratedTermScheduleResult>& generatedOptions) {
    options.clear();
    for (const auto& opt : generatedOptions) {
        if (!opt.selectedCourseIds.empty()) {
            options.push_back(opt);
            if (options.size() >= 3) break;
        }
    }
    if (options.empty()) {
        showEmpty();
        return;
    }
    currentTerm = term;
    termLabel->SetLabel("Term: " + wxString::FromUTF8(currentTerm.toString()));
    const char* labels[] = {"Option A", "Option B", "Option C"};
    optionChoice->Clear();
    for (std::size_t i = 0; i < options.size(); i++) optionChoice->Append(labels[i]);
    optionChoice->SetSelection(0);
    optionChoice->Enable(options.size() > 1);
    populateTable(0);
}

/** @brief Shows current options if available, otherwise resets to empty. */
void OutputExportPanel::RefreshView() {
    if (!options.empty()) return;
    showEmpty();
}

/** @brief Clears all displayed data and resets to the empty state. */
void OutputExportPanel::showEmpty() {
    options.clear();
    currentTerm = Term(Season::Fall, 0);
    termLabel->SetLabel("No schedule generated yet.");
    optionChoice->Clear();
    optionChoice->Enable(false);
    statusText->SetLabel("");
    scheduleList->DeleteAllItems();
    exportButton->Enable(false);
}

/** @brief Fills the schedule table with courses from the given option. */
void OutputExportPanel::populateTable(int optionIndex) {
    scheduleList->DeleteAllItems();
    if (options.empty() || optionIndex < 0 || static_cast<std::size_t>(optionIndex) >= options.size()) {
        statusText->SetLabel("No schedule generated yet.");
        exportButton->Enable(false);
        return;
    }
    const std::vector<std::string>& courseIds = options[static_cast<std::size_t>(optionIndex)].selectedCourseIds;
    int totalCourses = 0;
    double totalCredits = 0.0;
    long row = 0;
    for (const std::string& id : courseIds) {
        const Course* course = catalog->getCourse(id);
        const CourseOffering* offering = nullptr;
        if (course) {
            for (const CourseOffering& o : course->getOfferings()) {
                if (o.isOfferedIn(currentTerm)) {
                    offering = &o;
                    break;
                }
            }
        }
        scheduleList->InsertItem(row, course ? course->getCode() : id);
        scheduleList->SetItem(row, 1, course ? course->getTitle() : "(not in catalog)");
        scheduleList->SetItem(row, 2, course ? wxString::Format("%.1f", course->getCredits()) : "?");
        scheduleList->SetItem(row, 3, offering ? formatTimeSlots(*offering) : "TBA");
        if (course) {totalCourses++; totalCredits += course->getCredits();}
        row++;
    }
    if (totalCourses == 0) {
        statusText->SetLabel("No courses found in catalog for this option.");
        exportButton->Enable(false);
    } else {
        statusText->SetLabel(wxString::Format("%d course(s) | %.1f credits", totalCourses, totalCredits));
        exportButton->Enable(true);
    }
}

/** @brief Handles option dropdown selection changes. */
void OutputExportPanel::OnOptionChanged(wxCommandEvent&) {
    int sel = optionChoice->GetSelection();
    if (sel != wxNOT_FOUND) populateTable(sel);
}

/** @brief Exports the selected schedule option and remaining requirements to CSV. */
void OutputExportPanel::OnExport(wxCommandEvent&) {
    wxFileDialog dlg(this, "Export Schedule to CSV", "", "schedule.csv", "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK) return;
    std::ofstream file(dlg.GetPath().ToStdString());
    if (!file.is_open()) {
        wxMessageBox("Could not open file for writing.", "Export Failed", wxOK | wxICON_ERROR);
        return;
    }
    int sel = optionChoice->GetSelection();
    if (sel == wxNOT_FOUND || options.empty()) return;
    const std::vector<std::string>& courseIds = options[static_cast<std::size_t>(sel)].selectedCourseIds;
    const std::string termStr = currentTerm.toString();
    file << "Term,Code,Title,Credits,Schedule\n";
    for (const std::string& id : courseIds) {
        const Course* course = catalog->getCourse(id);
        if (!course) continue;
        const CourseOffering* offering = nullptr;
        for (const CourseOffering& o : course->getOfferings()) {
            if (o.isOfferedIn(currentTerm)) {
                offering = &o;
                break;
            }
        }
        file << "\"" << termStr << "\"," << course->getCode() << "," << "\"" << course->getTitle() << "\"," << wxString::Format("%.1f", course->getCredits()).ToStdString() << "," << "\"" << (offering ? formatTimeSlots(*offering) : "TBA") << "\"\n";
    }
    // Remaining graduation requirements
    std::string profPath = wxFileName(wxStandardPaths::Get().GetUserDataDir(), "profile.txt").GetFullPath().ToStdString();
    StudentProfile profile;
    if (profile.LoadFromFile(profPath)) {
        std::vector<std::string> missing = profile.GetMissingGraduationRequirements();
        file << "\nRemaining Requirements,,,,\n";
        file << "Code,Title,,,\n";
        if (missing.empty()) {
            file << "\"All graduation requirements met.\",,,,\n";
        } else {
            for (const std::string& code : missing) {
                const Course* c = catalog->getCourse(code);
                file << code << ",\"" << (c ? c->getTitle() : "") << "\",,,\n";
            }
        }
    }
    wxMessageBox("Schedule exported successfully.", "Export Complete", wxOK | wxICON_INFORMATION);
}
