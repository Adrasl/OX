#include <maingui.h>

#include <debugger.h> 

#define ID_MAINFRAME 1000
#define ID_VIEWER    1001

using namespace core;
using namespace core::igui;

MainGui* MainGui::instance = NULL; 
boost::mutex MainGui::m_mutex;
MainFrame* MainGui::main_frame = NULL;
//wxWindow* MainGui::viewer_win;
int MainGui::last_win_id = 0;
//std::map<int, IGuiWindow*> MainGui::registered_windows;
std::map<core::IGuiWindow*, int> MainGui::registered_windows;
std::map<core::IGuiWindow*, int> MainGui::fullscreenable_windows;

MainGui::MainGui(IApplicationConfiguration *app_config_, const std::string &title): app_config(app_config_)
{
	generic_controller = new GUIGenericController(this);
	main_frame = new MainFrame(this, NULL, wxID_ANY, wxString(title),wxDefaultPosition, wxSize(601,342));
	main_frame->Show();
}

MainGui::~MainGui()
{
	if ( generic_controller )  delete generic_controller;
	//ReleaseInstance();
}

void MainGui::Delete()
{
	ReleaseInstance();
}

void MainGui::Update()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (main_frame)
	{
		main_frame->Refresh();
		main_frame->Update();
	}
}

MainGui* MainGui::GetInstance(IApplicationConfiguration *app_config_, const std::string &title)
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (instance == NULL) 
		instance = new MainGui(app_config_, title);
	return instance;
}

void MainGui::ReleaseInstance()
{
	//boost::mutex::scoped_lock lock(m_mutex);
	//if (instance != NULL)
	//{
	//	main_frame->Delete();
	//	delete instance;
	//	instance = NULL;
	//}
}

void MainGui::RegisterWindow(core::IGuiWindow *window)
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::map<core::IGuiWindow*, int>::iterator iter = registered_windows.find(window);
	if (iter == registered_windows.end())
		registered_windows[window]=++last_win_id;
}

void MainGui::AddWindowToFullscreenList(core::IGuiWindow *window)
{
	boost::mutex::scoped_lock lock(m_mutex);
	int new_id;
	std::map<core::IGuiWindow*, int>::iterator iter = registered_windows.find(window);
	if (iter == registered_windows.end())
	{
		new_id = ++last_win_id;
		registered_windows[window] = new_id;
	}
	else
		new_id = iter->second;

	std::map<core::IGuiWindow*, int>::iterator iter_fulls = fullscreenable_windows.find(window);
	if (iter_fulls == fullscreenable_windows.end())
		fullscreenable_windows[window] = new_id;
}

void MainGui::ShowAll(bool value)
{
	boost::mutex::scoped_lock lock(m_mutex);
	for (std::map<core::IGuiWindow*, int>::iterator i = registered_windows.begin(); i != registered_windows.end(); i++)
		i->first->Show(value);
}

void MainGui::SetAllWindowsFullScreen(bool value)
{
	boost::mutex::scoped_lock lock(m_mutex);
	for (std::map<core::IGuiWindow*, int>::iterator i = fullscreenable_windows.begin(); i != fullscreenable_windows.end(); i++)
		i->first->FullScreen(value);
}

void MainGui::PostLogMessage(const std::string &message)
{
	if ( main_frame!= NULL )
		main_frame->PostLogMessage(message);
}

bool MainGui::CreateUser(const std::string &name, const std::string &passwd)
{
	if ( app!= NULL )
		return app->CreateUser(name, passwd);
	return false;
}

bool MainGui::DeleteUser(const std::string &name)
{
	if ( app!= NULL )
		return app->DeleteUser(name);
	return false;
}

bool MainGui::DeleteWorld(const std::string &name)
{
	if ( app!= NULL )
		return app->DeleteWorld(name);
	return false;
}

bool MainGui::CreateWorld(const std::string &name, const std::string &owner, const int &permissions)
{
	if ( app!= NULL )
		return app->CreateWorld(name, owner, permissions);
	return false;
}

bool MainGui::SaveWorldPermissions(const std::string &name, const int permissions)
{
	if ( app!= NULL )
		return app->SaveWorldPermissions(name, permissions);
	return false;
}

bool MainGui::LoginUser(const std::string &name, const std::string &passwd)
{
	if ( app!= NULL )
		return app->LoginUser(name, passwd);
	return false;
}

void MainGui::LogOut()
{
	if ( app!= NULL )
		app->LogOut();
	if (main_frame != NULL)
		main_frame->LogOut();
}

void MainGui::LogIn(const std::string &name, const std::string &passwd)
{
	if (main_frame != NULL)
		main_frame->LogIn(name, passwd);
}

bool MainGui::UserExists(const std::string &name)
{
	if ( app!= NULL )
		return app->UserExists(name);
	return false;
}

bool MainGui::WorldExists(const std::string &name)
{
	if ( app!= NULL )
		return app->WorldExists(name);
	return false;
}

bool MainGui::RunWorld(const std::string &name)
{
	if ( app!= NULL )
		return app->RunWorld(name);
	return false;
}

core::IUserPersistence* MainGui::GetCurrentUser()
{
	if (app != NULL)
		return app->GetCurrentUser();
	return NULL;
}

core::IWorldPersistence* MainGui::GetCurrentWorld()
{
	if (app != NULL)
		return app->GetCurrentWorld();
	return NULL;
}

void MainGui::ViewUserInfoPanel()
{
	if (main_frame != NULL)
		main_frame->ViewUserInfo();
}

void MainGui::ViewLogPanel()
{
	if (main_frame != NULL)
		main_frame->ViewLog();
}

void MainGui::ViewConfigurePanel()
{
	if (main_frame != NULL)
		main_frame->ViewConfigure();
}

void MainGui::ShowFaceAtGUI(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step)
{
	if (main_frame != NULL)
		main_frame->SetFace(data, size_x, size_y, n_channels, depth, width_step);
}

void MainGui::GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions)
{
	if (app != NULL)
		app->GetWorldList(user_name, names, permissions);
}

void MainGui::Calibrate()
{
	if (app != NULL)
		app->Calibrate();
}

void MainGui::CalculateHomography()
{
	if (app != NULL)
		app->CalculateHomography();
}

void MainGui::TrainBackground()
{
	if (app != NULL)
		app->TrainBackground();
}

bool MainGui::SetCameraRecording(const bool &value)
{
	if (app != NULL)
		return app->SetCameraRecording(value);
	return false;
}

bool MainGui::SetUseRecording(const bool &value, const std::string &video_url_basename)
{
	if (app != NULL)
		return app->SetUseRecording(value, video_url_basename);
	return false;
}

void MainGui::FillLoginUserGUI(const std::string username, const std::string userpasswd)
{
	if (main_frame)
		main_frame->FillLoginUserGUI(username, userpasswd);
}

void MainGui::FillNewUserGUI(const std::string username, const std::string userpasswd)
{
	if (main_frame)
		main_frame->FillNewUserGUI(username, userpasswd);
}