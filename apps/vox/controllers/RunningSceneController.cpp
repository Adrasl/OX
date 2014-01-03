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
//default_user(NULL), default_world(NULL), app_user(NULL), app_world(NULL), session_permissions(-1)
{
	contentCreationController = ContentCreationController::Instance();
	pdu.acceleration.x = pdu.acceleration.y = pdu.acceleration.z = 0;
	pdu.velocity.x = pdu.velocity.y = pdu.velocity.z = 0;
	pdu.position.x = pdu.position.y = pdu.position.z = 0;
	space_offset.x = space_offset.y = space_offset.z = 0;
	if ((perception) && (production))
		DoInit();
}

//bool RunningSceneController::LoadDefaultData()
//{
//	default_user  = new core::ipersistence::UserPersistence(); 
//	default_world  = new core::ipersistence::WorldPersistence(); 
//	bool successu = default_user->Load(std::string("default_user"));
//	bool successw = default_world->Load(std::string("default_world"));
//	return (successu && successw);
//}

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
//void RunningSceneController::Init()
//{
//	RunningSceneController::DoInit();
//}

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


	////--------------------------- 2D OK, 3D NOK 
	////BOOST GGL RTREE
	//boost::geometry::index::rtree<boost::geometry::box_3d, int> spatial_index(12, 4); // <box, ID>
 //  
	//// fill it somewhere, normally in a loop over polygons (here: polygon)
	//int id=0;
 //   boost::geometry::point_3d p1(0.0,0.0,0.0);
 //   boost::geometry::point_3d p2(0.1,0.1,0.1);
 //   boost::geometry::point_3d q1(0.2,0.2,0.2);
 //   boost::geometry::point_3d q2(0.3,0.3,0.3);
 //   boost::geometry::point_3d r1(0.15,0.15,0.15);
 //   boost::geometry::point_3d r2(0.25,0.25,0.25);

	//boost::geometry::box_3d box_p(p1,p2);
	//boost::geometry::box_3d box_q(q1,q2);
	//boost::geometry::box_3d box_r(r1,r2);
 //   spatial_index.insert(box_q, id++);
	//spatial_index.insert(box_p, id++);
	//spatial_index.insert(box_r, id++);

	//// get all ID's intersecting a specified box:
	//boost::geometry::point_3d box_corner_0(0.15,0.15,0.15);
	//boost::geometry::point_3d box_corner_1(0.30,0.30,0.30);
	//boost::geometry::box_3d query_box(box_corner_0,box_corner_1);
	//std::deque<int> overlapping = spatial_index.find(query_box);
	////cout << "result size=" << overlapping.size() << endl;
	//for (std::deque<int>::iterator iter = overlapping.begin(); iter != overlapping.end(); iter++)
	//	spatial_index.remove(query_box, *iter); 
	////---------------------------

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
			//std::vector<float> presence_volume;
			

			//BLOCK 1
			//----------------------------
			//double b1_timestamp = (double)clock()/CLOCKS_PER_SEC;
			presence_detected = perception->PresenceDetected();
			//double b1_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			//double b1_dif_time = b1_timestamp2 - b1_timestamp;
			//cout << "CALCULATING Block 1: " << b1_dif_time << "\n";
			//double b11_timestamp = (double)clock()/CLOCKS_PER_SEC;
			perception->GetHeadPosition(head_pos);
			//double b11_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			//double b11_dif_time = b11_timestamp2 - b11_timestamp;
			//cout << "CALCULATING Block 11: " << b11_dif_time << "\n";
			//double b12_timestamp = (double)clock()/CLOCKS_PER_SEC;
			perception->GetFeaturePosition("CENTER OF MASS", presence_center_of_mass);
			//double b12_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			//double b12_dif_time = b12_timestamp2 - b12_timestamp;
			//cout << "CALCULATING Block 12: " << b12_dif_time << "\n";
			//-----------------------------
			//TOO SLOW!! get ptoperties from cam not from images!!
			//-----------------------------
			//double b14_timestamp = (double)clock()/CLOCKS_PER_SEC;
			perception->GetSpaceBoundingBox(space_bounding_box_min, space_bounding_box_max);
			//double b14_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			//double b14_dif_time = b14_timestamp2 - b14_timestamp;
			//cout << "CALCULATING Block 14: " << b14_dif_time << "\n";

			//----------------------------
			
			//////perception->GetFeaturePositions("PRESENCE VOLUME", presence_volume, row_step, scale);
			//SLOW AS HELL // decomentar
			//double timestamp = (double)clock()/CLOCKS_PER_SEC;
			perception->GetFeatureWeightedPositions("PRESENCE VOLUME", presence_volume, scale);
			//std::vector<MotionElement> motion_elements = perception->GetMotionElements();
			//double timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			//double dif_time = timestamp2 - timestamp;
			//cout << "CALCULATING CLOUD: " << dif_time << "\n";


			//BLOCK 2: Positioning the camera
			//----------------------------
			//double b2_timestamp = (double)clock()/CLOCKS_PER_SEC;

			//cout << "PRESENCE-HEAD_POS        : X=" << head_pos.x << "Y=" << head_pos.y << "Z=" << head_pos.z << "\n";
			//cout << "PRESENCE-CENTEROFMASS_POS: X=" << presence_center_of_mass.x << "Y=" << presence_center_of_mass.y << "Z=" << presence_center_of_mass.z << "\n";

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

			
			//production->SetAvatar("teapot");
			//production->SetCamerasPosition(head_pos);
			//production->SetCamerasPDU(pdu);
		}

	}

	if ((app) && (perception) && (production))
	{
		// BLOCK 3: Creating the avatar
		if (presence_volume.size() > 0) 
		{	//std::cout << "---------------ENTERED BLOCK 3" << "\n";
			double timestamp = (double)clock()/CLOCKS_PER_SEC;
			void *graphic_node = production->CreateGraphicNode(presence_volume); 
			double timestamp2 = (double)clock()/CLOCKS_PER_SEC;
			double dif_time = timestamp2 - timestamp;
			//cout << "CALCULATING MESH: " << dif_time << "\n";
			production->SetAvatar(graphic_node); 
		}
		production->SetUserPosition(final_cam_pos);

		double timestamp = (double)clock()/CLOCKS_PER_SEC;
		double dif_time = timestamp - time_loop;
		//cout << "NAVIGATOR LOOP: " << dif_time << "\n";
		time_loop = timestamp;

		if (contentCreationController)
			contentCreationController->Update();
	}
		
}
