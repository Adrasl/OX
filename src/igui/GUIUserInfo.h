#ifndef _GUIUSERINFO_
#define _GUIUSERINFO_

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <string>
#include <vector>
#include <core/IApplicationConfiguration.h>
#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>
#include <ipersistence/types.h>

namespace core
{
	namespace igui
	{
		class GUIUserInfo : public wxPanel
		{
			public:

				GUIUserInfo(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id,  const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name = "panel");
				virtual ~GUIUserInfo();

				void Delete();
				void Clear();

				void OnPaint(wxPaintEvent &evt);
				void paintNow();	        
				void render(wxDC& dc);
				void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { /*Do not remove.*/ }
				void SetUser(core::IUserPersistence* user_);

				void DoLogOut();
				void DoRun();

			private:
			
				core::IUserPersistence* user;
				std::string user_name;
				std::vector<std::string> world_names;
				std::vector<int> world_permissions;
				int s_world_permissions;
				int s_user_permissions;

			private:

				IApplicationConfiguration *app_config;
				wxBitmap background_image;

				wxTextCtrl *world_name;//, *user_passwd, *new_user_name, *new_user_passwd, *new_user_passwd2;				
				wxButton *run_button, 
					     *logout_button, 
						 *newworld_button, 
					     *delete_user_button, *delete_world_button,
						 *save_permissions_to_selected_world_button;
				wxListBox *world_list;
				wxCheckBox *userflag_itself_modify_psi, *userflag_owner_actions_modify_psi, 
					       *userflag_owner_see, *userflag_owner_listen, *userflag_owner_touch, *userflag_owner_pick, *userflag_owner_create, *userflag_owner_destroy, 
						   *worldflag_itself_modify_psi, *worldflag_owner_actions_modify_psi, 
					       *worldflag_owner_see, *worldflag_owner_listen, *worldflag_owner_touch, *worldflag_owner_pick, *worldflag_owner_create, *worldflag_owner_destroy;
				wxArrayString wx_world_names;

				DECLARE_EVENT_TABLE()

				//void DoLogin(const std::string &name, const std::string &passwd);
				void ReloadUserInfo()	  { SetUser(user); }
				void SaveUserPermissions(){ user->SetPermissions(s_user_permissions); user->Save(); }
				void OnRunButton(wxCommandEvent& WXUNUSED(event));
				void OnDeleteUserButton(wxCommandEvent& WXUNUSED(event));
				void OnDeleteWorldButton(wxCommandEvent& WXUNUSED(event));
				void OnLogOutButton(wxCommandEvent& WXUNUSED(event));
				void OnNewWorldButton(wxCommandEvent& WXUNUSED(event));
				void OnWorldSelected(wxCommandEvent& WXUNUSED(event));
				void OnSaveWorldPermissions(wxCommandEvent& WXUNUSED(event));
				void CheckPermissions(wxCommandEvent& WXUNUSED(event));

		};
	}
}

#endif

