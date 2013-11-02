#ifndef _APPLICATIONCONFIGURATION_
#define _APPLICATIONCONFIGURATION_

//#include <core/Export.h>
#include <core/IApplicationConfiguration.h>
#include <string>
#include <vector>
#include <map>
#include <boost/thread.hpp>

using namespace core;

//namespace core
//{
	/** 
		\brief Contains the configuration of the application. 
		The information for this class is read from the config.ini file and other configuration files.
		
		\ingroup core
	*/
	class  ApplicationConfiguration : public core::IApplicationConfiguration
	{
		public:
			ApplicationConfiguration();
			~ApplicationConfiguration();

			static ApplicationConfiguration * GetInstance();

			virtual core::CameraData  GetCameraData(const int &id);
			virtual core::DisplayData GetDisplayData(const int &id);	

			virtual void SetRootDirectory(const std::string &value)  { boost::mutex::scoped_lock lock(m_mutex); root_directory  = value; }
			virtual void SetDataDirectory(const std::string &value)  { boost::mutex::scoped_lock lock(m_mutex); data_directory  = value; }
			virtual void SetModelDirectory(const std::string &value) { boost::mutex::scoped_lock lock(m_mutex); model_directory = value; }
			virtual void SetSoundDirectory(const std::string &value) { boost::mutex::scoped_lock lock(m_mutex); sound_directory = value; }
			virtual void SetImageDirectory(const std::string &value) { boost::mutex::scoped_lock lock(m_mutex); image_directory = value; }
			virtual void SetExternDirectory(const std::string &value){ boost::mutex::scoped_lock lock(m_mutex); extern_directory = value; }
			virtual void SetUIResourceDirectory(const std::string &value) { boost::mutex::scoped_lock lock(m_mutex); ui_resource_directory = value; }

			virtual void SetSDHost(const std::string &value)		{ boost::mutex::scoped_lock lock(m_mutex); db_host = value; }
			virtual void SetSDPort(const std::string &value)		{ boost::mutex::scoped_lock lock(m_mutex); db_port = value; }
			virtual void SetSDName(const std::string &value)		{ boost::mutex::scoped_lock lock(m_mutex); db_name = value; }
			virtual void SetSDUser(const std::string &value)		{ boost::mutex::scoped_lock lock(m_mutex); db_user = value; }
			virtual void SetSDPassword(const std::string &value)	{ boost::mutex::scoped_lock lock(m_mutex); db_password = value; }

			virtual void SetLanguage(const std::string &value)		{ boost::mutex::scoped_lock lock(m_mutex); language = value; }
			
			virtual void SetNumCams(const unsigned int &value)		{ boost::mutex::scoped_lock lock(m_mutex); num_cams = value;		}
			virtual void SetNumDisplays(const unsigned int &value)	{ boost::mutex::scoped_lock lock(m_mutex); num_displays = value; }
			virtual void SetUseRecognition(const bool &value)		{ boost::mutex::scoped_lock lock(m_mutex); use_recognition = value; }
			virtual void SetAutoLogin(const bool &value)			{ boost::mutex::scoped_lock lock(m_mutex); auto_login = value; }
			virtual void SetCameraData (const int &id, const CameraData  &value);
			virtual void SetDisplayData(const int &id, const DisplayData &value);

			virtual std::string GetRootDirectory()			{ boost::mutex::scoped_lock lock(m_mutex);  return root_directory;	}
			virtual std::string GetDataDirectory()			{ boost::mutex::scoped_lock lock(m_mutex);  return data_directory;	}
			virtual std::string GetModelDirectory()			{ boost::mutex::scoped_lock lock(m_mutex);  return model_directory;	}
			virtual std::string GetSoundDirectory()			{ boost::mutex::scoped_lock lock(m_mutex);  return sound_directory;	}
			virtual std::string GetImageDirectory()			{ boost::mutex::scoped_lock lock(m_mutex);  return image_directory;	}
			virtual std::string GetExternDirectory()		{ boost::mutex::scoped_lock lock(m_mutex);  return extern_directory;	}
			virtual std::string GetUIResourceDirectory()	{ boost::mutex::scoped_lock lock(m_mutex);  return ui_resource_directory; }

			virtual std::string GetSDHost()					{ boost::mutex::scoped_lock lock(m_mutex);  return db_host;			}
			virtual std::string GetSDPort()					{ boost::mutex::scoped_lock lock(m_mutex);  return db_port;			}
			virtual std::string GetSDName()					{ boost::mutex::scoped_lock lock(m_mutex);  return db_name;			}
			virtual std::string GetSDUser()					{ boost::mutex::scoped_lock lock(m_mutex);  return db_user;			}
			virtual std::string GetSDPassword()				{ boost::mutex::scoped_lock lock(m_mutex);  return db_password;		}

			virtual std::string GetLanguage()				{ boost::mutex::scoped_lock lock(m_mutex);  return language;			}

			virtual unsigned int GetNumCams()				{ boost::mutex::scoped_lock lock(m_mutex);  return num_cams;			}
			virtual unsigned int GetNumDisplays()			{ boost::mutex::scoped_lock lock(m_mutex);  return num_displays;		}
			virtual bool GetUseRecognition()				{ boost::mutex::scoped_lock lock(m_mutex);  return use_recognition;		}
			virtual bool GetAutoLogin()						{ boost::mutex::scoped_lock lock(m_mutex);  return auto_login;			}

			virtual void ShowCamCapture(const bool &value)			{ show_capture = value;			}
			virtual void ShowHomography(const bool &value)			{ show_homography = value;		}
			virtual void ShowFaceDetection(const bool &value)		{ show_face_detection = value;	}
			virtual void ShowForeground(const bool &value)			{ show_foreground = value;		}
			virtual void ShowMotion(const bool &value)				{ show_motion = value;			}

			virtual bool IsShownCamCapture()						{ return show_capture;			}
			virtual bool IsShownHomography()						{ return show_homography;		}
			virtual bool IsShownFaceDetection()						{ return show_face_detection;	}
			virtual bool IsShownForeground()						{ return show_foreground;		}
			virtual bool IsShownMotion()							{ return show_motion;			}
			

		private:

			std::string root_directory, data_directory, model_directory, sound_directory, extern_directory, 
				        image_directory, ui_resource_directory, db_host, db_port, db_name, 
						db_user, db_password, language;
			
			unsigned int num_cams;
			unsigned int num_displays;

			std::map< unsigned int, core::CameraData >  cam_map;
			std::map< unsigned int, core::DisplayData > win_map;

			bool modified;

			bool show_capture, show_homography, show_face_detection, show_foreground, show_motion,
				 use_recognition, auto_login;

		private:

			virtual void Modified() { modified = true; }

		//private:

			//std::string root_directory, data_directory, model_directory, sound_directory, 
			//	        image_directory, ui_resource_directory, db_host, db_port, db_name, 
			//			db_user, db_password, language;
			
			//unsigned int num_cams;
			//unsigned int num_displays;


			//bool modified;

		private:
			//ApplicationConfiguration();
			//~ApplicationConfiguration();

			static boost::mutex m_mutex;
			static ApplicationConfiguration *instance;
			//static std::map< unsigned int, core::CameraData >  cam_map;
			//static std::map< unsigned int, core::DisplayData > win_map;

	};
//}

#endif
