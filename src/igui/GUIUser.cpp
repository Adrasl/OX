#include <GUIUser.h>
#include <igui/controllers/GUIGenericController.h>

#include <debugger.h> 

#define wxID_LOGIN		100
#define wxID_REGISTER	101
#define wxID_USER_NAME	201
#define wxID_USER_PASS	202
#define wxID_NEW_USER_NAME	203
#define wxID_NEW_USER_PASS	204
#define wxID_NEW_USER_PASS2	204
#define wxID_DELETE		300

using namespace core;
using namespace core::igui;

BEGIN_EVENT_TABLE(GUIUser, wxPanel)
	EVT_PAINT	(						GUIUser::OnPaint		)
	EVT_BUTTON	( wxID_LOGIN			, OnLoginButton			)
	EVT_BUTTON	( wxID_REGISTER			, OnNewUserButton		)
	//EVT_BUTTON	( wxID_DELETE			, OnDeleteUserButton	)
	//EVT_BUTTON	( wxID_CONFIGURE	, OnConfigureButton	)
END_EVENT_TABLE()

GUIUser::GUIUser(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxPanel(parent, id, pos, size, style, name), user_logged_in(false), app_config(app_config_), face_detected(false)
{
	std::string uiresource_dir = "";
	if ( app_config != NULL )
		uiresource_dir = app_config->GetUIResourceDirectory();
	
	std::stringstream ss_image, ss_no_face_bitmap;
	ss_image << uiresource_dir << "default_backgr.png";
	ss_no_face_bitmap << uiresource_dir << "no_face.png";
	background_image = wxBitmap(ss_image.str(), wxBITMAP_TYPE_ANY);
	no_face_bitmap   = new wxBitmap(ss_no_face_bitmap.str(), wxBITMAP_TYPE_ANY);

	int col, row;
	col = 80, row = 40;
	user_name	 = new wxTextCtrl(this, wxID_ANY, "test1", wxPoint(col,row), wxDefaultSize);
	user_passwd  = new wxTextCtrl(this, wxID_ANY, "111", wxPoint(col,row+25), wxDefaultSize, wxTE_PASSWORD);
	login_button = new wxButton(this, wxID_LOGIN, "Log in", wxPoint(col,row+50), wxSize(70, 19));
	//delete_button = new wxButton(this, wxID_DELETE, "Delete", wxPoint(col,row+70), wxSize(70, 19));

	col = 320, row = 40;
	new_user_name	 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row), wxDefaultSize);
	new_user_passwd  = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row+25), wxDefaultSize, wxTE_PASSWORD);	
	new_user_passwd2 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row+50), wxDefaultSize, wxTE_PASSWORD);
	register_button  = new wxButton(this, wxID_REGISTER, "Register", wxPoint(col,row+75), wxSize(70, 19));
}

void GUIUser::Clear()
{
	user_name->Clear();
	user_passwd->Clear();
	new_user_name->Clear();
	new_user_passwd->Clear();
	new_user_passwd2->Clear();
	s_user_name = "";
	s_user_password = "";
	user_logged_in = false;
}

GUIUser::~GUIUser()
{
	if ( no_face_bitmap ) delete no_face_bitmap;
}

void GUIUser::Delete()
{
	wxPanel::Destroy();
}

void GUIUser::OnPaint(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void GUIUser::render(wxDC& dc)
{
	dc.DrawBitmap(background_image, 0, -20, false );
	dc.DrawRotatedText("Login", 80, 20, 0);
	dc.DrawRotatedText("Name", 20, 45, 0);
	dc.DrawRotatedText("Password", 20, 70, 0);
	dc.DrawRotatedText("New User", 320, 20, 0);
	dc.DrawRotatedText("Name", 220, 45, 0);
	dc.DrawRotatedText("Password", 220, 70, 0);
	dc.DrawRotatedText("Retype password", 220, 95, 0);

	if (!face_detected)
		dc.DrawBitmap( (*no_face_bitmap), 200, 150 );
	else
		dc.DrawBitmap( face_bitmap, 200, 150 );
}
void GUIUser::DoLogout()
{
	user_logged_in = false;
	Clear();
}
void GUIUser::DoLogin(const std::string &name, const std::string &passwd)
{
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	bool login = false;
	if (guiGc != NULL)
		login = guiGc->LoginUser(name, passwd);
	if (login)
	{
		//DoLogin
		user_logged_in = true;
		guiGc->ViewUserInfoPanel();
	}
	else
	{
		user_logged_in = false;
		wxMessageDialog message_dialog(this, _("User/password incorrect"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		message_dialog.ShowModal();
	}
}
void GUIUser::OnLoginButton(wxCommandEvent& WXUNUSED(event))
{
	s_user_name = user_name->GetValue();
	s_user_password = user_passwd->GetValue();
	DoLogin(s_user_name, s_user_password);
}

void GUIUser::OnNewUserButton(wxCommandEvent& WXUNUSED(event))
{
	s_user_name = new_user_name->GetValue();
	std::string pass = new_user_passwd->GetValue();
	std::string pass2 = new_user_passwd2->GetValue();
	s_user_password = ( pass == pass2 ) ? pass : "";
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	bool exists = false;
	if (guiGc != NULL)
		exists = guiGc->UserExists(s_user_name);
	if  (exists)
	{
		wxMessageDialog message_dialog(this, _("User already exists"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		message_dialog.ShowModal();
	}
	else if (s_user_password != "")
	{
		bool created = guiGc->CreateUser(s_user_name, s_user_password);
		if (created) 
			DoLogin(s_user_name, s_user_password);
		else 
		{
			wxMessageDialog message_dialog(this, _("Could not create user"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
			message_dialog.ShowModal();
		}
	}
	else 
	{
		wxMessageDialog message_dialog(this, _("Passwords void or missmatch"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		message_dialog.ShowModal();
	}
}

void GUIUser::SetFace(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step)
{
	if (data)
	{	wxImage new_image = wxImage( size_x, size_y, (unsigned char*)data, true );
		face_bitmap = wxBitmap( new_image.Scale(100, 100, wxIMAGE_QUALITY_HIGH) );
		//face_bitmap = wxBitmap( new_image );
		face_detected = true;
	} else 
		face_detected = false;
	Refresh();
	Update();

}

//void GUIUser::OnDeleteUserButton(wxCommandEvent& WXUNUSED(event))
//{
//	s_user_name = user_name->GetValue();
//	s_user_password = user_passwd->GetValue();
//	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
//	bool deleted = false;
//	if (guiGc != NULL)
//		deleted = guiGc->DeleteUser(name, passwd);
//	if (!deleted)
//	{
//		wxMessageDialog message_dialog(this, _("Could not delete user: user/password incorrect."), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
//		message_dialog.ShowModal();
//	}
//
//}

void GUIUser::FillLoginUserGUI(const std::string username, const std::string userpasswd)
{
	if (user_name)
		user_name->ChangeValue(username);
	if (user_passwd)
		user_passwd->ChangeValue(userpasswd);	
}
void GUIUser::FillNewUserGUI(const std::string username, const std::string userpasswd)
{
	if (new_user_name)
		new_user_name->ChangeValue(username);
	if (new_user_passwd)
		new_user_passwd->ChangeValue(userpasswd);	
	if (new_user_passwd2)
		new_user_passwd2->ChangeValue(userpasswd);	
}
