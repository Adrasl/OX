#include <MainFrame.h>

#include <debugger.h> 

#define FILE_AUTO		1000
#define FILE_LOGIN		1001
#define FILE_LOGOUT		1002
#define FILE_CLOSE		1003
#define VIEW_FULLSCREEN 2000
#define VIEW_START		2001
#define VIEW_USER		2002
#define VIEW_LOG_PANEL	2003
#define TOOLS_CONFIGURE	3000
#define HELP_ABOUT		4000

using namespace core::igui;

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU	 ( FILE_AUTO,		MainFrame::OnFileAuto			)
	EVT_MENU	 ( FILE_LOGIN,		MainFrame::OnFileLogin			)
	EVT_MENU	 ( FILE_LOGOUT,		MainFrame::OnFileLogout			)
	EVT_MENU	 ( FILE_CLOSE,		MainFrame::OnFileClose			)
	EVT_MENU	 ( VIEW_FULLSCREEN, MainFrame::OnViewFullScreen		)
	EVT_MENU	 ( VIEW_START,		MainFrame::OnViewStart			)
	EVT_MENU	 ( VIEW_USER,		MainFrame::OnViewUser			)
	EVT_MENU	 ( VIEW_LOG_PANEL,	MainFrame::OnViewLogPanel		)
	EVT_MENU	 ( VIEW_START,		MainFrame::OnViewStart			)
	EVT_MENU	 ( TOOLS_CONFIGURE,	MainFrame::OnToolsConfigure		)
	EVT_MENU	 ( HELP_ABOUT,		MainFrame::OnHelpAbout			)
END_EVENT_TABLE()

MainFrame::MainFrame(MainGui *maingui, wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxFrame(parent, id, title, pos, size, style, name), main_gui(maingui)
{
	Center();
	wxInitAllImageHandlers();

	IApplicationConfiguration *appc = NULL;
	if (main_gui != NULL) appc = main_gui->GetAppConfig();

	wxMenuBar *menu_bar = new wxMenuBar();

	wxSize client_size = this->GetClientSize();

	dummy_panel = new wxWindow(this, wxID_ANY, wxDefaultPosition, client_size);
	dummy_panel->Show(false);

	log_panel   = new GUILogPanel(appc, dummy_panel, wxID_ANY, wxDefaultPosition, client_size);
	help_panel  = new GUIHelp	 (appc, dummy_panel, wxID_ANY, wxDefaultPosition, client_size);
	start_panel = new GUIStart	 (appc, dummy_panel, wxID_ANY, wxDefaultPosition, client_size);
	user_panel  = new GUIUser	 (appc, dummy_panel, wxID_ANY, wxDefaultPosition, client_size);
	userinfo_panel  = new GUIUserInfo (appc, dummy_panel, wxID_ANY, wxDefaultPosition, client_size);
	configure_panel = new GUIConfiguration(appc, dummy_panel, wxID_ANY, wxDefaultPosition, client_size);
	//configure_panel->SetAppConfig(core::igui::MainGui::GetInstance()->GetAppConfig());
	start_panel->Reparent(this);

	file_menu = new wxMenu;
	item_file_auto  = new wxMenuItem(file_menu,FILE_AUTO,  wxString("Auto"),wxString("Auto"),false);
	item_file_login = new wxMenuItem(file_menu,FILE_LOGIN, wxString("Log in"),wxString("Log in"),false);
	item_file_logout= new wxMenuItem(file_menu,FILE_LOGOUT,wxString("Log out"),wxString("Log out"),false);
	item_file_close = new wxMenuItem(file_menu,FILE_CLOSE, wxString("Close"),wxString("Close"),false);
	file_menu->Append(item_file_auto);
	file_menu->Append(item_file_login);
	file_menu->Append(item_file_logout);
	file_menu->Append(item_file_close);
	file_menu->UpdateUI();

	view_menu = new wxMenu;
	item_view_start		 = new wxMenuItem(view_menu,VIEW_START,wxString("Start Page"),wxString("Start Page"),false);
	item_view_user		 = new wxMenuItem(view_menu,VIEW_USER,wxString("User Page"),wxString("User Page"),false);
	item_view_log_panel	 = new wxMenuItem(view_menu,VIEW_LOG_PANEL,wxString("Log Page"),wxString("Log Page"),false);
	item_view_fullscreen = new wxMenuItem(view_menu,VIEW_FULLSCREEN,wxString("FullScreen"),wxString("Toggle fullscreen"),false);
	view_menu->Append(item_view_start);
	view_menu->Append(item_view_user);
	//view_menu->Append(item_view_log_panel);
	view_menu->Append(item_view_fullscreen);
	view_menu->UpdateUI();

	tools_menu = new wxMenu;
	item_tools_configure = new wxMenuItem(view_menu,TOOLS_CONFIGURE,wxString("Configure"),wxString("Configure"),false);
	tools_menu->Append(item_tools_configure);
	tools_menu->Append(item_view_log_panel);
	tools_menu->UpdateUI();

	help_menu = new wxMenu;
	item_help_about = new wxMenuItem(view_menu,HELP_ABOUT,wxString("About"),wxString("About"),false);
	help_menu->Append(item_help_about);
	help_menu->UpdateUI();
	
	menu_bar->Append(file_menu, wxString("File"));
	menu_bar->Append(view_menu, wxString("View"));
	menu_bar->Append(tools_menu,wxString("Tools"));
	menu_bar->Append(help_menu, wxString("Help"));

	SetMenuBar(menu_bar);
	menu_bar->Refresh();

	InitShortCuts();
}

MainFrame::~MainFrame()
{
}

void MainFrame::InitShortCuts()
{
	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_NORMAL, WXK_F11, VIEW_FULLSCREEN);
	wxAcceleratorTable accel(1, entries);
	this->SetAcceleratorTable(accel);
}


void MainFrame::OnViewFullScreen(wxCommandEvent& WXUNUSED(event))
{
	bool value = this->IsFullScreen();
	if (main_gui != NULL)
		main_gui->SetAllWindowsFullScreen(!value);
	//core::igui::MainGui::GetInstance()->ShowAll(!value);
}

void MainFrame::Delete()
{
	wxFrame::Destroy();
}

void MainFrame::DismissPanels()
{
	start_panel->Reparent(dummy_panel);
	help_panel->Reparent(dummy_panel);
	log_panel->Reparent(dummy_panel);
	user_panel->Reparent(dummy_panel);
	userinfo_panel->Reparent(dummy_panel);
	configure_panel->Reparent(dummy_panel);
}

void MainFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
	PostLogMessage("Bye bye...");
	wxFrame::Close();
}

void MainFrame::OnFileAuto(wxCommandEvent& WXUNUSED(event))
{
	Refresh();
	Update();
}

void MainFrame::OnFileLogin(wxCommandEvent& WXUNUSED(event))
{
	DismissPanels();
	if (user_panel->IsUserLogged())
		userinfo_panel->Reparent(this);
	else
		user_panel->Reparent(this);
	Refresh();
	Update();
}

void MainFrame::LogOut()
{
	//DismissPanels();
	userinfo_panel->Clear();
	user_panel->Clear();
	ViewUserInfo();
	Refresh();
	Update();
}

void MainFrame::OnFileLogout(wxCommandEvent& WXUNUSED(event))
{
	userinfo_panel->DoLogOut();
	LogOut();
}

void MainFrame::OnViewStart(wxCommandEvent& WXUNUSED(event))
{
	DismissPanels();
	start_panel->Reparent(this);
	Refresh();
	Update();
}

void MainFrame::OnViewUser(wxCommandEvent& WXUNUSED(event))
{
	ViewUserInfo();
}

void MainFrame::ViewUserInfo()
{
	DismissPanels();
	if (user_panel->IsUserLogged())
	{
		GUIGenericController *guiGc	= GUIGenericController::GetInstance();
		if (guiGc != NULL)
			userinfo_panel->SetUser(guiGc->GetCurrentUser());
		userinfo_panel->Reparent(this);
	}
	else
		user_panel->Reparent(this);
	Refresh();
	Update();
}

void MainFrame::ViewLog()
{
	DismissPanels();
	log_panel->Reparent(this);
	Refresh();
	Update();
}

void MainFrame::OnViewLogPanel(wxCommandEvent& WXUNUSED(event))
{
	ViewLog();
}

void MainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
	DismissPanels();
	help_panel->Reparent(this);
	Refresh();
	Update();
}

void MainFrame::ViewConfigure()
{
	DismissPanels();
	configure_panel->Reparent(this);
	Refresh();
	Update();
}

void MainFrame::OnToolsConfigure(wxCommandEvent& WXUNUSED(event))
{
	ViewConfigure();
}

void MainFrame::PostLogMessage(const std::string &message)
{
	if (log_panel != NULL)
		log_panel->PostMessage(message);
}

void MainFrame::SetFace(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step)
{
	if (user_panel && user_panel->IsShown())
		user_panel->SetFace(data, size_x, size_y, n_channels, depth, width_step);
}

void MainFrame::FillLoginUserGUI(const std::string username, const std::string userpasswd)
{
	if (user_panel)
		user_panel->FillLoginUserGUI(username, userpasswd);
}
void MainFrame::FillNewUserGUI(const std::string username, const std::string userpasswd)
{
	if (user_panel)
		user_panel->FillNewUserGUI(username, userpasswd);
}

void MainFrame::LogIn(const std::string &name, const std::string &passwd)
{
	if (user_panel)
		user_panel->DoLogin(name, passwd);
}