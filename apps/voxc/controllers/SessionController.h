#ifndef _SESSIONCONTROLLER_
#define _SESSIONCONTROLLER_

#include <Application.h>
#include <core/IPersistence/IPersistence.h>
#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>
#include <ipersistence/types.h>
#include <vector>

class Application;

//basura
//class User : public core::IUserPersistence
//{	public:
//		 User(){}
//		~User(){}
//};

//class World : public core::IWorldPersistence
//{	public:
//		 World(){}
//		~World(){}
//};

class SessionController
{
	public:
		SessionController();
		~SessionController();

		bool CreateUser(const std::string &name, const std::string &passwd);
		bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions);
		bool LoginUser(const std::string &name, const std::string &passwd);
		void LogOut();
		core::IUserPersistence* GetCurrentUser()		{return  (core::IUserPersistence*)app_user;}
		core::IWorldPersistence* GetCurrentWorld()		{return  (core::IWorldPersistence*)app_world;}
		bool RunWorld(const std::string &name);
		bool OpenSession(const std::string &user_name, const std::string &passwd, const std::string &world_name);
		bool CloseSession();
		bool CloseWorld();
		////bool DeleteUser(int user_id);
		////bool DeleteWorld(int world_id);
		bool IsSessionClosed();
		bool IsCurrentUser(const std::string &name);
		bool IsCurrentWorld(const std::string &name);

	private:
		//User	*app_user;
		core::ipersistence::UserPersistence		*app_user;
		core::ipersistence::WorldPersistence	*app_world;
		int session_permissions;
};

#endif