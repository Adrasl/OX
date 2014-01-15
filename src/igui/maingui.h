#ifndef _MAINGUI_
#define _MAINGUI_

#include <core/IGUI/IGui.h>
#include <core/IGUI/IGuiWindow.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/timer.h>
#include <igui/Export.h>
#include <igui/MainFrame.h>
#include <igui/frame.h>
#include <igui/controllers/GUIGenericController.h>
#include <core/IPersistence/IUserPersistence.h>

#include <string>
#include <map>

#include <boost/thread.hpp>

#define IMPLEMENT_MIIAPP(name) IMPLEMENT_APP_NO_MAIN(name)

namespace core
{
	namespace igui
	{
		class MainFrame;
		class GUIGenericController;

		class MainGui : public core::IGui
		{
			public:
				virtual ~MainGui();
				virtual void Delete();
				virtual void SetApp(IApplication *app_) {app = app_;}
				virtual void SetAppConfig(IApplicationConfiguration *app_config_) {app_config = app_config_;}
				IApplicationConfiguration *GetAppConfig() {return app_config;}
				virtual void Update();
				virtual void LogOut();
				virtual void LogIn(const std::string &name, const std::string &passwd);

				virtual void ShowFaceAtGUI(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step);

				void RegisterWindow(IGuiWindow *window);
				void AddWindowToFullscreenList(IGuiWindow *window);
				void ShowAll(bool value);
				void SetAllWindowsFullScreen(bool value);
				void PostLogMessage(const std::string &message);

				static MainGui* GetInstance(IApplicationConfiguration *app_config_=NULL, const std::string &title = "");
				static void		ReleaseInstance();

				bool CreateUser(const std::string &name, const std::string &passwd);
				bool DeleteUser(const std::string &name);
				bool DeleteWorld(const std::string &name);
				bool SaveWorldPermissions(const std::string &name, const int permissions);
				bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions);
				bool LoginUser(const std::string &name, const std::string &passwd);
				bool UserExists(const std::string &name);
				bool WorldExists(const std::string &name);
				bool RunWorld(const std::string &name);
				core::IUserPersistence* GetCurrentUser();
				core::IWorldPersistence* GetCurrentWorld();
				void GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions);
				bool GetAutoLogin();
				void SetAutoLogin(const bool &value);
				void ViewUserInfoPanel();
				void ViewLogPanel();
				void ViewConfigurePanel();
				void Calibrate();
				void CalculateHomography();
				void TrainBackground();
				bool SetCameraRecording(const bool &value);	
				bool SetUseRecording(const bool &value, const std::string &video_url_basename);
				void FillLoginUserGUI(const std::string username, const std::string userpasswd);
				void FillNewUserGUI(const std::string username, const std::string userpasswd);

			private:
				IApplication *app;
				IApplicationConfiguration *app_config;
				MainGui(IApplicationConfiguration *app_config_, const std::string &title = "");

				static MainGui* instance; 
				static boost::mutex m_mutex;

				static MainFrame *main_frame;
				GUIGenericController *generic_controller;

				static int last_win_id;
				static std::map<IGuiWindow*, int> registered_windows;
				static std::map<IGuiWindow*, int> fullscreenable_windows;

		};
	}
}

#endif

