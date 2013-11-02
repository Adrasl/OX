#include <GUIStart.h>
#include <igui/controllers/GUIGenericController.h>

#include <debugger.h> 

#define wxID_LOGIN		100
#define wxID_AUTO		101
#define wxID_CONFIGURE	102

using namespace core;
using namespace core::igui;

BEGIN_EVENT_TABLE(GUIStart, wxPanel)
	EVT_PAINT	(					  GUIStart::OnPaint	)
	EVT_BUTTON	( wxID_LOGIN		, OnLoginButton		)
	EVT_BUTTON	( wxID_AUTO			, OnAutoButton		)
	EVT_BUTTON	( wxID_CONFIGURE	, OnConfigureButton	)
END_EVENT_TABLE()

GUIStart::GUIStart(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxPanel(parent, id, pos, size, style, name), app_config(app_config_)
{
	std::stringstream s_image;
	if ( app_config != NULL )
		s_image << app_config->GetUIResourceDirectory() << "ProjectVOX.png";
	//background_image = wxBitmap("c:/etc/ProjectVOX.png", wxBITMAP_TYPE_ANY);
	background_image = wxBitmap(s_image.str(), wxBITMAP_TYPE_ANY);

	login_button = new wxButton(this, wxID_LOGIN, "Log in", wxPoint(185,240), wxSize(70, 19));
	start_button = new wxButton(this, wxID_AUTO, "AUTO", wxPoint(265,240), wxSize(70, 19));
	configure_button = new wxButton(this, wxID_CONFIGURE, "Configure", wxPoint(345,240), wxSize(70, 19));
}

GUIStart::~GUIStart()
{
}

void GUIStart::Delete()
{
	wxPanel::Destroy();
}

void GUIStart::OnPaint(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void GUIStart::render(wxDC& dc)
{
	dc.DrawBitmap(background_image, 0, -20, false );
}

void GUIStart::OnLoginButton(wxCommandEvent& WXUNUSED(event))
{
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc != NULL)
		guiGc->ViewUserInfoPanel();
}

void GUIStart::OnAutoButton(wxCommandEvent& WXUNUSED(event))
{
}

void GUIStart::OnConfigureButton(wxCommandEvent& WXUNUSED(event))
{
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc != NULL)
		guiGc->ViewConfigurePanel();
}