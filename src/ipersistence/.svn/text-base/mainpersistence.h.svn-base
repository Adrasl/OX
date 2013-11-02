#ifndef _MAINPERSISTENCE_
#define _MAINPERSISTENCE_

#include <core/IPersistence/IPersistence.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>
#include <ipersistence/UserPersistence.h>
#include <ipersistence/WorldPersistence.h>
#include <ipersistence/EntityPersistence.h>
#include <dba/dba.h>
#include <dba/double_filter.h>
#include <string>
#include <sstream>

namespace core
{
	namespace ipersistence
	{
		class UserPersistence;
		class WorldPersistence;
		class EntityPersistence;

		class MainPersistence : public core::IPersistence
		{
			public:

				MainPersistence(IApplicationConfiguration *app_config_);
				virtual ~MainPersistence();
				virtual void Delete();
				virtual void SetApp(IApplication *app_) {app = app_;}
				virtual void SetAppConfig(IApplicationConfiguration *app_config_) {app_config = app_config_;}

				virtual bool UserExists(const std::string &name);
				virtual bool WorldExists(const std::string &name);
				virtual bool DeleteWorld(const std::string &name);
				virtual bool SaveWorldPermissions(const std::string &name, const int permissions);
				virtual bool DeleteUser(const std::string &name);
				virtual void MainPersistence::GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions);

				virtual void GetUserData(const int &id, std::string &name, std::string &passwd);
				//virtual void ListProjects	();
				//virtual void LoadProject	(unsigned int id);
				//virtual void LoadProjectInfo(unsigned int id);

				static dba::SQLArchive * GetArchive() { return &ar;             }
				static dba::SQL * GetCounterCreate () { return &counter_create; }

			private:

				IApplication* app;
				IApplicationConfiguration *app_config;
				/** SQL Archive */
				static dba::SQLArchive ar;
				static dba::SQL counter_create;

				void ProcessException(const dba::SQLException& pEx);
				void ProcessException(const dba::Exception& pEx);
		};
	}
}

#endif

