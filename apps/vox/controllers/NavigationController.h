#ifndef _NAVIGATIONCONTROLLER_
#define _NAVIGATIONCONTROLLER_

#include <Application.h>
#include <controllers/ContentCreationController.h>
//#include <core/IPersistence/IPersistence.h>
//#include <core/IPersistence/IUserPersistence.h>
//#include <core/IPersistence/IWorldPersistence.h>
//#include <ipersistence/types.h>
#include <boost/thread.hpp>
#include <core/types.h>
#include <vector>
//#include <SpatialIndex.h>

class Application;
class ContentCreationController;

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

class NavigationController
{
	public:
		NavigationController(core::IUserDataModelController *user_datamodel, core::IPercept *perception_module, core::IProd *production_module);
		~NavigationController();

		//bool CreateUser(const std::string &name, const std::string &passwd);
		//bool CreateWorld(const std::string &name, const std::string &owner, const int &permissions);
		//bool LoginUser(const std::string &name, const std::string &passwd);
		//void LogOut();
		//core::IUserPersistence* GetCurrentUser()		{return  (core::IUserPersistence*)app_user;}
		//core::IWorldPersistence* GetCurrentWorld()		{return  (core::IWorldPersistence*)app_world;}
		//core::IUserPersistence* GetDefaultUser()		{return  (core::IUserPersistence*)default_user;}
		//core::IWorldPersistence* GetDefaultWorld()		{return  (core::IWorldPersistence*)default_world;}
		//bool RunWorld(const std::string &name);
		//bool OpenSession(const std::string &user_name, const std::string &passwd, const std::string &world_name);
		//bool CloseSession();
		//bool CloseWorld();
		//bool LoadDefaultData();
		//////bool DeleteUser(int user_id);
		//////bool DeleteWorld(int world_id);
		//bool IsSessionClosed();
		//bool IsCurrentUser(const std::string &name);
		//bool IsCurrentWorld(const std::string &name);

	private:

		core::IUserDataModelController *userDataModel;
		core::IPercept	*perception;
		core::IProd		*production;

		ContentCreationController *contentCreationController;

		boost::shared_ptr<boost::thread> m_thread;
		boost::try_mutex m_mutex;
		bool initialized, stop_requested;
		bool presence_detected;
		core::corePoint3D<double> head_pos, presence_center_of_mass, com_to_head,
								  space_bounding_box_min, space_bounding_box_max, space_center, 
								  threshold_distance_to_minmax, space_offset;

		corePDU3D<double> pdu;

		double las_time;

	private:

		void DoInit();
		void Delete();
		void DoMainLoop();
		void Iterate();

		////User	*app_user;
		//core::ipersistence::UserPersistence		*app_user, *default_user;
		//core::ipersistence::WorldPersistence	*app_world, *default_world;
		//int session_permissions;
};

#endif