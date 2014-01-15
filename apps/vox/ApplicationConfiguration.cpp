#include <ApplicationConfiguration.h>
#include <debugger.h>

using namespace core;

ApplicationConfiguration* ApplicationConfiguration::instance = NULL;
boost::mutex ApplicationConfiguration::m_mutex;

ApplicationConfiguration::ApplicationConfiguration(): modified(false), root_directory(""), data_directory(""), model_directory(""), sound_directory(""), image_directory(""), 
													  ui_resource_directory(""), db_host(""), db_port(""), db_name(""),	db_user(""), db_password(""), language(""), 
													  num_cams(1), num_displays(1), show_capture(false), show_homography(false), show_face_detection(false), show_foreground(false), show_motion(false)
{
	boost::mutex::scoped_lock lock(m_mutex);
	
	cam_map.clear();
	win_map.clear();
	
	instance = this;
}

ApplicationConfiguration::~ApplicationConfiguration()
{
}

ApplicationConfiguration* ApplicationConfiguration::GetInstance()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (instance == NULL)
		instance = new ApplicationConfiguration;
	return instance;
}

core::CameraData  ApplicationConfiguration::GetCameraData(const int &id)
{ 
	boost::mutex::scoped_lock lock(m_mutex);
	std::map< unsigned int, core::CameraData >::iterator found  = cam_map.find(id); 
	if ( found != cam_map.end() ) 
		return found->second; 
	CameraData dummy; 
	cam_map[id]=dummy; 
	return dummy;			
}
core::DisplayData ApplicationConfiguration::GetDisplayData(const int &id)	    
{ 
	boost::mutex::scoped_lock lock(m_mutex);
	std::map< unsigned int, core::DisplayData >::iterator found = win_map.find(id); 
	if ( found != win_map.end() ) 
		return found->second; 
	DisplayData dummy; 
	win_map[id]=dummy; 
	return dummy;			
}

void ApplicationConfiguration::SetCameraData(const int &id, const CameraData &value)
{ 
	boost::mutex::scoped_lock lock(m_mutex); 
	cam_map[id] = value; 
}

void ApplicationConfiguration::SetDisplayData(const int &id, const DisplayData &value)
{ 
	boost::mutex::scoped_lock lock(m_mutex); 
	win_map[id] = value; 
}