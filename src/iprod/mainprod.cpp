#include <mainprod.h>
#include <boost/filesystem.hpp>
#include <pnmImage.h>
#include <filename.h>
#include <executionEnvironment.h>
#include <renderAttrib.h>
#include <cullFaceAttrib.h>
#include <string>
#include <antialiasAttrib.h>

#if PANDA_NUMERIC_VERSION >= 1008000
#define Colorf LColorf
#endif

#include "auto_bind.h"
#include "AnimControlCollection.h"
#include "ambientLight.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "spotlight.h"
#include "geomTriangles.h"

#ifdef WIN32
#include <Aclapi.h>
#endif
//#include <debugger.h> 

#include "ProdMarchingCubes.h"

using namespace core;
using namespace core::iprod;
unsigned int MainProd::num_windows = 2;

#define SHOW_COLLISION false
#define USE_AVATAR_3D_MESH false




PandaFramework MainProd::framework;
Thread* MainProd::graphic_thread(NULL);
PT(AsyncTaskManager) MainProd::taskMgr;
PT(ClockObject) MainProd::globalClock;

VirtualFileSystem* MainProd::vfs;

boost::try_mutex MainProd::m_mutex;
bool MainProd::initialized = false;
bool MainProd::stop_requested = false;

boost::shared_ptr<boost::thread> MainProd::m_thread;
std::map<int, Prod3DWindow*>     MainProd::prod3Dwindow_array;
std::map<int, WindowFramework*>	 MainProd::pandawindows_array;
std::map<int, NodePath>			 MainProd::windowcamera_array;
std::map<int, CCommonFilters*>	 MainProd::ccommonfilters_array;
std::map<int, DisplayRegion *>	 MainProd::displayregions_array;
std::vector< NodePath >			 MainProd::camera_array;
int MainProd::last_window_id = 0;
int MainProd::m_argc = 0;
char** MainProd::m_argv;
NodePath MainProd::cam_viewpoint = NodePath();
NodePath MainProd::origin = NodePath();
NodePath MainProd::up = NodePath();

//sf::SoundBufferRecorder MainProd::Recorder;
//sf::SoundBuffer MainProd::recordingBuffer;

float MainProd::m_peo=0;
float MainProd::m_caca=0;

sf::SoundBuffer MainProd::sound_Buffer;
double MainProd::listener_position[]	= {50.0f,50.0f,1.0f};
double MainProd::listener_target[]		= {0.0f,-1.0f,0.0f};
double MainProd::sound_pos[]			= {0.1f,0.1f,0.1f};
sf::Sound MainProd::Sound;

core::IUserPersistence   *MainProd::current_user  = NULL;
core::IWorldPersistence  *MainProd::current_world = NULL;
core::IUserPersistence   *MainProd::default_user  = NULL;
Prod3DEntity			 *MainProd::user_entity	  = NULL;
NodePath				 *MainProd::user_nodepath = NULL;
core::IEntityPersistence *MainProd::user_dummyPersistence = NULL;
std::vector<Prod3DEntity *> MainProd::scene_entities;
std::map<Prod3DEntity*, NodePath*> MainProd::scene_entities_nodepaths;
std::map<NodePath*, Prod3DEntity*>	MainProd::scene_nodepaths_entities;
bool MainProd::use_master_display = true;
Prod3DWindow*	 MainProd::master_prod3Dwindow=NULL;
WindowFramework* MainProd::master_pandawindow=NULL;
NodePath MainProd::master_camera;

IApplicationConfiguration *MainProd::app_config =NULL;

//Fog *MainProd::m_fog;
//PT(Fog) MainProd::m_fog;
Fog MainProd::m_fog("SceneFog");
corePoint3D<float> MainProd::background_color;
corePoint3D<float> MainProd::fog_color;
float MainProd::fog_intensity = 0.25;
corePoint3D<float> MainProd::desired_background_color;
corePoint3D<float> MainProd::desired_fog_color;
float MainProd::desired_fog_intensity = 0.25;

////---------destroy when ready
//NodePath MainProd::environment;
//NodePath MainProd::pandaActor;
//NodePath MainProd::pandaActor2;
//NodePath MainProd::pandaActor3; 
//NodePath MainProd::pandaActor4;
////---------------------------

core::corePoint3D<double> MainProd::pt0, MainProd::pt1, MainProd::pti, MainProd::vel, MainProd::vel0, MainProd::vel1, MainProd::acc;
double MainProd::last_interval=0;
double MainProd::last_time, MainProd::last_loop_t = 0;
core::corePDU3D<double> MainProd::last_pdu;
int MainProd::dummy_erase_me=0;

CollisionHandlerQueue *MainProd::collision_handler_queue=NULL;
CollisionTraverser *MainProd::collision_traverser=NULL;
std::map< Prod3DEntity *, CollisionNode * > MainProd::entity_collider_array;
std::map< const CollisionSolid *, core::corePDU3D<double> > MainProd::avatar_collider_array;
NodePath *MainProd::avatar_current_graphicNodePath = NULL;
std::map< const CollisionSolid *, Prod3DEntity* > MainProd::entities_collider_array;
std::vector< Prod3DEntity * > MainProd::entity_collidable_array_to_register;
std::map< Prod3DEntity *, double > MainProd::entity_array_to_be_loaded_afterseconds;
//std::map< Prod3DEntity *, double > MainProd::entity_array_to_be_removed_afterseconds;
std::vector<NodePath*> MainProd::testnodepaths;
bool MainProd::insert_now = false;

std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> > MainProd::music_melody_samples; 
std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > MainProd::music_base_samples;
std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > MainProd::music_decoration_samples;

float MainProd::background_animation_starttimestamp = 0.0f;
float MainProd::background_animation_endtimestamp   = 0.0f;
float MainProd::current_timestamp   = 0.0f;

AnimControlCollection MainProd::SceneAnimControlCollection;

bool MainProd::enableEffects = false;
bool MainProd::enable_simpleINVERTEffect = false;
bool MainProd::enable_simpleBLOOMEffect = false;
bool MainProd::enable_simpleTOONEffect = false;
bool MainProd::enable_simpleBLUREffect = false;
bool MainProd::enable_simpleVOLUMETRICLIGHTSEffect = false;
bool MainProd::enable_simpleSSAOEffect = false;
NodePath* MainProd::fake_background_quad = NULL; //As there's a panda bug that forbids changing the background color when using ImageFilters
NodePath* MainProd::default_ambientLight = NULL;

MainProd::MainProd(IApplicationConfiguration *app_config_, int argc, char *argv[], bool enable_effects) : mesh_factory(NULL)
{
	boost::mutex::scoped_lock lock(m_mutex);

	enableEffects = enable_effects;

	m_argc = argc;
	m_argv = argv;
	app_config = app_config_;

	background_color.x = background_color.y = background_color.z = 0.0;
	fog_color.x = fog_color.y = fog_color.z = 0.0;
	fog_intensity = 0.0f;

	desired_background_color.x = desired_background_color.y = desired_background_color.z = 0.0;
	desired_fog_color.x = desired_fog_color.y = desired_fog_color.z = 0.0;
	desired_fog_intensity = 0.0f;

	pt0.x = pt0.y = pt0.z = 0;
	pt1 = pti = vel = vel0 = vel1 = acc = pt0;
	last_pdu.velocity = last_pdu.position = last_pdu.acceleration = pt0;
		
	//ApplicationConfiguration *app_config = ApplicationConfiguration::GetInstance();
	if (app_config != NULL)
	{
		num_windows = app_config->GetNumDisplays();
	}

	//int xargg = 0;
	//char **xargv;
	//framework.open_framework( argc, argv );
	bool t  = Thread::is_threading_supported();
	bool tt = Thread::is_true_threads();

	//framework.open_framework( m_argc, m_argv );

	mesh_factory = new MeshFactory();
	mesh_factory->SetMarchingCubesConfig(75, 10, 0.005, 0.085, 48.0); //MarchingCubesConfig 16x16x16 original//retomar
}

MainProd::~MainProd()
{
	boost::mutex::scoped_lock lock(m_mutex);

	m_thread->join();
	for (std::map<int, Prod3DWindow*>::iterator iter=prod3Dwindow_array.begin(); iter != prod3Dwindow_array.end(); iter++)
		delete iter->second;
	prod3Dwindow_array.erase(prod3Dwindow_array.begin(), prod3Dwindow_array.end());

	/*for (std::map< Prod3DEntity *, CollisionNode * >::iterator iter = entity_collider_array.begin(); iter != entity_collider_array.end(); iter++)
		delete iter->second;

	if(collision_handler_queue)
		delete collision_handler_queue;
	if(collision_traverser)
		delete collision_traverser;*/
}

void MainProd::Delete()
{
	CloseWorld();
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void MainProd::PostLogMessage(const std::string &message)
{
	if (app != NULL) app->PostLogMessage(message);
}

void MainProd::DoMainLoop()
{
	{
		boost::mutex::scoped_lock lock(m_mutex);

		framework.open_framework( m_argc, m_argv );
		GraphicsPipe* gpipe = framework.get_default_pipe();
		GraphicsEngine* gengine = framework.get_graphics_engine();
		gengine->init_type();

		#ifndef _DEBUG
		//Collisions
		if (!collision_handler_queue)
		{
			collision_handler_queue = new CollisionHandlerQueue();
			collision_handler_queue->clear_entries();
		}
		if (!collision_traverser)
			collision_traverser = new CollisionTraverser();
		#endif
		
		//collision_traverser.add_collider(NodePath collider, collision_handler_queue);
		
		//boost::thread::native_handle_type native_handler_t = m_thread->native_handle();

		//#ifdef WIN32
		//unsigned int cores = boost::thread::hardware_concurrency();
		//int mask = (1 << cores)-1;

		//HANDLE hProces = GetCurrentProcess();
		////HANDLE hThread = m_thread->native_handle();//GetCurrentThread();
		//int ss = SetSecurityInfo(hProces, SE_UNKNOWN_OBJECT_TYPE ,PROCESS_ALL_ACCESS, NULL, NULL, NULL, NULL);
		////int sx = SetSecurityInfo(hThread, SE_UNKNOWN_OBJECT_TYPE ,THREAD_ALL_ACCESS, NULL, NULL, NULL, NULL);
		//int pe = SetProcessAffinityMask(hProces, mask);
		////int re = SetThreadAffinityMask(hThread, 4);
		////re = SetThreadAffinityMask(hThread, 4);
		//#endif

		//while (true)
		//{
		//	int peo = 5;
		//	m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(1));
		//}
		//m_fog = new Fog();
		//m_fog->set_mode(Fog::M_exponential);
		//m_fog->set_color(0.5f,0.0f,1.0f); 
		//m_fog->set_exp_density(0.5f); 

		bool ts = Thread::is_threading_supported();
		bool tt = Thread::is_true_threads();
		//Thread::init_main_thread();
		//freopen("panda_log_info.txt", "w+", stdout);
		//printf("xXcaca");
		graphic_thread = Thread::get_current_thread();	
		Thread *mt = Thread::get_main_thread();
		//framework.open_framework( m_argc, m_argv );
		//int xargg = 0;
		//char **xargv;
		//framework.open_framework( m_argc, xargv );
		boost::filesystem::path data_path = boost::filesystem::initial_path();
		taskMgr = AsyncTaskManager::get_global_ptr();
		globalClock = ClockObject::get_global_clock();
	//	graphic_thread = Thread::get_current_thread();	

		//vfs = VirtualFileSystem::get_global_ptr();

		CreateDefaultWindows(num_windows);
		if (collision_traverser && SHOW_COLLISION)
			collision_traverser->show_collisions(pandawindows_array[1]->get_render());

	   if(!pandawindows_array[1]->get_graphics_window()->get_gsg()->get_supports_basic_shaders())
		  cout << "Glow Filter: Video driver reports that shaders are not supported.";

		//NOTES---------
		GraphicsEngine	*ge = framework.get_graphics_engine();		//only one, global, pointers and buffers
		GraphicsPipe	*gp = framework.get_default_pipe();			//at least one, OGL/DX
		GraphicsWindow	*gw = pandawindows_array[1]->get_graphics_window();		//can be many
		//GraphicsWindow	*gw2 = pandawindows_array[1]->get_graphics_window();
		DisplayInformation	*dp = gp->get_display_information();
		GraphicsDevice		*gd = gp->get_device();
		//WindowProperties win_props_fs = WindowProperties(); 
		//int d_id = dp->get_device_id();
		//dp->get_display_mode_height(d_id);
		//int max_w = dp->_maximum_window_width;
		//int max_h = dp->_maximum_window_height;
		//win_props_fs.set_size(max_w, max_h); 
		//win_props_fs.set_fullscreen(true);
		////gw->request_properties(win_props_fs);
		//gw->set_properties_now(win_props_fs);
		//framework.close_window(1);
		//window = framework.open_window(win_props_fs);
		//--------------

		//Load the environment
		/////////////////////////////////////////////////////////////LoadDefaultScene();
		//LoadEmptyScene();
		//LoadDefaultScene();

		//---------------Sound stuff-----------------------------
		sf::SoundBuffer Buffer;
		if (!Buffer.LoadFromFile("f://etc//repos//OX//motor.wav"))
			int error_loadfromfile = 1;		
		unsigned int chan = Buffer.GetChannelsCount();
		sf::Listener::SetGlobalVolume(100.f);
		Sound.SetBuffer(Buffer);
		Sound.SetLoop(true);
		Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);
		//Sound.SetRelativeToListener(true);
		Sound.SetMinDistance(10.f);
		Sound.SetAttenuation(0.75f);
		Sound.SetPitch(1.0f);
		Sound.SetVolume(100.f);
		//Recorder.Start();
		//------------------------------------------------------

		//initialized = true;
		
		//----sound stuff----------------
		//Sound.Play();	
		//-------------------------------

	}

	//m_fog = Fog("Scene Fog");

	while(!stop_requested)
	{
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
		Iterate();
	}

	//----sound stuff----------------
	//Recorder.Stop();
	//recordingBuffer = Recorder.GetBuffer();
	//-------------------------------

	framework.close_all_windows();
	framework.close_framework();
	
	//fclose(stdout);
}

void MainProd::Iterate()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);

	if (lock && initialized)
	{
		current_timestamp = (double)clock()/CLOCKS_PER_SEC;

		#ifndef _DEBUG
		CheckCollisions(); 
		UpdateEntities();  
		ProcessProd3DEntitiesToBeLoadedQueue();
		//ProcessProd3DEntitiesToBeRemovedQueue();
		#endif
		////-----
		//if (insert_now)
		//{
		//	AddTestQuad();
		//	insert_now = false;
		//}
		////-----
		//----------------------
		//WindowProperties win_props = WindowProperties(); 
		//ApplicationConfiguration *app_config = ApplicationConfiguration::GetInstance();

		for (unsigned int i = 1; lock && (i <= num_windows); i++)
		{
			if (app_config != NULL)
			{
				//background_color.x += 0.001; background_color.y += 0.001; background_color.z += 0.001;
				//fog_color.x += 0.001; fog_color.y += 0.001; fog_color.z += 0.001;
				//fog_intensity += 0.001;

				float bg_diff_time = background_animation_endtimestamp - current_timestamp;
				if (bg_diff_time >= 0.0f && CheckDesiredBackgroundAndFogRanges())
				{
					float factor = 1.0f;
					float anim_starttoend_time = background_animation_endtimestamp - background_animation_starttimestamp;
					if (anim_starttoend_time > 0.001f)
						factor = 1.0f - ( bg_diff_time / (background_animation_endtimestamp - background_animation_starttimestamp));

					background_color.x = background_color.x + ((desired_background_color.x - background_color.x)*factor);
					background_color.y = background_color.y + ((desired_background_color.y - background_color.y)*factor);
					background_color.z = background_color.z + ((desired_background_color.z - background_color.z)*factor);

					fog_color.x = fog_color.x + ((desired_fog_color.x - fog_color.x)*factor);
					fog_color.y = fog_color.y + ((desired_fog_color.y - fog_color.y)*factor);
					fog_color.z = fog_color.z + ((desired_fog_color.z - fog_color.z)*factor);

					fog_intensity = fog_intensity + ((desired_fog_intensity - fog_intensity)*factor);
					CheckBackgroundAndFogRanges();
				}
				m_fog.set_mode(Fog::M_exponential);
				m_fog.set_color(fog_color.x, fog_color.y, fog_color.z); 
				m_fog.set_exp_density(fog_intensity); 
				pandawindows_array[i]->get_render().set_fog(&m_fog, 1);
				pandawindows_array[i]->get_display_region_3d()->set_clear_color(LColor(background_color.x, background_color.y, background_color.z, 1));
				if (fake_background_quad)
					fake_background_quad->set_color(LColor(background_color.x, background_color.y, background_color.z, 1));
				
				
				//win_props.set_size(app_config->GetDisplayData(i).resolution_x, app_config->GetDisplayData(i).resolution_y);
				//pandawindows_array[i];
				core::DisplayData display_data = app_config->GetDisplayData(i);

				int flip_me_x = (display_data.flip_h) ? -1 : 1;
				int flip_me_y = (display_data.flip_v) ? -1 : 1;
				//unsigned int num_children = windowcamera_array[i].get_num_children(graphic_thread);
				unsigned int num_children = pandawindows_array[i]->get_num_cameras();
				//windowcamera_array[i].get_child(0,graphic_thread);
				//for (unsigned int i = 0; i < num_children; i++)
				//{
				//	pandawindows_array[i]->get_camera(i)->.set_sx(flip_me_x);
				//	windowcamera_array[i].get_child(i,graphic_thread).set_sy(flip_me_y);
				//}
				windowcamera_array[i].set_sx(flip_me_x);
				windowcamera_array[i].set_sz(flip_me_y);
				if ( (flip_me_x == -1) ^ (flip_me_y == -1) )
					pandawindows_array[i]->get_render().set_attrib(CullFaceAttrib::make_reverse());
				else
					pandawindows_array[i]->get_render().set_attrib(CullFaceAttrib::make_default());

				//view cam orientation--
				/** \todo fix this. make general */
				//FRONT
				if ( (display_data.x == 0) && (display_data.y == 1) && (display_data.z == 0) )
					windowcamera_array[i].set_hpr(0, 0, 0);
				//BACK
				else if ( (display_data.x == 0) && (display_data.y == -1) && (display_data.z == 0) )
					windowcamera_array[i].set_hpr(180.0, 0, 0);
				//RIGHT
				else if ( (display_data.x == 1) && (display_data.y == 0) && (display_data.z == 0) )
					windowcamera_array[i].set_hpr(90.0, 0, 0);
				//LEFT
				else if ( (display_data.x == -1) && (display_data.y == 0) && (display_data.z == 0) )
					windowcamera_array[i].set_hpr(-90.0, 0, 0);
				//UP
				else if ( (display_data.x == 0) && (display_data.y == 0) && (display_data.z == 1) )
					windowcamera_array[i].set_hpr(0, 90.0, 0);
				//DOWN
				else if ( (display_data.x == 0) && (display_data.y == 0) && (display_data.z == -1) )
					windowcamera_array[i].set_hpr(0, -90.0, 0);
				//-------------------------
			}
		}
		//--------------------------

		//Filename mydir = ExecutionEnvironment::get_binary_name();
		//mydir = mydir.get_dirname();

		//PNMFileType *mierda = NULL;
		//PNMImage image("/c/etc/projectVOX/help.png");
		//mierda = image.get_type();
		//std::stringstream wop;
		//image.write(wop, "/c/etc/projectVOX/help.png");
		//std::string wopp = wop.str();
		//std::ostringstream shit;
		//
		//PNMWriter *pnmw = image.make_writer(&shit);
		//image.write(pnmw);
		//std::string mierdas = shit.str();

		DoDoStuff();

		framework.do_frame(graphic_thread);
		//AM->update();
		CIntervalManager::get_global_ptr()->step();
	}
}

void MainProd::Init()
{
	MainProd::DoInit();
}

void MainProd::DoStuff()
{
	MainProd::DoDoStuff();
}

void MainProd::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&MainProd::DoMainLoop)));
	}
}

void MainProd::UpdateEntities() 
{
	for (std::vector< Prod3DEntity * >::iterator iter = scene_entities.begin(); iter != scene_entities.end(); ) //retomar x, el remove da problemas, problemas 
	{	if (*iter)
		{	if ((*iter)->IsReadyToDie())
			{
				IEntity *entity_to_remove = (IEntity*)(*iter);
				PrivateRemoveEntityFromCurrentWorld(entity_to_remove);
				int i = 666;
			}
			else
			{	
				(*iter)->OnUpdate();
				iter++;
			}
		}
	}
}

void MainProd::CheckCollisions() 
{
	collision_traverser->traverse(pandawindows_array[1]->get_render());

	//Check collision entries
	if (collision_handler_queue)
		for (int i = 0; i < collision_handler_queue->get_num_entries(); i++)
		{
			CollisionEntry *collision_entry = collision_handler_queue->get_entry(i);
			if (collision_entry)
			{
				const CollisionSolid *into_object = collision_entry->get_into();
				const CollisionSolid *from_object =collision_entry->get_from();
				const CollisionSolid *into_collision_node = collision_entry->get_into();
				if (into_object && from_object)
				{	std::map< const CollisionSolid *, core::corePDU3D<double> >::iterator found = avatar_collider_array.find(into_collision_node); 
					core::corePDU3D<double> avatar_collision_info;
					if ( found != avatar_collider_array.end() ) 
					{	avatar_collision_info = avatar_collider_array[into_collision_node];
						entities_collider_array[from_object]->OnUserCollisionCall(avatar_collision_info);
					}
					else
						entities_collider_array[from_object]->OnCollisionCall(entities_collider_array[into_object]);
				}
				//cout << "Entry collider: " << collision_handler_queue->get_entry(i) <<  "\n"; //getfrom getinto retomar
			}
		} 

	//Add new colliders
	bool clear_me = false;
	for (std::vector< Prod3DEntity * >::iterator iter = entity_collidable_array_to_register.begin(); iter != entity_collidable_array_to_register.end(); iter++)
	{
		if( collision_traverser && ( (*iter)->IsCollidable() || ((*iter)->GetData() == "teapot")  || ((*iter)->GetData() == "panda-model")) )
		{
			NodePath* np = (*iter)->GetNodePath();
			//int bounds = np->get_bounds();
			PT(BoundingSphere) bs = DCAST(BoundingSphere, np->get_bounds());
			int radius = bs->get_radius();
			float scale;
			(*iter)->GetEntity()->GetScale(scale);
			scale += 0.001f;
			LPoint3f centerOfEntity; centerOfEntity.zero();
			CollisionSphere *collision_solid = new CollisionSphere(0,0,0,0.5*radius/scale); 
			std::stringstream wop;
			wop << "e" << dummy_erase_me ;
			std::string nombre = wop.str();
			dummy_erase_me++;
			CollisionNode *collision_node = new CollisionNode(nombre); //No debería generar memory leaks al vincularse a un NodePath
			collision_node->add_solid(collision_solid);
			NodePath col_node = (*iter)->GetNodePath()->attach_new_node(collision_node);
			/*col_node.set_render_mode(RenderModeAttrib::Mode::M_wireframe, 2);*/
			if (SHOW_COLLISION) col_node.show();
			entity_collider_array[(*iter)] = collision_node;
			entities_collider_array[collision_solid] = (*iter);
			collision_traverser->add_collider(col_node, collision_handler_queue);
			clear_me = true;
		}
	}
	if (clear_me) 
		entity_collidable_array_to_register.clear();
}

void MainProd::CloseWorld()
{
	for (unsigned int i = 0; i < scene_entities.size(); i++)
		scene_entities[i]->Save();

	if (current_world)
		current_world->Save();

	ClearScene();
	{	
		boost::mutex::scoped_lock lock(m_mutex);
		
		if (user_entity)
			delete user_entity;

		current_user  = NULL;
		current_world = NULL;
		user_entity	  = NULL;
		user_dummyPersistence = NULL;

		for (unsigned int i = 0; i < scene_entities.size(); i++)
			delete scene_entities[i];

		scene_entities.clear();
		scene_entities_nodepaths.clear();
		scene_nodepaths_entities.clear();
		entity_collidable_array_to_register.clear();
		entities_collider_array.clear();
		avatar_collider_array.clear();
		testnodepaths.clear();

		//for (std::map< Prod3DEntity *, CollisionNode * >::iterator iter = entity_collider_array.begin(); iter != entity_collider_array.end(); iter++)
		//	delete iter->second;
	}
}

void MainProd::RemoveEntityFromScene(Prod3DEntity * entity)
{	
	if (entity)
	{
		NodePath *np = entity->GetNodePath(); 

		std::map<Prod3DEntity*, NodePath*>::iterator iter_enp =	scene_entities_nodepaths.find(entity);
		if (iter_enp != scene_entities_nodepaths.end())
			scene_entities_nodepaths.erase(iter_enp);

		std::map<NodePath*, Prod3DEntity*>::iterator iter_np = scene_nodepaths_entities.find(np);
		if (iter_np != scene_nodepaths_entities.end())
			scene_nodepaths_entities.erase(iter_np);

		std::vector<Prod3DEntity *>::iterator iter_e = std::find(scene_entities.begin(), scene_entities.end(), entity);
		if (iter_e != scene_entities.end())
			scene_entities.erase(iter_e);

		if (np) 
		{
			np->hide();
			np->stash();
			np->detach_node();
			np->remove_node(); //retomar //this is the only one needed, and for some reason it is not removing the node from the graph
		}

		//entity->OnDestroy();
		entity->DeletePersistence();
		delete entity; //retomar, memory leak
	}
}

void MainProd::ProcessProd3DEntitiesToBeLoadedQueue()
{
	for (std::map< Prod3DEntity *, double >::iterator iter = entity_array_to_be_loaded_afterseconds.begin(); (entity_array_to_be_loaded_afterseconds.size() >0 ) && (iter != entity_array_to_be_loaded_afterseconds.end());  )
	{
		double current_timestamp = (double)clock()/CLOCKS_PER_SEC;
		double birth_timestamp = iter->second;
		if ( (birth_timestamp - current_timestamp <= 0.0f) && (iter->first) )
		{	
			//current_world->AddEntity(*(iter->first->GetEntity()));
			//current_world->Save();
			LoadEntityIntoScene(iter->first);
			iter->first->OnStart();	
			std::map< Prod3DEntity *, double >::iterator iter_to_delete = iter;
			iter++;
			entity_array_to_be_loaded_afterseconds.erase(iter_to_delete);
		}
		else
		{
			iter++;
		}
	}
	//entity_array_to_be_loaded_afterseconds.clear();
}

//void MainProd::ProcessProd3DEntitiesToBeRemovedQueue()
//{
//	for (std::map< Prod3DEntity *, double >::iterator iter = entity_array_to_be_removed_afterseconds.begin(); iter != entity_array_to_be_removed_afterseconds.end();  )
//	{
//		double current_timestamp = (double)clock()/CLOCKS_PER_SEC;
//		double death_timestamp = iter->second;
//		if ( death_timestamp - current_timestamp <= 0.0f)
//		{	RemoveEntityFromScene(iter->first);
//			entity_array_to_be_removed_afterseconds.erase(iter);
//		}
//		else
//			iter++;
//	}
//	//entity_array_to_be_removed_afterseconds.clear();
//}

void MainProd::LoadEntityIntoScene(Prod3DEntity * entity)
{
	////std::string data = entity->GetData();
	//std::string data = "environment";
	//scene_entities_nodepaths[entity] = pandawindows_array[1]->load_model(framework.get_models(),data.c_str());
	////entity->SetNodePath(new_nodepath);
	////new_nodepath = entity->GetNodePath();
	////scene_entities_nodepaths[entity] = new_nodepath;

	//scene_entities_nodepaths[entity].set_scale(0.25,0.25,0.25);
	//scene_entities_nodepaths[entity].set_pos(-8,42,0);
	//scene_entities_nodepaths[entity].reparent_to(pandawindows_array[1]->get_render());

	//std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
	//iter++;
	//while(iter != pandawindows_array.end())
	//{
	//	scene_entities_nodepaths[entity].instance_to(iter->second->get_render());
	//	iter++;
	//}
	scene_entities.push_back(entity);
	std::string data = entity->GetData();
	std::stringstream model_url;
	std::stringstream animation_url;
	model_url << data << ".egg";	
	animation_url << data << "-animation.egg";	

	std::string model_path = model_url.str();
	std::string animation_path = animation_url.str();
	Filename model_file = Filename::from_os_specific(model_path);
	Filename animation_file = Filename::from_os_specific(animation_path);

	NodePath *new_model = new NodePath(pandawindows_array[1]->load_model(framework.get_models(),model_file));
	NodePath anim_np = pandawindows_array[1]->load_model(*new_model, animation_file);
	scene_entities_nodepaths[entity]    = new_model;
	scene_nodepaths_entities[new_model] = entity;
	entity->SetNodePath(scene_entities_nodepaths[entity]);
	entity->StartAnimations();
	if (entity->IsCollidable())
		entity_collidable_array_to_register.push_back(entity);

	scene_entities_nodepaths[entity]->reparent_to(pandawindows_array[1]->get_render());
	std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
	iter++;
	while(iter != pandawindows_array.end())
	{
		scene_entities_nodepaths[entity]->instance_to(iter->second->get_render());
		//iter->second->setup_trackball();//comment
		iter++;
	}
	if (use_master_display)
	{
		scene_entities_nodepaths[entity]->instance_to(master_pandawindow->get_render());
	}

	//if (anim_np.get_error_type() == NodePath::ET_ok)
	//	anim_collection.loop_all(true); //retomar MEMORY LEAK

	// RESTART ALL ANIMATIONS EVERYTIME WE INSERT AN ENTITY
	//pandawindows_array[1]->loop_animations(PartGroup::HMF_ok_part_extra | PartGroup::HMF_ok_anim_extra | PartGroup::HMF_ok_wrong_root_name);

}

bool MainProd::RunWorld(core::IUserPersistence  *user, core::IWorldPersistence *world)
{	
	//CloseWorld();
	
	boost::mutex::scoped_lock lock(m_mutex);

	if ( (user != NULL) && (world != NULL) )
	{	
		{	
			current_user  = user;
			current_world = world;	
			default_user = app->GetDefaultUser();

			////////////LoadEntityIntoScene(NULL);
			//////////for (int i=0; i < current_world->GetNumEntities(); i++)
			//////////{
			//////////	IEntityPersistence *ient = current_world->GetEntity(i);
			//////////	Prod3DEntity *new_entity = new Prod3DEntity(ient);
			//////////	//scene_entities.push_back(new_entity);
			//////////	LoadEntityIntoScene(new_entity);
			//////////}
			SetUpUser();

			//Sound.Play(); //retomar

			boost::filesystem::path data_path = boost::filesystem::initial_path();
			data_path = data_path / "../../bin/data"; 
			data_path = data_path / "/images/help.png";
			std::string mi_path = data_path.normalize().string();
			int pos = mi_path.find(":");
			if (pos != std::string::npos) mi_path.erase(pos, 1);
			std::replace(mi_path.begin(), mi_path.end(), '\\', '/');
			mi_path = "/" + mi_path;

			//boost::filesystem::path data_path0 = "./../../bin/data";
			//bool exists0_0 = boost::filesystem::exists(data_path0);

			//PNMImage image1("/c/etc/projectVOX/help.png");
			//PNMImage image2(mi_path);

			//Filename mydir = ExecutionEnvironment::get_binary_name();
			//mydir = mydir.get_dirname();



			int inn = 6;

			//----
			//PNMFileType *mierda = NULL;
			//PNMImage image("/c/etc/projectVOX/help.png");
			//mierda = image.get_type();
			////size_t size1 = _msize((void*)&image);
			////PNMImage image2("/c/etc/projectVOX/ProjectVOX.png");
			////size_t size2 = _msize((void*)&image2);
			////PNMImage image3("/c/etc/projectVOX/perro.jpg");
			////size_t size3 = _msize((void*)&image3);
			////PNMImage image4("/c/etc/projectVOX/celebras.jpg");
			////size_t size4 = _msize((void*)&image4);

			//std::ostringstream wop;
			//std::ostringstream shit;
			//image.write(shit,"",mierda);


			////-------
			//std::streambuf * buf;
			//std::ofstream of;
			//if(!false) { of.open("file.txt"); buf = of.rdbuf();
			//} else {buf = std::cout.rdbuf();}
			//std::ostream out(buf);
			////-------

			//image.write(wop);

			//for (int i=0; i < current_world->GetNumEntities(); i++)
			//{
			//	std::string mierdaaaa = wop.str();
			//	IEntityPersistence *ient = current_world->GetEntity(i);
			//	ient->SetModelData(wop.str());
			//	ient->Save();
			//}
			//
			//int peo;
			//----
		
			////-----
			//AddTestQuad();
			////-----
			insert_now = true;
		}
		//////////	//NodoSerio minodo(environment.node());
		//////////	/////////NodoSerio minodo(environment);
		//////////	//PandaNode* cacacaca = environment.node();
		//////////	//PandaNode cocococo= (*cacacaca);
		//////////	NodoSerio minodo;
		//////////	std::ostringstream wop;
		//////////	{
		//////////		boost::archive::binary_oarchive o_a(wop);
		//////////		// write class instance to archive
		//////////		o_a << minodo;
  //////////  			// archive and stream closed when destructors are called
		//////////	}
		//////////	for (int i=0; i < current_world->GetNumEntities(); i++)
		//////////	{
		//////////		std::string mierdaaaa = wop.str();
		//////////		IEntityPersistence *ient = current_world->GetEntity(i);
		//////////		ient->SetModelData(wop.str());
		//////////		ient->Save();
		//////////	}

		//////////	NodoSerio tunodo;
		//////////	{
		//////////		// create and open an archive for input
		//////////		std::istringstream wop2;
		//////////		boost::archive::binary_iarchive i_a(wop2);
		//////////		// read class state from archive
		//////////		i_a >> tunodo;
		//////////		// archive and stream closed when destructors are called
		//////////	}
		//////////	int peo;
		//////////}
		
		// Load Scene
		//LoadDefaultScene();
		initialized = true;
		//Sound.Play(); //retomar
		return true;
	}
	return false;
}

void MainProd::SetUserPosition(const core::corePoint3D<double> &pos)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if(initialized)
	{
		double time = globalClock->get_real_time();
		double interval = time- last_time;
		//interval *= 10;
		last_interval = time - last_time; 
		//last_time = time;

		pt0   = pti;
		pt1.x = pos.x; pt1.y = pos.y; pt1.z = pos.z;

		vel0 = vel;
		vel1.x = pt1.x - pt0.x; vel1.y = pt1.y - pt0.y; vel1.z = pt1.z - pt0.z; 
		vel1.x = vel1.x/interval; vel1.y = vel1.y/interval; vel1.z = vel1.z/interval;

		vel = vel1;

		acc.x = vel1.x - vel0.x; acc.y = vel1.y - vel0.y; acc.z = vel1.z - vel0.z; 
		acc.x = acc.x/interval; acc.y = acc.y/interval; acc.z = acc.z/interval;



	}

	////windowcamera_array[1].set_hpr(angledegrees, 0, 0);
	//for(std::map<int, NodePath>::iterator iter = windowcamera_array.begin(); iter != windowcamera_array.end(); iter++)
	//iter->second.set_pos(pos.x/10, pos.y/10, pos.z/10);
}
void MainProd::AddTestQuad()
{
	//----test quad
	//NodePath *testQuad = CreateQuad();
	std::vector<corePDU3D<double>> source_data;
	corePDU3D<double> point1, point2, point3, point4, point5, point6, point7;
	point1.position.x = point2.position.x = point3.position.x = point4.position.x = point5.position.x = point6.position.x = point7.position.x = 0.25;
	point1.position.y = point2.position.y = point3.position.y = point4.position.y = point5.position.y = point6.position.y = point7.position.y = 0.25;
	point1.position.z = point2.position.z = point3.position.z = point4.position.z = point5.position.z = point6.position.z = point7.position.z = 0.25;
	point2.position.x+=0.01; point2.position.y+=0; point5.position.z+=0;
	point3.position.x+=0.12; point3.position.y+=0.21; point5.position.z+=0;
	point4.position.x+=0.13; point4.position.y+=0.22; point5.position.z+=0.31;
	point5.position.x+=0.14; point5.position.y+=0.24; point5.position.z+=0.31;
	point6.position.x+=0.15; point6.position.y+=0.25; point6.position.z+=0.31;
	point7.position.x+=0.16; point7.position.y+=0.26; point7.position.z+=0.33;
	source_data.push_back(point1);
	source_data.push_back(point2);
	source_data.push_back(point3);
	source_data.push_back(point4);
	source_data.push_back(point5);
	source_data.push_back(point6);
	source_data.push_back(point7);
	NodePath *testQuad = CreateVoxelized(source_data);
	testnodepaths.push_back(testQuad);
	testnodepaths.back()->reparent_to(pandawindows_array[1]->get_render());
	std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
	iter++;
	while(iter != pandawindows_array.end())
	{
		testQuad->instance_to(iter->second->get_render());
		iter++;
	}
	if (use_master_display)
		testQuad->instance_to(master_pandawindow->get_render());
	//-------
}
void MainProd::SetCamerasPDU(core::corePDU3D<double> pdu)
{
	////////////boost::mutex::scoped_lock lock(m_mutex);
	////////////if(initialized)
	////////////{
	////////////	double threshold = 1;
	////////////	
	////////////	double time = globalClock->get_real_time();
	////////////	last_interval = time - last_time; 
	////////////	last_time = time;

	////////////	last_pdu.acceleration = pdu.acceleration;

	////////////	if ( ( abs(pdu.velocity.x - last_pdu.velocity.x) > threshold ) ||
	////////////		 ( abs(pdu.velocity.y - last_pdu.velocity.y) > threshold ) ||
	////////////		 ( abs(pdu.velocity.z - last_pdu.velocity.z) > threshold ) )
	////////////		last_pdu.velocity = pdu.velocity;

	////////////	if ( ( abs(pdu.position.x - pti.x) > threshold*100 ) ||
	////////////		 ( abs(pdu.position.y - pti.y) > threshold*100 ) ||
	////////////		 ( abs(pdu.position.z - pti.z) > threshold*100 ) )
	////////////		pti = pdu.position;


		//core::corePDU3D<double> caca1, caca2;
		//caca1 = caca2;

		//corePoint2D<int> io1, io2;
		//io1 = io2;
		//boost::mutex::scoped_lock lock(m_mutex);
		//if(initialized)
		//{
			//double time = globalClock->get_real_time();
		//	//last_interval = time - last_time; 
			//last_interval = time - last_time; 
		//	last_time = time;

		//	pt0.x = pti.x; pt0.y = pti.y; pt0.z = pti.z;
		//	pt1.x = pos.x; pt1.y = pos.y; pt1.z = pos.z;
		//}

		////windowcamera_array[1].set_hpr(angledegrees, 0, 0);
		//for(std::map<int, NodePath>::iterator iter = windowcamera_array.begin(); iter != windowcamera_array.end(); iter++)
		//iter->second.set_pos(pos.x/10, pos.y/10, pos.z/10);
	//}
}


void MainProd::DoDoStuff()
{
	//boost::try_mutex::scoped_try_lock lock(m_mutex);
	//if (lock && initialized)
	{
		////hago mis cosas

		//interpolate + vel + acc
		double time = globalClock->get_real_time();
		double interval = time - last_loop_t; 
		last_loop_t = time;
		//interval*=2;
		//interval *= 4;
		//interval *= 6;
		//interval = last_interval;

		vel.x = vel.x + interval*(acc.x);
		vel.y = vel.y + interval*(acc.y);
		vel.z = vel.z + interval*(acc.z);

		pti.x = pti.x + 10*interval*(vel.x);
		pti.y = pti.y + 10*interval*(vel.y);
		pti.z = pti.z + 10*interval*(vel.z);

		//////dead reckoning
		//////double time = globalClock->get_real_time();
		////last_pdu;

		////double now = globalClock->get_real_time();
		////double period = now-last_loop_t;
		////last_loop_t = now;


		////pti.x = pti.x + period*(last_pdu.velocity.x / last_interval);
		////pti.y = pti.y + period*(last_pdu.velocity.y / last_interval);
		////pti.z = pti.z + period*(last_pdu.velocity.z / last_interval);
		////last_pdu.velocity.x = last_pdu.velocity.x + period*(last_pdu.acceleration.x / last_interval);
		////last_pdu.velocity.y = last_pdu.velocity.y + period*(last_pdu.acceleration.y / last_interval);
		////last_pdu.velocity.z = last_pdu.velocity.z + period*(last_pdu.acceleration.z / last_interval);



		//interpolate two points
		////double time = globalClock->get_real_time();
		////double t = ( last_interval != 0 ) ? (time - last_time)/last_interval : 0;
		////if (t>1) t = 1;

		////pti.x = (pt1.x - pt0.x)*t + pt0.x;
		////pti.y = (pt1.y - pt0.y)*t + pt0.y;
		////pti.z = (pt1.z - pt0.z)*t + pt0.z;

		//UPDATE USER // retomar
		for(std::map<int, NodePath>::iterator iter = windowcamera_array.begin(); iter != windowcamera_array.end(); iter++)
		{	
			//std::map<int, NodePath>::iterator iter = windowcamera_array.begin();

			//if (user_nodepath) 
			//	user_nodepath->set_pos(pti.x/40, pti.y/40, pti.z/40);
			iter->second.set_pos(pti.x/40, pti.y/40, pti.z/40);
			user_entity->SetPosition(pti.x/40, pti.y/40, pti.z/40);
			if (current_user)
				current_user->SetPosition(pti.x/40, pti.y/40, pti.z/40);
			else if (default_user)
				default_user->SetPosition(pti.x/40, pti.y/40, pti.z/40);
		}

		////UPDATE ENTITIES // retomar
		//for (std::vector< Prod3DEntity * >::iterator iter = scene_entities.begin(); iter != scene_entities.end(); iter++)
		//{
		//	if((*iter)->GetData() == "teapot")
		//	{
		//		NodePath *np = (*iter)->GetNodePath();
		//		//np->set_pos(np->get_x()+0.01f, np->get_y(), np->get_z());
		//		(*iter)->SetPosition(np->get_x()+0.01f, np->get_y(), np->get_z());
		//	}
		//}

		//int width  = pandawindows_array[1]->get_graphics_window()->get_x_size();
		//int height = pandawindows_array[1]->get_graphics_window()->get_y_size();
		//float this_aspect_ratio = (float)width / (float)height;
		//pandawindows_array[1]->get_camera(1)->get_lens()->set_film_size(width, height);
		//pandawindows_array[1]->get_camera(1)->get_lens()->set_aspect_ratio(this_aspect_ratio);
		//pandawindows_array[1]->adjust_aspect_ratio();


		////------------move cam 1--------------
		//double time = globalClock->get_real_time();
		//double angledegrees = time * 2.0;
		//double angleradians = angledegrees * (3.14 / 180.0);
		//windowcamera_array[1].set_pos(20*sin(angleradians),-20.0*cos(angleradians),3);
		//windowcamera_array[1].set_hpr(angledegrees, 0, 0);
		////pandaActor4.set_pos(-20*sin(angleradians),20.0*cos(angleradians),3);
		////pandaActor4.set_hpr(angledegrees, 0, 0);

		//--------update sound----------------
		LPoint3f cam_pos = windowcamera_array[1].get_pos();
		LVector3f abs_vec_at, abs_vec_up;
		abs_vec_at = cam_viewpoint.get_pos(pandawindows_array[1]->get_render());
		abs_vec_up = up.get_pos(pandawindows_array[1]->get_render());
		//abs_vec_at =
		LPoint3f new_pos(cam_pos.get_x()  ,    cam_pos.get_z(),    -cam_pos.get_y());
		LPoint3f new_at(abs_vec_at.get_x(), abs_vec_at.get_z(), -abs_vec_at.get_y());
		LPoint3f new_up(abs_vec_up.get_x(), abs_vec_up.get_z(), -abs_vec_up.get_y());
		new_at = new_at - new_pos;
		new_up = new_up - new_pos;
		new_at.normalize();
		new_up.normalize();
		sf::Listener::SetPosition(new_pos.get_x(), new_pos.get_y(), new_pos.get_z());
		sf::Listener::SetTarget(new_at.get_x(), new_at.get_y(), new_at.get_z() , new_up.get_x(), new_up.get_y(), new_up.get_z());
		//-----------------------------------
		//m_peo+=0.001;
		//if ((m_caca == 0) && (m_peo > 1))
		//{
		//	PlaySoundCapture();
		//	m_caca = 1;
		//}
	}
	//else 
	{
		//más suerte la próxima
	}
}

void MainProd::OpenWindow()
{
}

void MainProd::CreateDefaultWindows(int num_windows)
{
	WindowProperties win_props = WindowProperties(); 
	win_props.set_size(800, 600); 
	//------------
	//ApplicationConfiguration *app_config = ApplicationConfiguration::GetInstance();
	//------------


	for (int i=last_window_id+1; i <= last_window_id + num_windows; i++)
	{
		if (app_config != NULL)
			win_props.set_size(app_config->GetDisplayData(i).resolution_x, app_config->GetDisplayData(i).resolution_y);
		else
			win_props.set_size(800, 600);
		prod3Dwindow_array[i] = new Prod3DWindow(&framework, win_props, true, true);
		pandawindows_array[i] = prod3Dwindow_array[i]->GetWindowFrameWork();
		pandawindows_array[i]->set_background_type(WindowFramework::BackgroundType::BT_black);
		pandawindows_array[i]->set_lighting(true);
		pandawindows_array[i]->set_perpixel(true);
		pandawindows_array[i]->get_render().set_antialias(AntialiasAttrib::Mode::M_multisample,1);
		windowcamera_array[i] = pandawindows_array[i]->get_camera_group();
		if (enableEffects)
		{
			pandawindows_array[i]->get_graphics_window()->make_display_region();
			NodePath cam_np = NodePath(pandawindows_array[i]->get_camera(0));
			GraphicsOutput *graphicoutput = pandawindows_array[i]->get_graphics_output();
			ccommonfilters_array[i] = new CCommonFilters(graphicoutput, cam_np);
			DisplayRegion *region = NULL;
			for(int j = 0 ; j < graphicoutput->get_num_active_display_regions(); ++j)
			{	PT(DisplayRegion) dr = graphicoutput->get_display_region(j);
				NodePath drcam = dr->get_camera();
				if(drcam == cam_np) { region = dr;		}}
			if (region)
			{	region->set_clear_color(LColor(0.0f, 0.0f, 1.0f, 1.0f));
				displayregions_array[i] = region;		}
		}

		if (!fake_background_quad)
		{
			fake_background_quad = CreateQuad();	
			fake_background_quad->set_pos(0,500,0);
			fake_background_quad->set_scale(5000.0);
		}
		//if (!default_ambientLight)
		//{
		//	default_ambientLight;
		//	AmbientLight ambient_light("default_ambient_light");
		//	ambient_light.set_color(LColor(0.2, 0.2, 0.2, 1));
		//	NodePath nodepath_ambient_light = pandawindows_array[i]->get_render().attach_new_node(ambient_light);
		//	pandawindows_array[i]->get_render().set_light(nodepath_ambient_light);
		//}


		//pandawindows_array[i]->setup_trackball();
		//pandawindows_array[i]->set_wireframe(true);
		//pandawindows_array[i]->set_texture(true);
		//pandawindows_array[i]->set_two_sided(false);
		//pandawindows_array[i]->set_one_sided_reverse(true);

////////////////////////////////////#ifndef _DEBUG //descomentame - no seguro, no útil
////////////////////////////////////		CollisionSphere *cam_collision_solid = new CollisionSphere(0,0,0,2);
////////////////////////////////////		std::stringstream wop;
////////////////////////////////////		wop << "CAM" << i;
////////////////////////////////////		CollisionNode *cam_collision_node = new CollisionNode(wop.str());
////////////////////////////////////		cam_collision_node->add_solid(cam_collision_solid);
////////////////////////////////////		NodePath col_node = windowcamera_array[i].attach_new_node(cam_collision_node);
////////////////////////////////////		col_node.show();
////////////////////////////////////		collision_traverser->add_collider(col_node, collision_handler_queue);
////////////////////////////////////#endif

		//objectNode_colliderNode_array[cam_collision_node] = windowcamera_array[i];
		


		//pandawindows_array[i]->get_render().set_fog(m_fog, 1);
		//if (app_config != NULL)
		//{
		//	int flip_me_x = (app_config->GetDisplayData(i).flip_h) ? -1 : 1;
		//	int flip_me_y = (app_config->GetDisplayData(i).flip_v) ? -1 : 1;
		//}
		//unsigned int num_cam = pandawindows_array[i]->get_num_cameras();
		//for (unsigned int cam = 0; cam < num_cam; cam++)
		//{
		//	//camera_array.push_back(windowcamera_array[i].get_child(cam,graphic_thread));
		//	camera_array.push_back(pandawindows_array[i]->get_camera(i)); //->.set_sx(flip_me_x);
		//	if (app_config != NULL)
		//	{
		//		//camera_array[cam]->get_display_region(->get->set_transform->set_attrib->set_sy(flip_me_y);
		//		//camera_array[cam]->set_sz(flip_me_y);

		//	}
		//}
		if (app_config != NULL)
		{
			int flip_me_x = (app_config->GetDisplayData(i).flip_h) ? -1 : 1;
			int flip_me_y = (app_config->GetDisplayData(i).flip_v) ? -1 : 1;
			windowcamera_array[i].set_sx(flip_me_x);
			windowcamera_array[i].set_sz(flip_me_y);
		}
	}

	if (use_master_display)
	{	win_props.set_size(400, 300); 
		master_prod3Dwindow = new Prod3DWindow(&framework, win_props, true, true);
		master_pandawindow = master_prod3Dwindow->GetWindowFrameWork();
		master_pandawindow->set_background_type(WindowFramework::BackgroundType::BT_gray);
		master_pandawindow->set_lighting(true);
		master_pandawindow->set_perpixel(true);
		master_pandawindow->get_render().set_antialias(AntialiasAttrib::Mode::M_multisample,1);
		master_camera = master_pandawindow->get_camera_group();
		master_pandawindow->setup_trackball();
	}


	if (master_pandawindow)
	{

		fake_background_quad->reparent_to(master_pandawindow->get_render());
		//fake_background_quad->reparent_to(pandawindows_array[1]->get_camera_group());
		std::map<int, NodePath>::iterator iter = windowcamera_array.begin();
		//if (iter != windowcamera_array.end())iter++;
		while(iter != windowcamera_array.end())
		{	
			fake_background_quad->instance_to(iter->second);
			//iter->second.attach_new_node(fake_background_quad->node());
			iter++;				
		}
	}

	origin = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
	origin.set_pos(0,0,0);
	origin.set_scale(0.001);
	origin.reparent_to(pandawindows_array[1]->get_camera_group());
	origin.hide();

	up = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
	up.set_pos(0,0,10);
	up.set_scale(0.002);
	up.reparent_to(pandawindows_array[1]->get_camera_group());
	up.hide();

	cam_viewpoint = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
	cam_viewpoint.set_pos(0,10,0);
	LPoint3f ppp = cam_viewpoint.get_pos();
	cam_viewpoint.set_scale(0.002);
	cam_viewpoint.reparent_to(pandawindows_array[1]->get_camera_group());
	cam_viewpoint.hide();

	LVector3f abs_vec_at, abs_vec_up;
	abs_vec_at = cam_viewpoint.get_pos(pandawindows_array[1]->get_render());
	abs_vec_up = up.get_pos(pandawindows_array[1]->get_render());

	last_window_id += num_windows;
}

void MainProd::ClearScene()
{	
	
	{
		boost::mutex::scoped_lock lock(m_mutex);
		
		initialized = false;
		Sound.Stop();	
		InternalRemoveAllBackgroundSound();
		ClearAvatarModel();

		for (std::map< Prod3DEntity *, double >::iterator iter = entity_array_to_be_loaded_afterseconds.begin(); iter != entity_array_to_be_loaded_afterseconds.end();  )
		{
			if (iter->first)
			{
				delete iter->first;
				std::map< Prod3DEntity *, double >::iterator iter_to_erase = iter;
				iter++;
				entity_array_to_be_loaded_afterseconds.erase(iter_to_erase);
			}
			else
				iter++;
		}
		entity_array_to_be_loaded_afterseconds.clear();

		/*if (collision_traverser)
		{	//collision_traverser->clear_colliders();
			//delete collision_traverser;
			//collision_traverser = NULL;
		}
		if (collision_handler_queue)
		{
			//collision_handler_queue->clear_entries();
			//delete collision_handler_queue;
			//collision_handler_queue = NULL;
		}*/

		//{
		//	environment = NodePath(NULL);
		//	pandaActor = NodePath(NULL);
		//	pandaActor2 = NodePath(NULL);
		//	pandaActor3 = NodePath(NULL);
		//	pandaActor4 = NodePath(NULL);
		//}

		if(use_master_display)
		{
			NodePathCollection npc = master_pandawindow->get_render().get_children();
			npc.detach();
			master_camera.reparent_to(master_pandawindow->get_render());
		}

		for (unsigned int i = 1; i <= pandawindows_array.size(); i++)
		{
			if (pandawindows_array[i])
			{
				NodePathCollection npc = pandawindows_array[i]->get_render().get_children();
				npc.detach();
				windowcamera_array[i].reparent_to(pandawindows_array[i]->get_render());
			}
		}

		if (user_nodepath)
			delete user_nodepath;
		user_nodepath = new NodePath("zip_nada");
		user_nodepath->reparent_to(pandawindows_array[1]->get_render());

		std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
		iter++;
		while(iter != pandawindows_array.end())
		{	user_nodepath->instance_to(iter->second->get_render());	
			iter++;				
		}
		if (use_master_display)
			user_nodepath->instance_to(master_pandawindow->get_render());

		//ContentCreationController::Instance()->Reset();

		//initialized = true;
		//////////////////initialized = false;
		////////////////////BASURA COCHINA! solo funciona la ventana principal
		//////////////////Thread *khvkjaehvgkjj = Thread::get_current_thread();
		//////////////////graphic_thread;

		//////////////////std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
		//////////////////NodePathCollection npc = iter->second->get_render().get_children();
		//////////////////int nnpc = npc.size();
		//////////////////NodePath camcaca = iter->second->get_camera_group();
		////////////////////npc.clear();
		//////////////////npc.detach();
		//////////////////camcaca.reparent_to(iter->second->get_render());
		//////////////////int nnpc_ = npc.size();
		//////////////////iter++;
		//////////////////while(iter != pandawindows_array.end())
		//////////////////{
		//////////////////	NodePathCollection npc2 = iter->second->get_render().get_children();
		//////////////////	int nnpc2 = npc2.size();
		//////////////////	//for (int i = 0; i < npc2.size(); i++)
		//////////////////	//{
		//////////////////	//	NodePath cavca = npc2[i];
		//////////////////	//	std::string MIERDANAME = cavca.get_name();
		//////////////////	//	int peo;
		//////////////////	//}
		//////////////////	//npc2.clear();
		//////////////////	NodePath camcacai = iter->second->get_camera_group();
		//////////////////	npc2.detach();
		//////////////////	camcacai.reparent_to(iter->second->get_render());
		//////////////////	int nnpc2_ = npc2.size();
		//////////////////	//.detach();
		//////////////////	/////////////iter->second->get_render().get_children().detach();
		//////////////////	iter++;
		//////////////////} 
		//environment.detach_node();
		//pandaActor4.detach_node();
		//environment.remove_node();
		//pandaActor4.remove_node();

		//////////////std::streambuf buffer;
		//////////////std::ostream oop(&buffer);
		//////////////environment.write_bam_stream(oop);
		//////////////int peo;

		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//BASURAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!
		//boost::mutex::scoped_lock lock(m_mutex);
		////std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
		////while(iter != pandawindows_array.end())
		////{
		////	//iter->second->get_render().clear_model_nodes();
		////	//iter->second->get_render().flatten_strong();
		////	iter++;
		////}
		//environment.detach_node(graphic_thread);
		//pandaActor.detach_node(graphic_thread);
		//pandaActor2.detach_node(graphic_thread);
		//pandaActor3.detach_node(graphic_thread);
		//pandaActor4.detach_node(graphic_thread);

		//environment.remove_node(graphic_thread);
		//pandaActor.remove_node(graphic_thread);
		//pandaActor2.remove_node(graphic_thread);
		//pandaActor3.remove_node(graphic_thread);
		//pandaActor4.remove_node(graphic_thread);

		//std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
		//while(iter != pandawindows_array.end())
		//{
		//	iter->second->get_render().get_children().detach();

		//	iter++;
		//} 

		//for (int i = 0; i < pandawindows_array.size(); i++)
		//{
		//	pandawindows_array[i]->get_render().get_children().detach();
		//	windowcamera_array[i].reparent_to(pandawindows_array[i]->get_render());
		//}

		//for (int i = 0; i < pandawindows_array.size(); i++)
		//{
		//	int n_children = pandawindows_array[i]->get_render().get_num_children();
		//	for (int c = 0; c < n_children; c++)
		//	{
		//		NodePath np = pandawindows_array[i]->get_render().get_child(c);
		//		if ( (np == environment) || (np == pandaActor) || (np == pandaActor2) || (np == pandaActor3) || (np == pandaActor4) )
		//			np.detach_node();//.get_children().detach();
		//	}
		//	//windowcamera_array[i].reparent_to(pandawindows_array[i]->get_render());
		//}
	}
}

void MainProd::LoadEmptyScene()
{
	//boost::mutex::scoped_lock lock(m_mutex);
	//if (pandawindows_array.begin() != pandawindows_array.end())
	//{
	//	Thread *khvkjaehvgkjj = Thread::get_current_thread();
	//	graphic_thread;
	//	std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
	//	iter++;

	//	while(iter != pandawindows_array.end())
	//	{
	//		iter->second->setup_trackball();
	//		iter++;
	//	}
	//	initialized = true;
	//}
}

void MainProd::LoadDefaultScene()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (pandawindows_array.begin() != pandawindows_array.end())
	{
		Thread *khvkjaehvgkjj = Thread::get_current_thread();
		graphic_thread;
		NodePath environment = pandawindows_array[1]->load_model(framework.get_models(),"environment");
		environment.set_scale(0.25,0.25,0.25);
		environment.set_pos(-8,42,0);
		//environment.reparent_to(pandawindows_array[1]->get_render());
		////environment = pandawindows_array[1]->get_render().attach_new_node(environment.node());

		NodePath pandaActor = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
		pandaActor.set_scale(0.005);
		LPoint3f pandapos = pandaActor.get_pos();
		//pandaActor.instance_to(pandawindows_array[1]->get_render());

		//------------------------
		NodePath pandaActor2 = pandawindows_array[1]->load_model(framework.get_models(), "car/yugo");
		pandaActor2.set_scale(0.5);
		pandaActor2.set_pos(10.0,0.0,0.0);
		LPoint3f pandapos2 = pandaActor2.get_pos();
		//pandaActor2.instance_to(pandawindows_array[1]->get_render());
		//------------------------
		NodePath pandaActor3 = pandawindows_array[1]->load_model(framework.get_models(), "bam");
		pandaActor3.set_scale(0.1);
		pandaActor3.set_pos(0.0,10.0,0.0);
		LPoint3f pandapos3 = pandaActor3.get_pos();
		//pandaActor3.instance_to(pandawindows_array[1]->get_render());
		//------------------------
		NodePath pandaActor4 = pandawindows_array[1]->load_model(framework.get_models(), "fofofo");
		//pandaActor4.set_scale(0.002);
		pandaActor4.set_pos(10.0,10.0,0.0);
		LPoint3f pandapos4 = pandaActor4.get_pos();
		//pandaActor4.instance_to(pandawindows_array[1]->get_render());
		//pandaActor4 = pandawindows_array[1]->get_render().attach_new_node(pandaActor4.node());
		//------------------------

		//pandawindows_array[1]->load_model(pandaActor, "panda-walk4");
		//pandawindows_array[1]->loop_animations(0);


		std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
		iter++;

		//taskMgr->add(new GenericAsyncTask("Spins the camera", &SpinCameraTask, (void*) NULL)); //ASSERTION FAILED
		//pandawindows_array[1]->setup_trackball();

		while(iter != pandawindows_array.end())
		{
			environment.instance_to(iter->second->get_render());
			pandaActor.instance_to(iter->second->get_render());
			pandaActor2.instance_to(iter->second->get_render());
			pandaActor3.instance_to(iter->second->get_render());
			pandaActor4.instance_to(iter->second->get_render());
			//iter->second->setup_trackball();
			iter++;
		}
		initialized = true;
		//Sound.Play();	 //retomar
	}
}

void MainProd::PlaySoundCapture()
{
	//Recorder.Stop();
	//recordingBuffer = Recorder.GetBuffer();
	//Sound.Stop();
	//Sound.SetBuffer(recordingBuffer);
	//Sound.Play();
}

////////////NodePath* MainProd::CreateQuad()
////////////{
////////////	
////////////
////////////	//step 1) create GeomVertexData and add vertex information 
////////////	CPT(GeomVertexFormat) format = GeomVertexFormat::get_v3n3c4t2();
////////////	PT(GeomVertexData) vdata = new GeomVertexData("vertices", format, GeomEnums::UH_static);
////////////
////////////	GeomVertexWriter vertex, normal, color, texcoord;
////////////	vertex = GeomVertexWriter(vdata, "vertex");
////////////	normal = GeomVertexWriter(vdata, "normal");
////////////	color = GeomVertexWriter(vdata, "color");
////////////	texcoord = GeomVertexWriter(vdata, "texcoord");
////////////	
////////////	//vertex.add_data3f(1, 0, 0);
////////////	//normal.add_data3f(0, 0, 1);
////////////	//color.add_data4f(0, 0, 1, 1);
////////////	//texcoord.add_data2f(1, 0);
////////////
////////////	//GeomVertexWriter *vertexWriter = new GeomVertexWriter(vdata, "vertex");
////////////	//vertexWriter->add_data3f(10,5,10) ;
////////////	//vertexWriter->add_data3f(50,5,0) ;
////////////	//vertexWriter->add_data3f(50,25,50) ;
////////////	//vertexWriter->add_data3f(0,5,50) ;
////////////
////////////	//vertex->add_data3f(10,5,10);
////////////	//color->add_data4f(0, 0, 1, 1);
////////////	//
////////////	//vertex->add_data3f(50,5,0);
////////////	//color->add_data4f(0, 0, 1, 1);
////////////	//
////////////	//vertex->add_data3f(50,25,50);
////////////	//color->add_data4f(0, 0, 1, 1);
////////////	//
////////////	//vertex->add_data3f(0,5,50);
////////////	//color->add_data4f(0, 0, 1, 1);
////////////
////////////	vertex.add_data3f(10,5,10);
////////////	normal.add_data3f(0, 0, 1);
////////////	color.add_data4f(1.0, 0.0, 1.0, 0.0);
////////////	texcoord.add_data2f(1, 0);
////////////	 
////////////	vertex.add_data3f(50,5,0);
////////////	normal.add_data3f(0, 0, 1);
////////////	color.add_data4f(1.0, 0.0, 1.0, 0.0);
////////////	texcoord.add_data2f(1, 1);
////////////	 
////////////	vertex.add_data3f(50,25,50);
////////////	normal.add_data3f(0, 0, 1);
////////////	color.add_data4f(1.0, 0.0, 1.0, 0.0);
////////////	texcoord.add_data2f(0, 1);
////////////	 
////////////	vertex.add_data3f(0,5,50);
////////////	normal.add_data3f(0, 0, 1);
////////////	color.add_data4f(1.0, 0.0, 1.0, 0.0);
////////////	texcoord.add_data2f(0, 0);
////////////
////////////	//step 2) make primitives and assign vertices to them 
////////////	PT(GeomTriangles) tris;
////////////	tris = new GeomTriangles(Geom::UH_static);
////////////	//have to add vertices one by one since they are not in order 
////////////	tris->add_vertex(0) ;
////////////	tris->add_vertex(1) ;
////////////	tris->add_vertex(3) ;
////////////	//indicates that we have finished adding vertices for the first triangle. 
////////////	tris->close_primitive() ;
////////////	//since the coordinates are in order we can use this convenience function. 
////////////	tris->add_consecutive_vertices(1,3); //#add vertex 1, 2 and 3 ;
////////////	tris->close_primitive() ;
////////////
////////////	//step 3) make a Geom object to hold the primitives 
////////////	PT(Geom) squareGeom = new Geom(vdata) ;
////////////	squareGeom->add_primitive(tris) ;
////////////
////////////	//now put squareGeom in a GeomNode. You can now position your geometry in the scene graph. 
////////////	PT(GeomNode) squareGN = new GeomNode("square") ;
////////////	squareGN->add_geom(squareGeom) ;
////////////	//render.attachNewNode(squareGN);
////////////
////////////	NodePath* quad = new NodePath( (PandaNode*)squareGN );
////////////	quad->set_two_sided(true);
////////////	quad->set_texture_off();
////////////	//PT(NodePath) quad = new NodePath( (PandaNode*)squareGN );
////////////
////////////	boost::filesystem::path data_path = boost::filesystem::initial_path();
////////////	data_path = data_path / "../../bin/data"; 
////////////	data_path = data_path / "/images/help.png";
////////////	std::string mi_path = data_path.normalize().string();
////////////	int pos = mi_path.find(":");
////////////	if (pos != std::string::npos) mi_path.erase(pos, 1);
////////////	std::replace(mi_path.begin(), mi_path.end(), '\\', '/');
////////////	mi_path = "/" + mi_path; //case sensitive y el boost saca la unidad de disco en mayuscula cuando panda la queire en minuscula
////////////	//Texture *text = TexturePool::load_texture(mi_path); 
////////////	Texture *text = TexturePool::load_texture("/c/etc/Iris.png");
////////////	quad->set_transparency(TransparencyAttrib::M_alpha);
////////////	quad->set_texture(text);
////////////
////////////
////////////	return quad;
////////////}

//#include <mainprod.h>
//#include <debugger.h> 
//
//using namespace core::iprod;
//
//#define DEFAULT_NUM_WINDOWS 2
//
//PandaFramework MainProd::framework;
//Thread* MainProd::graphic_thread(NULL);
//PT(AsyncTaskManager) MainProd::taskMgr;
//PT(ClockObject) MainProd::globalClock;
//
////PT(AudioManager) MainProd::AM;
////PT(AudioSound) MainProd::mySound;
//VirtualFileSystem* MainProd::vfs;
////string MainProd::filename;
//
//boost::try_mutex MainProd::m_mutex;
//bool MainProd::initialized = false;
//bool MainProd::stop_requested = false;
//
//boost::shared_ptr<boost::thread> MainProd::m_thread;
//std::map<int, Prod3DWindow*>     MainProd::prod3Dwindow_array;
//std::map<int, WindowFramework*>	 MainProd::pandawindows_array;
//std::map<int, NodePath>			 MainProd::windowcamera_array;
//int MainProd::last_window_id = 0;
//int MainProd::m_argc = 0;
//char** MainProd::m_argv;
//NodePath MainProd::cam_viewpoint(NULL);
//NodePath MainProd::origin(NULL);
//NodePath MainProd::up(NULL);
//
//sf::SoundBufferRecorder MainProd::Recorder;
//sf::SoundBuffer MainProd::recordingBuffer;
//
//float MainProd::m_peo=0;
//float MainProd::m_caca=0;
//
//sf::SoundBuffer MainProd::sound_Buffer;
//double MainProd::listener_position[]	= {50.0f,50.0f,1.0f};
//double MainProd::listener_target[]		= {0.0f,-1.0f,0.0f};
//double MainProd::sound_pos[]			= {0.1f,0.1f,0.1f};
//sf::Sound MainProd::Sound;
//
//
////AsyncTask::DoneStatus Shit(GenericAsyncTask* task, void* data) {
////	int peo;
////	return AsyncTask::DS_cont;
////}
////
////AsyncTask::DoneStatus MainProd::SpinCameraTask(GenericAsyncTask* task, void* data) {
////
////	boost::try_mutex::scoped_try_lock lock(m_mutex);
////	if (lock && initialized)
////	{
////		  // Calculate the new position and orientation (inefficient - change me!)
////		  double time = globalClock->get_real_time();
////		  double angledegrees = time * 6.0;
////		  double angleradians = angledegrees * (3.14 / 180.0);
////		  windowcamera_array[1].set_pos(20*sin(angleradians),-20.0*cos(angleradians),3);
////		  windowcamera_array[1].set_hpr(angledegrees, 0, 0);
////		  windowcamera_array[2].set_pos(-20*sin(angleradians),20.0*cos(angleradians),3);
////		  windowcamera_array[2].set_hpr(-angledegrees, 0, 0);
////	}
////  // Tell the task manager to continue this task the next frame.
////  return AsyncTask::DS_cont;
////}
//
//MainProd::MainProd(int argc, char *argv[])
//{
//	m_argc = argc;
//	m_argv = argv;
//}
//
//MainProd::~MainProd()
//{
//	m_thread->join();
//	for (std::map<int, Prod3DWindow*>::iterator iter=prod3Dwindow_array.begin(); iter != prod3Dwindow_array.end(); iter++)
//		delete iter->second;
//	prod3Dwindow_array.erase(prod3Dwindow_array.begin(), prod3Dwindow_array.end());
//}
//
//void MainProd::Delete()
//{
//	stop_requested = true;
//	assert(m_thread);
//	m_thread->join();
//}
//
//void MainProd::PostLogMessage(const std::string &message)
//{
//	if (app != NULL) app->PostLogMessage(message);
//}
//
//void MainProd::DoMainLoop()
//{
//	framework.open_framework( m_argc, m_argv);
//
//	taskMgr = AsyncTaskManager::get_global_ptr();
//	globalClock = ClockObject::get_global_clock();
//	graphic_thread = Thread::get_current_thread();	
//
//	//vfs = VirtualFileSystem::get_global_ptr();
//
//	CreateDefaultWindows(DEFAULT_NUM_WINDOWS);
//
//	//NOTES---------
//	GraphicsEngine	*ge = framework.get_graphics_engine();		//only one, global, pointers and buffers
//	GraphicsPipe	*gp = framework.get_default_pipe();			//at least one, OGL/DX
//	GraphicsWindow	*gw = pandawindows_array[1]->get_graphics_window();		//can be many
//	GraphicsWindow	*gw2 = pandawindows_array[2]->get_graphics_window();
//	DisplayInformation	*dp = gp->get_display_information();
//	GraphicsDevice		*gd = gp->get_device();
//	//WindowProperties win_props_fs = WindowProperties(); 
//	//int d_id = dp->get_device_id();
//	//dp->get_display_mode_height(d_id);
//	//int max_w = dp->_maximum_window_width;
//	//int max_h = dp->_maximum_window_height;
//	//win_props_fs.set_size(max_w, max_h); 
//	//win_props_fs.set_fullscreen(true);
//	////gw->request_properties(win_props_fs);
//	//gw->set_properties_now(win_props_fs);
//	//framework.close_window(1);
//	//window = framework.open_window(win_props_fs);
//	//--------------
//
//	//Load the environment
//	LoadDefaultScene();
//
//	//-Sound stuff
//	sf::SoundBuffer Buffer;
//	if (!Buffer.LoadFromFile("c://etc//motor.wav"))
//		int error_loadfromfile = 1;		
//	unsigned int chan = Buffer.GetChannelsCount();
//
//	sf::Listener::SetGlobalVolume(100.f);
//
//	Sound.SetBuffer(Buffer);
//	Sound.SetLoop(true);
//	Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);
//	//Sound.SetRelativeToListener(true);
//	Sound.SetMinDistance(10.f);
//	Sound.SetAttenuation(0.75f);
//
//	//-recording
//	Recorder.Start();
//
//	initialized = true;
//	
//	Sound.Play();	
//
//	while(!stop_requested)
//	{
//		Iterate();
//		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
//	}
//
//	Recorder.Stop();
//	recordingBuffer = Recorder.GetBuffer();
//
//	framework.close_all_windows();
//	framework.close_framework();
//}
//
//void MainProd::Iterate()
//{
//	boost::try_mutex::scoped_try_lock lock(m_mutex);
//	if (lock)
//	{
//		framework.do_frame(graphic_thread);
//		//AM->update();
//		CIntervalManager::get_global_ptr()->step();
//	}
//}
//
//void MainProd::Init()
//{
//	MainProd::DoInit();
//}
//
//void MainProd::DoStuff()
//{
//	MainProd::DoDoStuff();
//}
//
//void MainProd::DoInit()
//{
//	if (!initialized)
//	{
//		PostLogMessage("Initializing Production...");
//		assert(!m_thread);
//		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&MainProd::DoMainLoop)));
//	}
//}
//
//void MainProd::DoDoStuff()
//{
//	boost::try_mutex::scoped_try_lock lock(m_mutex);
//	if (lock && initialized)
//	{
//		////hago mis cosas
//		//int width  = pandawindows_array[1]->get_graphics_window()->get_x_size();
//		//int height = pandawindows_array[1]->get_graphics_window()->get_y_size();
//		//float this_aspect_ratio = (float)width / (float)height;
//		//pandawindows_array[1]->get_camera(1)->get_lens()->set_film_size(width, height);
//		//pandawindows_array[1]->get_camera(1)->get_lens()->set_aspect_ratio(this_aspect_ratio);
//		//pandawindows_array[1]->adjust_aspect_ratio();
//		//move cam---
//		double time = globalClock->get_real_time();
//		double angledegrees = time * 2.0;
//		double angleradians = angledegrees * (3.14 / 180.0);
//		windowcamera_array[1].set_pos(20*sin(angleradians),-20.0*cos(angleradians),3);
//		windowcamera_array[1].set_hpr(angledegrees, 0, 0);
//
//		//--------update sound(Up issue)
//		LPoint3f cam_pos = windowcamera_array[2].get_pos();
//		LVector3f abs_vec_at, abs_vec_up;
//		abs_vec_at = cam_viewpoint.get_pos(pandawindows_array[2]->get_render());
//		abs_vec_up = up.get_pos(pandawindows_array[2]->get_render());
//		LPoint3f new_pos(cam_pos.get_x()  ,    cam_pos.get_z(),    -cam_pos.get_y());
//		LPoint3f new_at(abs_vec_at.get_x(), abs_vec_at.get_z(), -abs_vec_at.get_y());
//		LPoint3f new_up(abs_vec_up.get_x(), abs_vec_up.get_z(), -abs_vec_up.get_y());
//		new_at = new_at - new_pos;
//		new_up = new_up - new_pos;
//		new_at.normalize();
//		new_up.normalize();
//		sf::Listener::SetPosition(new_pos.get_x(), new_pos.get_y(), new_pos.get_z());
//		sf::Listener::SetTarget(new_at.get_x(), new_at.get_y(), new_at.get_z() , new_up.get_x(), new_up.get_y(), new_up.get_z());
//		//--------
//		m_peo+=0.001;
//		if ((m_caca == 0) && (m_peo > 1))
//		{
//			PlaySoundCapture();
//			m_caca = 1;
//		}
//	}
//	else 
//	{
//		//más suerte la próxima
//	}
//}
//
//void MainProd::OpenWindow()
//{
//}
//
//void MainProd::CreateDefaultWindows(int num_windows)
//{
//	WindowProperties win_props = WindowProperties(); 
//	win_props.set_size(800, 600); 
//
//	for (int i=last_window_id+1; i <= last_window_id + num_windows; i++)
//	{
//		prod3Dwindow_array[i] = new Prod3DWindow(&framework, win_props, true, true);
//		pandawindows_array[i] = prod3Dwindow_array[i]->GetWindowFrameWork();
//		pandawindows_array[i]->set_background_type(WindowFramework::BackgroundType::BT_white);
//		pandawindows_array[i]->set_lighting(true);
//		pandawindows_array[i]->set_perpixel(true);
//		windowcamera_array[i] = pandawindows_array[i]->get_camera_group();
//	}
//
//	origin = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//	origin.set_pos(0,0,0);
//	origin.set_scale(0.001);
//	origin.reparent_to(pandawindows_array[2]->get_render());
//
//	up = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//	up.set_pos(0,0,10);
//	up.set_scale(0.002);
//	up.reparent_to(pandawindows_array[2]->get_camera_group());
//
//	cam_viewpoint = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//	cam_viewpoint.set_pos(0,10,0);
//	cam_viewpoint.set_scale(0.002);
//	cam_viewpoint.reparent_to(pandawindows_array[2]->get_camera_group());
//
//	last_window_id += num_windows;
//}
//
//void MainProd::LoadDefaultScene()
//{
//	if (pandawindows_array.begin() != pandawindows_array.end())
//	{
//		NodePath environment = pandawindows_array[1]->load_model(framework.get_models(),"environment");
//		environment.set_scale(0.25,0.25,0.25);
//		environment.set_pos(-8,42,0);
//		environment.reparent_to(pandawindows_array[1]->get_render());
//
//		NodePath pandaActor = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//		pandaActor.set_scale(0.005);
//		LPoint3f pandapos = pandaActor.get_pos();
//		pandaActor.reparent_to(pandawindows_array[1]->get_render());
//
//		//------------------------
//		NodePath pandaActor2 = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//		pandaActor2.set_scale(0.004);
//		pandaActor2.set_pos(10.0,0.0,0.0);
//		LPoint3f pandapos2 = pandaActor2.get_pos();
//		pandaActor2.reparent_to(pandawindows_array[1]->get_render());
//		//------------------------
//		NodePath pandaActor3 = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//		pandaActor3.set_scale(0.003);
//		pandaActor3.set_pos(0.0,10.0,0.0);
//		LPoint3f pandapos3 = pandaActor3.get_pos();
//		pandaActor3.reparent_to(pandawindows_array[1]->get_render());
//		//------------------------
//		NodePath pandaActor4 = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
//		pandaActor4.set_scale(0.002);
//		pandaActor4.set_pos(10.0,10.0,0.0);
//		LPoint3f pandapos4 = pandaActor4.get_pos();
//		pandaActor4.reparent_to(pandawindows_array[1]->get_render());
//		//------------------------
//
//		pandawindows_array[1]->load_model(pandaActor, "panda-walk4");
//		pandawindows_array[1]->loop_animations(0);
//
//		std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
//		iter++;
//
//		//taskMgr->add(new GenericAsyncTask("Spins the camera", &SpinCameraTask, (void*) NULL)); //ASSERTION FAILED
//		//pandawindows_array[1]->setup_trackball();
//
//		while(iter != pandawindows_array.end())
//		{
//			environment.instance_to(iter->second->get_render());
//			pandaActor.instance_to(iter->second->get_render());
//			pandaActor2.instance_to(iter->second->get_render());
//			pandaActor3.instance_to(iter->second->get_render());
//			pandaActor4.instance_to(iter->second->get_render());
//			iter->second->setup_trackball();
//			iter++;
//		}
//	}
//}
//
//void MainProd::PlaySoundCapture()
//{
//	Recorder.Stop();
//	recordingBuffer = Recorder.GetBuffer();
//	Sound.Stop();
//	Sound.SetBuffer(recordingBuffer);
//	Sound.Play();
//}

void MainProd::AddNewEntitiesIntoCurrentWorld(const std::map<core::IEntity *, double> &new_entities_after_seconds)
{	
	boost::mutex::scoped_lock lock(m_mutex);

	for (std::map<core::IEntity *, double>::const_iterator iter = new_entities_after_seconds.begin(); iter != new_entities_after_seconds.end(); iter++)
	{
		if (iter->first)
		{
			if (current_world)
			{
				Prod3DEntity *prod3d_ent = (Prod3DEntity *)(iter->first);
				double desired_timestamp = (double)clock()/CLOCKS_PER_SEC + iter->second;
				entity_array_to_be_loaded_afterseconds[prod3d_ent] = desired_timestamp;
			}
		}
		else
		{
			delete iter->first;
		}
	}
	//Prod3DEntity *prod3d_ent = (Prod3DEntity *) ent;
	//if (prod3d_ent && current_world)
	//{	double desired_timestamp = (double)clock()/CLOCKS_PER_SEC + after_seconds;
	//	entity_array_to_be_loaded_afterseconds[prod3d_ent] = desired_timestamp;
	//} else
	//	delete ent;
}
void MainProd::RemoveEntityFromCurrentWorld(core::IEntity * ent)
{	//retomar after seconds
	boost::mutex::scoped_lock lock(m_mutex);
	PrivateRemoveEntityFromCurrentWorld(ent);

}

void MainProd::PrivateRemoveEntityFromCurrentWorld(core::IEntity * ent)
{
	Prod3DEntity *prod3d_ent = (Prod3DEntity *) ent;
	if (prod3d_ent && current_world)
	{	current_world->RemoveEntity((prod3d_ent->GetEntity()));
		current_world->Save();
		//double desired_timestamp = (double)clock()/CLOCKS_PER_SEC + after_seconds;
		RemoveEntityFromScene(prod3d_ent);
		//entity_array_to_be_removed_afterseconds[prod3d_ent] = desired_timestamp;
		//RemoveEntityFromScene(prod3d_ent); //retomar x: detach, remove_nodes, etc 
	}
}

//if an insertion is not needed because it has already been inserted
void MainProd::LoadEntityFromCurrentWorld(core::IEntity * ent)
{
	boost::mutex::scoped_lock lock(m_mutex);
	Prod3DEntity *prod3d_ent = (Prod3DEntity *) ent;
	if (prod3d_ent)
		LoadEntityIntoScene(prod3d_ent);
}

//void MainProd::LoadEntityIntoScene(core::IEntityPersistence* ent) 
//{
//	boost::mutex::scoped_lock lock(m_mutex);
//	IEntityPersistence *ent;
//
//	if (ent)
//	{
//		switch (ent->GetPsique(&ent_psique));
//		{
//			case 
//		}
//	}
//	Prod3DEntity *new_entity = new Prod3DEntity(ient);
//
//	LoadEntityIntoScene(new_entity);
//
//}

void MainProd::InsertEntityIntoScene(core::IEntityPersistence* ent) //Retomar, return *IEntity
{
	if (ent)
	{	boost::mutex::scoped_lock lock(m_mutex);
		current_world->AddEntity(ent);
		current_world->Save();
		Prod3DEntity *new_entity = new Prod3DEntity(ent);
		//scene_entities.push_back(new_entity);
		LoadEntityIntoScene(new_entity);
	}
}

void MainProd::InsertEntityIntoScene(core::IEntityPersistence* ent, std::vector<float> source_data, int row_step)
{
	if (ent)
	{	boost::mutex::scoped_lock lock(m_mutex);
		current_world->AddEntity(ent);
		current_world->Save();
		Prod3DEntity *new_entity = new Prod3DEntity(ent);
		//scene_entities.push_back(new_entity);
		LoadEntityIntoScene(new_entity);
	}
}

void MainProd::ClearAvatarModel()
{
	//user_nodepath->detach_node();
	//user_nodepath->remove_node();
	//user_nodepath = NULL;
	
	//avatar_collider_array.clear();
	//user_nodepath->remove_node();
	//user_nodepath = NULL;
	if (user_nodepath)
	{	NodePathCollection npc = user_nodepath->get_children();
		npc.detach();
	}

	//if(use_master_display)
	//{
	//	NodePathCollection npc = master_pandawindow->get_render().get_children();
	//	npc.detach();
	//	master_camera.reparent_to(master_pandawindow->get_render());
	//}

	//for (unsigned int i = 1; i <= pandawindows_array.size(); i++)
	//{
	//	if (pandawindows_array[i])
	//	{
	//		NodePathCollection npc = pandawindows_array[i]->get_render().get_children();
	//		npc.detach();
	//		windowcamera_array[i].reparent_to(pandawindows_array[i]->get_render());
	//	}
	//}
}

void MainProd::SetAvatar(const std::string model)
{ 
	if (current_user)
	{	boost::mutex::scoped_lock lock(m_mutex);
		current_user->SetModel(model);
		SetUpUser();
	}
}

void MainProd::SetAvatar(std::vector<float> source_data, int row_step)
{
	if (current_user)
	{	boost::mutex::scoped_lock lock(m_mutex);
		std::stringstream wop;
		wop << "::" << row_step;
		for (unsigned int i = 0; i < source_data.size(); i++)
			wop << "::" << source_data[i];
		current_user->SetModel(wop.str());
		SetUpUser();
	}
}

void MainProd::SetAvatar(void *graphic_node)
{ 
	if (current_user)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		SetUpUser(graphic_node);
	}
}

void MainProd::SetUpUser(void *graphic_node)
{
	//if (user_entity)
	//	delete user_entity;
	//user_entity = NULL;

	//Clear the avatar mesh and collider info
	//---------------------------------------
	//ClearAvatarModel(); 
	collision_handler_queue->clear_entries();
	//collision_traverser->clear_colliders(); //Could keep a reference to the into objects we are erasing but we should clear only the avatar objects
	//avatar_collider_array.clear();
	NodePathCollection npc = user_nodepath->get_children();
	npc.detach();
	//---------------------------------------

	if (user_nodepath == NULL)
	{
		user_nodepath = new NodePath("zip_nada");
		user_nodepath->reparent_to(pandawindows_array[1]->get_render());

		std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
		iter++;
		while(iter != pandawindows_array.end())
		{	user_nodepath->instance_to(iter->second->get_render());	
			iter++;				
		}
		if (use_master_display)
			user_nodepath->instance_to(master_pandawindow->get_render());
	}

	user_dummyPersistence = app->GetAvatarEntity();
	IEntityPersistence *u_ent = user_dummyPersistence;
	int psi = 0;
	float x, y, z;
	x = y = z = 0.0;

	if (current_user)
	{
		u_ent->SetModelData(current_user->GetModel());
		u_ent->SetName(current_user->GetName());
		current_user->GetOrientation(x, y, z);
		u_ent->SetOrientation(x, y, z);
		current_user->GetPosition(x, y, z);
		u_ent->SetPosition(x, y, z);
		current_user->GetPsique(psi);
		u_ent->SetPsique(psi);
		current_user->GetScale(x);
		u_ent->SetScale(x);
		current_user->GetUp(x, y, z);
		u_ent->SetUp(x, y, z);
	}

	if (!user_entity)
	{	Prod3DEntity *new_entity = new Prod3DEntity(u_ent);
		user_entity = new_entity;
	}
	
	std::vector<float> source_data;
	int row_step = 0;
	std::string data_set = user_entity->GetData();
	std::string data = data_set;
	size_t pos = data.find("::");
	
	
	if (graphic_node == NULL)
	{
		if ( pos != std::string::npos ) //in case it is coded data
		{
			bool found_row_step = false;
			data = data.substr(pos+2);
			while ( pos != std::string::npos )
			{
				int next_pos = data.find("::");
				{	float value = 0.0;
					std::stringstream wop;
					wop << data.substr(0, next_pos);
					wop >> value;
					if ( next_pos != std::string::npos)
					{
						if (found_row_step)
							source_data.push_back(value);
						else 
						{	row_step = value; found_row_step = true; }
						data = data.substr(next_pos+2);
					}
				}
				pos = next_pos;
			}
			int n_points = (row_step > 0) ? (source_data.size() / row_step) : 0;
			std::vector<corePDU3D<double>> source_points;
			for (int i = 0; i < n_points; i++)
			{	corePDU3D<double> new_point;
				new_point.position.x = source_data[i*row_step+0];
				new_point.position.y = (row_step > 1) ? source_data[i*row_step+1] : 0.0;
				new_point.position.z = (row_step > 2) ? source_data[i*row_step+2] : 0.0;
				source_points.push_back(new_point);
			}
			NodePath *testQuad = CreateVoxelized(source_points); 
			testQuad->reparent_to(*user_nodepath);
			user_nodepath->node()->add_child(testQuad->node());
			////user_nodepath = testQuad;
		}
		else //it is a filepath
		{   NodePath *aux_node = new NodePath();
			*aux_node = pandawindows_array[1]->load_model(framework.get_models(),data.c_str());
			aux_node->reparent_to(*user_nodepath);
			//*user_nodepath = pandawindows_array[1]->load_model(framework.get_models(),data.c_str());
		}
	}
	else
	{	
		if (avatar_current_graphicNodePath)
			avatar_current_graphicNodePath->remove_node();
		avatar_current_graphicNodePath = (NodePath*) graphic_node;
		((NodePath*) graphic_node)->reparent_to(*user_nodepath); 
	}

	user_entity->SetNodePath(user_nodepath);

	//NodePath *nodepath_graphicNode = (NodePath *)graphic_node;
	//delete nodepath_graphicNode;
	/*
	//MEMORY LEAK SOSPECHOSO






	//entity_collidable_array_to_register.push_back(user_entity);
	//user_nodepath->reparent_to(pandawindows_array[1]->get_render());
	
	//std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
	//iter++;
	//while(iter != pandawindows_array.end())
	//{	user_nodepath->instance_to(iter->second->get_render());	
	//	iter++;				
	//}
	//if (use_master_display)
	//	user_nodepath->instance_to(master_pandawindow->get_render());


	//LoadEntityIntoScene(user_entity);
	//static Prod3DEntity				*user_entity;
	//static NodePath					 user_nodepath;
	//std::string data = entity->GetData();
	//scene_entities_nodepaths[entity] = pandawindows_array[1]->load_model(framework.get_models(),data.c_str());
	//entity->SetNodePath(&(scene_entities_nodepaths[entity]));
	//entity_collidable_array_to_register.push_back(entity);
	//scene_entities_nodepaths[entity].reparent_to(pandawindows_array[1]->get_render());
	//std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin();
	//iter++;
	//while(iter != pandawindows_array.end())
	//{
	//	scene_entities_nodepaths[entity].instance_to(iter->second->get_render());
	//	
	//	//iter->second->setup_trackball();//comment
	//	iter++;
	//}
	//if (use_master_display)
	//{
	//	scene_entities_nodepaths[entity].instance_to(master_pandawindow->get_render());
	//}
	*/
}

void* MainProd::CreateGraphicNode(std::vector<float> source_data, int row_step)
{
	if (source_data.size() > 0)
	{
		int n_points = (row_step > 0) ? (source_data.size() / row_step) : 0;
		std::vector<corePDU3D<double>> source_points;
		for (int i = 0; i < n_points; i++)
		{	corePDU3D<double> new_point;
			new_point.position.x = source_data[i*row_step+0];
			new_point.position.y = (row_step > 1) ? source_data[i*row_step+1] : 0.0;
			new_point.position.z = (row_step > 2) ? source_data[i*row_step+2] : 0.0;
			source_points.push_back(new_point);
		}
		NodePath *testQuad = CreateVoxelized(source_points);
		return (void *)testQuad;
	}
}

void* MainProd::CreateGraphicNode(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data)
{	
	NodePath *resultNodePath = NULL;
	if (mesh_factory && source_weighted_data.size())
	{
		if (USE_AVATAR_3D_MESH) 
			//NodePath *resultNodePath = CreateVoxelized(source_weighted_data);
			resultNodePath = mesh_factory->CreateVoxelized(source_weighted_data); //retomar: future work: add a mesh collider for the user 3d-mesh
		else
		{	//Ridiculous set of instructions in order to create an empty object
			PT(GeomTriangles) tris;
			tris = new GeomTriangles(Geom::UH_static);
			tris->close_primitive();
			CPT(GeomVertexFormat) format = GeomVertexFormat::get_v3n3c4t2();
			PT(GeomVertexData) vdata = new GeomVertexData("emptydata", format, GeomEnums::UH_static);
			PT(Geom) squareGeom = new Geom(vdata) ;
			squareGeom->add_primitive(tris) ;
			PT(GeomNode) squareGN = new GeomNode("empty") ;
			squareGN->add_geom(squareGeom) ;
			resultNodePath = new NodePath( (PandaNode*)squareGN );
			//----------------------------------------------

			if (resultNodePath)
			{	NodePath* np = resultNodePath;
				avatar_collider_array.clear();
				int avatarcollierindex = 0;
				for (std::map< int, std::vector<corePDU3D<double>> >::iterator iter = source_weighted_data.begin(); iter != source_weighted_data.end(); iter++)
				{	if ((iter->first) > 3) for (std::vector<corePDU3D<double>>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) 
					{	float radius = 0.1*(iter->first);
						CollisionTube *collision_solid = new CollisionTube(	(*iter2).position.x-7.5, (*iter2).position.y, (*iter2).position.z-2.5, 
																			(*iter2).position.x-7.5, (*iter2).position.y+20, (*iter2).position.z-2.5, 
																			radius);
						std::stringstream wop;
						wop << "avatarcollider" << avatarcollierindex++ ;
						std::string nombre = wop.str();
						CollisionNode *collision_node = new CollisionNode(nombre);
						collision_node->add_solid(collision_solid);
						NodePath col_node = resultNodePath->attach_new_node(collision_node);
						//col_node.look_at(user_nodepath->get_pos());
						if (SHOW_COLLISION) col_node.show();
						avatar_collider_array[collision_solid] = (*iter2);
					}// a simpler capsule based set of colliders
				}
			}
		}
	}
	return (void *)resultNodePath;
}

void MainProd::RegisterEntity(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data)
{}
void MainProd::CreateAndRegisterEntity(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data)
{}


std::vector<std::string>  MainProd::GetBackgroundSounds()
{
	boost::mutex::scoped_lock lock(m_mutex);
	vector<std::string> result;

	for (std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> >::iterator iter = music_melody_samples.begin(); iter != music_melody_samples.end(); iter++)
		result.push_back(iter->first);

	return result;
}

std::string  MainProd::AddBackgroundSound(const std::string &file_name, const double &time_lerp)
{
	boost::mutex::scoped_lock lock(m_mutex);

	std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> >::iterator iterator_sounds = music_melody_samples.find(file_name);

	if ( (file_name != "") && (iterator_sounds == music_melody_samples.end()))
	{
		//Buffer
		bool error_loadfromfile = false;
		sf::SoundBuffer *new_buffer = new sf::SoundBuffer();
		if ( (new_buffer) && (!new_buffer->LoadFromFile(file_name)))
			error_loadfromfile = true;		
		unsigned int chan = new_buffer->GetChannelsCount();

		if (!error_loadfromfile)
		{
			core::corePoint3D<double> position;
			position.x = 0; position.y = 0; position.z = 0;

			//Sound
			sf::Sound *new_sound = new sf::Sound();
			new_sound->SetBuffer(*new_buffer);
			new_sound->SetLoop(true);
			new_sound->SetPosition(position.x, position.y, position.z);
			new_sound->SetRelativeToListener(false);
			new_sound->SetMinDistance(10.f);
			new_sound->SetAttenuation(0.75f);
			new_sound->SetPitch(1.0f);
			new_sound->SetVolume(100.f);

			//RESULT
			core::coreSound<sf::Sound, sf::SoundBuffer> new_background_melody(position, new_sound, new_buffer,true, false, 1.0, 100, 10.0, 0.75);
			music_melody_samples[file_name] = new_background_melody;

			new_sound->Play();

			return file_name;
		} 
	}
	return "";
}

void MainProd::SetPitchBackgroundSound(const std::string &id, const float &value, const double &time_lerp)
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> >::iterator iter = music_melody_samples.find(id);
	if ((music_melody_samples.size() > 0) && (iter != music_melody_samples.end()))
	{
		iter->second.pitch = value;
		iter->second.sound_data->SetPitch(value);
	}
}
void MainProd::SetAmplitudeBackgroundSound(const std::string &id, const float &value, const double &time_lerp)
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> >::iterator iter = music_melody_samples.find(id);
	if ((music_melody_samples.size() > 0) && (iter != music_melody_samples.end()))
	{
		iter->second.amplitude = value;
		iter->second.sound_data->SetVolume(value);
	}
}
void MainProd::RemoveBackgroundSound(const std::string &id, const double &time_lerp)
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> >::iterator iter = music_melody_samples.find(id);
	if ((music_melody_samples.size() > 0) && (iter != music_melody_samples.end()))	
	{
		if (iter->second.sound_data != NULL)
			iter->second.sound_data->Stop();
		delete (iter->second.sound_data); 
		delete (iter->second.sound_buffer);
		music_melody_samples.erase(iter);
	}
}
void MainProd::RemoveAllBackgroundSound(const double &time_lerp)
{
	boost::mutex::scoped_lock lock(m_mutex);
	InternalRemoveAllBackgroundSound(time_lerp);
}

void MainProd::InternalRemoveAllBackgroundSound(const double &time_lerp)
{
	for (std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> >::iterator iter = music_melody_samples.begin(); music_melody_samples.size() > 0; )
	{
		if ( (iter != music_melody_samples.end()) && (iter->second.sound_data != NULL) )
			iter->second.sound_data->Stop();
		delete (iter->second.sound_data); 
		delete (iter->second.sound_buffer);
		music_melody_samples.erase(iter);
	}
}

void MainProd::SetBackgroundColor(const float &R, const float &G, const float &B)
{
	boost::mutex::scoped_lock lock(m_mutex);
	background_color.x = R;
	background_color.y = G;
	background_color.z = B;
}
void MainProd::SetFogColor(const float &R, const float &G, const float &B)
{
	boost::mutex::scoped_lock lock(m_mutex);
	fog_color.x = R;
	fog_color.y = G;
	fog_color.z = B;
}
void MainProd::SetFogIntensity(const float &intensity)
{
	boost::mutex::scoped_lock lock(m_mutex);
	fog_intensity = intensity;
}

void MainProd::EnableSimpleBloomEffect(const bool &enable)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (enableEffects)
	{			
		for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
		{
			if (enable && !enable_simpleBLOOMEffect)
			{
				CCommonFilters::SetBloomParameters bloom_paramns; 
				bloom_paramns.blend		 = LVector4f(0.33,0.34,0.33,0.0);
				bloom_paramns.mintrigger = 0.90f;
				bloom_paramns.maxtrigger = 1.0f;
				bloom_paramns.desat		 = 0.8f;
				bloom_paramns.intensity	 = 0.9f;
				bloom_paramns.size		 = "medium";
				ccommonfilters_array[iter->first]->set_bloom(bloom_paramns);
			}
			else if (!enable)
				ccommonfilters_array[iter->first]->del_bloom();
		}
	}
	enable_simpleBLOOMEffect = enable;
}

void MainProd::EnableSimpleInverEffect(const bool &enable)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (enableEffects)
	{
		for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
		{
			if (enable && ! enable_simpleINVERTEffect)
				ccommonfilters_array[iter->first]->set_inverted();
			else if (!enable)
				ccommonfilters_array[iter->first]->del_inverted();
		}
	}
	enable_simpleINVERTEffect = enable;
}

void MainProd::EnableSimpleToonEffect(const bool &enable)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (enableEffects)
	{
		for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
		{
			if (enable && !enable_simpleTOONEffect)
				ccommonfilters_array[iter->first]->set_cartoon_ink();
			else if (!enable)
				ccommonfilters_array[iter->first]->del_cartoon_ink();
		}
	}
	enable_simpleTOONEffect = enable;
}

void MainProd::EnableSimpleBlurEffect(const bool &enable)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (enableEffects)
	{
		for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
		{
			if (enable && !enable_simpleBLUREffect)
				ccommonfilters_array[iter->first]->set_blur_sharpen(0.5f);
			else if (!enable)
				ccommonfilters_array[iter->first]->del_blur_sharpen();
		}
	}
	enable_simpleBLUREffect = enable;
}

void MainProd::EnableSimpleSSAOEffect(const bool &enable)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (enableEffects)
	{
		for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
		{
			if (enable && !enable_simpleSSAOEffect)
			{
				CCommonFilters::SetAmbientOcclusionParameters ambient_params;
				ambient_params.amount = 2.0f;
				ambient_params.falloff = 0.000002f;
				ambient_params.numsamples = 16;
				ambient_params.radius = 0.05f;
				ambient_params.strength = 0.01f;
				ccommonfilters_array[iter->first]->set_ambient_occlusion(ambient_params);
			}
			else if (!enable)
				ccommonfilters_array[iter->first]->del_ambient_occlusion();
		}	
	}
	enable_simpleSSAOEffect = enable;
}

void MainProd::EnableSimpleBackgroundVolumetricLightEffect(const bool &enable)
{

	boost::mutex::scoped_lock lock(m_mutex);

	if (enableEffects)
	{
		for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
		{
			if (enable && !enable_simpleVOLUMETRICLIGHTSEffect)
			{
				int offset = iter->second->get_graphics_window()->get_properties().get_x_size() * -1;
				fake_background_quad->set_pos(offset, 500, 0 );
				enable_simpleBLOOMEffect = enable;
				CCommonFilters::SetVolumetricLightingParameters vol_params(NodePath(iter->second->get_camera(0))); //retomar, The light Object, do not use the camera 
				vol_params.decay = 0.98f;
				vol_params.density = 5.0f;
				vol_params.exposure = 0.1f;
				vol_params.numsamples = 40;
				ccommonfilters_array[iter->first]->set_volumetric_lighting(vol_params);
			}
			else if (!enable)
				ccommonfilters_array[iter->first]->del_volumetric_lighting();
		}
	}
	enable_simpleVOLUMETRICLIGHTSEffect = enable;
}


void MainProd::SetBackgroundAndFog(const float &bg_R, const float &bg_G, const float &bg_B, const float &f_R, const float &f_G, const float &f_B, const float &intensity, const float animation_time)
{
	boost::mutex::scoped_lock lock(m_mutex);

	if (animation_time > 0.0f)
	{
		background_animation_starttimestamp = current_timestamp;
		background_animation_endtimestamp = current_timestamp + animation_time;
		
		desired_background_color.x = bg_R;
		desired_background_color.y = bg_G;
		desired_background_color.z = bg_B;

		desired_fog_color.x = f_R;
		desired_fog_color.y = f_G;
		desired_fog_color.z = f_B;

		desired_fog_intensity = intensity;
	}
	else
	{
		background_color.x = bg_R;
		background_color.y = bg_G;
		background_color.z = bg_B;

		fog_color.x = f_R;
		fog_color.y = f_G;
		fog_color.z = f_B;

		fog_intensity = intensity;

		CheckBackgroundAndFogRanges();
	}
}

bool MainProd::CheckDesiredBackgroundAndFogRanges()
{
	return ( (desired_background_color.x >= 0.0f) && (desired_background_color.y >= 0.0f) && (desired_background_color.z >= 0.0f) &&
			 (fog_color.x >= 0.0f) && (fog_color.y >= 0.0f) && (fog_color.z >= 0.0f) &&
			 (fog_intensity >= 0.0f) && (fog_intensity >= 0.0f) && (fog_intensity >= 0.0f) );
}

void MainProd::CheckBackgroundAndFogRanges()
{
	if (background_color.x > 1.0f) background_color.x = 1.0f; else if ((background_color.x < 0.0f)|| !(background_color.z -1.0f > -1.0f)) background_color.x = 0.0f;
	if (background_color.y > 1.0f) background_color.y = 1.0f; else if ((background_color.y < 0.0f)|| !(background_color.z -1.0f > -1.0f)) background_color.y = 0.0f;
	if (background_color.z > 1.0f) background_color.z = 1.0f; else if ((background_color.z < 0.0f) || !(background_color.z -1.0f > -1.0f)) background_color.z = 0.0f;
	if (fog_color.x > 1.0f) fog_color.x = 1.0f; else if ((fog_color.x < 0.0f)|| !(background_color.z -1.0f > -1.0f)) fog_color.x = 0.0f;
	if (fog_color.y > 1.0f) fog_color.y = 1.0f; else if ((fog_color.y < 0.0f)|| !(background_color.z -1.0f > -1.0f)) fog_color.y = 0.0f;
	if (fog_color.z > 1.0f) fog_color.z = 1.0f; else if ((fog_color.z < 0.0f)|| !(background_color.z -1.0f > -1.0f)) fog_color.z = 0.0f;
	if (fog_intensity > 1.0f) fog_intensity = 1.0f; else if ((fog_intensity < 0.0f)|| !(background_color.z -1.0f > -1.0f)) fog_intensity = 0.0f;
}

//void MainProd::PrepareSimpleEffects()
//{
//	enableEffects = true;
//
//	for (std::map<int, WindowFramework*>::iterator iter = pandawindows_array.begin(); iter != pandawindows_array.end(); iter++)
//	{
//		//It creates a quad of the size of the window for post-processing
//		//Panda3D bug: if we use ImageFilters we won't be able to resize or reorient the region after boot
//		//We also loose the option to change the region clear color, in order to patch that we create a fake_background quad
//		//that changes its color to the background color.
//		iter->second->get_graphics_window()->make_display_region();
//		NodePath cam_np = NodePath(iter->second->get_camera(0));
//		GraphicsOutput *graphicoutput = iter->second->get_graphics_output();
//		ccommonfilters_array[iter->first] = new CCommonFilters(graphicoutput, cam_np);
//		DisplayRegion *region = NULL;
//		for(int j = 0 ; j < graphicoutput->get_num_active_display_regions(); ++j)
//		{	PT(DisplayRegion) dr = graphicoutput->get_display_region(j);
//			NodePath drcam = dr->get_camera();
//			if(drcam == cam_np) { region = dr;		}}
//		if (region)
//		{	region->set_clear_color(LColor(0.0f, 0.0f, 1.0f, 1.0f));
//			displayregions_array[iter->first] = region;		}
//	
//		////INVERT COLOR
//		//ccommonfilters_array[iter->first]->set_inverted();
//		//////ccommonfilters_array[i]->del_inverted();
//		////BLOOM
//		////Parameters: https://www.panda3d.org/manual/index.php/Common_Image_Filters
//		//CCommonFilters::SetBloomParameters bloom_paramns; 
//		//bloom_paramns.blend		 = LVector4f(0.33,0.34,0.33,0.0);
//		//bloom_paramns.mintrigger = 0.90f;
//		//bloom_paramns.maxtrigger = 1.0f;
//		//bloom_paramns.desat		 = 0.8f;
//		//bloom_paramns.intensity	 = 0.9f;
//		//bloom_paramns.size		 = "medium";
//		//ccommonfilters_array[iter->first]->set_bloom(bloom_paramns);
//		////////TOON
//		////ccommonfilters_array[iter->first]->set_cartoon_ink();
//		////////BLUR
//		////ccommonfilters_array[iter->first]->set_blur_sharpen(0.25f);
//		////VOLUMETRIC LIGHTING
//		////CCommonFilters::SetVolumetricLightingParameters vol_params(NodePath(pandawindows_array[iter->first]->get_camera(0))); //retomar, The light Object, do not use the camera 
//		//pandawindows_array[iter->first]->get_render().set_shader_auto();	
//		//fake_background_quad->set_pos(-200,500,20);
//		//CCommonFilters::SetVolumetricLightingParameters vol_params(*fake_background_quad); //retomar, The light Object, do not use the camera 
//		//vol_params.decay = 0.98f;
//		//vol_params.density = 5.0f;
//		//vol_params.exposure = 0.1f;
//		//vol_params.numsamples = 64;
//		//ccommonfilters_array[iter->first]->set_volumetric_lighting(vol_params);
//		////////SPACE IMAGE AMBIENT OCCLUSION
//		//////CCommonFilters::SetAmbientOcclusionParameters ambient_params;
//		//////ambient_params.amount = 2.0f;
//		//////ambient_params.falloff = 0.000002f;
//		//////ambient_params.numsamples = 16;
//		//////ambient_params.radius = 0.05f;
//		//////ambient_params.strength = 0.01f;
//		//////ccommonfilters_array[iter->first]->set_ambient_occlusion(ambient_params);
//	}
//}

NodePath* MainProd::CreateQuad()
{
	CPT(GeomVertexFormat) format = GeomVertexFormat::get_v3n3c4t2();
	PT(GeomVertexData) vdata = new GeomVertexData("fake_background", format, GeomEnums::UH_static);

	GeomVertexWriter vertex, normal, color, texcoord;
	vertex = GeomVertexWriter(vdata, "vertex");
	normal = GeomVertexWriter(vdata, "normal");
	color = GeomVertexWriter(vdata, "color");
	texcoord = GeomVertexWriter(vdata, "texcoord");

	vertex.add_data3f(-1, 0, -1);
	normal.add_data3f(0, -1, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 0);
	 
	vertex.add_data3f(-1, 0, 1);
	normal.add_data3f(0, -1, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 1);
	 
	vertex.add_data3f(1, 0, -1);
	normal.add_data3f(0, -1, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 1);
	 
	vertex.add_data3f(1, 0, 1);
	normal.add_data3f(0, -1, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 0);


	PT(GeomTriangles) prim;
	prim = new GeomTriangles(Geom::UH_static);
	prim->add_vertices(0, 1, 2);
	prim->add_vertices(2, 1, 3);

	PT(Geom) geom;
	geom = new Geom(vdata);
	geom->add_primitive(prim);
 
	PT(GeomNode) node;
	node = new GeomNode("gnode");
	node->add_geom(geom);
 
	NodePath* quad = new NodePath( (PandaNode*)node );
	quad->set_two_sided(true);
	quad->set_texture_off();

	return quad;
}