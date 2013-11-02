#include <GUIUserInfo.h>
#include <igui/controllers/GUIGenericController.h>

#include <debugger.h> 

#define wxID_RUN		100
#define wxID_LOGOUT		101
#define wxID_WORLD_LIST	102
#define wxID_NEWWORLD	103
#define wxID_DELETE_USER	300
#define wxID_DELETE_WORLD	301
#define wxID_SAVE_W_PERM	400
//#define wxID_NEW_USER_NAME	203
//#define wxID_NEW_USER_PASS	204
//#define wxID_NEW_USER_PASS2	204

using namespace core;
using namespace core::igui;

BEGIN_EVENT_TABLE(GUIUserInfo, wxPanel)
	EVT_PAINT	(						  OnPaint				)
	EVT_BUTTON	( wxID_RUN				, OnRunButton			)
	EVT_BUTTON	( wxID_LOGOUT			, OnLogOutButton		)
	EVT_BUTTON	( wxID_NEWWORLD			, OnNewWorldButton		)
	EVT_BUTTON	( wxID_DELETE_USER		, OnDeleteUserButton	)
	EVT_BUTTON	( wxID_DELETE_WORLD		, OnDeleteWorldButton	)
	EVT_BUTTON	( wxID_SAVE_W_PERM		, OnSaveWorldPermissions)
	EVT_LISTBOX ( wxID_WORLD_LIST		, OnWorldSelected		)
	EVT_CHECKBOX( wxID_ANY				, CheckPermissions		)
END_EVENT_TABLE()

GUIUserInfo::GUIUserInfo(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxPanel(parent, id, pos, size, style, name), user(NULL), s_world_permissions(127), app_config(app_config_)
{
	std::stringstream s_image;
	if ( app_config != NULL )
		s_image << app_config->GetUIResourceDirectory() << "default_backgr.png";
	background_image = wxBitmap(s_image.str(), wxBITMAP_TYPE_ANY);

	int col, row;
	col = 20, row = 20;
	world_list = new wxListBox(this, wxID_WORLD_LIST, wxPoint(col, row+25), wxSize(150, 200), wx_world_names, wxLB_SINGLE | wxLB_SORT);

	col = 20, row = 260;

	run_button = new wxButton(this, wxID_RUN, _("RUN"), wxPoint(col,row), wxSize(70, 19));
	logout_button  = new wxButton(this, wxID_LOGOUT, _("Log out"), wxPoint(col+80,row), wxSize(70, 19));
	delete_world_button  = new wxButton(this, wxID_DELETE_WORLD, _("Delete world"), wxPoint(col+160,row), wxSize(70, 19));
	delete_user_button  = new wxButton(this, wxID_DELETE_USER, _("Delete user"), wxPoint(col+240,row), wxSize(70, 19));
	
	col = 350; row = 180;
	save_permissions_to_selected_world_button  = new wxButton(this, wxID_SAVE_W_PERM, _("Save Current"), wxPoint(col,row), wxSize(70, 19));
	world_name	 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row+30), wxSize(70, 19));
	newworld_button  = new wxButton(this, wxID_NEWWORLD, _("New world"), wxPoint(col,row+50), wxSize(70, 19));

	col = 480; row = 25;
	userflag_itself_modify_psi			= new wxCheckBox(this, wxID_ANY, _("Automodify psique"), wxPoint(col, row));
	userflag_owner_actions_modify_psi	= new wxCheckBox(this, wxID_ANY, _("Actions mod. psique"), wxPoint(col, row+20));
	userflag_owner_see		= new wxCheckBox(this, wxID_ANY, _("Owner can see"), wxPoint(col, row+40));
	userflag_owner_listen	= new wxCheckBox(this, wxID_ANY, _("Owner can listen"), wxPoint(col, row+60));
	userflag_owner_touch	= new wxCheckBox(this, wxID_ANY, _("Owner can touch"), wxPoint(col, row+80));
	userflag_owner_pick		= new wxCheckBox(this, wxID_ANY, _("Owner can pick"), wxPoint(col, row+100));
	userflag_owner_create	= new wxCheckBox(this, wxID_ANY, _("Owner can create"), wxPoint(col, row+120));
	userflag_owner_destroy	= new wxCheckBox(this, wxID_ANY, _("Owner can destroy"), wxPoint(col, row+140));

	col = 350; row = 25;
	worldflag_itself_modify_psi			= new wxCheckBox(this, wxID_ANY, _("Automodify psique"), wxPoint(col, row));
	worldflag_owner_actions_modify_psi	= new wxCheckBox(this, wxID_ANY, _("Actions mod. psique"), wxPoint(col, row+20));
	worldflag_owner_see		= new wxCheckBox(this, wxID_ANY, _("Owner can see"), wxPoint(col, row+40));
	worldflag_owner_listen	= new wxCheckBox(this, wxID_ANY, _("Owner can listen"), wxPoint(col, row+60));
	worldflag_owner_touch	= new wxCheckBox(this, wxID_ANY, _("Owner can touch"), wxPoint(col, row+80));
	worldflag_owner_pick		= new wxCheckBox(this, wxID_ANY, _("Owner can pick"), wxPoint(col, row+100));
	worldflag_owner_create	= new wxCheckBox(this, wxID_ANY, _("Owner can create"), wxPoint(col, row+120));
	worldflag_owner_destroy	= new wxCheckBox(this, wxID_ANY, _("Owner can destroy"), wxPoint(col, row+140));

}

void GUIUserInfo::Clear()
{
	world_names.clear();
	world_permissions.clear();
	wx_world_names.clear();
	world_list->Clear();
	world_name->Clear();
	user = NULL;
}

GUIUserInfo::~GUIUserInfo()
{
}
void GUIUserInfo::SetUser(core::IUserPersistence* user_)
{
	if (user_ != NULL)
	{
		user = user_;
		user_name = user->GetName();
		s_user_permissions = user->GetPermissions();

		world_names.clear();
		world_permissions.clear();
		wx_world_names.clear();

		GUIGenericController *guiGc	= GUIGenericController::GetInstance();
		if (guiGc != NULL)
			guiGc->GetWorldList(user_name, world_names, world_permissions);

		int size = world_names.size();
		if (size == world_permissions.size())
		{
			for (int i = 0; i< size; i++)
				wx_world_names.Add(world_names[i]);
			world_list->Set(wx_world_names);
		}

		Freeze();
		if ( (s_user_permissions & PERM_PSI_ITSELF_MODIFY) == PERM_PSI_ITSELF_MODIFY)
			userflag_itself_modify_psi->SetValue(true);
		if ( (s_user_permissions & PERM_PSI_OWNER_MODIFY) == PERM_PSI_OWNER_MODIFY)
			userflag_owner_actions_modify_psi->SetValue(true);
		if ( (s_user_permissions & PERM_OWNER_SEE) == PERM_OWNER_SEE)
			userflag_owner_see->SetValue(true);
		if ( (s_user_permissions & PERM_OWNER_TOUCH) == PERM_OWNER_TOUCH)
			userflag_owner_touch->SetValue(true);
		if ( (s_user_permissions & PERM_OWNER_PICK) == PERM_OWNER_PICK)
			userflag_owner_pick->SetValue(true);
		if ( (s_user_permissions & PERM_OWNER_CREATE) == PERM_OWNER_CREATE)
			userflag_owner_create->SetValue(true);
		if ( (s_user_permissions & PERM_OWNER_DESTROY) == PERM_OWNER_DESTROY)
			userflag_owner_destroy->SetValue(true);
		Thaw();
		Refresh();
		Update();

	}
}
void GUIUserInfo::Delete()
{
	wxPanel::Destroy();
}


void GUIUserInfo::DoLogOut()
{
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc != NULL)
		guiGc->LogOut();
}

void GUIUserInfo::OnPaint(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void GUIUserInfo::render(wxDC& dc)
{
	dc.DrawBitmap(background_image, 0, -20, false );
	dc.DrawRotatedText(_("User:"), 20, 10, 0); dc.DrawRotatedText(user_name, 60, 10, 0);
	dc.DrawRotatedText(_("Worlds"), 20, 30, 0);
	dc.DrawRotatedText(_("User flags"), 480, 10, 0);
	dc.DrawRotatedText(_("World flags"), 370, 10, 0);
}

void GUIUserInfo::OnRunButton(wxCommandEvent& WXUNUSED(event))
{
	DoRun();
}

void GUIUserInfo::OnLogOutButton(wxCommandEvent& WXUNUSED(event))
{
	DoLogOut();
}

void GUIUserInfo::DoRun()
{
	int selected = world_list->GetSelection();
	std::string selected_world = world_list->GetString(selected);

	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	bool run = false;
	if (guiGc != NULL)
	{
		run = guiGc->RunWorld(selected_world);
		if (run) guiGc->ViewLogPanel();
	}
	if  (!run)
	{
		wxMessageDialog message_dialog(this, _("Could not run world"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		message_dialog.ShowModal();
	}

	ReloadUserInfo();
}

void GUIUserInfo::OnNewWorldButton(wxCommandEvent& WXUNUSED(event))
{
	std::string s_new_world = world_name->GetValue();
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	bool exists = false;
	if (guiGc != NULL)
		exists = guiGc->WorldExists(s_new_world);
	if  (exists)
	{
		wxMessageDialog message_dialog(this, _("World already exists"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		message_dialog.ShowModal();
	}
	else 
	{
		bool created = guiGc->CreateWorld(s_new_world, user_name, s_world_permissions);
		if (created) 
			ReloadUserInfo();
		else 
		{
			wxMessageDialog message_dialog(this, _("Could not create world"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
			message_dialog.ShowModal();
		}
	}


}

void GUIUserInfo::OnSaveWorldPermissions(wxCommandEvent& WXUNUSED(event))
{
	int selected = world_list->GetSelection();
	std::string selected_world = world_list->GetString(selected);

	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	bool saved = false;
	if (guiGc != NULL)
		saved = guiGc->SaveWorldPermissions(selected_world, s_world_permissions);
	if  (!saved)
	{
		wxMessageDialog message_dialog(this, _("Could not save world permissions"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		message_dialog.ShowModal();
	}
	ReloadUserInfo();
}

void GUIUserInfo::OnWorldSelected(wxCommandEvent& WXUNUSED(event))
{
	s_world_permissions = 0;
	int selected = world_list->GetSelection();
	std::string selected_world = world_list->GetString(selected);

	for (unsigned int i = 0; i < world_names.size(); i++)
	{
		if (world_names[i] == selected_world)
			s_world_permissions = world_permissions[i];
	}

	Freeze();
	worldflag_itself_modify_psi->SetValue       ((s_world_permissions & PERM_PSI_ITSELF_MODIFY) == PERM_PSI_ITSELF_MODIFY );
	worldflag_owner_actions_modify_psi->SetValue((s_world_permissions & PERM_PSI_OWNER_MODIFY) == PERM_PSI_OWNER_MODIFY   );
	worldflag_owner_see->SetValue    ((s_world_permissions & PERM_OWNER_SEE)     == PERM_OWNER_SEE     );
	worldflag_owner_listen->SetValue ((s_world_permissions & PERM_OWNER_LISTEN)  == PERM_OWNER_LISTEN  );
	worldflag_owner_touch->SetValue  ((s_world_permissions & PERM_OWNER_TOUCH)   == PERM_OWNER_TOUCH   );
	worldflag_owner_pick->SetValue   ((s_world_permissions & PERM_OWNER_PICK)    == PERM_OWNER_PICK    );
	worldflag_owner_create->SetValue ((s_world_permissions & PERM_OWNER_CREATE)  == PERM_OWNER_CREATE  );
	worldflag_owner_destroy->SetValue((s_world_permissions & PERM_OWNER_DESTROY) == PERM_OWNER_DESTROY );
	Thaw();
	Refresh();
	Update();
}

void GUIUserInfo::CheckPermissions(wxCommandEvent& WXUNUSED(event))
{
	s_user_permissions  = 0;
	s_world_permissions = 0;

	if (userflag_itself_modify_psi->GetValue())
		s_user_permissions = s_user_permissions | PERM_PSI_ITSELF_MODIFY;
	if (userflag_owner_actions_modify_psi->GetValue())
		s_user_permissions = s_user_permissions | PERM_PSI_OWNER_MODIFY;
	if (userflag_owner_see->GetValue())
		s_user_permissions = s_user_permissions | PERM_OWNER_SEE;
	if (userflag_owner_listen->GetValue())
		s_user_permissions = s_user_permissions | PERM_OWNER_LISTEN;
	if (userflag_owner_touch->GetValue())
		s_user_permissions = s_user_permissions | PERM_OWNER_TOUCH;
	if (userflag_owner_pick->GetValue())
		s_user_permissions = s_user_permissions | PERM_OWNER_PICK;	
	if (userflag_owner_create->GetValue())
		s_user_permissions = s_user_permissions | PERM_OWNER_CREATE;	
	if (userflag_owner_destroy->GetValue())
		s_user_permissions = s_user_permissions | PERM_OWNER_DESTROY;	

	if (worldflag_itself_modify_psi->GetValue())
		s_world_permissions = s_world_permissions | PERM_PSI_ITSELF_MODIFY;
	if (worldflag_owner_actions_modify_psi->GetValue())
		s_world_permissions = s_world_permissions | PERM_PSI_OWNER_MODIFY;
	if (worldflag_owner_see->GetValue())
		s_world_permissions = s_world_permissions | PERM_OWNER_SEE;
	if (worldflag_owner_listen->GetValue())
		s_world_permissions = s_world_permissions | PERM_OWNER_LISTEN;
	if (worldflag_owner_touch->GetValue())
		s_world_permissions = s_world_permissions | PERM_OWNER_TOUCH;
	if (worldflag_owner_pick->GetValue())
		s_world_permissions = s_world_permissions | PERM_OWNER_PICK;	
	if (worldflag_owner_create->GetValue())
		s_world_permissions = s_world_permissions | PERM_OWNER_CREATE;	
	if (worldflag_owner_destroy->GetValue())
		s_world_permissions = s_world_permissions | PERM_OWNER_DESTROY;	

	SaveUserPermissions();
		
}

void GUIUserInfo::OnDeleteUserButton(wxCommandEvent& WXUNUSED(event))
{
	wxMessageDialog message_dialog(this, _("Are you sure? (Enter passwrod)"), "Message box", wxOK | wxCANCEL | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
	if ( message_dialog.ShowModal() == wxID_OK )	
	{
		GUIGenericController *guiGc	= GUIGenericController::GetInstance();
		bool deleted = false;
		if (guiGc != NULL)
			deleted = guiGc->DeleteUser(user_name);
		if (deleted)
		{
			DoLogOut();
		}
		else
		{	wxMessageDialog message_dialog(this, _("Could not delete user"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
			message_dialog.ShowModal();		}
	}
}

void GUIUserInfo::OnDeleteWorldButton(wxCommandEvent& WXUNUSED(event))
{
	wxMessageDialog message_dialog(this, _("Are you sure? (Enter passwrod)"), "Message box", wxOK | wxCANCEL | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
	if ( message_dialog.ShowModal() == wxID_OK )	
	{
		int w_selected = world_list->GetSelection();
		std::string w_selected_name = world_list->GetString(w_selected);
		
		GUIGenericController *guiGc	= GUIGenericController::GetInstance();
		bool deleted = false;
		if (guiGc != NULL)
			deleted = guiGc->DeleteWorld(w_selected_name);
		if (deleted)
			ReloadUserInfo();
		else
		{	wxMessageDialog message_dialog(this, _("Could not delete world"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
			message_dialog.ShowModal();		}
	}
}

