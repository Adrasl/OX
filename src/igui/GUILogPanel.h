#ifndef _GUILOG_
#define _GUILOG_

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/textctrl.h>
#include <string>
#include <core/IApplicationConfiguration.h>

namespace core
{
	namespace igui
	{
		class GUILogPanel : public wxPanel
		{
			public:

				GUILogPanel(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id,  const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name = "panel");
				virtual ~GUILogPanel();

				void Delete();
				void PostMessage(const std::string &value);

				void OnPaint(wxPaintEvent &evt);
				void paintNow();	        
				void render(wxDC& dc);
				void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { /*Do not remove.*/ }

			private:
				
				IApplicationConfiguration *app_config;
				wxBitmap background_image;
				static wxTextCtrl *log_ctrl;

				DECLARE_EVENT_TABLE()

		};
	}
}

#endif

