#ifndef _GUICONFIGURATION_
#define _GUICONFIGURATION_

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/spinctrl.h>
#include <wx/listbook.h>
#include <wx/choice.h>
#include <wx/tglbtn.h>
#include <wx/radiobox.h>
#include <wx/statbmp.h>
#include <string>
#include <vector>
#include <map>
#include <core/IApplicationConfiguration.h>

namespace core
{
	namespace igui
	{

		class GUIConfiguration : public wxPanel
		{
			public:

				GUIConfiguration(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id,  const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name = "panel");
				virtual ~GUIConfiguration();

				void Delete();
				//void Clear();

				void SetAppConfig(IApplicationConfiguration *app_config_) {app_config = app_config_;}
				//void SetNumCams(const unsigned int &value)		{ num_cams = value;		}
				//void SetNumDisplays(const unsigned int &value)	{ num_windows = value;	}
				void SetCameraData(const int &id, const CameraData &value)			{ cam_map[id]=value; num_cams_ctrl->SetValue(cam_map.size()); }
				void SetDisplayData(const int &id, const DisplayData &value)		{ win_map[id]=value; num_windows_ctrl->SetValue(win_map.size()); }

				unsigned int GetNumCams()							{ return num_cams;			}
				unsigned int GetNumDisplays()						{ return num_windows;		}
				core::CameraData  GetCameraData(const int &id)		{ std::map< unsigned int, core::CameraData >::iterator found  = cam_map.find(id); if (found != cam_map.end() ) return found->second; CameraData dummy; return dummy;			}
				core::DisplayData GetDisplayData(const int &id)		{ std::map< unsigned int, core::DisplayData >::iterator found = win_map.find(id); if (found != win_map.end() ) return found->second; DisplayData dummy;return dummy;			}

				void OnPaint(wxPaintEvent &evt);
				void paintNow();	        
				void render(wxDC& dc);
				void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { /*Do not remove.*/ }

			private:
			
				unsigned int num_cams, num_windows,
					         on_boot_num_cams, on_boot_num_windows;

				std::map< unsigned int, core::CameraData >  cam_map, on_boot_cam_map;
				std::map< unsigned int, core::DisplayData > win_map, on_boot_win_map;

				IApplicationConfiguration *app_config;

			private:

				wxBitmap background_image;
				wxListbook *panel_book;
				wxPanel *general_panel, *visulization_panel, *cam_panel, *win_panel;

				wxSpinCtrl *num_cams_ctrl, *num_windows_ctrl, 
					       *win_res_x_ctrl, *win_res_y_ctrl;
				wxSpinCtrlDouble *cam_x, *cam_y, *cam_z,
					             *win_x, *win_y, *win_z;

				wxChoice	*cam_choice, 
							*window_choice, 
							*language_choice;

				wxCheckBox *cam_capture, *homography, *face_detection, *foreground, *motion;

				wxRadioBox *win_flip, *cam_flip, //_hv, *win_flip_grafphical,
					       *win_layout, *cam_mode;//_dice, *win_layout_manual;

				wxBitmapToggleButton *win_up_opposite, *win_down_opposite, *win_up_straight, *win_down_straight,
									 *win_front, *win_back, *win_left, *win_right, *win_up, *win_down, 
									 *cam_front, *cam_back, *cam_left, *cam_right, *cam_up, *cam_down, 
									 *win_flip_v, *win_flip_h,
									 *cam_flip_v, *cam_flip_h;

				wxBitmap *bm_win_up_opposite, *bm_win_down_opposite, *bm_win_up_straight, *bm_win_down_straight,
									 *bm_win_front, *bm_win_back, *bm_win_left, *bm_win_right, *bm_win_up, *bm_win_down, 
									 *bm_cam_up_opposite, *bm_cam_down_opposite, *bm_cam_up_straight, *bm_cam_down_straight,
									 *bm_cam_front, *bm_cam_back, *bm_cam_left, *bm_cam_right, *bm_cam_up, *bm_cam_down,
									 *bm_win_flip_v, *bm_win_flip_h,
									 *bm_cam_flip_v, *bm_cam_flip_h;

				wxStaticText *num_cam_st, *num_win_st, *configure_cam_st, *configure_win_st,
					         *win_res_x_st, *win_res_y_st,
							 *language_st,
							 *visualization_st;
				wxStaticBitmap *layout_win_manual, *layout_cam_manual;

				wxButton *ok_button, *cancel_button, *calibrate_cam_button, *startRecording_cam_button, *calc_homography_cam_button, *train_background_button;

				wxTextCtrl *videoClipURL_TextControl;

				bool needs_restarting, isRecording;
	

				DECLARE_EVENT_TABLE()

				void InitGeneralPanel();
				void InitVisualizationPanel();
				void InitCamPanel();
				void InitWinPanel();

				void LoadConfig();

				void HideWinButtons();
				void HideCamButtons();
				void ShowFlipHV();
				void ShowFlipSchema();
				void ShowLayoutCube();
				void ShowLayoutManual();
				void ShowCamFlipHV();
				void ShowCamLayoutCube();
				void ShowCamLayoutManual();
				void RefreshWinButtons(const core::DisplayData &data);
				void RefreshCamButtons(const core::CameraData &data);
				void EnableDisplayControls(const bool &value = true);
				void EnableCameraControls(const bool &value = true);

				//void DoLogout();
				//void DoLogin(const std::string &name, const std::string &passwd);
				void OnOKButton(wxCommandEvent& WXUNUSED(event));
				void OnCancelButton(wxCommandEvent& WXUNUSED(event));
				void OnCalibrateButton(wxCommandEvent& WXUNUSED(event));
				void OnStartRecordingButton(wxCommandEvent& WXUNUSED(event));
				void OnCalculateHomography(wxCommandEvent& WXUNUSED(event));				
				void OnTrainBackground(wxCommandEvent& WXUNUSED(event));
				void OnCamSelected(wxCommandEvent& WXUNUSED(event));
				void OnWinSelected(wxCommandEvent& WXUNUSED(event));
				void OnNumCamChanged(wxSpinEvent& WXUNUSED(event));
				void OnNumWinChanged(wxSpinEvent& WXUNUSED(event));
				void OnWinResolutionChanged(wxSpinEvent& WXUNUSED(event));
				void OnCamCoordsChanged(wxSpinDoubleEvent& WXUNUSED(event));
				void OnWinCoordsChanged(wxSpinDoubleEvent& WXUNUSED(event));
				void OnCamFlipV(wxCommandEvent& WXUNUSED(event));
				void OnCamFlipH(wxCommandEvent& WXUNUSED(event));
				void OnWinFlipV(wxCommandEvent& WXUNUSED(event));
				void OnWinFlipH(wxCommandEvent& WXUNUSED(event));
				void OnWinFront(wxCommandEvent& WXUNUSED(event));
				void OnWinBack(wxCommandEvent& WXUNUSED(event));
				void OnWinLeft(wxCommandEvent& WXUNUSED(event));
				void OnWinRight(wxCommandEvent& WXUNUSED(event));
				void OnWinUp(wxCommandEvent& WXUNUSED(event));
				void OnWinDown(wxCommandEvent& WXUNUSED(event));
				void OnWinOU(wxCommandEvent& WXUNUSED(event));
				void OnWinOD(wxCommandEvent& WXUNUSED(event));
				void OnWinSU(wxCommandEvent& WXUNUSED(event));
				void OnWinSD(wxCommandEvent& WXUNUSED(event));
				void OnCamFront(wxCommandEvent& WXUNUSED(event));
				void OnCamBack(wxCommandEvent& WXUNUSED(event));
				void OnCamLeft(wxCommandEvent& WXUNUSED(event));
				void OnCamRight(wxCommandEvent& WXUNUSED(event));
				void OnCamUp(wxCommandEvent& WXUNUSED(event));
				void OnCamDown(wxCommandEvent& WXUNUSED(event));
				void OnCamOU(wxCommandEvent& WXUNUSED(event));
				void OnCamOD(wxCommandEvent& WXUNUSED(event));
				void OnCamSU(wxCommandEvent& WXUNUSED(event));
				void OnCamSD(wxCommandEvent& WXUNUSED(event));
				void OnRadioChanged(wxCommandEvent& WXUNUSED(event));
				void OnCamRadioChanged(wxCommandEvent& WXUNUSED(event));
				void OnCamModeChanged(wxCommandEvent& WXUNUSED(event));
				void OnLayoutRadioChanged(wxCommandEvent& WXUNUSED(event));

		};
	}
}

#endif

