#include <GUIConfiguration.h>
#include <igui/controllers/GUIGenericController.h>
#include <limits>

#include <debugger.h> 

#define wxID_OK			100
#define wxID_CANCEL		101
#define wxID_CALIBRATE	103
#define wxID_CALCHOMOGF 104
#define wxID_TRAINFOREG 105
#define wxID_STARTRECORDING		106
#define wxID_CAM_CHOICE	201
#define wxID_WIN_CHOICE	202
#define wxID_NUM_CAM	203
#define wxID_NUM_WIN	204
#define wxID_CAM_FLIP_V	300
#define wxID_CAM_FLIP_H	301
#define wxID_WIN_FLIP_V	400
#define wxID_WIN_FLIP_H	401
#define wxID_WIN_UP_OPP			402
#define wxID_WIN_DOWN_OPP		403
#define wxID_WIN_UP_STRAIGHT	404
#define wxID_WIN_DOWN_STRAIGHT	405
#define wxID_WIN_FRONT	406
#define wxID_WIN_BACK	407
#define wxID_WIN_LEFT	408
#define wxID_WIN_RIGHT	409
#define wxID_WIN_UP		410
#define wxID_WIN_DOWN	411
#define wxID_CAM_UP_OPP			1402
#define wxID_CAM_DOWN_OPP		1403
#define wxID_CAM_UP_STRAIGHT	1404
#define wxID_CAM_DOWN_STRAIGHT	1405
#define wxID_CAM_FRONT	1406
#define wxID_CAM_BACK	1407
#define wxID_CAM_LEFT	1408
#define wxID_CAM_RIGHT	1409
#define wxID_CAM_UP		1410
#define wxID_CAM_DOWN	1411
#define wxID_RADIO		500
#define wxID_LAYOUT_WIN	501
#define wxID_CAM_RADIO	502
#define wxID_CAM_MODE	503
#define wxID_CAM_XYZ	601
#define wxID_WIN_XYZ	602
#define wxID_WIN_RES	700
#define wxID_LANG_CHOICE 801



using namespace core;
using namespace core::igui;

BEGIN_EVENT_TABLE(GUIConfiguration, wxPanel)
	EVT_PAINT		 (							GUIConfiguration::OnPaint		)
	EVT_BUTTON		 ( wxID_OK					, OnOKButton					)
	EVT_BUTTON		 ( wxID_CANCEL				, OnCancelButton				)
	EVT_BUTTON		 ( wxID_CALIBRATE			, OnCalibrateButton				)
	EVT_BUTTON		 ( wxID_STARTRECORDING		, OnStartRecordingButton		)
	EVT_BUTTON		 ( wxID_CALCHOMOGF			, OnCalculateHomography			)
	EVT_BUTTON		 ( wxID_TRAINFOREG			, OnTrainBackground				)
	EVT_SPINCTRL	 ( wxID_NUM_CAM				, OnNumCamChanged				)
	EVT_SPINCTRL	 ( wxID_NUM_WIN				, OnNumWinChanged				)
	EVT_SPINCTRL	 ( wxID_WIN_RES				, OnWinResolutionChanged		)
	EVT_SPINCTRLDOUBLE(wxID_CAM_XYZ				, OnCamCoordsChanged			)
	EVT_SPINCTRLDOUBLE(wxID_WIN_XYZ				, OnWinCoordsChanged			)
	EVT_CHOICE		 ( wxID_CAM_CHOICE			, OnCamSelected					)
	EVT_CHOICE		 ( wxID_WIN_CHOICE			, OnWinSelected					)
	EVT_TOGGLEBUTTON ( wxID_CAM_FLIP_V			, OnCamFlipV					)
	EVT_TOGGLEBUTTON ( wxID_CAM_FLIP_H			, OnCamFlipH					)
	EVT_TOGGLEBUTTON ( wxID_WIN_FLIP_V			, OnWinFlipV					)
	EVT_TOGGLEBUTTON ( wxID_WIN_FLIP_H			, OnWinFlipH					)
	EVT_TOGGLEBUTTON ( wxID_WIN_FRONT			, OnWinFront					)
	EVT_TOGGLEBUTTON ( wxID_WIN_BACK			, OnWinBack						)
	EVT_TOGGLEBUTTON ( wxID_WIN_LEFT			, OnWinLeft						)
	EVT_TOGGLEBUTTON ( wxID_WIN_RIGHT			, OnWinRight					)
	EVT_TOGGLEBUTTON ( wxID_WIN_UP				, OnWinUp						)
	EVT_TOGGLEBUTTON ( wxID_WIN_DOWN			, OnWinDown						)
	EVT_TOGGLEBUTTON ( wxID_WIN_UP_OPP			, OnWinOU						)
	EVT_TOGGLEBUTTON ( wxID_WIN_DOWN_OPP		, OnWinOD						)
	EVT_TOGGLEBUTTON ( wxID_WIN_UP_STRAIGHT		, OnWinSU						)
	EVT_TOGGLEBUTTON ( wxID_WIN_DOWN_STRAIGHT	, OnWinSD						)
	EVT_TOGGLEBUTTON ( wxID_CAM_FRONT			, OnCamFront					)
	EVT_TOGGLEBUTTON ( wxID_CAM_BACK			, OnCamBack						)
	EVT_TOGGLEBUTTON ( wxID_CAM_LEFT			, OnCamLeft						)
	EVT_TOGGLEBUTTON ( wxID_CAM_RIGHT			, OnCamRight					)
	EVT_TOGGLEBUTTON ( wxID_CAM_UP				, OnCamUp						)
	EVT_TOGGLEBUTTON ( wxID_CAM_DOWN			, OnCamDown						)
	//EVT_TOGGLEBUTTON ( wxID_CAM_UP_OPP		, OnCamOU						)
	//EVT_TOGGLEBUTTON ( wxID_CAM_DOWN_OPP		, OnCamOD						)
	//EVT_TOGGLEBUTTON ( wxID_CAM_UP_STRAIGHT	, OnCamSU						)
	//EVT_TOGGLEBUTTON ( wxID_CAM_DOWN_STRAIGHT	, OnCamSD						)
	EVT_RADIOBOX	 ( wxID_RADIO				, OnRadioChanged				)
	EVT_RADIOBOX	 ( wxID_CAM_RADIO			, OnCamRadioChanged				)
	EVT_RADIOBOX	 ( wxID_CAM_MODE			, OnCamModeChanged				)
	//EVT_RADIOBOX	 ( wxID_RADIO			, OnRadioChanged			)
END_EVENT_TABLE()

GUIConfiguration::GUIConfiguration(IApplicationConfiguration *app_config_, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) 
: wxPanel(parent, id, pos, size, style, name), needs_restarting(false), isRecording(false), app_config(app_config_)
{
	std::stringstream s_image;
	if ( app_config != NULL )
		s_image << app_config->GetUIResourceDirectory() << "background.png";
	background_image = wxBitmap(s_image.str(), wxBITMAP_TYPE_ANY);

	int width, height;
	GetClientSize(&width, &height);

	panel_book = new wxListbook(this, wxID_ANY, wxPoint(20, 30), wxSize(width-40, height-75));

	InitGeneralPanel();
	InitVisualizationPanel();
	InitCamPanel();
	InitWinPanel();

	panel_book->AddPage(general_panel, _("General"), true);
	panel_book->AddPage(visulization_panel, _("Visualization"));
	panel_book->AddPage(cam_panel, _("Cameras"));
	panel_book->AddPage(win_panel, _("Displays"));

	ok_button     = new wxButton(this, wxID_OK, _("Ok"), wxPoint(width-200,height-42), wxSize(70, 19));
	cancel_button = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(width-100,height-42), wxSize(70, 19));

	//int col, row;
	//col = 80, row = 40;
	//user_name	 = new wxTextCtrl(this, wxID_ANY, "test1", wxPoint(col,row), wxDefaultSize);
	//user_passwd  = new wxTextCtrl(this, wxID_ANY, "111", wxPoint(col,row+25), wxDefaultSize, wxTE_PASSWORD);
	//login_button = new wxButton(this, wxID_LOGIN, "Log in", wxPoint(col,row+50), wxSize(70, 19));
	////delete_button = new wxButton(this, wxID_DELETE, "Delete", wxPoint(col,row+70), wxSize(70, 19));

	//col = 320, row = 40;
	//new_user_name	 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row), wxDefaultSize);
	//new_user_passwd  = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row+25), wxDefaultSize, wxTE_PASSWORD);	
	//new_user_passwd2 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(col,row+50), wxDefaultSize, wxTE_PASSWORD);
	//register_button  = new wxButton(this, wxID_REGISTER, "Register", wxPoint(col,row+75), wxSize(70, 19));

	LoadConfig();

	EnableCameraControls(false);
	EnableDisplayControls(false);

	//cam_choice->SetSelection(0);
	//window_choice->SetSelection(0);
}
//void GUIConfiguration::Clear()
//{
//	//user_name->Clear();
//	//user_passwd->Clear();
//	//new_user_name->Clear();
//	//new_user_passwd->Clear();
//	//new_user_passwd2->Clear();
//	//s_user_name = "";
//	//s_user_password = "";
//	//user_logged_in = false;
//}

GUIConfiguration::~GUIConfiguration()
{
	if ( bm_win_front )	delete bm_win_front;
	if ( bm_win_back )	delete bm_win_back;
	if ( bm_win_left )	delete bm_win_left;
	if ( bm_win_right )	delete bm_win_right;
	if ( bm_win_up )	delete bm_win_up;
	if ( bm_win_down )	delete bm_win_down;

	if ( bm_win_up_opposite )	delete bm_win_up_opposite;
	if ( bm_win_down_opposite )	delete bm_win_down_opposite;
	if ( bm_win_up_straight )	delete bm_win_up_straight;
	if ( bm_win_down_straight )	delete bm_win_down_straight;

	if ( bm_win_flip_v )delete bm_win_flip_v;
	if ( bm_win_flip_h )delete bm_win_flip_h;

	if ( bm_cam_flip_v )delete bm_cam_flip_v;
	if ( bm_cam_flip_h )delete bm_cam_flip_h;
	if ( bm_cam_front )	delete bm_cam_front;
	if ( bm_cam_back )	delete bm_cam_back;
	if ( bm_cam_left )	delete bm_cam_left;
	if ( bm_cam_right )	delete bm_cam_right;
	if ( bm_cam_up )	delete bm_cam_up;
	if ( bm_cam_down )	delete bm_cam_down;
}

void GUIConfiguration::Delete()
{
	wxPanel::Destroy();
}

void GUIConfiguration::OnPaint(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void GUIConfiguration::render(wxDC& dc)
{
	dc.SetTextForeground(wxColour(255,255,255));
	dc.DrawBitmap(background_image, 0, -20, false );
	dc.DrawRotatedText("Configuration", 20, 10, 0);
}

void GUIConfiguration::InitGeneralPanel()
{
	int width, height;
	GetClientSize(&width, &height);

	general_panel = new wxPanel(panel_book, wxID_ANY);

	if (app_config != NULL)
	{
		language_st = new wxStaticText(general_panel, wxID_ANY, _("Language: "), wxPoint(10,5));
		language_choice = new wxChoice(general_panel, wxID_LANG_CHOICE, wxPoint(110, 5), wxSize(100,19));
	}
}

void GUIConfiguration::InitVisualizationPanel()
{
	int width, height;
	GetClientSize(&width, &height);

	visulization_panel = new wxPanel(panel_book, wxID_ANY);

	if (app_config != NULL)
	{
		int col, row;
		col = 10; row = 5;
		visualization_st = new wxStaticText(visulization_panel, wxID_ANY, _("Activating these options may slow down your computer. "), wxPoint(col,row));
		cam_capture		= new wxCheckBox(visulization_panel, wxID_ANY, _("Cam capture"), wxPoint(col, row+20));
		homography		= new wxCheckBox(visulization_panel, wxID_ANY, _("Homography"), wxPoint(col, row+40));
		face_detection	= new wxCheckBox(visulization_panel, wxID_ANY, _("Face Detection"), wxPoint(col, row+60));
		foreground		= new wxCheckBox(visulization_panel, wxID_ANY, _("Foreground"), wxPoint(col, row+80));
		motion			= new wxCheckBox(visulization_panel, wxID_ANY, _("Motion"), wxPoint(col, row+100));

	}
}

void GUIConfiguration::InitCamPanel()
{
	int width, height;
	GetClientSize(&width, &height);

	cam_panel = new wxPanel(panel_book, wxID_ANY);

	if (app_config != NULL)
	{
		num_cam_st = new wxStaticText(cam_panel, wxID_ANY, _("Number of cameras: "), wxPoint(10,5));
		num_cams_ctrl = new wxSpinCtrl(cam_panel, wxID_NUM_CAM, _(""), wxPoint(110, 5), wxSize(100,19), 0, 0, INT_MAX, 1);
		calibrate_cam_button = new wxButton(cam_panel, wxID_CALIBRATE, _("Calibrate"), wxPoint(250, 5), wxSize(70, 19));
		startRecording_cam_button = new wxButton(cam_panel, wxID_STARTRECORDING, _("Start Recording"), wxPoint(250, 30), wxSize(100, 19));
		videoClipURL_TextControl = new wxTextCtrl(cam_panel, wxID_ANY, "sample_cam", wxPoint(360,30), wxDefaultSize);
		calc_homography_cam_button = new wxButton(cam_panel, wxID_CALCHOMOGF, _("Calc. Homography"), wxPoint(330, 5), wxSize(70, 19));
		train_background_button = new wxButton(cam_panel, wxID_TRAINFOREG, _("Train foreground"), wxPoint(400, 5), wxSize(70, 19));

		configure_cam_st = new wxStaticText(cam_panel, wxID_ANY, _("Configure camera: "), wxPoint(10,30));
		cam_choice = new wxChoice(cam_panel, wxID_CAM_CHOICE, wxPoint(110, 30), wxSize(100,19));

		std::stringstream image_h, image_v, ss_bm_cam_front, ss_bm_cam_back, ss_bm_cam_left, ss_bm_cam_right, ss_bm_cam_up, ss_bm_cam_down, ss_manual_layout;
		std::string uiresource_dir = app_config->GetUIResourceDirectory();
		image_h					<< uiresource_dir << "fliph.png";
		image_v					<< uiresource_dir << "flipv.png";
		ss_bm_cam_front			<< uiresource_dir << "cube_front.png";
		ss_bm_cam_back			<< uiresource_dir << "cube_back.png";
		ss_bm_cam_left			<< uiresource_dir << "cube_left.png";
		ss_bm_cam_right			<< uiresource_dir << "cube_right.png";
		ss_bm_cam_up			<< uiresource_dir << "cube_up.png";
		ss_bm_cam_down			<< uiresource_dir << "cube_down.png";
		ss_manual_layout		<< uiresource_dir << "manual_layout.png";

		bm_cam_flip_v = new wxBitmap(image_v.str(),wxBITMAP_TYPE_ANY);
		bm_cam_flip_h = new wxBitmap(image_h.str(),wxBITMAP_TYPE_ANY);
		bm_cam_front  = new wxBitmap(ss_bm_cam_front.str(),wxBITMAP_TYPE_ANY);
		bm_cam_back   = new wxBitmap(ss_bm_cam_back.str(),wxBITMAP_TYPE_ANY);
		bm_cam_left   = new wxBitmap(ss_bm_cam_left.str(),wxBITMAP_TYPE_ANY);
		bm_cam_right  = new wxBitmap(ss_bm_cam_right.str(),wxBITMAP_TYPE_ANY);
		bm_cam_up     = new wxBitmap(ss_bm_cam_up.str(),wxBITMAP_TYPE_ANY);
		bm_cam_down   = new wxBitmap(ss_bm_cam_down.str(),wxBITMAP_TYPE_ANY);

		cam_flip_v = new wxBitmapToggleButton(cam_panel, wxID_CAM_FLIP_V, *bm_cam_flip_v, wxPoint(100,95), wxSize(128, 128), 0);
		cam_flip_h = new wxBitmapToggleButton(cam_panel, wxID_CAM_FLIP_H, *bm_cam_flip_h, wxPoint(250,95), wxSize(128, 128), 0);

		cam_front= new wxBitmapToggleButton(cam_panel, wxID_CAM_FRONT, *bm_cam_front, wxPoint(10,85+32), wxSize(64, 64), 0);
		cam_back = new wxBitmapToggleButton(cam_panel, wxID_CAM_BACK, *bm_cam_back, wxPoint(20+64,85+32), wxSize(64, 64), 0);
		cam_left = new wxBitmapToggleButton(cam_panel, wxID_CAM_LEFT, *bm_cam_left, wxPoint(30+64*2,85+32), wxSize(64, 64), 0);
		cam_right= new wxBitmapToggleButton(cam_panel, wxID_CAM_RIGHT, *bm_cam_right, wxPoint(40+64*3,85+32), wxSize(64, 64), 0);
		cam_up   = new wxBitmapToggleButton(cam_panel, wxID_CAM_UP, *bm_cam_up, wxPoint(50+64*4,85+32), wxSize(64, 64), 0);
		cam_down = new wxBitmapToggleButton(cam_panel, wxID_CAM_DOWN, *bm_cam_down, wxPoint(60+64*5,85+32), wxSize(64, 64), 0);

		cam_x = new wxSpinCtrlDouble(cam_panel, wxID_CAM_XYZ, wxEmptyString, wxPoint(10, height-100), wxSize(100,20), 0, -1*FLT_MAX, FLT_MAX, 0, 1);
		cam_y = new wxSpinCtrlDouble(cam_panel, wxID_CAM_XYZ, wxEmptyString, wxPoint(115, height-100), wxSize(100,20), 0, -1*FLT_MAX, FLT_MAX, 0, 1); 
		cam_z = new wxSpinCtrlDouble(cam_panel, wxID_CAM_XYZ, wxEmptyString, wxPoint(220, height-100), wxSize(100,20), 0, -1*FLT_MAX, FLT_MAX, 0, 1);

		layout_cam_manual = new wxStaticBitmap(cam_panel, wxID_ANY, wxBitmap(ss_manual_layout.str(),wxBITMAP_TYPE_ANY),wxPoint(150,90));

		wxString str_vector[3];
		str_vector[0] = _("Flip H/V");
		str_vector[1] = _("Cube layout");
		str_vector[2] = _("Manual");
		cam_flip = new wxRadioBox(cam_panel, wxID_CAM_RADIO, _("Options"), wxPoint(10, 50), wxDefaultSize, 3, str_vector, 0, wxHORIZONTAL );

		wxString str_vector_Mode[2];
		str_vector_Mode[0] = _("Live Cam");
		str_vector_Mode[1] = _("Video clip");
		cam_mode = new wxRadioBox(cam_panel, wxID_CAM_MODE, _("Use"), wxPoint(250, 50), wxDefaultSize, 2, str_vector_Mode, 0, wxHORIZONTAL );

		ShowCamFlipHV();

	}
}

void GUIConfiguration::InitWinPanel()
{
	win_panel = new wxPanel(panel_book, wxID_ANY);

	int width, height;
	GetClientSize(&width, &height);

	if (app_config != NULL)
	{
		num_win_st = new wxStaticText(win_panel, wxID_ANY, _("Number of displays: "), wxPoint(10,5));
		num_windows_ctrl = new wxSpinCtrl(win_panel, wxID_NUM_WIN, _(""), wxPoint(110, 5), wxSize(100,19), 0, 0, INT_MAX, 1);

		configure_win_st = new wxStaticText(win_panel, wxID_ANY, _("Configure Display: "), wxPoint(10,27));
		window_choice = new wxChoice(win_panel, wxID_WIN_CHOICE, wxPoint(110, 27), wxSize(100,17));

		win_res_x_st = new wxStaticText(win_panel, wxID_ANY, _("Resolution X: "), wxPoint(215,5));
		win_res_x_ctrl = new wxSpinCtrl(win_panel, wxID_WIN_RES, _(""), wxPoint(315, 5), wxSize(100,19), 0, 20, INT_MAX, 400);
		win_res_y_st = new wxStaticText(win_panel, wxID_ANY, _("Resolution Y: "), wxPoint(215,27));
		win_res_y_ctrl = new wxSpinCtrl(win_panel, wxID_WIN_RES, _(""), wxPoint(315, 27), wxSize(100,19), 0, 20, INT_MAX, 400);

		std::stringstream ss_bm_win_up_opposite, ss_bm_win_down_opposite, ss_bm_win_up_straight, ss_bm_win_down_straight,
						  ss_bm_win_front, ss_bm_win_back, ss_bm_win_left, ss_bm_win_right, ss_bm_win_up, ss_bm_win_down, ss_bm_win_flip_v, ss_bm_win_flip_h, 
						  ss_manual_layout;

		std::string uiresource_dir = app_config->GetUIResourceDirectory();
		ss_bm_win_up_opposite	<< uiresource_dir << "flip_u_opposite.png";
		ss_bm_win_down_opposite << uiresource_dir << "flip_d_opposite.png";
		ss_bm_win_up_straight	<< uiresource_dir << "flip_u_straight.png";
		ss_bm_win_down_straight << uiresource_dir << "flip_d_straight.png";
		ss_bm_win_front			<< uiresource_dir << "cube_front.png";
		ss_bm_win_back			<< uiresource_dir << "cube_back.png";
		ss_bm_win_left			<< uiresource_dir << "cube_left.png";
		ss_bm_win_right			<< uiresource_dir << "cube_right.png";
		ss_bm_win_up			<< uiresource_dir << "cube_up.png";
		ss_bm_win_down			<< uiresource_dir << "cube_down.png";
		ss_bm_win_flip_v		<< uiresource_dir << "flipv.png";
		ss_bm_win_flip_h		<< uiresource_dir << "fliph.png";
		ss_manual_layout		<< uiresource_dir << "manual_layout.png";

		wxString str_vector_typeOfLayout[4];
		str_vector_typeOfLayout[0] = _("Flip H/V");
		str_vector_typeOfLayout[1] = _("Flip Schema");
		str_vector_typeOfLayout[2] = _("Cube layout");
		str_vector_typeOfLayout[3] = _("Manual");

		win_flip = new wxRadioBox(win_panel, wxID_RADIO, _("Options"), wxPoint(10, 50), wxDefaultSize, 4, str_vector_typeOfLayout, 0, wxHORIZONTAL );

		bm_win_flip_v = new wxBitmap(ss_bm_win_flip_v.str(),wxBITMAP_TYPE_ANY);
		bm_win_flip_h = new wxBitmap(ss_bm_win_flip_h.str(),wxBITMAP_TYPE_ANY);
		win_flip_v = new wxBitmapToggleButton(win_panel, wxID_WIN_FLIP_V, *bm_cam_flip_v, wxPoint(100,95), wxSize(128, 128), 0);
		win_flip_h = new wxBitmapToggleButton(win_panel, wxID_WIN_FLIP_H, *bm_cam_flip_h, wxPoint(250,95), wxSize(128, 128), 0);

		bm_win_up_opposite   = new wxBitmap(ss_bm_win_up_opposite.str(),wxBITMAP_TYPE_ANY);
		bm_win_down_opposite = new wxBitmap(ss_bm_win_down_opposite.str(),wxBITMAP_TYPE_ANY);
		bm_win_up_straight   = new wxBitmap(ss_bm_win_up_straight.str(),wxBITMAP_TYPE_ANY);
		bm_win_down_straight = new wxBitmap(ss_bm_win_down_straight.str(),wxBITMAP_TYPE_ANY);
		win_up_opposite   = new wxBitmapToggleButton(win_panel, wxID_WIN_UP_OPP, *bm_win_up_opposite, wxPoint(50+10,85+32), wxSize(64, 64), 0);
		win_down_opposite = new wxBitmapToggleButton(win_panel, wxID_WIN_DOWN_OPP, *bm_win_down_opposite, wxPoint(50+94,85+32), wxSize(64, 64), 0);
		win_up_straight   = new wxBitmapToggleButton(win_panel, wxID_WIN_UP_STRAIGHT, *bm_win_up_straight, wxPoint(50+175,85+32), wxSize(64, 64), 0);
		win_down_straight = new wxBitmapToggleButton(win_panel, wxID_WIN_DOWN_STRAIGHT, *bm_win_down_straight, wxPoint(50+259,85+32), wxSize(64, 64), 0);


		//wxString str_layout_vector[2];
		//str_layout_vector[0] = _("Cube");
		//str_layout_vector[1] = _("Manual");
		//win_layout = new wxRadioBox(win_panel, wxID_LAYOUT_WIN, _("Layout"), wxPoint(250, 40), wxSize(200,40), 2, str_layout_vector, 0, wxHORIZONTAL);

		bm_win_front= new wxBitmap(ss_bm_win_front.str(),wxBITMAP_TYPE_ANY);
		bm_win_back = new wxBitmap(ss_bm_win_back.str(),wxBITMAP_TYPE_ANY);
		bm_win_left = new wxBitmap(ss_bm_win_left.str(),wxBITMAP_TYPE_ANY);
		bm_win_right= new wxBitmap(ss_bm_win_right.str(),wxBITMAP_TYPE_ANY);
		bm_win_up   = new wxBitmap(ss_bm_win_up.str(),wxBITMAP_TYPE_ANY);
		bm_win_down = new wxBitmap(ss_bm_win_down.str(),wxBITMAP_TYPE_ANY);
		win_front= new wxBitmapToggleButton(win_panel, wxID_WIN_FRONT, *bm_win_front, wxPoint(10,85+32), wxSize(64, 64), 0);
		win_back = new wxBitmapToggleButton(win_panel, wxID_WIN_BACK, *bm_win_back, wxPoint(20+64,85+32), wxSize(64, 64), 0);
		win_left = new wxBitmapToggleButton(win_panel, wxID_WIN_LEFT, *bm_win_left, wxPoint(30+64*2,85+32), wxSize(64, 64), 0);
		win_right= new wxBitmapToggleButton(win_panel, wxID_WIN_RIGHT, *bm_win_right, wxPoint(40+64*3,85+32), wxSize(64, 64), 0);
		win_up   = new wxBitmapToggleButton(win_panel, wxID_WIN_UP, *bm_win_up, wxPoint(50+64*4,85+32), wxSize(64, 64), 0);
		win_down = new wxBitmapToggleButton(win_panel, wxID_WIN_DOWN, *bm_win_down, wxPoint(60+64*5,85+32), wxSize(64, 64), 0);

		layout_win_manual = new wxStaticBitmap(win_panel, wxID_ANY, wxBitmap(ss_manual_layout.str(),wxBITMAP_TYPE_ANY),wxPoint(150,90));

		win_x = new wxSpinCtrlDouble(win_panel, wxID_WIN_XYZ, wxEmptyString, wxPoint(10, height-103), wxSize(100,20), 0, -1*FLT_MAX, FLT_MAX, 0, 1);
		win_y = new wxSpinCtrlDouble(win_panel, wxID_WIN_XYZ, wxEmptyString, wxPoint(115, height-103), wxSize(100,20), 0, -1*FLT_MAX, FLT_MAX, 0, 1); 
		win_z = new wxSpinCtrlDouble(win_panel, wxID_WIN_XYZ, wxEmptyString, wxPoint(220, height-103), wxSize(100,20), 0, -1*FLT_MAX, FLT_MAX, 0, 1);

		ShowFlipHV();
	}

}

void GUIConfiguration::HideWinButtons()
{
	win_up_opposite->Show(false);
	win_down_opposite->Show(false);
	win_up_straight->Show(false);
	win_down_straight->Show(false);
	
	win_front->Show(false);
	win_back->Show(false);
	win_left->Show(false);
	win_right->Show(false);
	win_up->Show(false);
	win_down->Show(false);

	layout_win_manual->Show(false);
	win_x->Show(false);
	win_y->Show(false);
	win_z->Show(false);

	win_flip_v->Show(false);
	win_flip_h->Show(false);
}
void GUIConfiguration::HideCamButtons()
{
	cam_front->Show(false);
	cam_back->Show(false);
	cam_left->Show(false);
	cam_right->Show(false);
	cam_up->Show(false);
	cam_down->Show(false);

	layout_cam_manual->Show(false);
	cam_x->Show(false);
	cam_y->Show(false);
	cam_z->Show(false);

	cam_flip_v->Show(false);
	cam_flip_h->Show(false);
}
void GUIConfiguration::ShowFlipHV()
{
	HideWinButtons();

	win_flip_v->Show(true);
	win_flip_h->Show(true);

	Refresh();
	Update();
}

void GUIConfiguration::ShowCamFlipHV()
{
	HideCamButtons();

	cam_flip_v->Show(true);
	cam_flip_h->Show(true);

	Refresh();
	Update();
}

void GUIConfiguration::ShowFlipSchema()
{
	HideWinButtons();

	win_up_opposite->Show(true);
	win_down_opposite->Show(true);
	win_up_straight->Show(true);
	win_down_straight->Show(true);

	Refresh();
	Update();
}
void GUIConfiguration::ShowLayoutCube()
{
	HideWinButtons();

	win_front->Show(true);
	win_back->Show(true);
	win_left->Show(true);
	win_right->Show(true);
	win_up->Show(true);
	win_down->Show(true);

	win_x->Show(true);
	win_y->Show(true);
	win_z->Show(true);

	Refresh();
	Update();
}
void GUIConfiguration::ShowCamLayoutCube()
{
	HideCamButtons();

	cam_front->Show(true);
	cam_back->Show(true);
	cam_left->Show(true);
	cam_right->Show(true);
	cam_up->Show(true);
	cam_down->Show(true);

	cam_x->Show(true);
	cam_y->Show(true);
	cam_z->Show(true);

	Refresh();
	Update();
}
void GUIConfiguration::ShowLayoutManual()
{
	HideWinButtons();

	layout_win_manual->Show(true);
	win_x->Show(true);
	win_y->Show(true);
	win_z->Show(true);

	Refresh();
	Update();
}
void GUIConfiguration::ShowCamLayoutManual()
{
	HideCamButtons();

	layout_cam_manual->Show(true);
	cam_x->Show(true);
	cam_y->Show(true);
	cam_z->Show(true);

	Refresh();
	Update();
}
void GUIConfiguration::OnOKButton(wxCommandEvent& WXUNUSED(event))
{
	if (app_config != NULL)
	{
		app_config->SetNumCams(num_cams);
		app_config->SetNumDisplays(num_windows);

		needs_restarting =	(on_boot_num_cams != num_cams) || (on_boot_num_windows != num_windows);
		for (unsigned int i = 1; i <= num_cams; i++)
			app_config->SetCameraData(i, cam_map[i]);
		for (unsigned int i = 1; i <= num_windows; i++)
		{
			app_config->SetDisplayData(i, win_map[i]);
			needs_restarting = needs_restarting ||	(on_boot_win_map[i].resolution_x != win_map[i].resolution_x) 
												||	(on_boot_win_map[i].resolution_y != win_map[i].resolution_y);
		}

		app_config->ShowCamCapture(cam_capture->GetValue());
		app_config->ShowHomography(homography->GetValue());
		app_config->ShowFaceDetection(face_detection->GetValue());
		app_config->ShowForeground(foreground->GetValue());
		app_config->ShowMotion(motion->GetValue());

		if (needs_restarting)
		{
			wxMessageDialog message_dialog(this, _("Some changes need the \n application to be restarted"), "Message box", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
			message_dialog.ShowModal();
		}
	}
}

void GUIConfiguration::OnCancelButton(wxCommandEvent& WXUNUSED(event))
{
	LoadConfig();
}

void GUIConfiguration::OnCalibrateButton(wxCommandEvent& WXUNUSED(event))
{
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc)
		guiGc->Calibrate();
}

void GUIConfiguration::OnStartRecordingButton(wxCommandEvent& WXUNUSED(event))
{
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc)
	{
		if (!isRecording)
		{	guiGc->SetCameraRecording(true);
			startRecording_cam_button->SetLabel("STOP Recording");
		} else
		{	guiGc->SetCameraRecording(false);
			startRecording_cam_button->SetLabel("Start Recording");
		}
		isRecording = !isRecording;
	}
}


void GUIConfiguration::OnCalculateHomography(wxCommandEvent& WXUNUSED(event))
{
	//calculate homography matrix
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc)
		guiGc->CalculateHomography();
}

void GUIConfiguration::OnTrainBackground(wxCommandEvent& WXUNUSED(event))
{
	//train background/foreground detector
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	if (guiGc)
		guiGc->TrainBackground();
}

void GUIConfiguration::OnCamSelected(wxCommandEvent& WXUNUSED(event))
{
	int i = cam_choice->GetSelection()+1;

	core::CameraData data = cam_map[i];

	cam_flip_h->SetValue(data.flip_h);
	cam_flip_v->SetValue(data.flip_v);
	cam_x->SetValue(data.x);
	cam_y->SetValue(data.y);
	cam_z->SetValue(data.z);

	EnableCameraControls();
}

void GUIConfiguration::OnWinSelected(wxCommandEvent& WXUNUSED(event))
{
	int i = window_choice->GetSelection()+1;

	core::DisplayData data = win_map[i];

	win_flip_h->SetValue(data.flip_h);
	win_flip_v->SetValue(data.flip_v);
	win_x->SetValue(data.x);
	win_y->SetValue(data.y);
	win_z->SetValue(data.z);
	win_res_x_ctrl->SetValue(data.resolution_x);
	win_res_y_ctrl->SetValue(data.resolution_y);

	RefreshWinButtons(data);

	EnableDisplayControls();
}

void GUIConfiguration::OnNumCamChanged(wxSpinEvent& WXUNUSED(event))
{
	if (app_config != NULL)
	{
		num_cams = num_cams_ctrl->GetValue();

		cam_choice->Clear();

		for (unsigned int i = 1;  i <= num_cams; i++)
		{
			if ( cam_map.find(i) == cam_map.end() )
				cam_map[i] = app_config->GetCameraData(i);
			std::stringstream wop;
			wop << i;
			cam_choice->Append(wop.str());
		}

		cam_choice->SetSelection(0);
	}
}

void GUIConfiguration::OnNumWinChanged(wxSpinEvent& WXUNUSED(event))
{
	if (app_config !=NULL)
	{
		num_windows = num_windows_ctrl->GetValue();

		window_choice->Clear();

		for (unsigned int i = 1;  i <= num_windows; i++)
		{
			if ( win_map.find(i) == win_map.end() )
				win_map[i] = app_config->GetDisplayData(i);
			std::stringstream wop;
			wop << i;
			window_choice->Append(wop.str());
		}

		window_choice->SetSelection(0);
	}
}

void GUIConfiguration::OnWinResolutionChanged(wxSpinEvent& WXUNUSED(event))
{
	unsigned int res_x = win_res_x_ctrl->GetValue();
	unsigned int res_y = win_res_y_ctrl->GetValue();

	int i = window_choice->GetSelection()+1;

	win_map[i].resolution_x = res_x;
	win_map[i].resolution_y = res_y;
}

void GUIConfiguration::OnWinCoordsChanged(wxSpinDoubleEvent& WXUNUSED(event))
{
	int i = window_choice->GetSelection()+1;

	win_map[i].x = win_x->GetValue();
	win_map[i].y = win_y->GetValue();
	win_map[i].z = win_z->GetValue();

	RefreshWinButtons(win_map[i]);
}

void GUIConfiguration::OnCamCoordsChanged(wxSpinDoubleEvent& WXUNUSED(event))
{
	int i = cam_choice->GetSelection()+1;

	cam_map[i].x = cam_x->GetValue();
	cam_map[i].y = cam_y->GetValue();
	cam_map[i].z = cam_z->GetValue();
}

void GUIConfiguration::OnCamFlipV(wxCommandEvent& WXUNUSED(event))
{
	int i = cam_choice->GetSelection()+1;

	cam_map[i].flip_v = cam_flip_v->GetValue();
}

void GUIConfiguration::OnCamFlipH(wxCommandEvent& WXUNUSED(event))
{
	int i = cam_choice->GetSelection()+1;

	cam_map[i].flip_h = cam_flip_h->GetValue();
}

void GUIConfiguration::OnWinFlipV(wxCommandEvent& WXUNUSED(event))
{
	int i = window_choice->GetSelection()+1;

	win_map[i].flip_v = win_flip_v->GetValue();

	RefreshWinButtons(win_map[i]);
}

void GUIConfiguration::OnWinFlipH(wxCommandEvent& WXUNUSED(event))
{
	int i = window_choice->GetSelection()+1;

	win_map[i].flip_h = win_flip_h->GetValue();

	RefreshWinButtons(win_map[i]);
}

void GUIConfiguration::OnRadioChanged(wxCommandEvent& WXUNUSED(event))
{
	int sel = win_flip->GetSelection();
	switch (sel)
	{
		case 0:
			ShowFlipHV();
			break;
		case 1: 
			ShowFlipSchema();
			break;
		case 2: 
			ShowLayoutCube();
			break;
		case 3:
			ShowLayoutManual();
			break;
		default:
			break;
	}
	//if ( sel == 0 )
	//	ShowFlipHV();
	//else if ( sel == 1 )
	//	ShowFlipSchema();
}

void GUIConfiguration::OnCamRadioChanged(wxCommandEvent& WXUNUSED(event))
{
	int sel = cam_flip->GetSelection();
	switch (sel)
	{
		case 0:
			ShowCamFlipHV();
			break;
		case 1: 
			ShowCamLayoutCube();
			break;
		case 2:
			ShowCamLayoutManual();
			break;
		default:
			break;
	}
}

void GUIConfiguration::OnCamModeChanged(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);

	wxString video_url_basename = videoClipURL_TextControl->GetLabel();
	if ( video_url_basename.empty() )
	{
		wxMessageDialog *dialog = new wxMessageDialog(this, "Please add a video source basename");
		dialog->ShowModal();
		cam_mode->SetSelection(0);
		SetEvtHandlerEnabled(true);
		return;
	}

	//Apply the change
	GUIGenericController *guiGc	= GUIGenericController::GetInstance();
	int mode_index = cam_mode->GetSelection();

	std::string video_url_basename_str = video_url_basename;
	if ( mode_index == 0)
		guiGc->SetUseRecording(false, video_url_basename_str);
	else if ( mode_index == 1 )
		guiGc->SetUseRecording(true, video_url_basename_str);
	
	SetEvtHandlerEnabled(true);
}

void GUIConfiguration::OnLayoutRadioChanged(wxCommandEvent& WXUNUSED(event))
{
	int sel = win_layout->GetSelection();
	if ( sel == 0)
		ShowLayoutCube();
	else if ( sel == 1 )
		ShowLayoutManual();
}

void GUIConfiguration::LoadConfig()
{
	if (app_config != NULL)
	{
		on_boot_num_cams    = num_cams    = app_config->GetNumCams();
		on_boot_num_windows = num_windows = app_config->GetNumDisplays();

		num_cams_ctrl->SetValue(num_cams);
		num_windows_ctrl->SetValue(num_windows);

		cam_map.clear();
		win_map.clear();

		cam_choice->Clear();
		window_choice->Clear();

		for (unsigned int i = 1; i <= num_cams; i++)
		{	std::stringstream wop;
			wop << i;
			cam_map[i] = app_config->GetCameraData(i);
			on_boot_cam_map[i] = cam_map[i];
			cam_choice->Append(wop.str());
		}
		for (unsigned int i = 1; i <= num_windows; i++)
		{	std::stringstream wop;
			wop << i;
			win_map[i] = app_config->GetDisplayData(i);
			on_boot_win_map[i] = win_map[i];
			window_choice->Append(wop.str());
		}

		//cam_choice->SetSelection(0);
		//window_choice->SetSelection(0);
	}
}

void GUIConfiguration::OnWinFront(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].x = 0.0f;
	win_map[i].y = 1.0f;
	win_map[i].z = 0.0f;
	win_x->SetValue(win_map[i].x);
	win_y->SetValue(win_map[i].y);
	win_z->SetValue(win_map[i].z);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnCamFront(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = cam_choice->GetSelection()+1;
	cam_map[i].x = 0.0f;
	cam_map[i].y = 1.0f;
	cam_map[i].z = 0.0f;
	cam_x->SetValue(cam_map[i].x);
	cam_y->SetValue(cam_map[i].y);
	cam_z->SetValue(cam_map[i].z);
	RefreshCamButtons(cam_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinBack(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].x = 0.0f;
	win_map[i].y = -1.0f;
	win_map[i].z = 0.0f;
	win_x->SetValue(win_map[i].x);
	win_y->SetValue(win_map[i].y);
	win_z->SetValue(win_map[i].z);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnCamBack(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = cam_choice->GetSelection()+1;
	cam_map[i].x = 0.0f;
	cam_map[i].y = -1.0f;
	cam_map[i].z = 0.0f;
	cam_x->SetValue(cam_map[i].x);
	cam_y->SetValue(cam_map[i].y);
	cam_z->SetValue(cam_map[i].z);
	RefreshCamButtons(cam_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinLeft(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].x = -1.0f;
	win_map[i].y = 0.0f;
	win_map[i].z = 0.0f;
	win_x->SetValue(win_map[i].x);
	win_y->SetValue(win_map[i].y);
	win_z->SetValue(win_map[i].z);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnCamLeft(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = cam_choice->GetSelection()+1;
	cam_map[i].x = -1.0f;
	cam_map[i].y = 0.0f;
	cam_map[i].z = 0.0f;
	cam_x->SetValue(cam_map[i].x);
	cam_y->SetValue(cam_map[i].y);
	cam_z->SetValue(cam_map[i].z);
	RefreshCamButtons(cam_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinRight(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].x = 1.0f;
	win_map[i].y = 0.0f;
	win_map[i].z = 0.0f;
	win_x->SetValue(win_map[i].x);
	win_y->SetValue(win_map[i].y);
	win_z->SetValue(win_map[i].z);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnCamRight(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = cam_choice->GetSelection()+1;
	cam_map[i].x = 1.0f;
	cam_map[i].y = 0.0f;
	cam_map[i].z = 0.0f;
	cam_x->SetValue(cam_map[i].x);
	cam_y->SetValue(cam_map[i].y);
	cam_z->SetValue(cam_map[i].z);
	RefreshCamButtons(cam_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinUp(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].x = 0.0f;
	win_map[i].y = 0.0f;
	win_map[i].z = 1.0f;
	win_x->SetValue(win_map[i].x);
	win_y->SetValue(win_map[i].y);
	win_z->SetValue(win_map[i].z);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnCamUp(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = cam_choice->GetSelection()+1;
	cam_map[i].x = 0.0f;
	cam_map[i].y = 0.0f;
	cam_map[i].z = 1.0f;
	cam_x->SetValue(cam_map[i].x);
	cam_y->SetValue(cam_map[i].y);
	cam_z->SetValue(cam_map[i].z);
	RefreshCamButtons(cam_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinDown(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].x = 0.0f;
	win_map[i].y = 0.0f;
	win_map[i].z = -1.0f;
	win_x->SetValue(win_map[i].x);
	win_y->SetValue(win_map[i].y);
	win_z->SetValue(win_map[i].z);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnCamDown(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = cam_choice->GetSelection()+1;
	cam_map[i].x = 0.0f;
	cam_map[i].y = 0.0f;
	cam_map[i].z = -1.0f;
	cam_x->SetValue(cam_map[i].x);
	cam_y->SetValue(cam_map[i].y);
	cam_z->SetValue(cam_map[i].z);
	RefreshCamButtons(cam_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinOU(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].flip_v = false;
	win_map[i].flip_h = false;
	win_flip_v->SetValue(win_map[i].flip_v);
	win_flip_h->SetValue(win_map[i].flip_h);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinOD(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].flip_v = true;
	win_map[i].flip_h = false;
	win_flip_v->SetValue(win_map[i].flip_v);
	win_flip_h->SetValue(win_map[i].flip_h);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinSU(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].flip_v = false;
	win_map[i].flip_h = true;
	win_flip_v->SetValue(win_map[i].flip_v);
	win_flip_h->SetValue(win_map[i].flip_h);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}
void GUIConfiguration::OnWinSD(wxCommandEvent& WXUNUSED(event))
{
	SetEvtHandlerEnabled(false);
	int i = window_choice->GetSelection()+1;
	win_map[i].flip_v = true;
	win_map[i].flip_h = true;
	win_flip_v->SetValue(win_map[i].flip_v);
	win_flip_h->SetValue(win_map[i].flip_h);
	RefreshWinButtons(win_map[i]);
	SetEvtHandlerEnabled(true);
}

void GUIConfiguration::RefreshWinButtons(const core::DisplayData &data)
{
	win_up_opposite->SetValue  (!(data.flip_h) && !(data.flip_v));
	win_down_opposite->SetValue(!(data.flip_h) && (data.flip_v));
	win_up_straight->SetValue  ((data.flip_h) && !(data.flip_v));
	win_down_straight->SetValue((data.flip_h) && (data.flip_v));

	win_front->SetValue( (data.x == 0.0f)  && (data.y == 1.0f)  && (data.z == 0.0f) );
	win_back->SetValue ( (data.x == 0.0f)  && (data.y == -1.0f) && (data.z == 0.0f) );
	win_left->SetValue ( (data.x == -1.0f) && (data.y == 0.0f)  && (data.z == 0.0f) );
	win_right->SetValue( (data.x == 1.0f)  && (data.y == 0.0f)  && (data.z == 0.0f) );
	win_up->SetValue   ( (data.x == 0.0f)  && (data.y == 0.0f)  && (data.z == 1.0f) );
	win_down->SetValue ( (data.x == 0.0f)  && (data.y == 0.0f)  && (data.z == -1.0f));
}

void GUIConfiguration::RefreshCamButtons(const core::CameraData &data)
{
	cam_front->SetValue( (data.x == 0.0f)  && (data.y == 1.0f)  && (data.z == 0.0f) );
	cam_back->SetValue ( (data.x == 0.0f)  && (data.y == -1.0f) && (data.z == 0.0f) );
	cam_left->SetValue ( (data.x == -1.0f) && (data.y == 0.0f)  && (data.z == 0.0f) );
	cam_right->SetValue( (data.x == 1.0f)  && (data.y == 0.0f)  && (data.z == 0.0f) );
	cam_up->SetValue   ( (data.x == 0.0f)  && (data.y == 0.0f)  && (data.z == 1.0f) );
	cam_down->SetValue ( (data.x == 0.0f)  && (data.y == 0.0f)  && (data.z == -1.0f));
}

void GUIConfiguration::EnableDisplayControls(const bool &value)
{
	win_flip->Enable(value);

	win_up_opposite->Enable(value);
	win_down_opposite->Enable(value);
	win_up_straight->Enable(value);
	win_down_straight->Enable(value);
	win_front->Enable(value);
	win_back->Enable(value);
	win_left->Enable(value);
	win_right->Enable(value);
	win_up->Enable(value);
	win_down->Enable(value);
	win_flip_v->Enable(value);
	win_flip_h->Enable(value);

	win_x->Enable(value);
	win_y->Enable(value);
	win_z->Enable(value);

	win_res_x_ctrl->Enable(value);
	win_res_y_ctrl->Enable(value);
}

void GUIConfiguration::EnableCameraControls(const bool &value)
{
	cam_flip->Enable(value);
	cam_flip_v->Enable(value);
	cam_flip_h->Enable(value);

	cam_x->Enable(value);
	cam_y->Enable(value);
	cam_z->Enable(value);
}
