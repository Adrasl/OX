#include <controllers/SessionController.h>

SessionController::SessionController() : app_user(NULL), app_world(NULL), session_permissions(-1)
{}

SessionController::~SessionController()
{
	if ( app_user != NULL )  delete app_user;
	if ( app_world != NULL ) delete app_world;
}

bool SessionController::IsSessionClosed()
{
	return ((app_user == NULL) && (app_world == NULL));
}

bool SessionController::IsCurrentUser(const std::string &name)
{
	if (app_user != NULL)
		return (app_user->GetName() == name );
	return false;
}

bool SessionController::IsCurrentWorld(const std::string &name)
{
	if (app_world != NULL)
		return (app_world->GetName() == name );
	return false;
}

bool SessionController::CreateUser(const std::string &name, const std::string &passwd)
{
	CloseSession();
	if ( (app_user == NULL) && (app_world == NULL) )
	{
		app_user  = /*(User *) */new core::ipersistence::UserPersistence(name, passwd, PERM_OWNER_ALL); 
		app_user->Save();

		return true;
	}
	return false;
}

bool SessionController::CreateWorld(const std::string &name, const std::string &owner, const int &permissions)
{
	CloseWorld();
	app_world  = new core::ipersistence::WorldPersistence(name, owner, permissions); 
	app_world->Save();
	return true;
}

bool SessionController::LoginUser(const std::string &name, const std::string &passwd)
{
	CloseSession();
	if ( (app_user == NULL) && (app_world == NULL) )
	{
		app_user  = /*(User *) */new core::ipersistence::UserPersistence(); 

		bool success = app_user->Load(name);
		success = success && (app_user->GetPassword() == passwd);
		if ( success  )
		{ 
			//DO STUFF 
		}
		else 
			CloseSession();

		return success;
	}
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
	if ( (app_user != NULL) && (app_world == NULL) )
	{	app_world  = new core::ipersistence::WorldPersistence(); 
		success = app_world->Load(name);
		if ( success  )
			session_permissions = app_user->GetPermissions() & app_world->GetPermissions();
		else 
			CloseWorld();
		return success;
	}
	return false;
}

bool SessionController::OpenSession(const std::string &user_name, const std::string &passwd, const std::string &world_name)
{
	CloseSession();
	bool user_logged  = LoginUser(user_name, passwd);
	bool world_loaded = RunWorld(world_name); 
	if ( user_logged && world_loaded )
	{	session_permissions = app_user->GetPermissions() & app_world->GetPermissions();
		return true;	}
	else
		CloseSession();
	return false;
}

bool SessionController::CloseSession()
{
	session_permissions = -1;
	if ( app_user != NULL )
	{	delete app_user;
		app_user = NULL;		}
	if ( app_world != NULL )
	{	delete app_world;
		app_world = NULL;		}
	return true;
}

bool SessionController::CloseWorld()
{
	session_permissions = -1;
	if ( app_world != NULL )
	{	delete app_world;
		app_world = NULL;		}
	return true;
}