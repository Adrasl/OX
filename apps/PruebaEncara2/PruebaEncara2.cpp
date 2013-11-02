// PruebaEncara2.cpp : main project file.

//#include "stdafx.h"
//using namespace System;

//OPENCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//ENCARA2
#include <ENCARA2_2lib.h>

int main(int argc, char *argv[ ])
{
	//ENCARA2
	//FILE *fdata=fopen("ENCARAdataconfig.txt","r");

	//std::stringstream wop_config;
	//wop_config << ini_path << "ENCARAdataconfig.txt";
	//std::string config_file_name = "ENCARAdataconfig.txt";
	//std::ifstream config_file(config_file_name.c_str());

	//if ( config_file.is_open() )
	//{
	//}

	char ENCARAdataDir[256];
	char *ENCARA2=getenv("ENCARA2");
	sprintf(ENCARAdataDir,"%s\\ENCARA2data",ENCARA2);
	CENCARA2_2Detector *ENCARAFaceDetector;
	ENCARAFaceDetector = new CENCARA2_2Detector(ENCARAdataDir,320,240);


	CvCapture *m_capture;
	m_capture=cvCreateCameraCapture(0);//Since ver 1.1 using -1 crashes
	cvNamedWindow("Camera", 1);

	while (true)
	{
		IplImage *h=cvQueryFrame(m_capture);
		if (h)
		{
			//Process the image using ENCAAR2 //The last parameter indicates that recognition 
			ENCARAFaceDetector->ApplyENCARA2(h,2);
			//Paints results
			ENCARAFaceDetector->PaintFacialData(h,CV_RGB(0,255,0));
			cvShowImage("Camera",h);
		}
		cvWaitKey(10);
	}

	cvDestroyWindow("Camera");
	cvReleaseCapture(&m_capture);
	cvDestroyWindow("Camera");
	cvDestroyWindow("Image");

	//ENCARA2
	delete ENCARAFaceDetector;


    return 0;
}
