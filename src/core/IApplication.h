#ifndef _IAPPLICATION_
#define _IAPPLICATION_

#include <string>
#include <vector>
#include <core/IProd/IEntity.h>
#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>
#include <core/IUserDataModelController.h>

namespace core
{
	//class IEntity;

	class IApplication
	{
		public:

			virtual ~IApplication(){}
			virtual void ExitApplication()= 0;
			virtual IApplication* GetInstance() = 0;
			
			virtual bool PostLogMessage(const std::string &message) = 0;
			virtual bool CreateUser(const std::string &name, const std::string &passwd) = 0;
			virtual bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions) = 0;
			virtual bool SaveWorldPermissions(const std::string &name, const int permissions) = 0;
			virtual bool LoginUser(const std::string &name, const std::string &passwd) = 0;
			virtual void LogOut() = 0;
			virtual bool UserExists(const std::string &name) = 0;
			virtual bool WorldExists(const std::string &name) = 0;
			virtual bool RunWorld(const std::string &name) = 0;
			virtual void AddNewEntityIntoCurrentWorld(core::IEntity* new_entity, const double &after_seconds = 0.0f) = 0;
			virtual void RemoveEntityFromCurrentWorld(core::IEntity* new_entity) = 0;
			virtual core::IUserPersistence* GetCurrentUser() = 0;
			virtual core::IWorldPersistence* GetCurrentWorld()= 0;
			virtual core::IUserPersistence* GetDefaultUser() = 0;
			virtual core::IWorldPersistence* GetDefaultWorld()= 0;
			virtual core::IEntityPersistence* GetAvatarEntity() = 0;
			virtual void GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions)=0;
			//virtual bool RunWorld(const int &world_id) = 0;
			virtual bool CloseSession() = 0;
			virtual bool DeleteUser(const std::string &name) = 0;
			virtual bool DeleteWorld(const std::string &name) = 0;

			virtual bool Calibrate()=0;
			virtual bool CalculateHomography()=0;
			virtual bool TrainBackground()=0;

			virtual bool SetCameraRecording(const bool &value)=0;	
			virtual bool SetUseRecording(const bool &value, const std::string &video_url_basename)=0;

			virtual IUserDataModelController* GetUserDataModelControllerInstance() = 0;


	};
}


#endif

