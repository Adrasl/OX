#ifndef _GUIGENERICCONTROLLER_
#define _GUIGENERICCONTROLLER_

#include <igui/maingui.h>
#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>

namespace core
{
	namespace igui
	{
		class MainGui;

		class GUIGenericController
		{
			public:
				GUIGenericController(MainGui *main_gui_);
				~GUIGenericController();

				static GUIGenericController* GetInstance() {return instance;}

				bool CreateUser(const std::string &name, const std::string &passwd);
				bool LoginUser(const std::string &name, const std::string &passwd);
				bool DeleteUser(const std::string &name);
				bool DeleteWorld(const std::string &name);
				void LogOut();
				bool UserExists(const std::string &name);
				core::IUserPersistence* GetCurrentUser();
				void GetWorldList(const std::string &user_name, std::vector<std::string> &names, std::vector<int> &permissions);
				bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions);				
				bool RunWorld(const std::string &name);
				bool WorldExists(const std::string &name);
				bool SaveWorldPermissions(const std::string &name, const int permissions);
				core::IWorldPersistence* GetCurrentWorld();
				void ViewUserInfoPanel();
				void ViewLogPanel();
				void ViewConfigurePanel();

				/** \brief Requests GUIModule to request the application to calibrate the cameras. */
				void Calibrate();
				void CalculateHomography();
				void TrainBackground();

				bool SetCameraRecording(const bool &value);	
				bool SetUseRecording(const bool &value, const std::string &url);	

			private:

				static GUIGenericController *instance;
				MainGui *main_gui;
		};
	}
}

#endif