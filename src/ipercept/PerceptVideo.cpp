#include <gmtl/gmtl.h>
#include <gmtl/Matrix.h>
#include <PerceptVideo.h>
#include <iostream>
#ifdef WIN32
#include <Aclapi.h>
#endif

#include <RTreeTemplate/RTree.h>
#include <iostream>

#include <debugger.h> 

#define _USE_ENCARA2_

using namespace core;
using namespace core::ipercept;
using namespace cv;
using namespace std;

//RTree callback
bool MySearchCallback(int id, void* arg) 
{ printf("Hit data rect %d\n", id);
  return true; // keep going
}

static enum PlaneOrientation {	FRONT,
								BACK,
								LEFT,
								RIGHT,
								UP,
								DOWN };

unsigned int PerceptVideo::num_cams = 1;
std::map< int, CvCapture* > PerceptVideo::capture_cam_array;
std::map< int, CvCapture* > PerceptVideo::capture_videofiles_array;
std::map< int, CvVideoWriter* > PerceptVideo::capture_videowriter;
std::map< std::string, CamWindow* > PerceptVideo::camWindow_array;
std::map< std::string, CamWindow* > PerceptVideo::debugcamWindow_array;
std::vector<bool> PerceptVideo::flip_h;
std::vector<bool> PerceptVideo::flip_v;
std::vector<int> PerceptVideo::search_results;
std::map<int, IplImage *> PerceptVideo::capture_img;
boost::try_mutex PerceptVideo::m_mutex, PerceptVideo::bbox_mutex;
boost::try_mutex PerceptVideo::homography_mutex;
boost::shared_ptr<boost::thread> PerceptVideo::m_thread;
bool PerceptVideo::initialized = false;
bool PerceptVideo::stop_requested = false;
bool PerceptVideo::is_recording = false;
bool PerceptVideo::is_using_videosource = false;
IFaceRecognition *PerceptVideo::face_recognizer=NULL;
IApplicationConfiguration *PerceptVideo::app_config = NULL;
IUserDataModelController *PerceptVideo::user_dataModel_controller = NULL;
std::vector<IFaceDetection*> PerceptVideo::face_detectors;
std::vector<IMotionDetection*> PerceptVideo::motion_detectors;
std::vector<IPresenceDetection*> PerceptVideo::presence_detectors;
std::vector<core::Image> PerceptVideo::face_history;
bool PerceptVideo::calibrating=false;
bool PerceptVideo::show_cam_capture=false;
bool PerceptVideo::show_homography=false;
bool PerceptVideo::show_face_detection=false;
bool PerceptVideo::show_foreground=false;
bool PerceptVideo::show_motion=false;
bool PerceptVideo::presence_detected=false;
std::map< int, CvMat* > PerceptVideo::intrinsics;
std::map< int, CvMat* > PerceptVideo::distortion;
std::map< int, IplImage* > PerceptVideo::undistort_mapx;
std::map< int, IplImage* > PerceptVideo::undistort_mapy;
std::map< int, CvMat* > PerceptVideo::homography;
std::map<int, vector2I> PerceptVideo::cam_capture_size;
double PerceptVideo::las_time=0;
double PerceptVideo::capture_fps=10;
double PerceptVideo::timestamp_Recording_latestframe=0;
corePoint3D<double> PerceptVideo::BoundinBoxMin, PerceptVideo::BoundinBoxMax;

gmtl::Matrix<float, 4, 4>	MatrixTransform_CubeFRONT, MatrixTransform_CubeBACK,
							MatrixTransform_CubeLEFT, MatrixTransform_CubeRIGHT,
							MatrixTransform_CubeTOP, MatrixTransform_CubeBOTTOM,
							MatrixTransform;

inline void InitMatrixTransform()
{
	MatrixTransform_CubeFRONT.setState(gmtl::Matrix<float, 4, 4>::FULL);
	MatrixTransform_CubeBACK.setState(gmtl::Matrix<float, 4, 4>::FULL);
	MatrixTransform_CubeLEFT.setState(gmtl::Matrix<float, 4, 4>::FULL);
	MatrixTransform_CubeRIGHT.setState(gmtl::Matrix<float, 4, 4>::FULL);
	MatrixTransform_CubeTOP.setState(gmtl::Matrix<float, 4, 4>::FULL);
	MatrixTransform_CubeBOTTOM.setState(gmtl::Matrix<float, 4, 4>::FULL);
	MatrixTransform.setState(gmtl::Matrix<float, 4, 4>::FULL);

	for (int i = 0; i < 4; i++) //Transposed memory order
	for (int j = 0; j < 4; j++)
	{
		MatrixTransform_CubeFRONT[i][j] = 0.0;
		MatrixTransform_CubeBACK[i][j] = 0.0;
		MatrixTransform_CubeLEFT[i][j] = 0.0;
		MatrixTransform_CubeRIGHT[i][j] = 0.0;
		MatrixTransform_CubeTOP[i][j] = 0.0;
		MatrixTransform_CubeBOTTOM[i][j] = 0.0;
	}

	//FRONT
	MatrixTransform_CubeFRONT[0][0] = 1.0;
	MatrixTransform_CubeFRONT[2][1] = -1.0;
	MatrixTransform_CubeFRONT[3][3] = 1.0;
	//BACK
	MatrixTransform_CubeBACK[0][0] = -1.0;
	MatrixTransform_CubeBACK[2][1] = -1.0;
	MatrixTransform_CubeBACK[3][3] = 1.0;
	//LEFT
	MatrixTransform_CubeLEFT[1][0] = -1.0;
	MatrixTransform_CubeLEFT[2][1] = -1.0;
	MatrixTransform_CubeLEFT[3][3] = 1.0;
	//RIGHT
	MatrixTransform_CubeRIGHT[1][0] = -1.0;
	MatrixTransform_CubeRIGHT[2][1] = -1.0;
	MatrixTransform_CubeRIGHT[3][3] = 1.0;
	//UP
	MatrixTransform_CubeTOP[0][0] = 1.0;
	MatrixTransform_CubeTOP[1][1] = 1.0;
	MatrixTransform_CubeTOP[3][3] = 1.0;
	//DOWN
	MatrixTransform_CubeBOTTOM[0][0] = -1.0;
	MatrixTransform_CubeBOTTOM[1][1] = 1.0;
	MatrixTransform_CubeBOTTOM[3][3] = 1.0;

}

PerceptVideo::PerceptVideo(IApplicationConfiguration *app_config_)
{

	app_config = app_config_;
	if (app_config != NULL)
	{
		num_cams = app_config->GetNumCams();
	}
	if (!initialized)
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			//Capture
			if (app_config != NULL)
			{
					flip_h.push_back(app_config->GetCameraData(i+1).flip_h);
					flip_v.push_back(app_config->GetCameraData(i+1).flip_v);
			}
			capture_cam_array[i+1] = cvCaptureFromCAM( i );
			//cvSetCaptureProperty(capture_cam_array[i+1],CV_CAP_PROP_FPS,capture_fps);
			
			//Face detectors
			#ifdef _USE_ENCARA2_
			face_detectors.push_back(new Encara2FaceDetection(this, i+1));
			#else
			face_detectors.push_back(new SimpleFaceDetection(app_config, this, i+1));
			#endif

			//Calibration
			std::stringstream intrinsics_filename, distortion_filename;
			if(app_config)
			{	intrinsics_filename << app_config->GetRootDirectory() << "Intrinsics" << i+1 << ".xml";
				distortion_filename << app_config->GetRootDirectory() << "Distortion" << i+1 << ".xml";		}
			else
			{	intrinsics_filename << "./Intrinsics" << i+1 << ".xml";
				distortion_filename << "./Distortion" << i+1 << ".xml";	}
			intrinsics[i+1] = (CvMat*)cvLoad( intrinsics_filename.str().c_str() );
			distortion[i+1] = (CvMat*)cvLoad( distortion_filename.str().c_str() );
			capture_img[i+1] = cvQueryFrame(capture_cam_array[i+1]);
			vector2I cam_dim;
			if (capture_img[i+1])
			{
				cam_dim.x = capture_img[i+1]->width;
				cam_dim.y = capture_img[i+1]->height;
			}
			cam_capture_size[i+1] = cam_dim;
			undistort_mapx[i+1] = cvCreateImage( cvGetSize( capture_img[i+1] ), IPL_DEPTH_32F, 1 );
			undistort_mapy[i+1] = cvCreateImage( cvGetSize( capture_img[i+1] ), IPL_DEPTH_32F, 1 );
			if( (intrinsics[i+1]) && (distortion[i+1]) && (intrinsics[i+1]) && (distortion[i+1]) )
				cvInitUndistortMap( intrinsics[i+1], distortion[i+1], undistort_mapx[i+1], undistort_mapy[i+1] );

			//Homographies
			std::stringstream homographies_filename;
			if(app_config) homographies_filename << app_config->GetRootDirectory() << "Homography" << i+1 << ".xml";
			else   		   homographies_filename << "./Homography" << i+1 << ".xml";
			homography[i+1] = (CvMat*)cvLoad( homographies_filename.str().c_str() );

			//Background substraction
			PresenceDetection *pd = new PresenceDetection(this, i+1);
			presence_detectors.push_back(pd);

			//Motion
			motion_detectors.push_back(new MotionDetection(this, i+1));

		}
		//Face recognizer
		face_recognizer = new FaceRecognition(app_config);
	}
}

void PerceptVideo::ShowCamCapture(const bool &value)
{	//Cam Windows
	if(value)
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream window_name;
			window_name << "Cam" << i+1 << ":";
			camWindow_array[window_name.str()] = new CamWindow(window_name.str());
			camWindow_array[window_name.str()]->ShowImage(capture_img[i+1]);
		}
	}
	else
	{
		for (std::map< std::string, CamWindow* >::iterator iter = camWindow_array.begin(); iter!=camWindow_array.end(); iter++)
			delete iter->second;
		camWindow_array.erase(camWindow_array.begin(),camWindow_array.end());
	}
	show_cam_capture = value;
}

void PerceptVideo::ShowHomography(const bool &value)
{	//Homography
	if(value)
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream h_window_name;
			h_window_name << "Homography " << i+1 << ":";
			debugcamWindow_array[h_window_name.str()] = new CamWindow(h_window_name.str());
			debugcamWindow_array[h_window_name.str()]->ShowImage(capture_img[i+1]);
		}
	}
	else
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream h_window_name;
			h_window_name << "Homography " << i+1 << ":";
			std::map< std::string, CamWindow* >::iterator iter = debugcamWindow_array.find(h_window_name.str());
			if(iter != debugcamWindow_array.end())
			{
				delete iter->second;
				debugcamWindow_array.erase(iter);
			}
		}
	}
	show_homography = value;
}

void PerceptVideo::ShowFaceDetection(const bool &value)
{	//Face detectors
	if(value)
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream fd_window_name;
			fd_window_name << "face detector " << i+1 << ":";
			debugcamWindow_array[fd_window_name.str()] = new CamWindow(fd_window_name.str());
		}
	}
	else
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream fd_window_name;
			fd_window_name << "face detector " << i+1 << ":";
			std::map< std::string, CamWindow* >::iterator iter = debugcamWindow_array.find(fd_window_name.str());
			if(iter != debugcamWindow_array.end())
			{
				delete iter->second;
				debugcamWindow_array.erase(iter);
			}
		}
	}
	show_face_detection = value;
}

void PerceptVideo::ShowForeground(const bool &value)
{	//Face detectors
	if(value)
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream fd_window_name;
			fd_window_name << "Presence detector " << i+1 << ":";
			debugcamWindow_array[fd_window_name.str()] = new CamWindow(fd_window_name.str());
		}
	}
	else
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream fd_window_name;
			fd_window_name << "Presence detector " << i+1 << ":";
			std::map< std::string, CamWindow* >::iterator iter = debugcamWindow_array.find(fd_window_name.str());
			if(iter != debugcamWindow_array.end())
			{
				delete iter->second;
				debugcamWindow_array.erase(iter);
			}
		}
	}
	show_foreground = value;
}


void PerceptVideo::ShowMotion(const bool &value)
{	//Motion detection
	if(value)
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream fd_window_name, fd_window_name2;
			fd_window_name << "Motion " << i+1 << ":";
			fd_window_name2 << "Motion Optical Flow" << i+1 << ":";
			debugcamWindow_array[fd_window_name.str()] = new CamWindow(fd_window_name.str());
			debugcamWindow_array[fd_window_name2.str()] = new CamWindow(fd_window_name2.str());
		}
	}
	else
	{
		for (unsigned int i=0; i<num_cams; i++)
		{
			std::stringstream fd_window_name, fd_window_name2;
			fd_window_name << "Motion " << i+1 << ":";
			fd_window_name2 << "Motion Optical Flow" << i+1 << ":";
			
			std::map< std::string, CamWindow* >::iterator iter = debugcamWindow_array.find(fd_window_name.str());
			if(iter != debugcamWindow_array.end())
			{	delete iter->second;
				debugcamWindow_array.erase(iter);	}

			std::map< std::string, CamWindow* >::iterator iter2 = debugcamWindow_array.find(fd_window_name2.str());
			if(iter2 != debugcamWindow_array.end())
			{	delete iter2->second;
				debugcamWindow_array.erase(iter2);	}
		}
	}
	show_motion = value;
}

PerceptVideo::~PerceptVideo()
{
	boost::mutex::scoped_lock lock(m_mutex);
	for (std::map< int, CvCapture * >::iterator iter = capture_cam_array.begin(); iter!=capture_cam_array.end(); iter++)
	{
		std::stringstream window_name;
		window_name << "Cam" << iter->first << ":";
		cvReleaseCapture(&(iter->second));
	}
	capture_cam_array.erase(capture_cam_array.begin(),capture_cam_array.begin());

	for (std::map< int, CvCapture * >::iterator iter = capture_videofiles_array.begin(); iter!=capture_videofiles_array.end(); iter++)
	{
		std::stringstream window_name;
		window_name << "Cam" << iter->first << ":";
		cvReleaseCapture(&(iter->second));
	}
	capture_videofiles_array.erase(capture_videofiles_array.begin(),capture_videofiles_array.begin());

	//Cam Windows
	for (std::map< std::string, CamWindow* >::iterator iter = camWindow_array.begin(); iter!=camWindow_array.end(); iter++)
		delete iter->second;
	camWindow_array.erase(camWindow_array.begin(),camWindow_array.end());

	//Debug Cam Windows
	for (std::map< std::string, CamWindow* >::iterator iter = debugcamWindow_array.begin(); iter!=debugcamWindow_array.end(); iter++)
		delete iter->second;
	debugcamWindow_array.erase(debugcamWindow_array.begin(),debugcamWindow_array.end());

	//Face detectors
	for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); iter != face_detectors.end(); iter++)
	{
		(*iter)->Delete();
		delete (*iter);
	}
	face_detectors.clear();

	//Motion detectors
	for (std::vector<IMotionDetection*>::iterator iter = motion_detectors.begin(); iter != motion_detectors.end(); iter++)
	{
		(*iter)->Delete();
		delete (*iter);
	}
	motion_detectors.clear();

	//Presence detectors
	for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
	{
		(*iter)->Delete();
		delete (*iter);
	}
	presence_detectors.clear();

	//Calibration data
	for (static std::map< int, CvMat* >::iterator iter = intrinsics.begin(); iter != intrinsics.end(); iter++)
		cvReleaseMat( &(iter->second) );
	intrinsics.clear();
	for (static std::map< int, CvMat* >::iterator iter = distortion.begin(); iter != distortion.end(); iter++)
		cvReleaseMat( &(iter->second) );
	distortion.clear();
	for (static std::map< int, IplImage* >::iterator iter = undistort_mapx.begin(); iter != undistort_mapx.end(); iter++)
		cvReleaseImage( &(iter->second) );
	undistort_mapx.clear();
	for (static std::map< int, IplImage* >::iterator iter = undistort_mapy.begin(); iter != undistort_mapy.end(); iter++)
		cvReleaseImage( &(iter->second) );
	undistort_mapy.clear();

	//Homographies
	for (static std::map< int, CvMat* >::iterator iter = homography.begin(); iter != homography.end(); iter++)
		cvReleaseMat( &(iter->second) );
	homography.clear();

	//Face_recognition
	for (std::vector<core::Image>::iterator iter = face_history.begin(); iter != face_history.end(); iter++)
		free((*iter).image);
	face_history.clear();
	delete face_recognizer;
}

void PerceptVideo::Delete()
{
	stop_requested = true;
    m_thread->join();
	assert(m_thread);
}

void PerceptVideo::Init()
{
	PerceptVideo::DoInit();
}

void PerceptVideo::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		
		//Face detectors
		for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); iter != face_detectors.end(); iter++)
			(*iter)->Init();
		//Motion detectors
		for (std::vector<IMotionDetection*>::iterator iter = motion_detectors.begin(); iter != motion_detectors.end(); iter++)
			(*iter)->Init();
		//Presence detectors
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
			(*iter)->Init();

		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&PerceptVideo::DoMainLoop)));
	}
}

void PerceptVideo::DoMainLoop()
{
	initialized = true;

	while(!stop_requested)
	{
		double timestamp = (double)clock()/CLOCKS_PER_SEC;
		las_time = timestamp;
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}
}

void PerceptVideo::Iterate()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (lock)
	{
		if (!user_dataModel_controller)
			user_dataModel_controller = IUserDataModelController::GetInstance();
		if (!calibrating)
		{
			Capture();
			UpdateFaceHistory();
			ShowDebugWindows();
			SendImages();
		}
		else
			DoCalibrate();
	}
}

void PerceptVideo::SendImages()
{
	for (std::map<int, IplImage *>::iterator iter = capture_img.begin(); iter != capture_img.end(); iter++)
	{
		IplImage *image = iter->second;
		char *data;
		int w, h, channels, depth, step;
		w        = image->width; 
		h        = image->height;
		channels = image->nChannels;
		depth    = image->depth;
		step     = image->widthStep;
		data	 = image->imageData;

		//Face Detectors
		if( (unsigned int) ((iter->first)-1) < face_detectors.size())
			face_detectors[(iter->first)-1]->SetCurrentImage(w, h, channels, depth, step, data );
		//Motion Detectors
		if( (unsigned int) ((iter->first)-1) < motion_detectors.size())
			motion_detectors[(iter->first)-1]->SetCurrentImage(w, h, channels, depth, step, data );
		//Presence Detectors
		if( (unsigned int)((iter->first)-1) < presence_detectors.size())
			presence_detectors[(iter->first)-1]->SetCurrentImage(w, h, channels, depth, step, data );
	}
}

void PerceptVideo::Capture()
{
	double timestamp_Recording_currentframe = (double)clock()/CLOCKS_PER_SEC;
	double deltatime = timestamp_Recording_currentframe - timestamp_Recording_latestframe;
	if((is_recording || is_using_videosource) && ( deltatime > (1.0/capture_fps)))
		timestamp_Recording_latestframe = timestamp_Recording_currentframe;

	for (std::map<int, CvCapture *>::iterator iter = capture_cam_array.begin(); iter!=capture_cam_array.end(); iter++)
	{	
		int index = iter->first;
		std::stringstream window_name;
		window_name << "Cam" << index << ":";
		IplImage *aux_img = capture_img[index];
		
		if (!is_recording && !is_using_videosource)
			capture_img[index] = cvQueryFrame(iter->second);
		else if ( deltatime > (1.0/capture_fps))
		{	//Video recording
			if(is_recording)
			{
				capture_img[index] = cvQueryFrame(iter->second);
				cvWriteFrame(capture_videowriter[index], capture_img[index]);
			} else if (is_using_videosource)
			{
				IplImage *new_videoframe = cvQueryFrame(capture_videofiles_array[iter->first]);
				if (new_videoframe) 
					capture_img[index] = new_videoframe;
				else
					return;
			}
		} else
			return;

		//undistort
		if ( (undistort_mapx[iter->first]) && (undistort_mapy[iter->first]) && (intrinsics[iter->first]) && (distortion[iter->first]))
		{
			IplImage *t = cvCloneImage( capture_img[index] );
			cvRemap( t, capture_img[index], undistort_mapx[iter->first], undistort_mapy[iter->first] ); // undistort image
			cvReleaseImage( &t );
		}

		//reorient
		if ( app_config != NULL )
		{
			bool flip_me_x = false;
			bool flip_me_y = false;

			flip_me_x = app_config->GetCameraData(iter->first).flip_h;
			flip_me_y = app_config->GetCameraData(iter->first).flip_v;

			if(flip_me_x && !flip_me_y) //Flip Horizontally
			{	
				IplImage *aux = capture_img[index];
				cvFlip(capture_img[index], capture_img[index], 1);
			}
			else if(!flip_me_x && flip_me_y) //Flip Vertically
			{	
				IplImage *aux = capture_img[index];
				cvFlip(capture_img[index], capture_img[index], 0);
			}
			else if(flip_me_x && flip_me_y) //Flip Both
			{	
				IplImage *aux = capture_img[index];
				cvFlip(capture_img[index], capture_img[index], -1);
			}

			bool s_show = app_config->IsShownCamCapture();
			if (s_show != show_cam_capture)
					ShowCamCapture(s_show);
			
			s_show = app_config->IsShownHomography();
			if (s_show != show_homography)
					ShowHomography(s_show);

			s_show = app_config->IsShownFaceDetection();
			if (s_show != show_face_detection)
					ShowFaceDetection(s_show);

			s_show = app_config->IsShownForeground();
			if (s_show != show_foreground)
					ShowForeground(s_show);

			s_show = app_config->IsShownMotion();
			if (s_show != show_motion)
					ShowMotion(s_show);
		}
	}
}

void PerceptVideo::ShowDebugWindows()
{
	for (std::map<int, CvCapture *>::iterator iter = capture_cam_array.begin(); iter!=capture_cam_array.end(); iter++)
	{	
		int index = iter->first;
		IplImage * image = capture_img[iter->first];

		//Capture
		if(show_cam_capture)
		{
			std::stringstream window_name;
			window_name << "Cam" << index << ":";
			std::map< std::string, CamWindow* >::iterator cam_iter = camWindow_array.find(window_name.str());
			cam_iter->second->ShowImage(image);
		}

		//Homography
		if(show_homography) 
		{
			if(homography[index])
			{
				IplImage *h_image = cvCloneImage(image);
				cvWarpPerspective(image, h_image, homography[index], (CV_INTER_LINEAR | CV_WARP_INVERSE_MAP | CV_WARP_FILL_OUTLIERS), cvScalarAll(0)/*color to fill in unknown data */ );
				std::stringstream h_window_name;
				h_window_name << "Homography " << index << ":";
				debugcamWindow_array[h_window_name.str()]->ShowImage(h_image);
				cvReleaseImage( &h_image );
			}
		}
	}

	if(show_face_detection) 
	{
		//Face detectors
		int index = 1;
		for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); iter != face_detectors.end(); iter++)
		{
			std::stringstream window_name;
			window_name << "face detector " << index << ":";
			int size_x, size_y, n_channels, depth, width_step;
			char *new_image = (*iter)->GetCopyOfCurrentImage(size_x, size_y, n_channels, depth, width_step);
			if(new_image)
			{
				CvSize size;
				size.width = size_x;
				size.height = size_y;
				IplImage *image = cvCreateImage(size, depth, n_channels);
				image->imageData = new_image;

				debugcamWindow_array[window_name.str()]->ShowImage(image);
				if(image) 
				{	cvReleaseImage(&image);
					delete image;          }
				free(new_image);
			}

			index++;
		}
	}

	if(show_motion) 
	{
		//Motion detectors
		int index = 1;
		for (std::vector<IMotionDetection*>::iterator iter = motion_detectors.begin(); iter != motion_detectors.end(); iter++)
		{
			std::stringstream window_name;
			window_name << "Motion " << index << ":";
			int size_x, size_y, n_channels, depth, width_step;
			char *new_image = (*iter)->GetCopyOfCurrentImage(size_x, size_y, n_channels, depth, width_step);
			if(new_image)
			{
				CvSize size;
				size.width = size_x;
				size.height = size_y;
				IplImage *image = cvCreateImage(size, depth, n_channels);
				image->imageData = new_image;

				debugcamWindow_array[window_name.str()]->ShowImage(image);
				if(image) 
				{	cvReleaseImage(&image);
					delete image;          }
				free(new_image);
			}

			index++;
		}
		//Optical Flow
		index = 1;
		for (std::vector<IMotionDetection*>::iterator iter = motion_detectors.begin(); iter != motion_detectors.end(); iter++)
		{
			std::stringstream window_name;
			window_name << "Motion Optical Flow" << index << ":";
			int size_x, size_y, n_channels, depth, width_step;
			char *new_image = (*iter)->GetCopyOfCurrentImageOpticalFlow(size_x, size_y, n_channels, depth, width_step);
			if(new_image)
			{
				CvSize size;
				size.width = size_x;
				size.height = size_y;
				IplImage *image = cvCreateImage(size, depth, n_channels);
				image->imageData = new_image;

				debugcamWindow_array[window_name.str()]->ShowImage(image);
				if(image) 
				{	cvReleaseImage(&image);
					delete image;          }
				free(new_image);
			}

			index++;
		}
	}

	if(show_foreground) 
	{
		//Motion detectors
		int index = 1;
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		{

			std::stringstream window_name;
			window_name << "Presence detector " << index << ":";
			int size_x, size_y, n_channels, depth, width_step;
			char *new_image = (*iter)->GetCopyOfCurrentImage(size_x, size_y, n_channels, depth, width_step);
			if(new_image)
			{
				CvSize size;
				size.width = size_x;
				size.height = size_y;

				IplImage *zimage = cvCreateImage(size, depth, n_channels);
				zimage->imageData = new_image;

				if (debugcamWindow_array.find(window_name.str()) != debugcamWindow_array.end()) 
					debugcamWindow_array[window_name.str()]->ShowImage(zimage);
				if(zimage) 
				{	cvReleaseImage(&zimage);
					delete zimage;          }
				free(new_image);
			}
			index++;
		}
	}
	if( show_cam_capture || show_homography || show_foreground || show_face_detection || show_motion )
		cvWaitKey(2);
}

char * PerceptVideo::GetCopyOfCurrentImage(const int camera_index, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (lock)
	{
		std::map<int, IplImage *>::iterator iter = capture_img.find(camera_index);
		if (iter != capture_img.end())
		{
			char *source = capture_img[camera_index]->imageData;
			char *copy;
			IplImage *image = capture_img[camera_index];

			size_x     = image->width;
			size_y     = image->height;
			n_channels = image->nChannels;
			depth      = image->depth;
			width_step = image->widthStep;

			copy = (char *)malloc(sizeof(char)*size_x*size_y*n_channels);

			for (int y = 0; y < image->height; y++) {
				for (int x = 0; x < image->width; x++) {
					int first_channel = switch_rb ? 2 : 0;
					int second_channel = switch_rb ? 1 : 1;
					int third_channel  = switch_rb ? 0 : 2;
					((uchar*)(copy + width_step*y))[x*3]   = ((uchar*)(source+width_step*y))[x*3+first_channel];
					((uchar*)(copy + width_step*y))[x*3+1] = ((uchar*)(source+width_step*y))[x*3+second_channel];
					((uchar*)(copy + width_step*y))[x*3+2] = ((uchar*)(source+width_step*y))[x*3+third_channel];
				}
			}
			return copy;
		}
	}
	return NULL;
}

char * PerceptVideo::GetCopyOfCurrentFeature(const std::string &feature, int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	if (feature == "FACE")
	{
		int index = 1;
		for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); iter != face_detectors.end(); iter++)
		{
			char *img = (*iter)->GetCopyOfAreaOfInterest(size_x, size_y, n_channels, depth, width_step, switch_rb);
			if (img) return img;
			index++;
		}
	}
	else if ( feature == "PRESENCE")
	{
		int index = 1;
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		{
			char *img = (*iter)->GetCopyOfCurrentImage(size_x, size_y, n_channels, depth, width_step, switch_rb);
			if (img) return img;
			index++;
		}
	}
	return NULL;
}

void PerceptVideo::Calibrate(const bool &value)
{
	boost::try_mutex::scoped_lock lock(m_mutex);
	if (lock)
	{
		calibrating = value;
	}
}

void PerceptVideo::DoCalibrate()
{
	for (std::map<int, CvCapture *>::iterator iter = capture_cam_array.begin(); iter!=capture_cam_array.end(); iter++)
	{

		std::stringstream calib_window_name;
		calib_window_name      << "Calibration " << iter->first << ":";
		std::map< std::string, CamWindow* > calibrate_window;
		CamWindow *win = new CamWindow(calib_window_name.str());

		const int board_dt = 20;
		int n_boards = 8;
		int board_w = 5;
		int board_h = 8;
		int board_n = board_w * board_h;
		CvSize board_sz = cvSize( board_w, board_h );

		// Allocate Sotrage
		CvMat* image_points			= cvCreateMat( n_boards*board_n, 2, CV_32FC1 );
		CvMat* object_points		= cvCreateMat( n_boards*board_n, 3, CV_32FC1 );
		CvMat* point_counts			= cvCreateMat( n_boards, 1, CV_32SC1 );
		CvMat* intrinsic_matrix		= cvCreateMat( 3, 3, CV_32FC1 );
		CvMat* distortion_coeffs	= cvCreateMat( 5, 1, CV_32FC1 );

		CvPoint2D32f* corners = new CvPoint2D32f[ board_n ];
		int corner_count;
		int successes = 0;
		int step, frame = 0;

		IplImage *image = cvQueryFrame(iter->second);
		IplImage *gray_image = cvCreateImage( cvGetSize( image ), 8, 1 );

		while( successes < n_boards )
		{
			// Skip every board_dt frames to allow user to move chessboard
			if( frame++ % board_dt == 0 )
			{
				int found = cvFindChessboardCorners( image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
				cvCvtColor( image, gray_image, CV_BGR2GRAY );
				cvFindCornerSubPix( gray_image, corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
				cvDrawChessboardCorners( image, board_sz, corners, corner_count, found );

				std::stringstream calib_window_name;
				calib_window_name << "Calibration " << iter->first << ":";
				CvPoint text_p;
				text_p.x = text_p.y = 20;
				CvScalar green = CV_RGB(0,250,0);
				double hscale = 1.0;
				double vscale = 0.8;
				double shear = 0.2;
				int thickness = 1;
				int line_type = 8;
				CvFont font;
				cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,hscale,vscale,shear,thickness,line_type);
				cvPutText(image,"Move the chessboard, ESC to cancel.",text_p,&font,green);
				win->ShowImage(image);

				// If we got a good board, add it to our data
				if( corner_count == board_n )
				{
					step = successes*board_n;
					for( int i=step, j=0; j < board_n; ++i, ++j ){
						CV_MAT_ELEM( *image_points, float, i, 0 ) = corners[j].x;
						CV_MAT_ELEM( *image_points, float, i, 1 ) = corners[j].y;
						CV_MAT_ELEM( *object_points, float, i, 0 ) = j/board_w;
						CV_MAT_ELEM( *object_points, float, i, 1 ) = j%board_w;
						CV_MAT_ELEM( *object_points, float, i, 2 ) = 0.0f;
					}
					CV_MAT_ELEM( *point_counts, int, successes, 0 ) = board_n;
					successes++;
				}
			}

			int c = cvWaitKey( 15 );
			if( c == 27 ) // Handle ESC
			{	calibrating = false;
				return;					}

			image = cvQueryFrame(iter->second); // Get next image
			if ( app_config != NULL )
			{
				bool flip_me_x = false;
				bool flip_me_y = false;
				flip_me_x = app_config->GetCameraData(iter->first).flip_h;
				flip_me_y = app_config->GetCameraData(iter->first).flip_v;
				if(flip_me_x && !flip_me_y)			//Flip Horizontally
					cvFlip(image, image, 1);
				else if(!flip_me_x && flip_me_y)	//Flip Vertically
					cvFlip(image, image, 0);
				else if(flip_me_x && flip_me_y)		//Flip Both
					cvFlip(image, image, -1);
			}
		}

		// Allocate matrices according to how many chessboards found
		CvMat* object_points2 = cvCreateMat( successes*board_n, 3, CV_32FC1 );
		CvMat* image_points2 = cvCreateMat( successes*board_n, 2, CV_32FC1 );
		CvMat* point_counts2 = cvCreateMat( successes, 1, CV_32SC1 );
		
		// Transfer the points into the correct size matrices
		for( int i = 0; i < successes*board_n; ++i ){
			CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0 );
			CV_MAT_ELEM( *image_points2, float, i, 1) = CV_MAT_ELEM( *image_points, float, i, 1 );
			CV_MAT_ELEM( *object_points2, float, i, 0) = CV_MAT_ELEM( *object_points, float, i, 0 );
			CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1 );
			CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2 );
		}

		for( int i=0; i < successes; ++i )
		{
			CV_MAT_ELEM( *point_counts2, int, i, 0 ) = CV_MAT_ELEM( *point_counts, int, i, 0 );
		}
		cvReleaseMat( &object_points );
		cvReleaseMat( &image_points );
		cvReleaseMat( &point_counts );

		// At this point we have all the chessboard corners we need. 
		CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0; //Initiliazie the intrinsic matrix such that 
		CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0; //the two focal lengths have a ratio of 1.0
		
		// Calibrate the camera
		cvCalibrateCamera2( object_points2, image_points2, point_counts2, cvGetSize( image ), intrinsic_matrix, distortion_coeffs, NULL, NULL, CV_CALIB_FIX_ASPECT_RATIO ); 

		// Save the intrinsics and distortions
		intrinsics[iter->first] = intrinsic_matrix;
		distortion[iter->first] = distortion_coeffs;
		std::stringstream intrinsics_filename, distortion_filename;
		if(app_config)
		{	intrinsics_filename << app_config->GetRootDirectory() << "Intrinsics" << iter->first << ".xml";
			distortion_filename << app_config->GetRootDirectory() << "Distortion" << iter->first << ".xml";		}
		else
		{	intrinsics_filename << "./Intrinsics" << iter->first << ".xml";
			distortion_filename << "./Distortion" << iter->first << ".xml";	}
		cvSave( intrinsics_filename.str().c_str(), intrinsic_matrix );
		cvSave( distortion_filename.str().c_str(), distortion_coeffs );

		// Build the undistort map that we will use for all subsequent frames
		undistort_mapx[iter->first] = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
		undistort_mapy[iter->first] = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
		cvInitUndistortMap( intrinsic_matrix, distortion_coeffs, undistort_mapx[iter->first], undistort_mapy[iter->first] );

		while( image )
		{
			//Undistort
			IplImage *t = cvCloneImage( image );
			cvRemap( t, image, undistort_mapx[iter->first], undistort_mapy[iter->first] ); // undistort image
			cvReleaseImage( &t );
			CvPoint text_p;
			text_p.x = text_p.y = 20;
			CvScalar green = CV_RGB(0,250,0);
			double hscale = 1.0;
			double vscale = 0.8;
			double shear = 0.2;
			int thickness = 1;
			int line_type = 8;
			CvFont font;
			cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,hscale,vscale,shear,thickness,line_type);
			cvPutText(image,"ESC to continue.",text_p,&font,green);
			win->ShowImage(image);

			int c = cvWaitKey( 15 );
			if( c == 27 ) //Handle ESC
				break;
			image = cvQueryFrame(iter->second);
			if ( app_config != NULL )
			{
				bool flip_me_x = false;
				bool flip_me_y = false;
				flip_me_x = app_config->GetCameraData(iter->first).flip_h;
				flip_me_y = app_config->GetCameraData(iter->first).flip_v;
				if(flip_me_x && !flip_me_y)			//Flip Horizontally
					cvFlip(image, image, 1);
				else if(!flip_me_x && flip_me_y)	//Flip Vertically
					cvFlip(image, image, 0);
				else if(flip_me_x && flip_me_y)		//Flip Both
					cvFlip(image, image, -1);
			}
		}

		//Debug Cam Windows
		delete win;
		calibrating = false;
	}
}

void PerceptVideo::CalculateHomography()
{
	boost::try_mutex::scoped_lock lock(m_mutex);
	if (lock)
	{ boost::mutex::scoped_lock lock(homography_mutex);
		for (std::map<int, CvCapture *>::iterator iter = capture_cam_array.begin(); iter!=capture_cam_array.end(); iter++)
		{

			std::stringstream calib_window_name, homography_window_name;
			calib_window_name      << "Calibration " << iter->first << ":";
			homography_window_name << "Homography result"  << iter->first << ":";
			CamWindow *win  = new CamWindow(calib_window_name.str());
			CamWindow *winh = new CamWindow(homography_window_name.str());

			const int board_dt = 20;
			int n_boards = 8;
			int board_w = 5;
			int board_h = 8;
			int board_n = board_w * board_h;
			CvSize board_sz = cvSize( board_w, board_h );

			CvMat *H;

			CvPoint2D32f* corners = new CvPoint2D32f[ board_n ];
			int corner_count;
			int successes = 0;
			int frame = 0;

			IplImage *image = cvQueryFrame(iter->second);
			if ( (undistort_mapx[iter->first]) && (undistort_mapy[iter->first]) && (intrinsics[iter->first]) && (distortion[iter->first]))
			{
				IplImage *t = cvCloneImage( image );
				cvRemap( t, image, undistort_mapx[iter->first], undistort_mapy[iter->first] ); // undistort image
				cvReleaseImage( &t );
			}
			IplImage *gray_image = cvCreateImage( cvGetSize( image ), 8, 1 );

			while( successes < n_boards )
			{
				if( frame++ % board_dt == 0 )
				{
					int found = cvFindChessboardCorners( image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
					cvCvtColor( image, gray_image, CV_BGR2GRAY );
					cvFindCornerSubPix( gray_image, corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
					cvDrawChessboardCorners( image, board_sz, corners, corner_count, found );

					std::stringstream calib_window_name;
					calib_window_name << "Calibration " << iter->first << ":";
					
					CvPoint origin, xpoint, ypoint, xypoint;
					origin.x = origin.y = 0;
					xpoint.x = 100; xpoint.y = 0;
					ypoint.x = 0; ypoint.y = 100;
					xypoint.x = xypoint.y = 100;

					CvScalar red = CV_RGB(250,0,0);
					CvScalar green = CV_RGB(0,250,0);
					CvScalar blue = CV_RGB(0,0,250);
					CvScalar puf = CV_RGB(250,250,0);

					cvCircle(image,origin,15,red,3);
					cvCircle(image,xpoint,15,green,3);
					cvCircle(image,ypoint,15,blue,3);
					cvCircle(image,xypoint,15,puf,3);
					
					origin.x = corners[0].x; origin.y = corners[0].y;
					xpoint.x = corners[board_w-1].x; xpoint.y = corners[board_w-1].y;
					ypoint.x = corners[(board_h-1)*board_w].x; ypoint.y = corners[(board_h-1)*board_w].y;
					xypoint.x = corners[(board_h-1)*board_w + board_w-1].x; xypoint.y = corners[(board_h-1)*board_w + board_w-1].y;

					cvCircle(image,origin,20,red,6);
					cvCircle(image,xpoint,20,green,6);
					cvCircle(image,ypoint,20,blue,6);
					cvCircle(image,xypoint,20,puf,6);

					CvPoint text_p;
					text_p.x = text_p.y = 20;
					double hscale = 1.0;
					double vscale = 0.8;
					double shear = 0.2;
					int thickness = 1;
					int line_type = 8;
					CvFont font;
					cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,hscale,vscale,shear,thickness,line_type);
					cvPutText(image,"Finding corners, ESC to cancel.",text_p,&font,green);

					win->ShowImage(image);

					if( corner_count == board_n )
						successes++;
				}

				int c = cvWaitKey( 15 );
				if( c == 27 )// Handle ESC
				{
					calibrating = false;
					return;
				}
				image = cvQueryFrame(iter->second); // Get next image
				if ( (undistort_mapx[iter->first]) && (undistort_mapy[iter->first]) && (intrinsics[iter->first]) && (distortion[iter->first]))
				{
					IplImage *t = cvCloneImage( image );
					cvRemap( t, image, undistort_mapx[iter->first], undistort_mapy[iter->first] ); // undistort image
					cvReleaseImage( &t );
				}
			}

			// FIND THE HOMOGRAPHY, GET THE IMAGE AND OBJECT POINTS:
			CvPoint2D32f objPts[4], imgPts[4];
			float image_width  = cvGetSize( image ).width;
			float image_height = cvGetSize( image ).height;
			float board_width  = 100.0;
			float aspect_ratio = (float)board_h / board_w;
			objPts[0].x = (image_width-board_width)/2; 
			objPts[0].y = (image_height-(board_width*aspect_ratio))/2; 
			objPts[1].x = objPts[0].x + board_width; 
			objPts[1].y = objPts[0].y;
			objPts[2].x = objPts[0].x; 
			objPts[2].y = objPts[0].y+(board_width*aspect_ratio);
			objPts[3].x = objPts[1].x; 
			objPts[3].y = objPts[2].y;

			imgPts[0] = corners[0];
			imgPts[1] = corners[board_w-1];
			imgPts[2] = corners[(board_h-1)*board_w];
			imgPts[3] = corners[(board_h-1)*board_w + board_w-1];

			H = cvCreateMat( 3, 3, CV_32F);
			
			CvMat _pt1, _pt2;
			_pt1 = cvMat(1, 4, CV_32FC2, &objPts[0] );
			_pt2 = cvMat(1, 4, CV_32FC2, &imgPts[0] );
			cvFindHomography(&_pt1, &_pt2, H, CV_RANSAC, 5);
			if(homography[iter->first])
				cvReleaseMat(&homography[iter->first]);
			homography[iter->first] = H;

			std::stringstream homography_filename;
			if(app_config)
				homography_filename << app_config->GetRootDirectory() << "Homography" << iter->first << ".xml";
			else
				homography_filename << "./Homography" << iter->first << ".xml";
			cvSave( homography_filename.str().c_str(), H );

			while( image )
			{
				IplImage *h_image = cvCloneImage(image);
				cvWarpPerspective(h_image, image, homography[iter->first], (CV_INTER_LINEAR | CV_WARP_INVERSE_MAP | CV_WARP_FILL_OUTLIERS), cvScalarAll(0)/*color to fill in unknown data */ );
				winh->ShowImage(image);
				cvReleaseImage( &h_image );

				int c = cvWaitKey( 15 );
				if( c == 27 )			// Handle Esc
					break;
				image = cvQueryFrame(iter->second);
			}

			delete win;
			delete winh;
		}
	}
}

void PerceptVideo::TrainBackground()
{
	int index = 1;
	for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		if(*iter) (*iter)->TrainBackground();
}

void PerceptVideo::GetHeadPosition(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;

	int index = 1;
	if(app_config)
		for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); iter != face_detectors.end(); iter++)
		{
			float width, height;
			width = height = 0;
			std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(index);
			if (iter_cam_size != cam_capture_size.end())
			{	width  = iter_cam_size->second.x;
				height = iter_cam_size->second.y;
			}

			corePoint2D<int> plane_pos;
			(*iter)->GetFaceCenterPos(plane_pos);

			if ((plane_pos.x != 0) && (plane_pos.y != 0))
			{	
				boost::mutex::scoped_lock lock(homography_mutex);

				CvPoint3D32f objPts, imgPts;
				imgPts.x = plane_pos.x;
				imgPts.y = plane_pos.y;
				imgPts.z = 1;

				CvMat pt1, pt2;
				pt1 = cvMat( 3, 1, CV_32F, &objPts );
				pt2 = cvMat( 3, 1, CV_32F, &imgPts );

				if (homography[index])
				{
					cvMatMulAdd(homography[index], &pt2, 0, &pt1);
				}
				
				core::CameraData cam_data = app_config->GetCameraData(index);

				float offset_x = (width > 0)  ? width/2  : 0;
				float offset_y = (height > 0) ? height/2 : 0;
				float pt1_x = ((float*)(pt1.data.ptr + pt1.step*0))[0] - offset_x;
				float pt1_y = ((float*)(pt1.data.ptr + pt1.step*1))[0] - offset_y;
				

				/** \todo Make generic */
				//FRONT
				if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
				{	result.x =  pt1_x; result.z = -1*pt1_y; }
				//BACK
				else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
				{	result.x =  -1*pt1_x; result.z = -1*pt1_y; }
				//RIGHT
				else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y = pt1_x; result.z = -1*pt1_y; }
				//LEFT
				else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y = -1*pt1_x; result.z = -1*pt1_y; }
				//UP
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
				{	result.x =  pt1_x; result.y = pt1_y; }
				//DOWN
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
				{	result.x =  -1*pt1_x; result.y = pt1_y;	}
			}

			index++;
		}
}

void PerceptVideo::GetFeaturePosition(const std::string &feature, corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;

	if (feature == "CENTER OF MASS")
		ObtainCenterOfMass(result);

	return;
}

void PerceptVideo::GetFeaturePositions(const std::string &feature, std::vector<float> &result, int &row_step, const float &scale)
{
	boost::mutex::scoped_lock lock(m_mutex);
	
	if (feature == "PRESENCE VOLUME")
		ObtainPresenceVolume(result, row_step, scale);

	return;
}

void PerceptVideo::GetFeatureWeightedPositions(const std::string &feature, std::map< int, std::vector<corePDU3D<double>> > &result, const float &scale)
{
	//boost::mutex::scoped_lock lock(m_mutex);
	
	if (feature == "PRESENCE VOLUME")
		ObtainPresenceVolumeAsWeightPoints(result, scale);

	return;
}


std::vector<MotionElement> PerceptVideo::GetMotionElements()
{
	std::vector<MotionElement> result;
	result.clear();	

	//a�adir contenedor colisionadores para consultar puntos contra bounding volumnes

	//build up 3D motion blocks
	MatrixTransform_CubeFRONT;
	MatrixTransform_CubeBACK;
	MatrixTransform_CubeLEFT;
	MatrixTransform_CubeRIGHT;
	MatrixTransform_CubeTOP;
	MatrixTransform_CubeBOTTOM;

	InitMatrixTransform();

	for (int unsigned i = 0; i < motion_detectors.size(); i++)
	{
		std::vector<MotionElement> raw_motionelements;

		bool xz, yz, xy, has_x, has_y, has_z;
		xz = yz = xy = has_x = has_y = has_z = false;
		core::CameraData cam_data = app_config->GetCameraData(i);
		PlaneOrientation plane_str;

		/** \todo Make generic */
		if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
		{	xz = has_x = has_z = true; plane_str = FRONT; }
		else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
		{	xz = has_x = has_z = true; plane_str = BACK; }
		else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
		{	yz = has_y = has_z = true; plane_str = RIGHT;  }
		else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
		{	yz = has_y = has_z = true; plane_str = LEFT; }
		else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
		{	xy = has_x = has_y = true; plane_str = UP; }
		else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
		{	xy = has_x = has_y = true; plane_str = DOWN; }

		float width, height, offset_x, offset_y;
		width = height = 0;
		std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(i);
		if (iter_cam_size != cam_capture_size.end())
		{	width  = iter_cam_size->second.x;
			height = iter_cam_size->second.y;
			offset_x = -1 * width/2;
			offset_y = -1 * height/2;
		}

		corePoint3D<double> min, max;
		{	boost::mutex::scoped_lock lock(bbox_mutex);
			min = BoundinBoxMin;
			max = BoundinBoxMax; }

		gmtl::Matrix<float, 4, 4>	MatrixTransform_Translate, MatrixTransform_AxisTransform, MatrixTransform_Composited;
		MatrixTransform_Translate.setState(gmtl::Matrix<float, 4, 4>::FULL); //Identity as default
		MatrixTransform_AxisTransform.setState(gmtl::Matrix<float, 4, 4>::FULL); //Identity as default
		MatrixTransform_Composited.setState(gmtl::Matrix<float, 4, 4>::FULL); //Identity as default
		MatrixTransform_CubeFRONT[0][3] = offset_x;
		MatrixTransform_CubeFRONT[1][3] = offset_y;
		
		switch (plane_str)
		{
			case FRONT :
            { MatrixTransform_AxisTransform = MatrixTransform_CubeFRONT; break; }
			case BACK  :
            { MatrixTransform_AxisTransform = MatrixTransform_CubeBACK;  break; }
			case RIGHT :
            { MatrixTransform_AxisTransform = MatrixTransform_CubeRIGHT; break; } 
			case LEFT  :
            { MatrixTransform_AxisTransform = MatrixTransform_CubeLEFT;  break; } 
			case UP    :
            { MatrixTransform_AxisTransform = MatrixTransform_CubeTOP;   break; }
			case DOWN  :
            { MatrixTransform_AxisTransform = MatrixTransform_CubeBOTTOM;break;  }
		}

		if (motion_detectors[i])
			raw_motionelements = motion_detectors[i]->GetMotionElements();

		MatrixTransform_Composited = MatrixTransform_Translate * MatrixTransform_AxisTransform;

		for (int unsigned i_raw_motion_element = 0; i_raw_motion_element < raw_motionelements.size(); i_raw_motion_element++)
		{
			MotionElement candidate;
			gmtl::Matrix<float, 1, 4>	direction_to_transform, direction_transformed;
			gmtl::Matrix<float, 1, 4>	rectmin_to_transform, rect_min_transformed;
			gmtl::Matrix<float, 1, 4>	rectmax_to_transform, rect_max_transformed;
			
			core::vector3F new_direction;
			core::Rect3F new_rect;

			direction_to_transform[0][0] = raw_motionelements[i_raw_motion_element].direction.x;
			direction_to_transform[0][1] = raw_motionelements[i_raw_motion_element].direction.y;
			direction_to_transform[0][2] = raw_motionelements[i_raw_motion_element].direction.z;

			rectmin_to_transform[0][0] = raw_motionelements[i_raw_motion_element].rect.min[0];
			rectmin_to_transform[0][1] = raw_motionelements[i_raw_motion_element].rect.min[1];
			rectmin_to_transform[0][2] = raw_motionelements[i_raw_motion_element].rect.min[2];

			rectmax_to_transform[0][0] = raw_motionelements[i_raw_motion_element].rect.max[0];
			rectmax_to_transform[0][1] = raw_motionelements[i_raw_motion_element].rect.max[1];
			rectmax_to_transform[0][2] = raw_motionelements[i_raw_motion_element].rect.max[2];

			direction_transformed = direction_to_transform * MatrixTransform_Composited;
			rect_min_transformed = rectmin_to_transform * MatrixTransform_Composited;
			rect_max_transformed = rectmax_to_transform * MatrixTransform_Composited;

			new_direction.x = direction_transformed[0][0];
			new_direction.y = direction_transformed[0][1];
			new_direction.z = direction_transformed[0][2];

			new_rect.min[0] = rect_min_transformed[0][0];
			new_rect.min[1] = rect_min_transformed[0][1];
			new_rect.min[2] = rect_min_transformed[0][2];
			
			new_rect.max[0] = rect_max_transformed[0][0];
			new_rect.max[1] = rect_max_transformed[0][1];
			new_rect.max[2] = rect_max_transformed[0][2];

			candidate.magnitude = raw_motionelements[i_raw_motion_element].magnitude;
			candidate.direction = new_direction;
			candidate.rect = new_rect;

			result.push_back(candidate);
		}



	}

	return result;
}

bool PerceptVideo::PresenceDetected()
{
	bool result = false;

	int index = 1;
	if(app_config)
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
			result = result || (*iter)->PresenceDetected();

	presence_detected = result;

	return result;
}

bool PerceptVideo::FaceDetected()
{
	boost::mutex::scoped_lock lock(m_mutex);
	bool result = false;

	int index = 1;
	if(app_config)
		for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); iter != face_detectors.end(); iter++)
			result = result || (*iter)->FaceDetected();

	return result;
}

void PerceptVideo::ObtainPresenceVolumeAsWeightPoints(std::map< int, std::vector<corePDU3D<double>> > &weighted_points, const float &scale)
{
		int index = 1;
		IplImage *img_xz, *img_yz, *img_xy;
		std::map< PlaneOrientation, IplImage * > presence_images;
		std::map< PlaneOrientation, IMotionDetection * > mapped_motion_detectors;
		img_xz = img_yz = img_xy = NULL;
		float has_x, has_y, has_z;
		has_x = has_y = has_z = false;


		//BLOCK 1
		//----------------------------
		//scale image, classify coordinate axis
		//For each Presencen Detector, obtains a scaled image
		double b1_timestamp = (double)clock()/CLOCKS_PER_SEC;
		for (int iter = 0; iter < presence_detectors.size() && !(has_x && has_y && has_z); iter++)
		{
			if (!(presence_detectors[iter]->PresenceDetected())) 
			{	index++;
				continue;	}

			bool xz, yz, xy;
			xz = yz = xy = false;
			core::CameraData cam_data = app_config->GetCameraData(index);
			float has_x, has_y, has_z;
			PlaneOrientation plane_str;

			/** \todo fix this. make general */
			if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
			{	xz = has_x = has_z = true; plane_str = FRONT; }
			else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
			{	xz = has_x = has_z = true; plane_str = BACK; }
			else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
			{	yz = has_y = has_z = true; plane_str = RIGHT;  }
			else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
			{	yz = has_y = has_z = true; plane_str = LEFT; }
			else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
			{	xy = has_x = has_y = true; plane_str = UP; }
			else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
			{	xy = has_x = has_y = true; plane_str = DOWN; }

			int size_x, size_y, n_channels, depth, width_step;
			char *new_image = presence_detectors[iter]->GetCopyOfCurrentImage(size_x, size_y, n_channels, depth, width_step);
			if(new_image)
			{
				CvSize size, desired_size;
				size.width = size_x;
				size.height = size_y;
				desired_size.width = size_x*scale;
				desired_size.height = size_y*scale;

				IplImage *zimage = cvCreateImage(size, depth, n_channels);
				char *idata = zimage->imageData;
				for (int y = 0; y < zimage->height; y++) 
					for (int x = 0; x < zimage->width; x++) 
						((uchar*)(idata + width_step*y))[x]   = ((uchar*)(new_image+width_step*y))[x];

				IplImage *scaled = cvCreateImage(desired_size, depth, n_channels);
				cvResize(zimage, scaled, CV_INTER_AREA);

				std::map< PlaneOrientation, IplImage * >::iterator iter_pi = presence_images.find(plane_str);
				if (iter_pi  != presence_images.end())
					cvReleaseImage(&(iter_pi->second));

				presence_images[plane_str] = scaled; 
				mapped_motion_detectors[plane_str] = motion_detectors[iter];

				if(zimage) 
				{	cvReleaseImage(&zimage);
					delete zimage;          
				}
				free(new_image); 
			}
			index++;
		}
		double b1_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
		double b1_dif_time = b1_timestamp2 - b1_timestamp;

		//BLOCK 2
		//----------------------------
		//Coords transform and obtain full cloud 
		//set up 3D weighted-points and space-index
		double b2_timestamp = (double)clock()/CLOCKS_PER_SEC;
		RTree<int, float, 3, float> spatial_index; // <datatype, elementtype, dimensions, elementtype real>
		std::map< int, corePDU3D<double> > image_space_points;			// <id, point>
		std::map< int, corePDU3D<double> > relative_points;				// <id, point>
		std::vector<int> indexes;
		float relative_factor = 1.0;
		int point_id = 0;
		float delta = 1.0;
		bool is3D = false;
		if (presence_images.size() > 0)
		{
			std::map< PlaneOrientation, IplImage * >::iterator iter = presence_images.begin();
			std::map< PlaneOrientation, IplImage * >::iterator iter2 = presence_images.begin();
			iter2++;
			
			char *scaled_img = iter->second->imageData;
			int size_x, size_y, width_step;
			size_x = iter->second->width;
			size_y = iter->second->height;
			width_step = iter->second->widthStep;
			relative_factor = (size_x > size_y) ? size_x : size_y;

			//center of image == center of space (cam calibration)
			float offset_x, offset_y;
			offset_x = offset_y = 0;
			offset_x = (float)size_x/2;
			offset_y = (float)size_y/2;

			//2D image coords to 3D
			if (scaled_img)
			{	for (int y = 0; y < size_y; y++) {
				for (int x = 0; x < size_x; x++) {
					if ( ((((uchar*)(scaled_img+width_step*y))[x])  == 0xff) ) //WHITE
					{	float x_candidate, y_candidate, z_candidate;
						corePoint3D<float> vel_candidate;
						corePoint2D<int> image_coords;
						x_candidate = y_candidate = z_candidate = 0.0;
						vel_candidate.x = vel_candidate.y = vel_candidate.z = 0.0;
						image_coords.x = (int)x/scale;
						image_coords.y = (int)y/scale;
						
						switch (iter->first)
						{
							case FRONT :
							{	 x_candidate = (offset_x + x);
								 z_candidate = (offset_y + -1*y);
								 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter->first);
								 if (iter_motionDetector != mapped_motion_detectors.end())
								 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
									 vel_candidate.x = motioncoords.x;
									 vel_candidate.z = -1*motioncoords.y;
								 } break;								}
							case BACK :
							{	 x_candidate = (offset_x + -1*x);
								 z_candidate = (offset_y + -1*y);	
								 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter->first);
								 if (iter_motionDetector != mapped_motion_detectors.end())
								 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
									 vel_candidate.x = -1*motioncoords.x;
									 vel_candidate.z = -1*motioncoords.y;
								 } break;								}
							case RIGHT :
							{	 y_candidate = (offset_x + x);
								 z_candidate = (offset_y + -1*y);	
								 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter->first);
								 if (iter_motionDetector != mapped_motion_detectors.end())
								 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
									 vel_candidate.y = motioncoords.x;
									 vel_candidate.z = -1*motioncoords.y;
								 } break;								}
							case LEFT :
							{	 y_candidate = (offset_x + -1*x);
								 z_candidate = (offset_y + -1*y);	
								 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter->first);
								 if (iter_motionDetector != mapped_motion_detectors.end())
								 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
									 vel_candidate.y = -1*motioncoords.x;
									 vel_candidate.z = -1*motioncoords.y;
								 } break;								}
							case UP :
							{	 x_candidate = (offset_x + -1*x);
								 y_candidate = (offset_y + -1*y);	
								 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter->first);
								 if (iter_motionDetector != mapped_motion_detectors.end())
								 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
									 vel_candidate.x = -1*motioncoords.x;
									 vel_candidate.y = -1*motioncoords.y;
								 } break;								}
							case DOWN : 
							{	 x_candidate = (offset_x + -1*x);
								 y_candidate = (offset_y + -1*y);	
								 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter->first);
								 if (iter_motionDetector != mapped_motion_detectors.end())
								 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
									 vel_candidate.x = -1*motioncoords.x;
									 vel_candidate.y = -1*motioncoords.y;
								 } break;								}
						}

						if (presence_images.size() > 1)
						{	int size_x2, size_y2;
							size_x2 = iter2->second->width;
							size_y2 = iter2->second->height;
							int offset_x2, offset_y2;
							offset_x2 = offset_y2 = 0;
							offset_x2 = (float)size_x2/2;
							offset_y2 = (float)size_y2/2;
							char *scaled_img2 = iter2->second->imageData;
							relative_factor = (relative_factor > size_x2) ? relative_factor : size_x2;
							relative_factor = (relative_factor > size_y2) ? relative_factor : size_y2;

							//match data
							bool complementary = true;
							int first_init, second_init, first_array_size, second_array_size;
							first_init = second_init = 0;
							first_array_size = size_x2; 
							second_array_size = size_y2;
							//iter FRONT|BACK
							if ((iter->first == FRONT) || (iter->first == BACK))
							{	if ((iter2->first == LEFT) || (iter2->first == RIGHT))
								{	first_array_size = size_x2; second_array_size = y+1; second_init = y;	}
								else if ((iter2->first == UP) || (iter2->first == DOWN))
								{	first_array_size = x+1; second_array_size= size_y2;	first_init = x;		}
								else 
									complementary = false; }
							//iter RIGHT|LEFT
							else if ((iter->first == LEFT) || (iter->first == RIGHT))
							{	if ((iter2->first == FRONT) || (iter2->first == BACK))
								{	first_array_size = size_x2; second_array_size = y+1; second_init = y;	}
								else if ((iter2->first == UP) || (iter2->first == DOWN))
								{	first_array_size = size_x2; second_array_size = x+1; second_init = x;	}
								else 
									complementary = false; }
							//iter UP|DOWN
							else if ((iter->first == UP) || (iter->first == DOWN))
							{	if ((iter2->first == FRONT) || (iter->first == BACK))
								{	first_array_size = x+1; second_array_size = size_y2; first_init = x;	}
								else if ((iter2->first == LEFT) || (iter->first == RIGHT))
								{	first_array_size = y+1; second_array_size = size_y2; first_init = y;	}
								else 
									complementary = false; }

				
							if (complementary && (scaled_img2 != NULL))
							{	is3D = true;
								for (int y2 = second_init; (y2 < second_array_size) && (y2 < size_y2); y2++) {
								for (int x2 = first_init;  (x2 < first_array_size)  && (x2 < size_x2); x2++) {
									if ( ((((uchar*)(scaled_img2+width_step*y2))[x2])  == 0xff) )
									{
										switch (iter2->first)
										{
											case FRONT :
											{	 x_candidate = (offset_x + x);
												 z_candidate = (offset_y + -1*y);
												 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter2->first);
												 if (iter_motionDetector != mapped_motion_detectors.end())
												 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
													 vel_candidate.x = motioncoords.x;
													 vel_candidate.z = -1*motioncoords.y;
												 } break;								}
											case BACK :
											{	 x_candidate = (offset_x + -1*x);
												 z_candidate = (offset_y + -1*y);	
												 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter2->first);
												 if (iter_motionDetector != mapped_motion_detectors.end())
												 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
													 vel_candidate.x = -1*motioncoords.x;
													 vel_candidate.z = -1*motioncoords.y;
												 } break;								}
											case RIGHT :
											{	 y_candidate = (offset_x + x);
												 z_candidate = (offset_y + -1*y);	
												 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter2->first);
												 if (iter_motionDetector != mapped_motion_detectors.end())
												 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
													 vel_candidate.y = motioncoords.x;
													 vel_candidate.z = -1*motioncoords.y;
												 } break;								}
											case LEFT :
											{	 y_candidate = (offset_x + -1*x);
												 z_candidate = (offset_y + -1*y);	
												 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter2->first);
												 if (iter_motionDetector != mapped_motion_detectors.end())
												 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
													 vel_candidate.y = -1*motioncoords.x;
													 vel_candidate.z = -1*motioncoords.y;
												 } break;								}
											case UP :
											{	 x_candidate = (offset_x + -1*x);
												 y_candidate = (offset_y + -1*y);	
												 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter2->first);
												 if (iter_motionDetector != mapped_motion_detectors.end())
												 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
													 vel_candidate.x = -1*motioncoords.x;
													 vel_candidate.y = -1*motioncoords.y;
												 } break;								}
											case DOWN : 
											{	 x_candidate = (offset_x + -1*x);
												 y_candidate = (offset_y + -1*y);	
												 std::map< PlaneOrientation, IMotionDetection * >::iterator iter_motionDetector = mapped_motion_detectors.find(iter2->first);
												 if (iter_motionDetector != mapped_motion_detectors.end())
												 {	 corePoint3D<float> motioncoords = iter_motionDetector->second->GetMotionAtCoords(image_coords); 
													 vel_candidate.x = -1*motioncoords.x;
													 vel_candidate.y = -1*motioncoords.y;
												 } break;								}
										}
										
										corePDU3D<double> new_point, relative_point;
										new_point.position.x = x_candidate;
										new_point.position.y = y_candidate;
										new_point.position.z = z_candidate;
										new_point.velocity.x = vel_candidate.x;
										new_point.velocity.y = vel_candidate.y;
										new_point.velocity.z = vel_candidate.z;
										new_point.acceleration.x = new_point.acceleration.y = new_point.acceleration.z = 0.0;
										relative_point = new_point;


										relative_point.position.x = (1.0/scale)*(x_candidate+(relative_factor/2))/relative_factor;
										relative_point.position.y = (1.0/scale)*(y_candidate+(relative_factor/2))/relative_factor;
										relative_point.position.z = (1.0/scale)*(z_candidate+(relative_factor/2))/relative_factor;

										corePDU3D<double> var_1;
										corePDU3D<double> var_2;
										var_1 = var_2;

										int i = 666;

										image_space_points[point_id] = new_point;
										relative_points[point_id] = relative_point;
										indexes.push_back(point_id);

										float envelope = 0;//(float)delta/2;
										Rect3F point_rect(new_point.position.x-envelope,new_point.position.y-envelope,new_point.position.z-envelope,
														  new_point.position.x+envelope,new_point.position.y+envelope,new_point.position.z+envelope);
										spatial_index.Insert(point_rect.min, point_rect.max, point_id);

										point_id++;
									}
								}}
							}
						}
						else
						{
							corePDU3D<double> new_point, relative_point;
							new_point.position.x = x_candidate;
							new_point.position.y = y_candidate;
							new_point.position.z = z_candidate;
							new_point.velocity.x = vel_candidate.x;
							new_point.velocity.y = vel_candidate.y;
							new_point.velocity.z = vel_candidate.z;
							new_point.acceleration.x = new_point.acceleration.y = new_point.acceleration.z = 0.0;
							relative_point = new_point;

							relative_point.position.x = (1.0/scale)*(x_candidate+(relative_factor/2))/relative_factor;
							relative_point.position.y = (1.0/scale)*(y_candidate+(relative_factor/2))/relative_factor;
							relative_point.position.z = (1.0/scale)*(z_candidate+(relative_factor/2))/relative_factor;

							image_space_points[point_id] = new_point;
							relative_points[point_id] = relative_point;
							indexes.push_back(point_id);

							float envelope = 0;//(float)delta/2;
							Rect3F point_rect(new_point.position.x-envelope,new_point.position.y-envelope,new_point.position.z-envelope,
								              new_point.position.x+envelope,new_point.position.y+envelope,new_point.position.z+envelope);
							spatial_index.Insert(point_rect.min, point_rect.max, point_id);

							point_id++;
						}
					}
				}}
			}
		}
		double b2_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
		double b2_dif_time = b2_timestamp2 - b2_timestamp;

		//BLOCK 3
		//----------------------------
		//Simplifying cloud and adding weighted point a velocity
		//clusterize points, assign weight, simplfy point cloud by neightbourhood size
		double b3_timestamp = (double)clock()/CLOCKS_PER_SEC;
		int size_step = 1;
		int n_steps = 50; //retomar
		float third_delta = 1.0;
		float step_factor = n_steps * size_step;
		std::map< int, corePDU3D<double> >::iterator iter_isp = image_space_points.begin();

		while ( (iter_isp != image_space_points.end()) && !(image_space_points.empty()))
		{	iter_isp = image_space_points.begin();
			int random_i = ( rand() % image_space_points.size() );
			std::advance( iter_isp, random_i );
			int iter_isp_id = iter_isp->first;
			corePDU3D<double> new_point;
			new_point = iter_isp->second;

			for (int i = n_steps; i >= 0; )
			{	float search_delta = 1 + delta * i * size_step; //retomar
				float search_delta_window = (float)search_delta/2;
				bool candidate_step_found = false;

				Rect3F search_rect(new_point.position.x-search_delta_window,new_point.position.y-search_delta_window,new_point.position.z-search_delta_window, new_point.position.x+search_delta_window,new_point.position.y+search_delta_window,new_point.position.z+search_delta_window);
				search_results.clear();
				int overlapping_size = spatial_index.Search(search_rect.min, search_rect.max, RegisterPointIDIntoSearchResults, NULL);

				float n_deltas = search_delta/delta;
				third_delta = (is3D) ? n_deltas : 1.0;
				float success_criteria = 0.95; // CRITERIO DE �XITO, Encontr� una muestra v�lida de tama�o search_delta //retomar
				if ((overlapping_size > ( success_criteria * pow(n_deltas, ((is3D) ? 3 : 2)) ) ) &&// n_deltas * n_deltas * third_delta))
					(overlapping_size > 1) ) // n_deltas * n_deltas * third_delta))
				{
					new_point = relative_points[iter_isp->first];

					if (weighted_points.find(search_delta) != weighted_points.end())
						weighted_points[search_delta].push_back(new_point);
					else
					{	std::vector<corePDU3D<double>> new_vector;
						new_vector.push_back(new_point);
						weighted_points[search_delta] = new_vector;
					}

					for (std::vector<int>::iterator diter = search_results.begin(); diter != search_results.end(); diter++)
					{	spatial_index.Remove(search_rect.min, search_rect.max, *diter);
						image_space_points.erase(*diter);
					}
					candidate_step_found = true;
					i--;
				}
				else if (( i==0 ) || overlapping_size == 1 ) //remove if every test failed (noise)
				{	spatial_index.Remove(search_rect.min, search_rect.max, iter_isp_id);
					image_space_points.erase(iter_isp_id); 
					i--; }
				
				float candidate_step = pow(overlapping_size, (is3D) ? 1.0/3.0 : 1.0/2.0);
				candidate_step*=delta;
				candidate_step--;
				candidate_step/= delta*size_step;
				i = candidate_step;
			}
		}
		double b3_timestamp2 = (double)clock()/CLOCKS_PER_SEC;
		double b3_dif_time = b3_timestamp2 - b3_timestamp;

		for (std::map< PlaneOrientation, IplImage * >::iterator iter = presence_images.begin(); iter != presence_images.end(); iter++)
			cvReleaseImage(&(iter->second));
		presence_images.clear();

		spatial_index.RemoveAll();
}


void PerceptVideo::ObtainPresenceVolume(std::vector<float> &result, int &row_step, const float &scale)
{
		int index = 1;
		IplImage *img_xz, *img_yz, *img_xy;
		std::map< std::string, IplImage * > presence_images;
		img_xz = img_yz = img_xy = NULL;
		float has_x, has_y, has_z;
		has_x = has_y = has_z = false;
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); (iter != presence_detectors.end()) && !(has_x && has_y && has_z); iter++)
		{
			bool xz, yz, xy;
			xz = yz = xy = false;
			core::CameraData cam_data = app_config->GetCameraData(index);
			float has_x, has_y, has_z;
			std::string plane_str = "";
			/** \todo Make generic */
			//FRONT
			if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
			{	xz = has_x = has_z = true; plane_str = "FRONT"; }
			//BACK
			else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
			{	xz = has_x = has_z = true; plane_str = "BACK"; }
			//RIGHT
			else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
			{	yz = has_y = has_z = true; plane_str = "RIGHT";  }
			//LEFT
			else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
			{	yz = has_y = has_z = true; plane_str = "LEFT"; }
			//UP
			else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
			{	xy = has_x = has_y = true; plane_str = "UP"; }
			//DOWN
			else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
			{	xy = has_x = has_y = true; plane_str = "DOWN"; }


			int size_x, size_y, n_channels, depth, width_step;
			char *new_image = (*iter)->GetCopyOfCurrentImage(size_x, size_y, n_channels, depth, width_step);
			if(new_image)
			{
				CvSize size, desired_size;
				size.width = size_x;
				size.height = size_y;
				desired_size.width = size_x*scale;
				desired_size.height = size_y*scale;

				IplImage *zimage = cvCreateImage(size, depth, n_channels);
				char *idata = zimage->imageData;
				for (int y = 0; y < zimage->height; y++) {
					for (int x = 0; x < zimage->width; x++) {
						((uchar*)(idata + width_step*y))[x]   = ((uchar*)(new_image+width_step*y))[x];
					}
				}

				IplImage *scaled = cvCreateImage(desired_size, depth, n_channels);
				cvResize(zimage, scaled, CV_INTER_AREA);
				presence_images[plane_str] = scaled;

				if(zimage) 
				{	cvReleaseImage(&zimage);
					delete zimage;          }
				free(new_image);
			}
			index++;
		}

		if (presence_images.size() > 0)
		{
			std::map< std::string, IplImage * >::iterator iter = presence_images.begin();
			std::map< std::string, IplImage * >::iterator iter2 = presence_images.begin()++;
			char *scaled_img = iter->second->imageData;
			int size_x, size_y, width_step;
			size_x = iter->second->width;
			size_y = iter->second->height;
			width_step = iter->second->widthStep;

			int offset_x, offset_y;
			offset_x = offset_y = 0;
			offset_x = size_x;
			offset_y = size_y;
			float factor = 2.5;

			if (scaled_img)
			{	for (int y = 0; y < size_y; y++) {
				for (int x = 0; x < size_x; x++) {
					if ( ((((uchar*)(scaled_img+width_step*y))[x])  == 0xff) )
					{	float x_candidate, y_candidate, z_candidate;
						x_candidate = y_candidate = z_candidate = 0.0;
						if (iter->first == "FRONT")
						{	 x_candidate = (offset_x + (float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (iter->first == "BACK")
						{	 x_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (iter->first == "RIGHT")
						{	 y_candidate = (offset_x + (float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (iter->first == "LEFT")
						{	 y_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (iter->first == "UP")
						{	 x_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 y_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (iter->first == "DOWN")
						{	 x_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 y_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}

						if (presence_images.size() > 1)
						{	int size_x2, size_y2;
							size_x2 = iter2->second->width;
							size_y2 = iter2->second->height;
							int offset_x2, offset_y2;
							offset_x2 = offset_y2 = 0;
							offset_x2 = size_x2;
							offset_y2 = size_y2;
							char *scaled_img2 = iter2->second->imageData;
							if (scaled_img2)
							{	for (int y2 = 0; y2 < size_y2; y2++) {
								for (int x2 = 0; x2 < size_x2; x2++) {
									if ( ((((uchar*)(scaled_img2+width_step*y2))[x2])  == 0xff) )
									{
										if (iter2->first == "FRONT")
										{	 x_candidate = (offset_x2 + (float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (iter2->first == "BACK")
										{	 x_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (iter2->first == "RIGHT")
										{	 y_candidate = (offset_x2 + (float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (iter2->first == "LEFT")
										{	 y_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (iter2->first == "UP")
										{	 x_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 y_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (iter2->first == "DOWN")
										{	 x_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 y_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}

										result.push_back(x_candidate);
										result.push_back(y_candidate);
										result.push_back(z_candidate);
									}
								}}
							}
						}
						else
						{
							result.push_back(x_candidate);
							result.push_back(y_candidate);
							result.push_back(z_candidate);
						}
					row_step = 3;
					}
				}}
			}
		}

		for (std::map< std::string, IplImage * >::iterator iter = presence_images.begin(); iter != presence_images.end(); iter++)
			cvReleaseImage(&(iter->second));
		presence_images.clear();
}

void PerceptVideo::ObtainCenterOfMass(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;

	int index = 1;
	if(app_config)
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		{
			float width, height;
			width = height = 0;
			std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(index);
			if (iter_cam_size != cam_capture_size.end())
			{
				width  = iter_cam_size->second.x;
				height = iter_cam_size->second.y;
			}

			corePoint2D<int> plane_pos;
			(*iter)->GetPresenceCenterOfMass(plane_pos);

			if ((plane_pos.x != 0) && (plane_pos.y != 0))
			{
				boost::mutex::scoped_lock lock(homography_mutex);

				CvPoint3D32f objPts, imgPts;
				imgPts.x = plane_pos.x;
				imgPts.y = plane_pos.y;
				imgPts.z = 1;

				CvMat pt1, pt2;
				pt1 = cvMat( 3, 1, CV_32F, &objPts );
				pt2 = cvMat( 3, 1, CV_32F, &imgPts );

				if (homography[index])
					cvMatMulAdd(homography[index], &pt2, 0, &pt1);
				
				core::CameraData cam_data = app_config->GetCameraData(index);

				float offset_x = (width > 0)  ? width/2  : 0;
				float offset_y = (height > 0) ? height/2 : 0;
				float pt1_x = ((float*)(pt1.data.ptr + pt1.step*0))[0] - offset_x;
				float pt1_y = ((float*)(pt1.data.ptr + pt1.step*1))[0] - offset_y;
				
				/** \todo Make generic */
				//FRONT
				if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
				{	result.x =  pt1_x; result.z = -1*pt1_y; }
				//BACK
				else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
				{	result.x =  -1*pt1_x; result.z = -1*pt1_y; }
				//RIGHT
				else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y = pt1_x; result.z = -1*pt1_y; }
				//LEFT
				else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y = -1*pt1_x; result.z = -1*pt1_y; }
				//UP
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
				{	result.x =  pt1_x; result.y = pt1_y; }
				//DOWN
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
				{	result.x =  -1*pt1_x; result.y = pt1_y;	}
			}

			index++;
		}
}

void PerceptVideo::GetMainLateralDominance(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;

	int index = 1;
	if(app_config)
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		{
			float width, height;
			width = height = 0;
			std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(index);
			if (iter_cam_size != cam_capture_size.end())
			{
				width  = iter_cam_size->second.x;
				height = iter_cam_size->second.y;
			}

			double data = 0.0;
			(*iter)->GetPresenceArea(data);

			{	boost::mutex::scoped_lock lock(homography_mutex);

				int counter_x, counter_y, counter_z;
				counter_x = counter_y = counter_z = 0;
				core::CameraData cam_data = app_config->GetCameraData(index);
				
				/** \todo Make generic */
				//FRONT
				if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
				{	result.x +=  data*0.5; result.z += data*0.5;
				    counter_x++; counter_z++;                  }
				//BACK
				else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
				{	result.x +=  data*0.5; result.z += data*0.5; 
				    counter_x++; counter_z++;                  }
				//RIGHT
				else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y += data*0.5; result.z += data*0.5; 
				    counter_y++; counter_z++;                  }
				//LEFT
				else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y += data*0.5; result.z += data*0.5; 
				    counter_y++; counter_z++;                  }
				//UP
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
				{	result.x +=  data*0.5; result.y += data*0.5; 
				    counter_x++; counter_y++;                  }
				//DOWN
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
				{	result.x +=  data*0.5; result.y += data*0.5;	
				    counter_x++; counter_y++;                  }

				result.x /= (counter_x > 0.0) ? counter_x : 1.0;
				result.y /= (counter_y > 0.0) ? counter_y : 1.0;
				result.z /= (counter_z > 0.0) ? counter_z : 1.0;
			}

			index++;
		}
}

void PerceptVideo::GetMainOrientation(corePoint3D<double> &result)
{
	result.x = result.y = result.z = 0;

	int index = 1;
	if(app_config)
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		{
			float width, height;
			width = height = 0;
			std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(index);
			if (iter_cam_size != cam_capture_size.end())
			{
				width  = iter_cam_size->second.x;
				height = iter_cam_size->second.y;
			}

			double data = 0.0;
			(*iter)->GetPresenceOrientation(data);

			{	boost::mutex::scoped_lock lock(homography_mutex);

				int counter_x, counter_y, counter_z;
				counter_x = counter_y = counter_z = 0;
				core::CameraData cam_data = app_config->GetCameraData(index);

				/** \todo Make generic */
				//FRONT
				if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
				{	result.x +=  sin(data); result.z += cos(data);
				    counter_x++; counter_z++;                  }
				//BACK
				else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
				{	result.x +=  sin(data); result.z += cos(data); 
				    counter_x++; counter_z++;                  }
				//RIGHT
				else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y += sin(data); result.z += cos(data); 
				    counter_y++; counter_z++;                  }
				//LEFT
				else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	result.y += sin(data); result.z += cos(data); 
				    counter_y++; counter_z++;                  }
				//UP
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
				{	result.x +=  sin(data); result.y += cos(data); 
				    counter_x++; counter_y++;                  }
				//DOWN
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
				{	result.x +=  sin(data); result.y += cos(data);	
				    counter_x++; counter_y++;                  }

				result.x /= (counter_x > 0.0) ? counter_x : 1.0;
				result.y /= (counter_y > 0.0) ? counter_y : 1.0;
				result.z /= (counter_z > 0.0) ? counter_z : 1.0;
			}

			index++;
		}
}

void PerceptVideo::GetMainEccentricity(corePoint3D<double> &zero_means_round)
{
	zero_means_round.x = zero_means_round.y = zero_means_round.z = 0;

	int index = 1;
	if(app_config)
		for (std::vector<IPresenceDetection*>::iterator iter = presence_detectors.begin(); iter != presence_detectors.end(); iter++)
		{
			float width, height;
			width = height = 0;
			std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(index);
			if (iter_cam_size != cam_capture_size.end())
			{
				width  = iter_cam_size->second.x;
				height = iter_cam_size->second.y;
			}

			double data = 0.0;
			(*iter)->GetPresenceEccentricity(data);

			{	boost::mutex::scoped_lock lock(homography_mutex);

				int counter_x, counter_y, counter_z;
				counter_x = counter_y = counter_z = 0;
				core::CameraData cam_data = app_config->GetCameraData(index);
				
				/** \todo Make generic */
				//FRONT
				if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
				{	zero_means_round.x +=  data; zero_means_round.z += data;
				    counter_x++; counter_z++;                  }
				//BACK
				else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
				{	zero_means_round.x +=  data; zero_means_round.z += data; 
				    counter_x++; counter_z++;                  }
				//RIGHT
				else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	zero_means_round.y += data; zero_means_round.z += data; 
				    counter_y++; counter_z++;                  }
				//LEFT
				else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	zero_means_round.y += data; zero_means_round.z += data; 
				    counter_y++; counter_z++;                  }
				//UP
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
				{	zero_means_round.x +=  data; zero_means_round.y += data; 
				    counter_x++; counter_y++;                  }
				//DOWN
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
				{	zero_means_round.x +=  data; zero_means_round.y += data;	
				    counter_x++; counter_y++;                  }

				zero_means_round.x /= (counter_x > 0.0) ? counter_x : 1.0;
				zero_means_round.y /= (counter_y > 0.0) ? counter_y : 1.0;
				zero_means_round.z /= (counter_z > 0.0) ? counter_z : 1.0;
			}

			index++;
		}
}

void PerceptVideo::GetSpaceBoundingBox(corePoint3D<double> &min, corePoint3D<double> &max, const bool &recalculate)
{
	min.x = min.y = min.z = 0;
	max.x = max.y = max.z = 0;

	if (recalculate)
	{
		int index = 1;
		if(app_config)
			for (unsigned int i = 1; i <= num_cams; i++)
			{
				float width, height;
				width = height = 0;
				std::map<int, vector2I>::iterator iter_cam_size = cam_capture_size.find(i);
				if (iter_cam_size != cam_capture_size.end())
				{
					width  = iter_cam_size->second.x;
					height = iter_cam_size->second.y;
				}

				corePoint2D<int> min_2dcoord, max_2dcoord;
				min_2dcoord.x = min_2dcoord.y = 0;
				max_2dcoord.x = width; max_2dcoord.y = height;
				TransformCoordinates(min_2dcoord, index);
				TransformCoordinates(max_2dcoord, index);

				core::CameraData cam_data = app_config->GetCameraData(index);

				/** \todo Make generic */
				//FRONT
				if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
				{	min.x =  min_2dcoord.x; min.z = -1*min_2dcoord.y; 
					max.x =  max_2dcoord.x; max.z = -1*max_2dcoord.y; }
				//BACK
				else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
				{	min.x =  -1*min_2dcoord.x; min.z = -1*min_2dcoord.y; 
					max.x =  -1*max_2dcoord.x; max.z = -1*max_2dcoord.y; }
				//RIGHT
				else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	min.y = min_2dcoord.x; min.z = -1*min_2dcoord.y; 
					max.y = max_2dcoord.x; max.z = -1*max_2dcoord.y; }
				//LEFT
				else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
				{	min.y = -1*min_2dcoord.x; min.z = -1*min_2dcoord.y; 
					max.y = -1*max_2dcoord.x; max.z = -1*max_2dcoord.y; }
				//UP
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
				{	min.x =  min_2dcoord.x; min.y = min_2dcoord.y; 
					max.x =  max_2dcoord.x; max.y = max_2dcoord.y; }
				//DOWN
				else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
				{	min.x =  -1*min_2dcoord.x; min.y = min_2dcoord.y;	
					max.x =  -1*max_2dcoord.x; max.y = max_2dcoord.y;	}

				index++;

				if (min.x > max.x) 
				{	double aux = min.x; min.x = max.x; max.x = aux;	}
				if (min.y > max.y) 
				{	double aux = min.y; min.y = max.y; max.y = aux;	}
				if (min.z > max.z) 
				{	double aux = min.z; min.z = max.z; max.z = aux;	}
			}

			{	boost::mutex::scoped_lock lock(bbox_mutex);
				BoundinBoxMin = min;
				BoundinBoxMax = max; }
	}
	else
	{	boost::mutex::scoped_lock lock(bbox_mutex);
		min = BoundinBoxMin;
		max = BoundinBoxMax;
	}
}

void PerceptVideo::TransformCoordinates(corePoint2D<int> &result, const int &cam_index)
{
	float width, height;
	width = height = 0;
	std::map<int, IplImage *>::iterator iter_img = capture_img.find(cam_index);
	if (iter_img != capture_img.end())
	{
		width  = cvGetSize( iter_img->second ).width;
		height = cvGetSize( iter_img->second ).height;
	}

	CvPoint3D32f objPts, imgPts;
	imgPts.x = result.x;
	imgPts.y = result.y;
	imgPts.z = 1;

	CvMat pt1, pt2;
	pt1 = cvMat( 3, 1, CV_32F, &objPts );
	pt2 = cvMat( 3, 1, CV_32F, &imgPts );

	if (homography[cam_index])
		cvMatMulAdd(homography[cam_index], &pt2, 0, &pt1);

	float offset_x = (width > 0)  ? width/2  : 0;
	float offset_y = (height > 0) ? height/2 : 0;
	float pt1_x = ((float*)(pt1.data.ptr + pt1.step*0))[0] - offset_x;
	float pt1_y = ((float*)(pt1.data.ptr + pt1.step*1))[0] - offset_y;

	result.x = pt1_x;
	result.y = pt1_y;
}

/** \brief Analizes the current face image in order to recognize whether it is of a known user or not */
int  PerceptVideo::RecognizeCurrentFace()
{ 
	boost::mutex::scoped_lock lock(m_mutex);
	int face_vec_size = face_history.size();
	if ( face_recognizer && (face_vec_size > 0) )
	{	core::Image *face = &(face_history[face_vec_size-1]);
		return face_recognizer->RecognizeFromImage(face->image, face->width, face->height, face->nchannels, face->depth_, face->widthstep);
	}
	return -1;
}
/** \brief Uses latest set of faces to add a new user and retrain Face Recognizer */
void PerceptVideo::AddNewUserToRecognition(const int &user_id)
{
	boost::mutex::scoped_lock lock(m_mutex);
	int face_vec_size = face_history.size();
	if (face_recognizer && (face_vec_size > 0) )
		face_recognizer->AddUser(face_history, user_id);
}

bool PerceptVideo::IsFacePoolReady()
{
	boost::mutex::scoped_lock lock(m_mutex);
	int vec_size = face_history.size();
	return (vec_size == 10);
}

void PerceptVideo::UpdateFaceHistory()
{
	char *img = NULL;
	int size_x, size_y, n_channels, depth, width_step;
	if(presence_detected) 
	{
		for (std::vector<IFaceDetection*>::iterator iter = face_detectors.begin(); !img && (iter != face_detectors.end()); iter++)
		{	img = (*iter)->GetCopyOfAreaOfInterest(size_x, size_y, n_channels, depth, width_step);
		}

		core::Image new_face(img, size_x, size_y, n_channels, depth, width_step);
		if (img)
		{
			int vec_size = face_history.size();
			if(vec_size == 10)
			{	delete [] ((*(face_history.begin())).image);
				face_history.erase(face_history.begin());
			}
			face_history.push_back(new_face);
		}
	}
	else
	{	for (std::vector<core::Image>::iterator iter = face_history.begin(); iter != face_history.end(); iter++)
			free((*iter).image);
		face_history.clear();
	}
}

void PerceptVideo::CalculatePresenceModel(std::vector<float> &result, int &row_step, const float &scale)
{
		int index = 1;
		IplImage *img_xz, *img_yz, *img_xy;
		std::map< std::string, IplImage * > presence_images, motion_images;
		img_xz = img_yz = img_xy = NULL;
		float has_x, has_y, has_z;
		has_x = has_y = has_z = false;
		std::vector<IPresenceDetection*>::iterator presenceDetection_iter = presence_detectors.begin();
		std::vector<IMotionDetection*>::iterator motionDetection_iter = motion_detectors.begin();
		if( presence_detectors.size() == motion_detectors.size())
		for (int unsigned i=0; i < presence_detectors.size() && !(has_x && has_y && has_z); i++)
		{
			bool xz, yz, xy;
			xz = yz = xy = false;
			core::CameraData cam_data = app_config->GetCameraData(index);
			float has_x, has_y, has_z;
			std::string plane_str = "";
			/** \todo Make generic */
			//FRONT
			if ( (cam_data.x == 0) && (cam_data.y == 1) && (cam_data.z == 0) )
			{	xz = has_x = has_z = true; plane_str = "FRONT"; }
			//BACK
			else if ( (cam_data.x == 0) && (cam_data.y == -1) && (cam_data.z == 0) )
			{	xz = has_x = has_z = true; plane_str = "BACK"; }
			//RIGHT
			else if ( (cam_data.x == 1) && (cam_data.y == 0) && (cam_data.z == 0) )
			{	yz = has_y = has_z = true; plane_str = "RIGHT";  }
			//LEFT
			else if ( (cam_data.x == -1) && (cam_data.y == 0) && (cam_data.z == 0) )
			{	yz = has_y = has_z = true; plane_str = "LEFT"; }
			//UP
			else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == 1) )
			{	xy = has_x = has_y = true; plane_str = "UP"; }
			//DOWN
			else if ( (cam_data.x == 0) && (cam_data.y == 0) && (cam_data.z == -1) )
			{	xy = has_x = has_y = true; plane_str = "DOWN"; }


			int size_x_presence, size_y_presence, n_channels_presence, depth_presence, width_step_presence;
			int size_x_motion, size_y_motion, n_channels_motion, depth_motion, width_step_motion;
			char *new_Presenceimage = (*presenceDetection_iter)->GetCopyOfCurrentImage(size_x_presence, size_y_presence, n_channels_presence, depth_presence, width_step_presence);
			char *new_Motionimage = (*motionDetection_iter)->GetCopyOfCurrentImage(size_x_motion, size_y_motion, n_channels_motion, depth_motion, width_step_motion);
			if(new_Presenceimage && (size_x_presence == size_x_motion) && (size_y_presence == size_y_motion))
			{

				CvSize size, desired_size;
				size.width = size_x_presence;
				size.height = size_y_presence;
				desired_size.width = size_x_presence*scale;
				desired_size.height = size_y_presence*scale;

				IplImage *zimage_presence = cvCreateImage(size, depth_presence, n_channels_presence);
				IplImage *zimage_motion = cvCreateImage(size, depth_motion, n_channels_motion);
				char *idata_presence = zimage_presence->imageData;
				char *idata_motion = zimage_motion->imageData;
				for (int y = 0; y < zimage_presence->height; y++) {
					for (int x = 0; x < zimage_presence->width; x++) {
						//presence copy x*3 
						((uchar*)(idata_presence + width_step_presence*y))[x]   = ((uchar*)(new_Presenceimage+width_step_presence*y))[x];
						if (n_channels_presence > 1)((uchar*)(idata_presence + width_step_presence*y))[x*3+1] = ((uchar*)(new_Presenceimage+width_step_presence*y))[x*3+1];
						if (n_channels_presence > 2)((uchar*)(idata_presence + width_step_presence*y))[x*3+2] = ((uchar*)(new_Presenceimage+width_step_presence*y))[x*3+2];
						//motion copy
						((uchar*)(idata_motion + width_step_motion*y))[x]   = ((uchar*)(new_Motionimage+width_step_presence*y))[x];
						if (n_channels_motion > 1)((uchar*)(idata_motion + width_step_motion*y))[x*3+1] = ((uchar*)(new_Motionimage+width_step_motion*y))[x*3+1];
						if (n_channels_motion > 2)((uchar*)(idata_motion + width_step_motion*y))[x*3+2] = ((uchar*)(new_Motionimage+width_step_motion*y))[x*3+2];
					}
				}

				IplImage *scaled_presence = cvCreateImage(desired_size, depth_presence, n_channels_presence);
				IplImage *scaled_motion = cvCreateImage(desired_size, depth_motion, n_channels_motion);
				cvResize(zimage_presence, scaled_presence, CV_INTER_AREA);
				cvResize(zimage_motion, scaled_motion, CV_INTER_AREA);
				presence_images[plane_str] = scaled_presence;
				motion_images[plane_str] = scaled_motion;

				if(zimage_presence) 
				{	cvReleaseImage(&zimage_presence);
					delete zimage_presence;          }
				free(new_Presenceimage);

				if(zimage_motion) 
				{	cvReleaseImage(&zimage_motion);
					delete zimage_motion;          }
				free(new_Motionimage);
			}
			index++;
			presenceDetection_iter++;
			motionDetection_iter++;
		}

		if ((presence_images.size() > 0) && (motion_images.size() == presence_images.size()))
		{
			std::map< std::string, IplImage * >::iterator presenceDetection_iter = presence_images.begin();
			std::map< std::string, IplImage * >::iterator presenceDetection_iter2 = presence_images.begin()++;
			std::map< std::string, IplImage * >::iterator motionDetection_iter = motion_images.begin();
			std::map< std::string, IplImage * >::iterator motionDetection_iter2 = motion_images.begin()++;
			char *scaled_img = presenceDetection_iter->second->imageData;
			int size_x, size_y, width_step;
			size_x = presenceDetection_iter->second->width;
			size_y = presenceDetection_iter->second->height;
			width_step = presenceDetection_iter->second->widthStep;

			int offset_x, offset_y;
			offset_x = offset_y = 0;
			offset_x = size_x;
			offset_y = size_y;
			float factor = 2.5;

			if (scaled_img)
			{	for (int y = 0; y < size_y; y++) {
				for (int x = 0; x < size_x; x++) {
					if ( ((((uchar*)(scaled_img+width_step*y))[x])  == 0xff) )
					{	float x_candidate, y_candidate, z_candidate;
						x_candidate = y_candidate = z_candidate = 0.0;
						if (presenceDetection_iter->first == "FRONT")
						{	 x_candidate = (offset_x + (float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (presenceDetection_iter->first == "BACK")
						{	 x_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (presenceDetection_iter->first == "RIGHT")
						{	 y_candidate = (offset_x + (float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (presenceDetection_iter->first == "LEFT")
						{	 y_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 z_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (presenceDetection_iter->first == "UP")
						{	 x_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 y_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}
						else if (presenceDetection_iter->first == "DOWN")
						{	 x_candidate = (offset_x + -1*(float)x)*factor / (float)size_x;
							 y_candidate = (offset_y + -1*(float)y)*factor / (float)size_y;	}

						if (presence_images.size() > 1)
						{	int size_x2, size_y2;
							size_x2 = presenceDetection_iter2->second->width;
							size_y2 = presenceDetection_iter2->second->height;
							int offset_x2, offset_y2;
							offset_x2 = offset_y2 = 0;
							offset_x2 = size_x2;
							offset_y2 = size_y2;
							char *scaled_img2 = presenceDetection_iter2->second->imageData;
							char *scaled_img2_motion = motionDetection_iter2->second->imageData;
							if (scaled_img2)
							{	for (int y2 = 0; y2 < size_y2; y2++) {
								for (int x2 = 0; x2 < size_x2; x2++) {
									if ( ((((uchar*)(scaled_img2+width_step*y2))[x2])  == 0xff) )
									{
										if (presenceDetection_iter2->first == "FRONT")
										{	 x_candidate = (offset_x2 + (float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (presenceDetection_iter2->first == "BACK")
										{	 x_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (presenceDetection_iter2->first == "RIGHT")
										{	 y_candidate = (offset_x2 + (float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (presenceDetection_iter2->first == "LEFT")
										{	 y_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 z_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (presenceDetection_iter2->first == "UP")
										{	 x_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 y_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}
										else if (presenceDetection_iter2->first == "DOWN")
										{	 x_candidate = (offset_x2 + -1*(float)x2)*factor / (float)size_x2;
											 y_candidate = (offset_y2 + -1*(float)y2)*factor / (float)size_y2;	}


										//calculate velocity of point
										if  ( ((((uchar*)(scaled_img2_motion+width_step*y2))[x2])  != 0x00) );
										{
											//get velocity of point.
										}

										result.push_back(x_candidate);
										result.push_back(y_candidate);
										result.push_back(z_candidate);
									}


								}}
							}
						}
						else
						{
							result.push_back(x_candidate);
							result.push_back(y_candidate);
							result.push_back(z_candidate);
						}
					row_step = 3;
					}
				}}
			}
		}

		for (std::map< std::string, IplImage * >::iterator presenceDetection_iter = presence_images.begin(); presenceDetection_iter != presence_images.end(); presenceDetection_iter++)
			cvReleaseImage(&(presenceDetection_iter->second));
		presence_images.clear();
}

void PerceptVideo::CalculateFullUserDataElements(IUserDataModelController* user_datamodel_c)
{
	user_datamodel_c;
	user_dataModel_controller;
}

//RTree callback
bool PerceptVideo::RegisterPointIDIntoSearchResults(int id, void* arg) 
{	//printf("Hit data rect %d\n", id);
	search_results.push_back(id);
	return true; // keep going
}

bool PerceptVideo::SetCameraRecording(const bool &value)
{
	boost::mutex::scoped_lock lock(m_mutex);

	double timestamp = (double)clock()/CLOCKS_PER_SEC;
	timestamp_Recording_latestframe = timestamp;

	for (unsigned int i=0; i<num_cams; i++)
	{ 
		if (is_recording && !value)
		{   //Stop Recording and release the writers.
			cvReleaseVideoWriter(&capture_videowriter[i+1]);
			capture_videowriter[i+1] = NULL;
		}
		else if (value) 
		{	//Start Recording prepares the recording, the actual writing is made each time a frame is captured.
			double cam_w = cvGetCaptureProperty(capture_cam_array[i+1], CV_CAP_PROP_FRAME_WIDTH);
			double cam_h = cvGetCaptureProperty(capture_cam_array[i+1], CV_CAP_PROP_FRAME_HEIGHT);
			double fps = cvGetCaptureProperty(capture_cam_array[i+1], CV_CAP_PROP_FPS); 
			if (fps < 10) fps = capture_fps;

			//CV_FOURCC('M','P','G','4')
			std::stringstream file_name;
			file_name << "sample_" << (int)timestamp << "_cam" << i+1 << ".avi";
			std::string str_file_name = file_name.str();
			capture_videowriter[i+1] = cvCreateVideoWriter(str_file_name.c_str(), CV_FOURCC('X','V','I','D'), fps, cvSize((int)cam_w,(int)cam_h), 1);
			if (capture_videowriter[i+1] == NULL)
				cout << "Could not create the video writer (missing XVID codec? http://www.fourcc.org/): cvCreateVideoWriter for camera " << i+1 << "\n";
		} 
	}
	is_recording = value;
	return false;
}
bool PerceptVideo::SetUseRecording(const bool &value, const std::string &video_url_basename)
{
	boost::mutex::scoped_lock lock(m_mutex);

	double timestamp = (double)clock()/CLOCKS_PER_SEC;
	timestamp_Recording_latestframe = timestamp;

	if (is_recording)
		return false;

	for (unsigned int i=1; i<=num_cams; i++)
	{ 
		is_using_videosource = value;

		if (is_using_videosource)
		{

			std::stringstream file_name;
			file_name << video_url_basename << i << ".avi";
			std::string str_file_name = file_name.str();
			capture_videofiles_array[i] = cvCaptureFromFile(str_file_name.c_str());
			double fps = cvGetCaptureProperty(capture_videofiles_array[i], CV_CAP_PROP_FPS); 
		}
		else
		{
			cvReleaseCapture(&capture_videofiles_array[i]);
		}
	}

	return false;
}