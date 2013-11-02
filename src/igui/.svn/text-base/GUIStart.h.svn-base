#ifndef _GUISTART_
#define _GUISTART_

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/menuitem.h>
#include <wx/bitmap.h>
#include <core/IApplicationConfiguration.h>
#include <string>
#include <sstream>


namespace core
{
	namespace igui
	{
		class GUIStart : public wxPanel
		{
			public:

				GUIStart(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id,  const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name = "panel");
				virtual ~GUIStart();

				void Delete();

				void OnPaint(wxPaintEvent &evt);
				void paintNow();	        
				void render(wxDC& dc);
				void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { /*Do not remove.*/ }

			private:
				
				IApplicationConfiguration *app_config;
				wxBitmap background_image;

				DECLARE_EVENT_TABLE()
				
				wxButton *login_button, *start_button, *configure_button;

				//void InitShortCuts();
				//void OnViewFullScreen(wxCommandEvent& WXUNUSED(event));
				//void OnClose(wxCommandEvent& WXUNUSED(event));

				void OnLoginButton(wxCommandEvent& WXUNUSED(event));
				void OnAutoButton(wxCommandEvent& WXUNUSED(event));
				void OnConfigureButton(wxCommandEvent& WXUNUSED(event));

		};
	}
}

#endif

