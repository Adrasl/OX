#include <controllers/RunningSceneController.h>

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/adapted/tuple_cartesian.hpp>
#include <boost/geometry/geometries/adapted/c_array_cartesian.hpp>
#include <boost/geometry/geometries/cartesian3d.hpp>
#include <boost/geometry/extensions/index/rtree/rtree.hpp>


#include <debugger.h> 

#define THRESHOLD 0.25
#define OFFSET_STEP 0.25

double time_loop  = 0;

RunningSceneController::RunningSceneController(core::IApplication *app_, core::IUserDataModelController *user_datamodel, core::IPercept *perception_module, core::IProd *production_module) 
: userDataModel(user_datamodel), perception(perception_module), production(production_module), initialized(false), stop_requested(false),
  presence_detected(false), contentCreationController(NULL), app(app_)
{
	contentCreationController = ContentCreationController::Instance();
	pdu.acceleration.x = pdu.acceleration.y = pdu.acceleration.z = 0;
	pdu.velocity.x = pdu.velocity.y = pdu.velocity.z = 0;
	pdu.position.x = pdu.position.y = pdu.position.z = 0;
	space_offset.x = space_offset.y = space_offset.z = 0;
	if ((perception) && (production))
		DoInit();
}

RunningSceneController::~RunningSceneController()
{
	Delete();
}

void RunningSceneController::Delete()
{
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void RunningSceneController::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&RunningSceneController::DoMainLoop, this ) ));
		//SetThreadName(m_thread->get_id(), "RunningSceneController");
	}
}

void RunningSceneController::DoMainLoop()
{
	initialized = true;
	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(5));
	}
}

void RunningSceneController::Iterate()
{
	std::map< int, std::vector<corePDU3D<double>> > presence_volume;
	core::corePoint3D<double> final_cam_pos;

	{
		boost::try_mutex::scoped_try_lock lock(m_mutex);

		if ((lock) && (app) && (perception) && (production))
		{
			current_user  = app->GetCurrentUser();
			current_world = app->GetCurrentWorld();

			int row_step = 3;
			float scale = 0.2;
			
			//BLOCK 1
			//----------------------------
			presence_detected = perception->PresenceDetected();
			perception->GetHeadPosition(head_pos);
			perception->GetFeaturePosition("CENTER OF MASS", presence_center_of_mass);
			perception->GetSpaceBoundingBox(space_bounding_box_min, space_bounding_box_max);
			perception->GetFeatureWeightedPositions("PRESENCE VOLUME", presence_volume, scale);

			//BLOCK 2: Positioning the camera
			//----------------------------
			com_to_head.x = head_pos.x - presence_center_of_mass.x;
			com_to_head.y = head_pos.y - presence_center_of_mass.y;
			com_to_head.z = head_pos.z - presence_center_of_mass.z;
			space_center.x = (space_bounding_box_max.x + space_bounding_box_min.x)/2;
			space_center.y = (space_bounding_box_max.y + space_bounding_box_min.y)/2;
			space_center.z = (space_bounding_box_max.z + space_bounding_box_min.z)/2;
			threshold_distance_to_minmax.x = THRESHOLD * (space_bounding_box_max.x - space_bounding_box_min.x);
			threshold_distance_to_minmax.y = THRESHOLD * (space_bounding_box_max.y - space_bounding_box_min.y);
			threshold_distance_to_minmax.z = THRESHOLD * (space_bounding_box_max.z - space_bounding_box_min.z);

			double diff = abs(space_bounding_box_max.x - presence_center_of_mass.x);
			if ( diff < abs(threshold_distance_to_minmax.x))
				space_offset.x += OFFSET_STEP*diff;
			else
			{	diff = abs(-1*space_bounding_box_min.x + presence_center_of_mass.x);
				if ( diff < abs(threshold_distance_to_minmax.x))
					space_offset.x -= OFFSET_STEP*diff;				}

			diff = abs(space_bounding_box_max.y - presence_center_of_mass.y);
			if ( diff< abs(threshold_distance_to_minmax.y))
				space_offset.y += OFFSET_STEP*diff;
			else
			{	diff = abs(-1*space_bounding_box_min.y + presence_center_of_mass.y);
				if ( diff < abs(threshold_distance_to_minmax.y))
					space_offset.y -= OFFSET_STEP*diff;				}

			diff = abs(space_bounding_box_max.z - presence_center_of_mass.z);
			if ( diff < abs(threshold_distance_to_minmax.z))
				space_offset.z += OFFSET_STEP*diff;
			else
			{	diff = abs(-1*space_bounding_box_min.z + presence_center_of_mass.z);
				if ( diff < abs(threshold_distance_to_minmax.z))
					space_offset.z -= OFFSET_STEP*diff;				}

			
			final_cam_pos.x = 0-(presence_center_of_mass.x + space_offset.x + com_to_head.x);
			final_cam_pos.y = presence_center_of_mass.y + space_offset.y + com_to_head.y;
			final_cam_pos.z = presence_center_of_mass.z + space_offset.z + com_to_head.z;

			final_cam_pos.x = presence_center_of_mass.x + 5*com_to_head.x;
			final_cam_pos.y = presence_center_of_mass.y + 5*com_to_head.y;
			final_cam_pos.z = presence_center_of_mass.z + 5*com_to_head.z;
		}

	}

	if ((app) && (perception) && (production))
	{
		// BLOCK 3: Creating the avatar
		//----------------------------
		if (presence_volume.size() > 0) 
		{	double timestamp = (double)clock()/CLOCKS_PER_SEC;
			void *graphic_node = production->CreateGraphicNode(presence_volume); 
			double timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			double dif_time = timestamp2 - timestamp;
			production->SetAvatar(graphic_node); 
		}
		production->SetUserPosition(final_cam_pos);

		double timestamp = (double)clock()/CLOCKS_PER_SEC;
		double dif_time = timestamp - time_loop;
		time_loop = timestamp;

		if (contentCreationController)
			contentCreationController->Update();
	}
		
}
