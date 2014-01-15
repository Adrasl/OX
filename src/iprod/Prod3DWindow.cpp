#include <Prod3DWindow.h>

#include <debugger.h> 

using namespace core::iprod;

Prod3DWindow::Prod3DWindow(PandaFramework *framework, const WindowProperties &props, bool registrable, bool fullscreenable) : isShown(true), m_windowFramework(NULL), m_properties(props)
{
	if (registrable)
		core::igui::MainGui::GetInstance()->RegisterWindow(((core::IGuiWindow*)this));
	if (fullscreenable)
		core::igui::MainGui::GetInstance()->AddWindowToFullscreenList(((core::IGuiWindow*)this));
	m_windowFramework = framework->open_window();
	m_windowFramework->get_graphics_window()->request_properties(props);
}

Prod3DWindow::~Prod3DWindow()
{}

void Prod3DWindow::Show(const bool &value)
{}

void Prod3DWindow::FullScreen(const bool &value)
{
	m_properties.set_fullscreen(true);
	m_properties.set_undecorated(true);
	m_windowFramework->get_graphics_window()->request_properties(m_properties);
}