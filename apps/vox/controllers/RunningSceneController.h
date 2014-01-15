#ifndef _RUNNINGSCENECONTROLLER_
#define _RUNNINGSCENECONTROLLER_

#include <Application.h>
#include <controllers/ContentCreationController.h>
#include <boost/thread.hpp>
#include <core/types.h>
#include <vector>

class Application;
class ContentCreationController;

class RunningSceneController
{
	public:
		RunningSceneController(core::IApplication *app_, core::IUserDataModelController *user_datamodel, core::IPercept *perception_module, core::IProd *production_module);
		~RunningSceneController();

	private:

		core::IUserPersistence* current_user;
		core::IWorldPersistence* current_world;

		core::IApplication *app;
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
};

#endif