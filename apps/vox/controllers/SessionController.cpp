#include <controllers/SessionController.h>
#include <debugger.h> 

boost::mutex SessionController::m_mutex;

SessionController::SessionController() : default_user(NULL), default_world(NULL), app_user(NULL), app_world(NULL), session_permissions(-1)
{
}

bool SessionController::LoadDefaultData()
{
	boost::mutex::scoped_lock lock(m_mutex);

	default_user  = new core::ipersistence::UserPersistence(); 
	default_world  = new core::ipersistence::WorldPersistence(); 
	bool successu = default_user->Load(std::string("default_user"));
	bool successw = default_world->Load(std::string("default_world"));
	
	Notify(this, "RUN WORLD");

	return (successu && successw);
}

SessionController::~SessionController()
{
	boost::mutex::scoped_lock lock(m_mutex);

	if ( default_user != NULL )  delete default_user;
	if ( default_world != NULL ) delete default_world;

	if ( app_user != NULL )  delete app_user;
	if ( app_world != NULL ) delete app_world;

	detach_all();
}

bool SessionController::IsSessionClosed()
{
	boost::mutex::scoped_lock lock(m_mutex);

	return ((app_user == NULL) && (app_world == NULL));
}

bool SessionController::IsCurrentUser(const std::string &name)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (app_user != NULL)
		return (app_user->GetName() == name );
	return false;
}

bool SessionController::IsCurrentWorld(const std::string &name)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (app_world != NULL)
		return (app_world->GetName() == name );
	return false;
}

bool SessionController::CreateUser(const std::string &name, const std::string &passwd)
{
	CloseSession();
	{	boost::mutex::scoped_lock lock(m_mutex);
		if ( (app_user == NULL) && (app_world == NULL) )
		{	app_user  = /*(User *) */new core::ipersistence::UserPersistence(name, passwd, PERM_OWNER_ALL); 
			app_user->Save();
			return true;
		}
	}
	return false;
}

bool SessionController::CreateWorld(const std::string &name, const std::string &owner, const int &permissions)
{
	CloseWorld();
	{	boost::mutex::scoped_lock lock(m_mutex);
		app_world  = new core::ipersistence::WorldPersistence(name, owner, permissions); 
		app_world->Save();
		return true;
	}
}

bool SessionController::LoginUser(const std::string &name, const std::string &passwd)
{
	CloseSession();
	bool success = false;
	{	boost::mutex::scoped_lock lock(m_mutex);
		if ( (app_user == NULL) && (app_world == NULL) )
		{
			app_user  = new core::ipersistence::UserPersistence(); 
			success = app_user->Load(name);
			success = success && (app_user->GetPassword() == passwd);
		}
	}

	if (success)
	{	
		return true;   
	}
	CloseSession();
	return false;
}

void SessionController::LogOut()
{
	CloseSession();
}

bool SessionController::RunWorld(const std::string &name)
{
	CloseWorld();
	bool success = false;
	{	boost::mutex::scoped_lock lock(m_mutex);
		if ( (app_user != NULL) && (app_world == NULL) )
		{	app_world  = new core::ipersistence::WorldPersistence(); 
			success = app_world->Load(name);
			if ( success  )
			{
				session_permissions = app_user->GetPermissions() & app_world->GetPermissions();
			}
		}
	}
	if (success)
		Notify(this, "RUN WORLD");
	else
		CloseWorld();
	return success;
}

bool SessionController::OpenSession(const std::string &user_name, const std::string &passwd, const std::string &world_name)
{
	CloseSession();
	{	boost::mutex::scoped_lock lock(m_mutex);
		bool user_logged  = LoginUser(user_name, passwd);
		bool world_loaded = RunWorld(world_name); 
		if ( user_logged && world_loaded )
		{	session_permissions = app_user->GetPermissions() & app_world->GetPermissions();
			return true;	}			
	}
	CloseSession();
	return false;
}

bool SessionController::CloseSession()
{
	Notify(this, "CLOSE WORLD");

	{	boost::mutex::scoped_lock lock(m_mutex);
		session_permissions = -1;
		if ( app_user != NULL )
		{	app_user->Save();
			delete app_user;
			app_user = NULL;		}
		if ( app_world != NULL )
		{	app_world->Save();
			delete app_world;
			app_world = NULL;		}
	}

	return true;
}

bool SessionController::CloseWorld()
{
	boost::mutex::scoped_lock lock(m_mutex);

	session_permissions = -1;
	if ( app_world != NULL )
	{	delete app_world;
		app_world = NULL;		}
	return true;
}