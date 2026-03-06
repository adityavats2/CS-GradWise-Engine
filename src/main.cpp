#include <wx/wx.h>
#include "MainFrame.h"

/** @brief Main application class */
class MyApp : public wxApp {
public:
    bool OnInit() override {
        MainFrame* frame = new MainFrame("CS Gradwise");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);