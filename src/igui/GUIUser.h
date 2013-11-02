#ifndef _GUIUSER_
#define _GUIUSER_

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <string>
#include <core/IApplicationConfiguration.h>

namespace core
{
	namespace igui
	{
		class GUIUser : public wxPanel
		{
			public:

				GUIUser(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id,  const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name = "panel");
				virtual ~GUIUser();

				void Delete();
				void Clear();
				bool IsUserLogged() {return user_logged_in;}
				void SetFace(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step);
				void FillLoginUserGUI(const std::string username, const std::string userpasswd);
				void FillNewUserGUI(const std::string username, const std::string userpasswd);
				void DoLogin(const std::string &name, const std::string &passwd);

			private:
			
				std::string s_user_name, s_user_password;
				bool user_logged_in;
				bool face_detected;

			private:
				
				IApplicationConfiguration *app_config;
				wxBitmap background_image;

				wxTextCtrl *user_name, *user_passwd, *new_user_name, *new_user_passwd, *new_user_passwd2;				
				wxButton *login_button, *register_button;
				wxBitmap face_bitmap, *no_face_bitmap;

				DECLARE_EVENT_TABLE()

				void DoLogout();
				void OnLoginButton(wxCommandEvent& WXUNUSED(event));
				void OnNewUserButton(wxCommandEvent& WXUNUSED(event));
				void OnPaint(wxPaintEvent &evt);
				void paintNow();	        
				void render(wxDC& dc);
				void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { /*Do not remove.*/ }

		};
	}
}

#endif

