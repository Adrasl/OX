#ifndef _SESSIONCONTROLLER_
#define _SESSIONCONTROLLER_

#include <Application.h>
#include <core/IPersistence/IPersistence.h>
#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>
#include <core/types.h>

#include <ipersistence/types.h>
#include <vector>

class Application;

class SessionController : public core::Subject
{
	public:
		SessionController();
		~SessionController();

		bool CreateUser(const std::string &name, const std::string &passwd);
		bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions);
		bool LoadUser(const int &user_id);
		bool LoginUser(const std::string &name, const std::string &passwd);
		void LogOut();
		core::IUserPersistence* GetCurrentUser()		{boost::mutex::scoped_lock lock(m_mutex); return  (core::IUserPersistence*)app_user;}
		core::IWorldPersistence* GetCurrentWorld()		{boost::mutex::scoped_lock lock(m_mutex); return  (core::IWorldPersistence*)app_world;}
		core::IUserPersistence* GetDefaultUser()		{boost::mutex::scoped_lock lock(m_mutex); return  (core::IUserPersistence*)default_user;}
		core::IWorldPersistence* GetDefaultWorld()		{boost::mutex::scoped_lock lock(m_mutex); return  (core::IWorldPersistence*)default_world;}
		bool RunWorld(const std::string &name);
		bool OpenSession(const std::string &user_name, const std::string &passwd, const std::string &world_name);
		bool CloseSession();
		bool CloseWorld();
		bool LoadDefaultData();
		bool IsSessionClosed();
		bool IsCurrentUser(const std::string &name);
		bool IsCurrentWorld(const std::string &name);

	private:

		static boost::mutex m_mutex;
		core::ipersistence::UserPersistence		*app_user, *default_user;
		core::ipersistence::WorldPersistence	*app_world, *default_world;
		int session_permissions;
};

#endif