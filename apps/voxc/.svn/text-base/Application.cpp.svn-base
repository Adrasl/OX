#include <Application.h>

////-----Platform dependant----------------
//#ifdef WIN32
//#include <windows.h>
//#endif
////---------------------------------------

//REMEMBER ENVIRONMENT
//PATH=%PATH%;..\..\extern\bin;..\..\extern\bin\opencv;..\..\extern\panda3d\debug\bin;..\..\extern\panda3d\debug\python;..\..\extern\panda3d\debug\python\DLLs;
//PATH=%PATH%;..\..\extern\bin;..\..\extern\bin\opencv;..\..\extern\panda3d\debug\bin;..\..\extern\panda3d\debug\python;..\..\extern\panda3d\debug\python\DLLs;..\..\extern\openal\debug;..\..\extern\openal\build\Debug;..\..\extern\alure\build\Debug;

//DEBUG
//PATH=%PATH%;..\..\extern\bin;..\..\extern\bin\opencv;..\..\extern\panda3d\debug\bin;..\..\extern\panda3d\debug\python;..\..\extern\panda3d\debug\python\DLLs;..\..\extern\sfml\extlibs\bin;
//RELEASE
//PATH=%PATH%;..\..\extern\bin;..\..\extern\bin\opencv;..\..\extern\panda3d\built\bin;..\..\extern\panda3d\built\python;..\..\extern\panda3d\built\python\DLLs;..\..\extern\sfml\extlibs\bin;
#include <debugger.h> 

#define MAINLOOP_EVT 12345

BEGIN_EVENT_TABLE(Application, wxApp)
	EVT_IDLE	(Application::OnIdle)
	EVT_TIMER	(MAINLOOP_EVT, Application::DoMainLoopStuff)
END_EVENT_TABLE()

core::IApplication* Application::app = NULL;

Application::Application(void) : app_maingui(NULL), app_mainpercept(NULL), app_mainprod(NULL), session_controller(NULL)
{
	app = (IApplication*) this;
	app_timer.SetOwner(this, MAINLOOP_EVT);
}

Application::~Application(void)
{
	if (app_mainpercept!=NULL)
	{	app_mainpercept->Delete();
		delete app_mainpercept;	}

	if (app_mainprod!=NULL)
	{	app_mainprod->Delete();
		delete app_mainprod;	}

	if (app_maingui!=NULL)
	{	app_maingui->Delete();
		delete app_maingui;		}
	
	if (app_mainpersistence!=NULL)
	{	app_mainpersistence->Delete();
		delete app_mainpersistence;		}
	
}

bool Application::OnInit()
{
	session_controller = new SessionController();
	app_maingui		    =  core::igui::MainGui::GetInstance("VOX");
	app_mainprod	    = (core::IProd *) new core::iprod::MainProd(argc, argv); 
	app_mainpercept     = (core::IPercept *) new core::ipercept::MainPercept(); 
	app_mainpersistence	= (core::IPersistence *) new core::ipersistence::MainPersistence(); 

	app_maingui->SetApp((IApplication *)this);
	app_mainprod->SetApp((IApplication *)this);
	app_mainpercept->SetApp((IApplication *)this);
	app_mainpersistence->SetApp((IApplication *)this);

	app_mainpersistence->ListProjects();

	PostLogMessage("Modules running...");

	PostLogMessage("Initializing production...");
	app_mainprod->Init();
	PostLogMessage("Initializing perception...");
	app_mainpercept->Init();


	return true;
}

void Application::OnIdle(wxIdleEvent &event)
{
	//To Do less important/frequent stuff
}

int Application::OnRun()
{
	app_timer.Start(10);
	return wxApp::OnRun();
}

void Application::ExitApplication()
{
	wxApp::Exit();
}

void Application::DoMainLoopStuff(wxTimerEvent& event)
{
	//To Do important/frequent stuff
	app_mainprod->DoStuff();
}

bool Application::PostLogMessage(const std::string &message)
{
	if (app_maingui != NULL)
	{	app_maingui->PostLogMessage(message);
		return true;}
	return false;
}


bool Application::CreateUser(const std::string &name, const std::string &passwd)
{
	if (session_controller != NULL) 
	{
		return session_controller->CreateUser(name, passwd);
	}
	return false;
}

bool Application::DeleteUser(const std::string &name)
{
	if ( (session_controller != NULL) && (app_mainpersistence != NULL) ) 
	{
		if (!(session_controller->IsCurrentUser(name)))
			session_controller->CloseSession();
			return app_mainpersistence->DeleteUser(name);
	}
	return false;
}

bool Application::DeleteWorld(const std::string &name)
{
	if ( (session_controller != NULL) && (app_mainpersistence != NULL) ) 
	{
		if ((session_controller->IsCurrentWorld(name)))
			session_controller->CloseWorld();
		return app_mainpersistence->DeleteWorld(name);
	}
	return false;
}

bool Application::SaveWorldPermissions(const std::string &name, const int permissions)
{
	if ( (session_controller != NULL) && (app_mainpersistence != NULL) ) 
	{
		if ((session_controller->IsCurrentWorld(name)))
			session_controller->CloseWorld();
		return app_mainpersistence->SaveWorldPermissions(name, permissions);
	}
	return false;
}

bool Application::CreateWorld(const std::string &name, const std::string &owner, const int &permissions)
{
	if (session_controller != NULL) 
	{
		return session_controller->CreateWorld(name, owner, permissions);
	}
	return false;
}

bool Application::LoginUser(const std::string &name, const std::string &passwd)
{
	if (session_controller != NULL) 
		return session_controller->LoginUser(name, passwd);
	return false;
}

void Application::LogOut()
{
	CloseSession();
}

bool Application::UserExists(const std::string &name)
{
	if (app_mainpersistence != NULL) 
	{
		return app_mainpersistence->UserExists(name);
	}
	return false;
}

bool Application::WorldExists(const std::string &name)
{
	if (app_mainpersistence != NULL) 
	{
		return app_mainpersistence->WorldExists(name);
	}
	return false;
}

bool Application::RunWorld(const std::string &name)
{
	if ( (session_controller != NULL) && (app_mainpersistence != NULL) && (app_mainprod != NULL) ) 
	{
		session_controller->RunWorld(name);
		if (!session_controller->IsSessionClosed())
		{
			core::IUserPersistence  *user  = session_controller->GetCurrentUser();
			core::IWorldPersistence *world = session_controller->GetCurrentWorld();

			app_mainpercept;	//SetUser
			bool world_running = app_mainprod->RunWorld(user, world);

			return true;
		}
		//return app_mainpersistence->RunWorld(name);
	}
	return false;
}

core::IUserPersistence* Application::GetCurrentUser()
{
	if (session_controller != NULL) 
		return session_controller->GetCurrentUser();
	return NULL;
}
core::IWorldPersistence* Application::GetCurrentWorld()
{
	if (session_controller != NULL) 
		return session_controller->GetCurrentWorld();
	return NULL;
}

void Application::GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions)
{
	if (app_mainpersistence != NULL) 
		app_mainpersistence->GetWorldList(user_name, names, permissions);
}

//bool Application::RunWorld(const std::string &world_id)
//{
//	if (session_controller != NULL) 
//		return session_controller->RunWorld(world_id);
//	return false;
//}
bool Application::CloseSession()
{
	if ( (session_controller != NULL) && (app_mainprod != NULL) ) 
	{
		app_mainprod->CloseWorld();
		return session_controller->CloseSession();
	}
	return false;
}

void Application::ResetProductionModule()
{
	//BASURAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!!!!!!!
	//if (app_mainprod != NULL)
	//{
	//	app_mainprod->Delete();
	//	delete app_mainprod;
	//	app_mainprod = NULL;
	//	app_mainprod = (core::IProd *) new core::iprod::MainProd(argc, argv); 
	//	app_mainprod->SetApp((IApplication *)this);
	//	PostLogMessage("Initializing production module...");
	//	app_mainprod->Init();
	//}
}
//bool Application::DeleteUser(int user_id)
//{
//	if (session_controller != NULL) 
//		return session_controller->DeleteUser(user_id);
//	return false;
//}
//bool Application::DeleteWorld(int world_id)
//{
//	if (session_controller != NULL) 
//		return session_controller->DeleteWorld(world_id);
//	return false;
//}
