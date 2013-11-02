#ifndef _IAPPLICATIONCONFIGURATION_
#define _IAPPLICATIONCONFIGURATION_

#include <core/types.h>
#include <core/Export.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace core
{
		/** \brief data class for camera configuration */
		class CameraData
		{
			public:
				CameraData() : x(0.0f), y(0.0f), z(0.0f), flip_h(false), flip_v(false) {}
				~CameraData(){}
			//private:
				double x, y, z;
				bool flip_h, flip_v;
		};

		/** \brief data class for display configuration */
		class DisplayData
		{
			public:
				DisplayData() : x(0.0f), y(0.0f), z(0.0f), flip_h(false), flip_v(false), resolution_x(800), resolution_y(600) {}
				~DisplayData() {}
			//private:
				double x, y, z;
				bool flip_h, flip_v;
				unsigned int resolution_x, resolution_y;
		};

	/** 
		\brief Contains the configuration of the application. 
		The information for this class is read from the config.ini file and other configuration files.
		
		\ingroup core
	*/
	class /*COREEXPORT*/ IApplicationConfiguration
	{
		public:
			//static IApplicationConfiguration * GetInstance();
			//static void ReleaseInstance();
			virtual ~IApplicationConfiguration(){}

			virtual core::CameraData  GetCameraData(const int &id)=0;
			virtual core::DisplayData GetDisplayData(const int &id)=0;	

			virtual void SetRootDirectory(const std::string &value) = 0;
			virtual void SetDataDirectory(const std::string &value) = 0;
			virtual void SetModelDirectory(const std::string &value) = 0;
			virtual void SetSoundDirectory(const std::string &value) = 0;
			virtual void SetImageDirectory(const std::string &value) = 0;
			virtual void SetExternDirectory(const std::string &value) = 0;
			virtual void SetUIResourceDirectory(const std::string &value) = 0;

			virtual void SetSDHost(const std::string &value) = 0;
			virtual void SetSDPort(const std::string &value) = 0;
			virtual void SetSDName(const std::string &value) = 0;
			virtual void SetSDUser(const std::string &value) = 0;
			virtual void SetSDPassword(const std::string &value) = 0;

			virtual void SetLanguage(const std::string &value) = 0;
			
			virtual void SetNumCams(const unsigned int &value) = 0;
			virtual void SetNumDisplays(const unsigned int &value) = 0;
			virtual void SetCameraData(const int &id, const CameraData &value) = 0;
			virtual void SetDisplayData(const int &id, const DisplayData &value) = 0;

			virtual std::string GetRootDirectory() = 0;
			virtual std::string GetDataDirectory() = 0;
			virtual std::string GetModelDirectory() = 0;
			virtual std::string GetSoundDirectory() = 0;
			virtual std::string GetImageDirectory()	= 0;
			virtual std::string GetExternDirectory()= 0;
			virtual std::string GetUIResourceDirectory() = 0;

			virtual std::string GetSDHost() = 0;
			virtual std::string GetSDPort() = 0;
			virtual std::string GetSDName() = 0;
			virtual std::string GetSDUser() = 0;
			virtual std::string GetSDPassword() = 0;

			virtual std::string GetLanguage() = 0;

			virtual unsigned int GetNumCams() = 0;
			virtual unsigned int GetNumDisplays() = 0;

			virtual void SetUseRecognition(const bool &value)= 0;
			virtual void SetAutoLogin(const bool &value) = 0;
			virtual bool GetUseRecognition() = 0;
			virtual bool GetAutoLogin() = 0;

			virtual bool IsShownCamCapture() = 0;
			virtual bool IsShownHomography() = 0;
			virtual bool IsShownFaceDetection() = 0;
			virtual bool IsShownForeground() = 0;
			virtual bool IsShownMotion() = 0;
			virtual void ShowCamCapture(const bool &value) = 0;
			virtual void ShowHomography(const bool &value) = 0;
			virtual void ShowFaceDetection(const bool &value) = 0;
			virtual void ShowForeground(const bool &value) = 0;
			virtual void ShowMotion(const bool &value) = 0;

		//private:

		//	virtual void Modified() { modified = true; }

		//private:

			//std::string root_directory, data_directory, model_directory, sound_directory, 
			//	        image_directory, ui_resource_directory, db_host, db_port, db_name, 
			//			db_user, db_password, language;
			//
			//unsigned int num_cams;
			//unsigned int num_displays;

			//std::map< unsigned int, core::CameraData >  cam_map;
			//std::map< unsigned int, core::DisplayData > win_map;

			//bool modified;

		private:
			//IApplicationConfiguration();
			//~IApplicationConfiguration();

			//static boost::mutex m_mutex;
			//static IApplicationConfiguration *instance
	};
}

#endif
