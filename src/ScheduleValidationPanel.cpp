#include "ScheduleValidationPanel.h"

ScheduleValidationPanel::ScheduleValidationPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Schedule Validation Page");
    sizer->Add(title, 0, wxALL, 20);
    SetSizer(sizer);
}