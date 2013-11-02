#include <core/IApplicationConfiguration.h>
#include <debugger.h>

using namespace core;

//core::IApplicationConfiguration* core::IApplicationConfiguration::instance = NULL;
//boost::mutex core::IApplicationConfiguration::m_mutex;
//OpenThreads::Mutex IApplicationConfiguration::appmutex;
//std::map< unsigned int, core::CameraData >  core::IApplicationConfiguration::cam_map;
//std::map< unsigned int, core::DisplayData > core::IApplicationConfiguration::win_map;
//unsigned int core::IApplicationConfiguration::num_cams = 1;
//unsigned int core::IApplicationConfiguration::num_displays = 1;

//IApplicationConfiguration::IApplicationConfiguration(): modified(false), root_directory(""), data_directory(""), model_directory(""), sound_directory(""), image_directory(""), ui_resource_directory(""), db_host(""), db_port(""), db_name(""),	db_user(""), db_password(""), language("")
//{
//}
//
//IApplicationConfiguration::~IApplicationConfiguration()
//{
//}

//IApplicationConfiguration* IApplicationConfiguration::GetInstance()
//{
//	boost::mutex::scoped_lock lock(m_mutex);						
//	if (instance == NULL)
//		instance = new IApplicationConfiguration(); 
//	return instance;
//}
//
//void IApplicationConfiguration::ReleaseInstance()
//{
//	boost::mutex::scoped_lock lock(m_mutex);
//	if (instance != NULL)
//	{
//		delete instance;
//		instance = NULL;
//	}
//}
//core::CameraData  IApplicationConfiguration::GetCameraData(const int &id)
//{ 
//	boost::mutex::scoped_lock lock(m_mutex);
//	std::map< unsigned int, core::CameraData >::iterator found  = cam_map.find(id); 
//	if ( found != cam_map.end() ) 
//		return found->second; 
//	CameraData dummy; 
//	cam_map[id]=dummy; 
//	return dummy;			
//}
//core::DisplayData IApplicationConfiguration::GetDisplayData(const int &id)	    
//{ 
//	boost::mutex::scoped_lock lock(m_mutex);
//	std::map< unsigned int, core::DisplayData >::iterator found = win_map.find(id); 
//	if ( found != win_map.end() ) 
//		return found->second; 
//	DisplayData dummy; 
//	win_map[id]=dummy; 
//	return dummy;			
//}