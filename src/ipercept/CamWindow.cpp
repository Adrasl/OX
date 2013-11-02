#include <CamWindow.h>

#include <debugger.h> 

using namespace core::ipercept;
using namespace cv;

CamWindow::CamWindow(const std::string &_window_name) : window_name(_window_name), isShown(true)
{
	cvNamedWindow(window_name.c_str(),1);
	core::igui::MainGui::GetInstance()->RegisterWindow(((core::IGuiWindow*)this));
}

CamWindow::~CamWindow()
{
	cvDestroyWindow(window_name.c_str());
}

void CamWindow::ShowImage(const IplImage *image)
{
	if (isShown)
		cvShowImage(window_name.c_str(), image);
}

void CamWindow::Show(const bool &value)
{
	//isShown = value;
	//if (value == true)
	//	cvDestroyWindow(window_name.c_str());
	//else
	//	cvNamedWindow(window_name.c_str(),1);
	
	//nothing - cvWindows cannot be edited
	bool nothing = value;
}

void CamWindow::FullScreen(const bool &value)
{
	//nothing - cvWindows cannot be edited
	bool nothing = value;
}