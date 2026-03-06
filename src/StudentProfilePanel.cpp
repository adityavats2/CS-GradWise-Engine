#include "StudentProfilePanel.h"

StudentProfilePanel::StudentProfilePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Student Profile Input Page");
    sizer->Add(title, 0, wxALL, 20);
    SetSizer(sizer);
}