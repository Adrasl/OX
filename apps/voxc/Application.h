#ifndef _APPLICATION_
#define _APPLICATION_

#ifdef _WINDOWS
#include <winsock2.h>
#endif

#include <core/IApplication.h>
#include <core/IGUI/IGui.h>
#include <core/ICog/ICog.h>
#include <core/IProd/IProd.h>
#include <core/IPercept/IPercept.h>
#include <core/IPersistence/IPersistence.h>
#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>

#include <igui/maingui.h>
#include <icog/maincog.h>
#include <iprod/mainprod.h>
#include <ipercept/mainpercept.h>
#include <ipersistence/mainpersistence.h>

#include <controllers/SessionController.h>


class SessionController;

class Application : public wxApp, public core::IApplication
{
	public:

		Application(void);
		~Application(void);

		IApplication* GetInstance() {if (app != NULL) return app; return NULL; }

		bool OnInit();

		void ExitApplication();

		bool PostLogMessage(const std::string &message);
		bool CreateUser(const std::string &name, const std::string &passwd);
		bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions);
		bool SaveWorldPermissions(const std::string &name, const int permissions);
		bool LoginUser(const std::string &name, const std::string &passwd);
		void LogOut();
		bool UserExists(const std::string &name);
		bool WorldExists(const std::string &name);
		bool RunWorld(const std::string &name);
		core::IUserPersistence* GetCurrentUser();
		core::IWorldPersistence* GetCurrentWorld();
		void GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions);
		//bool RunWorld(const int &world_id);
		bool CloseSession();
		bool DeleteUser(const std::string &name);
		bool DeleteWorld(const std::string &name);


	private:

		DECLARE_EVENT_TABLE();

		wxTimer app_timer;

		core::IGui			*app_maingui;
		core::IPercept		*app_mainpercept;
		core::IProd			*app_mainprod;
		core::IPersistence	*app_mainpersistence;

		SessionController *session_controller;

		static core::IApplication* app;

		void	OnIdle(wxIdleEvent &event);
		int		OnRun();
		void	DoMainLoopStuff(wxTimerEvent& event);
		void	ResetProductionModule();
		//int		MainLoop();



};

#endif