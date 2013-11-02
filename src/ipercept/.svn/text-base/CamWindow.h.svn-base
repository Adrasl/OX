#ifndef _IPERCEPT_CAM_WINDOW_
#define _IPERCEPT_CAM_WINDOW_

#include <igui/maingui.h>

#include <string>
#include <vector>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

namespace core
{
	namespace ipercept
	{
		class CamWindow : public core::IGuiWindow
		{
			public:

				CamWindow(const std::string &_window_name);
				virtual ~CamWindow();

				void ShowImage(const IplImage *image);
				
				void Show(const bool &value = true);
				void FullScreen( const bool &value = true );
				bool IsShown() {return isShown;}

			private:

				std::string window_name;
				bool isShown;
		};
	}
}

#endif

