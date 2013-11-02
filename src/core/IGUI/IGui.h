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

			virtual void RegisterWindow(IGuiWindow *window)=0;
			virtual void AddWindowToFullscreenList(IGuiWindow *window)=0;
			virtual void ShowAll(bool value)=0;
			virtual void SetAllWindowsFullScreen(bool value)=0;
			virtual void PostLogMessage(const std::string &message)=0;
			virtual void SetApp(IApplication *app_) = 0 ;
			virtual void SetAppConfig(IApplicationConfiguration *app_config_) = 0;
			virtual void LogOut() = 0;
			virtual void LogIn(const std::string &name, const std::string &passwd) = 0;
			virtual void ShowFaceAtGUI(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step) = 0;
			virtual void FillLoginUserGUI(const std::string username, const std::string userpasswd) = 0;
			virtual void FillNewUserGUI(const std::string username, const std::string userpasswd) = 0;

		private:

	};
}


#endif

