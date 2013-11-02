#ifndef _IPROD_3D_WINDOW_
#define _IPROD_3D_WINDOW_

#include <winsock2.h>

#include <igui/maingui.h>

#include <string>
#include <vector>

#include <pandaFramework.h>

namespace core
{
	namespace iprod
	{
		class Prod3DWindow : public core::IGuiWindow
		{
			public:

				Prod3DWindow(PandaFramework *framework, const WindowProperties &props, bool registrable = false, bool fullscreenable = false);
				virtual ~Prod3DWindow();

				WindowFramework *GetWindowFrameWork() {return m_windowFramework;}

				void Show(const bool &value = true);
				void FullScreen( const bool &value = true );
				bool IsShown() {return isShown;}

			private:

				bool isShown;
				WindowFramework *m_windowFramework;
				WindowProperties m_properties;
		};
	}
}

#endif

