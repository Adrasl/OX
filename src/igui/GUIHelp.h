#ifndef _GUIHELP_
#define _GUIHELP_

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/menuitem.h>
#include <wx/bitmap.h>
#include <core/IApplicationConfiguration.h>
#include <string>


namespace core
{
	namespace igui
	{
		class GUIHelp : public wxPanel
		{
			public:

				GUIHelp(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id,  const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name = "panel");
				virtual ~GUIHelp();

				void Delete();

				void OnPaint(wxPaintEvent &evt);
				void paintNow();	        
				void render(wxDC& dc);
				void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { /*Do not remove.*/ }

			private:
				
				IApplicationConfiguration *app_config;
				wxBitmap background_image;

				DECLARE_EVENT_TABLE()

				//void InitShortCuts();
				//void OnViewFullScreen(wxCommandEvent& WXUNUSED(event));
				//void OnClose(wxCommandEvent& WXUNUSED(event));

		};
	}
}

#endif

