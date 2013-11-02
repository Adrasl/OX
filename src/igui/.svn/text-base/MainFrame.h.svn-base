#ifndef _IGUIMAINFRAME_
#define _IGUIMAINFRAME_

#include <core/IGUI/IGui.h>
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/menuitem.h>
#include <wx/bitmap.h>

#include <igui/Export.h>
#include <string>

#include <igui/maingui.h>
#include <igui/GUILogPanel.h>
#include <igui/GUIHelp.h>
#include <igui/GUIStart.h>
#include <igui/GUIUser.h>
#include <igui/GUIUserInfo.h>
#include <igui/GUIConfiguration.h>

#include <igui/controllers/GUIGenericController.h>


namespace core
{
	namespace igui
	{
		class MainGui;

		class MainFrame : public wxFrame
		{
			public:

				MainFrame(MainGui *maingui, wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN , const wxString &name=wxFrameNameStr);
				virtual ~MainFrame();

				void Delete();

				void PostLogMessage(const std::string &message);
				void ViewUserInfo();
				void ViewLog();
				void ViewConfigure();
				void LogOut();
				void LogIn(const std::string &name, const std::string &passwd);

				void SetFace(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step);

				void FillLoginUserGUI(const std::string username, const std::string userpasswd);
				void FillNewUserGUI(const std::string username, const std::string userpasswd);


			private:
				
				MainGui *main_gui;

				wxMenu *file_menu, 
					   *view_menu, 
					   *tools_menu, 
					   *help_menu;

				wxMenuItem *item_file_close, *item_file_login, *item_file_logout,*item_file_auto,
					       *item_view_fullscreen, *item_view_start, *item_view_user, *item_view_log_panel, 
						   *item_tools_configure,
						   *item_help_about;

				wxWindow     *dummy_panel;
				GUIHelp      *help_panel;
				GUILogPanel  *log_panel;
				GUIStart     *start_panel;
				GUIUser		 *user_panel;
				GUIConfiguration *configure_panel;
				GUIUserInfo	 *userinfo_panel;

				DECLARE_EVENT_TABLE()

				void InitShortCuts();
				void DismissPanels();

				void OnFileAuto(wxCommandEvent& WXUNUSED(event));
				void OnFileLogin(wxCommandEvent& WXUNUSED(event));
				void OnFileLogout(wxCommandEvent& WXUNUSED(event));
				void OnFileClose(wxCommandEvent& WXUNUSED(event));
				void OnViewFullScreen(wxCommandEvent& WXUNUSED(event));
				void OnViewStart(wxCommandEvent& WXUNUSED(event));
				void OnViewUser(wxCommandEvent& WXUNUSED(event));
				void OnViewLogPanel(wxCommandEvent& WXUNUSED(event));
				void OnToolsConfigure(wxCommandEvent& WXUNUSED(event));
				void OnHelpAbout(wxCommandEvent& WXUNUSED(event));
				
		};
	}
}

#endif

