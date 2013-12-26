#include <GUIHelp.h>

#include <debugger.h> 
#include <sstream>
#include <debugger.h> 

using namespace core;
using namespace core::igui;

BEGIN_EVENT_TABLE(GUIHelp, wxPanel)
	EVT_PAINT	 (					GUIHelp::OnPaint)
END_EVENT_TABLE()

GUIHelp::GUIHelp(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxPanel(parent, id, pos, size, style, name), app_config(app_config_)
{
	std::stringstream s_image;
	if ( app_config != NULL )
		s_image << app_config->GetUIResourceDirectory() << "help.png";
	background_image = wxBitmap(s_image.str(), wxBITMAP_TYPE_ANY);
}

GUIHelp::~GUIHelp()
{
}

void GUIHelp::Delete()
{
	wxPanel::Destroy();
}

void GUIHelp::OnPaint(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void GUIHelp::render(wxDC& dc)
{
	dc.DrawBitmap(background_image, 0, -20, false );
	dc.SetTextForeground(wxColour(255,255,255));
	dc.DrawRotatedText("Project OX", 250, 30, 0);
	//dc.DrawRotatedText("www._______________.com", 250, 50, 0);
	dc.DrawRotatedText("Antonio José Sánchez López", 250, 70, 0);
	dc.DrawRotatedText("Facultad de Informática", 250, 90, 0);
	dc.DrawRotatedText("Universidad de Las Palmas de Gran Canaria", 250, 110, 0);
	dc.DrawRotatedText("Version 0.1", 250, 130, 0);
	dc.DrawRotatedText("@antoniojotasl", 250, 150, 0);
}