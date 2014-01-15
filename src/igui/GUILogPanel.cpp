#include <GUILogPanel.h>
#include <wx/sizer.h>

#include <debugger.h> 

#define wxID_TEXTCTRL		100

using namespace core;
using namespace core::igui;

wxTextCtrl *GUILogPanel::log_ctrl=NULL;

BEGIN_EVENT_TABLE(GUILogPanel, wxPanel)
	EVT_PAINT	(					  GUILogPanel::OnPaint	)
END_EVENT_TABLE()

GUILogPanel::GUILogPanel(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxPanel(parent, id, pos, size, style, name), app_config(app_config_)
{
	std::stringstream s_image;
	if ( app_config != NULL )
		s_image << app_config->GetUIResourceDirectory() << "background.png";
	background_image = wxBitmap(s_image.str(), wxBITMAP_TYPE_ANY);
	
	wxBoxSizer *top_sizer = new wxBoxSizer( wxVERTICAL );

	log_ctrl    = new wxTextCtrl(this, wxID_TEXTCTRL, "", wxDefaultPosition, size, wxTE_READONLY | wxTE_MULTILINE );
	top_sizer->Add(log_ctrl, 1, wxEXPAND | wxALL, 10);
	
	this->SetSizer(top_sizer);

	PostMessage("SYSTEM ALIVE...");
	PostMessage("Loging stuff...");
}

GUILogPanel::~GUILogPanel()
{
}

void GUILogPanel::Delete()
{
	wxPanel::Destroy();
}

void GUILogPanel::PostMessage(const std::string &value)
{
	if ( log_ctrl != NULL )
		log_ctrl->AppendText(wxString(value + "\n"));
}

void GUILogPanel::OnPaint(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void GUILogPanel::render(wxDC& dc)
{
	dc.DrawBitmap(background_image, 0, -20, false );
}