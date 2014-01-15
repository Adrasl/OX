#ifndef _IPERSISTENCE_
#define _IPERSISTENCE_

//#include <core/IPersistence/IEntityPersistence.h>
//#include <core/IPersistence/IWorldPersistence.h>
//#include <core/IPersistence/IUserPersistence.h>

#include <string>
#include <core/Export.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>

namespace core
{
	class _COREEXPORT_ IPersistence
	{
		public:

			virtual ~IPersistence(){}
			virtual void Delete()=0;
			virtual void SetApp(IApplication *app_) = 0 ;
			virtual void SetAppConfig(IApplicationConfiguration *app_config_) = 0;

			virtual bool UserExists(const std::string &name) = 0;
			virtual bool WorldExists(const std::string &name) = 0;
			virtual bool DeleteWorld(const std::string &name) = 0;
			virtual bool DeleteUser(const std::string &name) = 0;
			virtual void GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions) = 0;
			/** \brief Saves the given permissions (not available yet). **/
			virtual bool SaveWorldPermissions(const std::string &name, const int permissions) = 0;
			virtual void GetUserData(const int &id, std::string &name, std::string &passwd) = 0;

	};
}


#endif

