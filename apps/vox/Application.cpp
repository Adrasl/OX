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
//PATH=..\..\extern\opencv\bin;%PATH%;..\..\extern\bin;..\..\extern\bin\opencv;..\..\extern\panda3d\lib;..\..\extern\panda3d\built\bin;..\..\extern\panda3d\built\python;..\..\extern\panda3d\built\python\libs;..\..\..\BBDD\PostgreSQL\bin;..\..\extern\debea\vc_release_static;..\..\extern\sfml\extlibs\bin;

#ifdef WIN32
#include <Aclapi.h>
#endif

#include <debugger.h> 
//#define CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#define MAINLOOP_EVT 12345
#define BENEFIT_OF_THE_DOUBT 100

BEGIN_EVENT_TABLE(Application, wxApp)
	EVT_IDLE	(Application::OnIdle)
	EVT_TIMER	(MAINLOOP_EVT, Application::DoMainLoopStuff)
END_EVENT_TABLE()

core::IApplication* Application::app = NULL;

Application::Application(void) : app_maingui(NULL), app_mainpercept(NULL), app_mainprod(NULL), session_controller(NULL), configuration_controller(NULL), navigation_controller(NULL), user_dataModel_controller(NULL), app_mainpersistence(NULL), benefit_of_the_doubt(0), avatar_entity(NULL), contentcreation_controller(NULL)
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	app = (IApplication*) this;
	app_timer.SetOwner(this, MAINLOOP_EVT);

	app_config = new ApplicationConfiguration();
	configuration_controller = new ConfigurationController();
	configuration_controller->Load();
	session_controller = new SessionController();
}

Application::~Application(void)
{
	configuration_controller->Save();

	if (configuration_controller!=NULL)
	{	delete configuration_controller;}

	if (navigation_controller!=NULL)
	{	delete navigation_controller;	}

	if (user_dataModel_controller!=NULL)
	{	delete user_dataModel_controller;	}

	if (session_controller != NULL)
	{	delete session_controller;		}

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

	if (contentcreation_controller!=NULL)
	{	delete contentcreation_controller;	}

	if ( app_config ) delete app_config;

	//_CrtDumpMemoryLeaks();

//	fclose(stdout);
	//#ifdef _MSVC
	//#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
	//#endif
	//#endif
	
}

bool Application::OnInit()
{
	//freopen("VOX_log_info.txt", "w+", stdout);
	//printf("LOGGING...\n");

	#ifdef WIN32
	unsigned int cores = boost::thread::hardware_concurrency();
	int mask = (1 << cores)-1;
	HANDLE hProces = GetCurrentProcess();
	int ss = SetSecurityInfo(hProces, SE_UNKNOWN_OBJECT_TYPE ,PROCESS_ALL_ACCESS, NULL, NULL, NULL, NULL);
	int pe = SetProcessAffinityMask(hProces, mask);
	#endif

	core::IApplicationConfiguration* iapp_config = (core::IApplicationConfiguration*)app_config;

	app_maingui		    =  core::igui::MainGui::GetInstance(iapp_config, "VOX");
	app_mainprod	    = (core::IProd *) new core::iprod::MainProd(iapp_config, argc, argv); 
	app_mainpercept     = (core::IPercept *) new core::ipercept::MainPercept(iapp_config); 
	app_mainpersistence	= (core::IPersistence *) new core::ipersistence::MainPersistence(iapp_config); 
	avatar_entity		= (core::IEntityPersistence *) new core::ipersistence::EntityPersistence ("avatar");

	

	if(app_maingui)
		app_maingui->SetApp((IApplication *)this);
	if(app_mainprod)
		app_mainprod->SetApp((IApplication *)this);
	if(app_mainpercept)
		app_mainpercept->SetApp((IApplication *)this);
	if(app_mainpersistence)
		app_mainpersistence->SetApp((IApplication *)this);
	if(app_mainprod && app_mainpercept)
	{
		contentcreation_controller = ContentCreationController::Instance();
		if (contentcreation_controller)
			contentcreation_controller->SetApp((IApplication *)this, iapp_config, app_mainpercept, app_mainprod);
	}

	if( (app_mainprod) && (app_mainpercept))
	{
		user_dataModel_controller = new UserDataModelController();
		navigation_controller = new NavigationController((IApplication *)this, user_dataModel_controller, app_mainpercept, app_mainprod);
	}


	//app_mainpersistence->ListProjects();

	PostLogMessage("Modules running...");

	if(app_mainprod)
	{
		PostLogMessage("Initializing production...");
		app_mainprod->Init();
	}
	if(app_mainpercept)
	{
		PostLogMessage("Initializing perception...");
		app_mainpercept->Init();
	}

	session_controller->LoadDefaultData();

	//Attaching Observers to Subjects
	if(session_controller && contentcreation_controller)
	{
		core::Observer* observer = dynamic_cast<core::Observer*> (contentcreation_controller);
		if (observer)
			session_controller->attach(observer);
	}

	RunDefaultWorld();

	return true;
}

void Application::OnIdle(wxIdleEvent &event)
{
	//To Do less important/frequent stuff
	if(!app_config)
		return;

	bool use_recon = app_config->GetUseRecognition();
	bool autologin = app_config->GetAutoLogin();
	if(session_controller && app_mainpercept && app_maingui && use_recon)
	{
		bool session_closed = session_controller->IsSessionClosed();
		bool presence_detected = app_mainpercept->PresenceDetected(); 

		if(autologin && !session_closed && !presence_detected)
			app_maingui->LogOut();

		if(session_closed && presence_detected)
		{
			//DetectarCara
			bool face_detected = app_mainpercept->FaceDetected();
			char *img = NULL;
			int size_x, size_y, n_channels, depth, width_step;
			if(face_detected)
				img = app_mainpercept->GetCopyOfCurrentFeature("FACE", size_x, size_y, n_channels, depth, width_step, true);

			if (app_maingui != NULL)
				app_maingui->ShowFaceAtGUI(img, size_x, size_y, n_channels, depth, width_step);

			if(face_detected)
			{	//Reconocer
				int user_detected_id = app_mainpercept->RecognizeCurrentFace();

				if (user_detected_id != -1) //known face
				{
					std::string name, passwd;
					app_mainpersistence->GetUserData(user_detected_id, name, passwd);
					app_maingui->FillLoginUserGUI(name, passwd);
					if (autologin)
					{	
						std::vector<std::string> world_names;
						std::vector<int> world_permissions;
						app_mainpersistence->GetWorldList(name, world_names, world_permissions);
						if (world_names.size() > 0)
						{
							std::string world_name = world_names[world_names.size()-1];
							app_maingui->LogIn(name, passwd);
							//session_controller->OpenSession(name, passwd, world_name);
							this->RunWorld(world_name);
						}
					}
					benefit_of_the_doubt = 0;
				} else { //unknown face //retomar puede que tenga algunas caras en local que no se guardaron en la BD
					std::string name, passwd;
					name = passwd = "anonymous";
					app_maingui->FillNewUserGUI(name, passwd);
					if (autologin && app_mainpercept->IsFacePoolReady() && (benefit_of_the_doubt > BENEFIT_OF_THE_DOUBT))
					{	std::stringstream wop_user, wop_world;
						wop_user << "anonymous";
						wop_world << "world";
						session_controller->CreateUser(name, passwd);
						core::IUserPersistence  *user = session_controller->GetCurrentUser();
						int user_id;
						user->GetId(user_id);
						wop_user << "_" << user_id;
						wop_world<< "_" << user_id;
						name = wop_user.str();
						user->SetName(name);
						user->Save();
						session_controller->CreateWorld(wop_world.str().c_str(), name, 127);
						session_controller->CloseSession();
						app_mainpercept->AddNewUserToRecognition(user_id);
						benefit_of_the_doubt = 0;
					}
					benefit_of_the_doubt++;
				}
			}
		}
	}
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
	//app_mainprod->DoStuff();
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
	RunDefaultWorld();
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
		app_mainprod->CloseWorld();
		session_controller->RunWorld(name);
		if (!session_controller->IsSessionClosed())
		{
			core::IUserPersistence  *user  = session_controller->GetCurrentUser();
			core::IWorldPersistence *world = session_controller->GetCurrentWorld();

			//edit entities depending on psique before creating 3dprodEntities

			app_mainpercept;	//SetUser
			if (user && world)
			{	for (int i=0; i < world->GetNumEntities(); i++)
				{	int ient_psique = 0;
					IEntityPersistence *ient = world->GetEntity(i);
					iprod::Prod3DEntity *new_entity = NULL;
					if (ient) 
					{
						core::Subject* subject = dynamic_cast<core::Subject*> (ient);
						core::Observer* observer = dynamic_cast<core::Observer*> (contentcreation_controller);
						subject->attach(observer);
						ient->GetPsique(ient_psique);
					}

					switch (ient_psique)
					{	case NatureOfEntity::STANDALONE :
					   {	new_entity = new iprod::OXStandAloneEntity(ient); break;	   }
						case NatureOfEntity::BOID		:
					   {	new_entity = new iprod::OXBoidsEntity(ient); break;	   }
						//case NatureOfEntity::TREE		:
					 //  {	new_entity = new iprod::OXTreeEntity(ient); break;	   }
						default							: 
					   {	new_entity = new iprod::Prod3DEntity(ient); break;	   }
					}

					if (new_entity)
						app_mainprod->LoadEntityFromCurrentWorld(new_entity);
				}
			}
			bool world_running = app_mainprod->RunWorld(user, world);
			ContentCreationController::Instance()->Reset();
			return true;
		}
	}
	return false;
}

bool Application::RunDefaultWorld()
{
	if ( (session_controller != NULL) && (app_mainpersistence != NULL) && (app_mainprod != NULL) ) 
	{
		app_mainprod->CloseWorld();
		if (!session_controller->IsSessionClosed())
			session_controller->CloseSession();

		core::IUserPersistence  *user  = session_controller->GetDefaultUser();
		core::IWorldPersistence *world = session_controller->GetDefaultWorld();

		app_mainpercept;	//SetUser

		if (user && world)
		{	for (int i=0; i < world->GetNumEntities(); i++)
			{	int ient_psique = 0;
				IEntityPersistence *ient = world->GetEntity(i);
				iprod::Prod3DEntity *new_entity = NULL;
				if (ient) ient->GetPsique(ient_psique);

					switch (ient_psique)
					{	case NatureOfEntity::STANDALONE :
					   {	new_entity = new iprod::OXStandAloneEntity(ient); break;	   }
						case NatureOfEntity::BOID		:
					   {	new_entity = new iprod::OXBoidsEntity(ient); break;	   }
						//case NatureOfEntity::TREE		:
					 //  {	new_entity = new iprod::OXTreeEntity(ient); break;	   }
						default							: 
					   {	new_entity = new iprod::Prod3DEntity(ient); break;	   }
					}

				if (new_entity)
					app_mainprod->LoadEntityFromCurrentWorld(new_entity);
			}
		}

		bool   world_running = app_mainprod->RunWorld(user, world);
		ContentCreationController::Instance()->Reset();
		return world_running;
	}
	return false;
}

void Application::AddNewEntitiesIntoCurrentWorld(const std::map<core::IEntity *, double> &new_entities_after_seconds)
{
	if (app_mainprod)
		app_mainprod->AddNewEntitiesIntoCurrentWorld(new_entities_after_seconds);
}

void Application::RemoveEntityFromCurrentWorld(core::IEntity* new_entity)
{
	if (app_mainprod)
		app_mainprod->RemoveEntityFromCurrentWorld(new_entity);
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

core::IUserPersistence* Application::GetDefaultUser()
{
	if (session_controller != NULL) 
		return session_controller->GetDefaultUser();
	return NULL;
}
core::IWorldPersistence* Application::GetDefaultWorld()
{
	if (session_controller != NULL) 
		return session_controller->GetDefaultWorld();
	return NULL;
}

core::IEntityPersistence* Application::GetAvatarEntity()
{
	return avatar_entity;
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


bool Application::Calibrate()
{
	if (app_mainpercept)
		app_mainpercept->Calibrate(true);
	return true;
}

bool Application::CalculateHomography()
{
	if (app_mainpercept)
		app_mainpercept->CalculateHomography();
	return true;
}

bool Application::TrainBackground()
{
	if (app_mainpercept)
		app_mainpercept->TrainBackground();
	return true;
}

bool Application::SetCameraRecording(const bool &value)
{
	if (app_mainpercept)
		return app_mainpercept->SetCameraRecording(value);
	return false;
}
bool Application::SetUseRecording(const bool &value, const std::string &video_url_basename)
{
	if (app_mainpercept)
		return app_mainpercept->SetUseRecording(value, video_url_basename);
	return false;
}

IUserDataModelController* Application::GetUserDataModelControllerInstance()
{
	return user_dataModel_controller;
}