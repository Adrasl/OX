#ifndef _IGUI_
#define _IGUI_

#include <string>
#include <core/Export.h>
#include <core/IGUI/IGuiWindow.h>
#include <core/IApplication.h>
#include <core/IApplicationConfiguration.h>

namespace core
{
	class _COREEXPORT_ IGui
	{
		public:

			virtual ~IGui(){}
			virtual void Delete()=0;
			virtual void Update()=0;

			/** \brief Register a Windows so that it can be managed. */
			virtual void RegisterWindow(IGuiWindow *window)=0;
			/** \brief Adds a window to the fullscreen list (not available yet). */
			virtual void AddWindowToFullscreenList(IGuiWindow *window)=0;
			/** \brief Show all windows. */
			virtual void ShowAll(bool value)=0;
			/** \brief Sets fullscreen for all windows (not available yet). */
			virtual void SetAllWindowsFullScreen(bool value)=0;
			/** \brief Posts a message to the Log Panel. */
			virtual void PostLogMessage(const std::string &message)=0;
			
			virtual void SetApp(IApplication *app_) = 0 ;
			virtual void SetAppConfig(IApplicationConfiguration *app_config_) = 0;
			virtual void LogOut() = 0;
			virtual void LogIn(const std::string &name, const std::string &passwd) = 0;

			/** \brief Shows the given face image. */
			virtual void ShowFaceAtGUI(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step) = 0;
			/** \brief Fills login input fields. */
			virtual void FillLoginUserGUI(const std::string username, const std::string userpasswd) = 0;
			/** \brief Fills new user input fields. */
			virtual void FillNewUserGUI(const std::string username, const std::string userpasswd) = 0;

		private:

	};
}


#endif

