#include <mainprod.h>

#include <debugger.h> 

using namespace core::iprod;

#define DEFAULT_NUM_WINDOWS 2

PandaFramework MainProd::framework;
Thread* MainProd::graphic_thread(NULL);
PT(AsyncTaskManager) MainProd::taskMgr;
PT(ClockObject) MainProd::globalClock;

//PT(AudioManager) MainProd::AM;
//PT(AudioSound) MainProd::mySound;
VirtualFileSystem* MainProd::vfs;
//string MainProd::filename;

boost::try_mutex MainProd::m_mutex;
bool MainProd::initialized = false;
bool MainProd::stop_requested = false;

boost::shared_ptr<boost::thread> MainProd::m_thread;
std::map<int, Prod3DWindow*>     MainProd::prod3Dwindow_array;
std::map<int, WindowFramework*>	 MainProd::pandawindows_array;
std::map<int, NodePath>			 MainProd::windowcamera_array;
int MainProd::last_window_id = 0;
int MainProd::m_argc = 0;
char** MainProd::m_argv;
NodePath MainProd::cam_viewpoint(NULL);
NodePath MainProd::origin(NULL);
NodePath MainProd::up(NULL);

sf::SoundBufferRecorder MainProd::Recorder;
sf::SoundBuffer MainProd::recordingBuffer;

float MainProd::m_peo=0;
float MainProd::m_caca=0;

sf::SoundBuffer MainProd::sound_Buffer;
double MainProd::listener_position[]	= {50.0f,50.0f,1.0f};
double MainProd::listener_target[]		= {0.0f,-1.0f,0.0f};
double MainProd::sound_pos[]			= {0.1f,0.1f,0.1f};
sf::Sound MainProd::Sound;


//AsyncTask::DoneStatus Shit(GenericAsyncTask* task, void* data) {
//	int peo;
//	return AsyncTask::DS_cont;
//}
//
//AsyncTask::DoneStatus MainProd::SpinCameraTask(GenericAsyncTask* task, void* data) {
//
//	boost::try_mutex::scoped_try_lock lock(m_mutex);
//	if (lock && initialized)
//	{
//		  // Calculate the new position and orientation (inefficient - change me!)
//		  double time = globalClock->get_real_time();
//		  double angledegrees = time * 6.0;
//		  double angleradians = angledegrees * (3.14 / 180.0);
//		  windowcamera_array[1].set_pos(20*sin(angleradians),-20.0*cos(angleradians),3);
//		  windowcamera_array[1].set_hpr(angledegrees, 0, 0);
//		  windowcamera_array[2].set_pos(-20*sin(angleradians),20.0*cos(angleradians),3);
//		  windowcamera_array[2].set_hpr(-angledegrees, 0, 0);
//	}
//  // Tell the task manager to continue this task the next frame.
//  return AsyncTask::DS_cont;
//}

MainProd::MainProd(int argc, char *argv[])
{
	m_argc = argc;
	m_argv = argv;
}

MainProd::~MainProd()
{
	m_thread->join();
	for (std::map<int, Prod3DWindow*>::iterator iter=prod3Dwindow_array.begin(); iter != prod3Dwindow_array.end(); iter++)
		delete iter->second;
	prod3Dwindow_array.erase(prod3Dwindow_array.begin(), prod3Dwindow_array.end());
}

void MainProd::Delete()
{
	stop_requested = true;
	assert(m_thread);
	m_thread->join();
}

void MainProd::DoMainLoop()
{
	framework.open_framework( m_argc, m_argv);

	taskMgr = AsyncTaskManager::get_global_ptr();
	globalClock = ClockObject::get_global_clock();
	graphic_thread = Thread::get_current_thread();	

	vfs = VirtualFileSystem::get_global_ptr();

	CreateDefaultWindows(DEFAULT_NUM_WINDOWS);

	//NOTES---------
	GraphicsEngine	*ge = framework.get_graphics_engine();		//only one, global, pointers and buffers
	GraphicsPipe	*gp = framework.get_default_pipe();			//at least one, OGL/DX
	GraphicsWindow	*gw = pandawindows_array[1]->get_graphics_window();		//can be many
	GraphicsWindow	*gw2 = pandawindows_array[2]->get_graphics_window();
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
	LoadDefaultScene();

	//-Sound stuff
	sf::SoundBuffer Buffer;
	if (!Buffer.LoadFromFile("c://etc//motor.wav"))
		int error_loadfromfile = 1;		
	unsigned int chan = Buffer.GetChannelsCount();

	sf::Listener::SetGlobalVolume(100.f);

	Sound.SetBuffer(Buffer);
	Sound.SetLoop(true);
	Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);
	//Sound.SetRelativeToListener(true);
	Sound.SetMinDistance(10.f);
	Sound.SetAttenuation(0.75f);

	//-recording
	Recorder.Start();

	initialized = true;
	
	Sound.Play();	

	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}

	Recorder.Stop();
	recordingBuffer = Recorder.GetBuffer();

	framework.close_all_windows();
	framework.close_framework();
}

void MainProd::Iterate()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (lock)
	{
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

void MainProd::DoDoStuff()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (lock && initialized)
	{
		////hago mis cosas
		//int width  = pandawindows_array[1]->get_graphics_window()->get_x_size();
		//int height = pandawindows_array[1]->get_graphics_window()->get_y_size();
		//float this_aspect_ratio = (float)width / (float)height;
		//pandawindows_array[1]->get_camera(1)->get_lens()->set_film_size(width, height);
		//pandawindows_array[1]->get_camera(1)->get_lens()->set_aspect_ratio(this_aspect_ratio);
		//pandawindows_array[1]->adjust_aspect_ratio();
		//move cam---
		double time = globalClock->get_real_time();
		double angledegrees = time * 2.0;
		double angleradians = angledegrees * (3.14 / 180.0);
		windowcamera_array[1].set_pos(20*sin(angleradians),-20.0*cos(angleradians),3);
		windowcamera_array[1].set_hpr(angledegrees, 0, 0);

		//--------update sound(Up issue)
		LPoint3f cam_pos = windowcamera_array[2].get_pos();
		LVector3f abs_vec_at, abs_vec_up;
		abs_vec_at = cam_viewpoint.get_pos(pandawindows_array[2]->get_render());
		abs_vec_up = up.get_pos(pandawindows_array[2]->get_render());
		LPoint3f new_pos(cam_pos.get_x()  ,    cam_pos.get_z(),    -cam_pos.get_y());
		LPoint3f new_at(abs_vec_at.get_x(), abs_vec_at.get_z(), -abs_vec_at.get_y());
		LPoint3f new_up(abs_vec_up.get_x(), abs_vec_up.get_z(), -abs_vec_up.get_y());
		new_at = new_at - new_pos;
		new_up = new_up - new_pos;
		new_at.normalize();
		new_up.normalize();
		sf::Listener::SetPosition(new_pos.get_x(), new_pos.get_y(), new_pos.get_z());
		sf::Listener::SetTarget(new_at.get_x(), new_at.get_y(), new_at.get_z() , new_up.get_x(), new_up.get_y(), new_up.get_z());
		//--------
		m_peo+=0.001;
		if ((m_caca == 0) && (m_peo > 1))
		{
			PlaySoundCapture();
			m_caca = 1;
		}
	}
	else 
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

	for (int i=last_window_id+1; i <= last_window_id + num_windows; i++)
	{
		prod3Dwindow_array[i] = new Prod3DWindow(&framework, win_props, true, true);
		pandawindows_array[i] = prod3Dwindow_array[i]->GetWindowFrameWork();
		pandawindows_array[i]->set_background_type(WindowFramework::BackgroundType::BT_white);
		pandawindows_array[i]->set_lighting(true);
		pandawindows_array[i]->set_perpixel(true);
		windowcamera_array[i] = pandawindows_array[i]->get_camera_group();
	}

	origin = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
	origin.set_pos(0,0,0);
	origin.set_scale(0.001);
	origin.reparent_to(pandawindows_array[2]->get_render());

	up = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
	up.set_pos(0,0,10);
	up.set_scale(0.002);
	up.reparent_to(pandawindows_array[2]->get_camera_group());

	cam_viewpoint = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
	cam_viewpoint.set_pos(0,10,0);
	cam_viewpoint.set_scale(0.002);
	cam_viewpoint.reparent_to(pandawindows_array[2]->get_camera_group());

	last_window_id += num_windows;
}

void MainProd::LoadDefaultScene()
{
	if (pandawindows_array.begin() != pandawindows_array.end())
	{
		NodePath environment = pandawindows_array[1]->load_model(framework.get_models(),"environment");
		environment.set_scale(0.25,0.25,0.25);
		environment.set_pos(-8,42,0);
		environment.reparent_to(pandawindows_array[1]->get_render());

		NodePath pandaActor = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
		pandaActor.set_scale(0.005);
		LPoint3f pandapos = pandaActor.get_pos();
		pandaActor.reparent_to(pandawindows_array[1]->get_render());

		//------------------------
		NodePath pandaActor2 = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
		pandaActor2.set_scale(0.004);
		pandaActor2.set_pos(10.0,0.0,0.0);
		LPoint3f pandapos2 = pandaActor2.get_pos();
		pandaActor2.reparent_to(pandawindows_array[1]->get_render());
		//------------------------
		NodePath pandaActor3 = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
		pandaActor3.set_scale(0.003);
		pandaActor3.set_pos(0.0,10.0,0.0);
		LPoint3f pandapos3 = pandaActor3.get_pos();
		pandaActor3.reparent_to(pandawindows_array[1]->get_render());
		//------------------------
		NodePath pandaActor4 = pandawindows_array[1]->load_model(framework.get_models(), "panda-model");
		pandaActor4.set_scale(0.002);
		pandaActor4.set_pos(10.0,10.0,0.0);
		LPoint3f pandapos4 = pandaActor4.get_pos();
		pandaActor4.reparent_to(pandawindows_array[1]->get_render());
		//------------------------

		pandawindows_array[1]->load_model(pandaActor, "panda-walk4");
		pandawindows_array[1]->loop_animations(0);

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
			iter->second->setup_trackball();
			iter++;
		}
	}
}

void MainProd::PlaySoundCapture()
{
	Recorder.Stop();
	recordingBuffer = Recorder.GetBuffer();
	Sound.Stop();
	Sound.SetBuffer(recordingBuffer);
	Sound.Play();
}