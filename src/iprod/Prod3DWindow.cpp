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
	//m_windowFramework->
	//m_windowFramework = framework->open_window(props, 0, new GraphicsPipe(), new GraphicsStateGuardian());
	//m_windowFramework = framework->open_window(props); //comprobar graphic pipe graphic state guardian != m
	//m_windowFramework = framework->open_window();
}

Prod3DWindow::~Prod3DWindow()
{
	//delete m_windowFramework;
}

void Prod3DWindow::Show(const bool &value)
{

}

void Prod3DWindow::FullScreen(const bool &value)
{
		////NOTES---------
		////GraphicsEngine	*ge = framework.get_graphics_engine();		//only one, global, pointers and buffers
		////GraphicsPipe	*gp = framework.get_default_pipe();			//at least one, OGL/DX
		////GraphicsWindow	*gw = pandawindows_array[1]->get_graphics_window();		//can be many
		////GraphicsWindow	*gw2 = pandawindows_array[2]->get_graphics_window();
		////DisplayInformation	*dp = gp->get_display_information();
		////GraphicsDevice		*gd = gp->get_device();



		////WindowProperties win_props_fs = m_windowFramework->get_graphics_window()->request_properties();->requ->get_WindowProperties(); 
		////int d_id = dp->get_device_id();
		////dp->get_display_mode_height(d_id);
		////int max_w = dp->_maximum_window_width;
		////int max_h = dp->_maximum_window_height;
		////win_props_fs.set_size(max_w, max_h); 
		m_properties.set_fullscreen(true);
		m_properties.set_undecorated(true);
		m_windowFramework->get_graphics_window()->request_properties(m_properties);
		//////gw->request_properties(win_props_fs);
		////gw->set_properties_now(win_props_fs);
		////framework.close_window(1);
		////window = framework.open_window(win_props_fs);
		////--------------
}