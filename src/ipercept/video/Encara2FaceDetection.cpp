#include <ipercept/video/Encara2FaceDetection.h>


#include <debugger.h> 

using namespace core;
using namespace core::ipercept;
using namespace cv;

Encara2FaceDetection::Encara2FaceDetection(IPerceptVideo* video_perception, const int camera_index): v_perception(video_perception), cam_index(camera_index), initialized(false), stop_requested(false), face_detected(false), ENCARAFaceDetector(NULL), image(NULL), las_time(0), face_img(NULL)
{
	faceRec_a.x = -1; faceRec_a.y = -1;
	faceRec_b.x = -1; faceRec_b.y = -1;
	faceCenterPos.x = -1;
	faceCenterPos.y = -1;

	//std::stringstream ssname;
	//ssname << "Miniface" << cam_index;
	//cvNamedWindow(ssname.str().c_str(),1);
}

Encara2FaceDetection::~Encara2FaceDetection()
{
	cvReleaseImage(&image);
	if(ENCARAFaceDetector)
		delete ENCARAFaceDetector;
}

void Encara2FaceDetection::Delete()
{
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void Encara2FaceDetection::Init()
{
	Encara2FaceDetection::DoInit();
}

void Encara2FaceDetection::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Encara2FaceDetection::DoMainLoop, this ) ));
		//m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&Encara2FaceDetection::DoMainLoop)));
		//SetThreadName(m_thread->get_id(), "Encara2FaceDetection");
	}
}

void Encara2FaceDetection::DoMainLoop()
{
	initialized = true;

	char ENCARAdataDir[256];
	char *ENCARA2=getenv("ENCARA2");
	sprintf(ENCARAdataDir,"%s\\ENCARA2data",ENCARA2);
	ENCARAFaceDetector = new CENCARA2_2Detector(ENCARAdataDir,320,240);

	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}
}

void Encara2FaceDetection::Iterate()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(updated))
	{
		//Capture();
		if (updated) Process();
	}
}

void Encara2FaceDetection::SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data)
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(data))
	{
		char *old_image = (image) ? image->imageData : NULL;
		char *new_image = data; 

		CvSize size;
		size.width = size_x;
		size.height = size_y;
		IplImage *aux_img = image;
		image = cvCreateImage(size, depth, n_channels);
		char *idata = image->imageData;

		for (int y = 0; y < image->height; y++) {
			for (int x = 0; x < image->width; x++) {
				((uchar*)(idata + width_step*y))[x*3]   = ((uchar*)(new_image+width_step*y))[x*3];
				((uchar*)(idata + width_step*y))[x*3+1] = ((uchar*)(new_image+width_step*y))[x*3+1];
				((uchar*)(idata + width_step*y))[x*3+2] = ((uchar*)(new_image+width_step*y))[x*3+2];
			}
		}

		if(aux_img) 
		{	cvReleaseImage(&aux_img);
			delete aux_img;          }
		//if(new_image) free(new_image);

		//double timestamp = (double)clock()/CLOCKS_PER_SEC;
		//std::cout << "Encara2 Period: " << timestamp-las_time << "\n";
		//las_time = timestamp;

		updated = true;
	}
}

void Encara2FaceDetection::Process()
{
	face_detected = Apply();
	updated = false;

	//double timestamp = (double)clock()/CLOCKS_PER_SEC;
	//std::cout << "Encara2 Period: " << timestamp-las_time << "FPS: " << 1/(timestamp-las_time) << "\n";
	//las_time = timestamp;

}

bool Encara2FaceDetection::FaceDetected() 
{
	boost::try_mutex::scoped_lock lock(m_mutex);
	return face_detected; 
}

void Encara2FaceDetection::Capture()
{
	char *old_image = (image) ? image->imageData : NULL;
	int size_x, size_y, n_channels, depth, width_step;
	char *new_image = NULL;
	
	//while(!new_image)
	{	new_image = (v_perception) ? v_perception->GetCopyOfCurrentImage(cam_index, size_x, size_y, n_channels, depth, width_step) : NULL;
		//m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}

	if(new_image)
	{
		CvSize size;
		size.width = size_x;
		size.height = size_y;
		IplImage *aux_img = image;
		image = cvCreateImage(size, depth, n_channels);
		//image->imageData = new_image;
		char *idata = image->imageData;

		for (int y = 0; y < image->height; y++) {
			for (int x = 0; x < image->width; x++) {
				((uchar*)(idata + width_step*y))[x*3]   = ((uchar*)(new_image+width_step*y))[x*3];
				((uchar*)(idata + width_step*y))[x*3+1] = ((uchar*)(new_image+width_step*y))[x*3+1];
				((uchar*)(idata + width_step*y))[x*3+2] = ((uchar*)(new_image+width_step*y))[x*3+2];
			}
		}

		//Image fd_image(NULL, size_x, size_y, n_channels, depth, width_step);
		//int face_x, face_y;
		face_detected = Apply();

		if(aux_img) 
		{	cvReleaseImage(&aux_img);
			delete aux_img;          }
		if(new_image) free(new_image);

		double timestamp = (double)clock()/CLOCKS_PER_SEC;
		//std::cout << "Encara2 Period: " << timestamp-las_time << "\n";
		las_time = timestamp;
	}
	else
		std::cout << "ENCARA2 FAILED!!\n";
}

bool Encara2FaceDetection::Apply()
{
	//ENCARAFaceDetector->DetectorReset();
	ENCARAFaceDetector->DetectorLostReset();
	IplImage *h = image;
	if (h)
	{
		ENCARAFaceDetector->ApplyENCARA2(h,2);
		CFacialDataperImage * facial_data = ENCARAFaceDetector->GetFacialData();
		int face_index = facial_data->GetLargestDetectedFace();
		if (facial_data && (face_index != -1))
		{
			{boost::mutex::scoped_lock lock(face_data_mutex);
			facial_data->Faces[face_index]->Gender;
			faceRec_a.x = facial_data->Faces[face_index]->x1;
			faceRec_a.y = facial_data->Faces[face_index]->y1;
			faceRec_b.x = facial_data->Faces[face_index]->x2;
			faceRec_b.y = facial_data->Faces[face_index]->y2;
			faceCenterPos.x = (faceRec_a.x + faceRec_b.x) / 2;
			faceCenterPos.y = (faceRec_a.y + faceRec_b.y) / 2;}

			IplImage *aux_img = face_img;
			face_img = Sub_Image(h, cvRect( faceRec_a.x, faceRec_a.y, faceRec_b.x-faceRec_a.x, faceRec_b.y-faceRec_a.y ));
			ENCARAFaceDetector->PaintFacialData(h,CV_RGB(0,255,0));
			if (aux_img) cvReleaseImage(&aux_img);
			
			//std::stringstream ssname;
			//ssname << "Miniface" << cam_index;
			//cvShowImage(ssname.str().c_str(), face_img);

			return true;
		}
	}
	return false;
}

char * Encara2FaceDetection::GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(image))
	{
		char *source = image->imageData;
		char *copy;

		size_x     = image->width;
		size_y     = image->height;
		n_channels = image->nChannels;
		depth      = image->depth;
		width_step = image->widthStep;

		copy = (char *)malloc(sizeof(char)*size_x*size_y*n_channels);

		for (int y = 0; y < size_y; y++) {
			for (int x = 0; x < size_x; x++) {
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
	return NULL;
}

char * Encara2FaceDetection::GetCopyOfAreaOfInterest(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(face_img))
	{
		n_channels = face_img->nChannels;
		depth      = face_img->depth;
		IplImage *scaled = cvCreateImage(cvSize(200,200), depth, n_channels);
		cvResize(face_img, scaled, CV_INTER_LINEAR);

		char *source = scaled->imageData;
		char *copy;
		size_x     = scaled->width;
		size_y     = scaled->height;
		width_step = scaled->widthStep;

		copy = new char[size_x*size_y*n_channels];

		for (int y = 0; y < size_y; y++) {
			for (int x = 0; x < size_x; x++) {
					int first_channel = switch_rb ? 2 : 0;
					int second_channel = switch_rb ? 1 : 1;
					int third_channel  = switch_rb ? 0 : 2;
					((uchar*)(copy + width_step*y))[x*3]   = ((uchar*)(source+width_step*y))[x*3+first_channel];
					((uchar*)(copy + width_step*y))[x*3+1] = ((uchar*)(source+width_step*y))[x*3+second_channel];
					((uchar*)(copy + width_step*y))[x*3+2] = ((uchar*)(source+width_step*y))[x*3+third_channel];
			}
		}
		cvReleaseImage(&scaled);
		return copy;
	}
	return NULL;
}

void Encara2FaceDetection::GetFaceCenterPos(corePoint2D<int> &pos)
{
	boost::try_mutex::scoped_lock lock(face_data_mutex);
	pos.x = faceCenterPos.x;
	pos.y = faceCenterPos.y;
}
void Encara2FaceDetection::GetFaceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b)
{
	boost::try_mutex::scoped_lock lock(face_data_mutex);
	corner_a.x = faceRec_a.x;
	corner_a.y = faceRec_a.y;
	corner_b.x = faceRec_b.x;
	corner_b.y = faceRec_b.y;
}
void Encara2FaceDetection::GetFeaturePos(const std::string &feature, corePoint2D<int> &pos)
{
	//Nothing, no features available
}

IplImage* Encara2FaceDetection::Sub_Image(IplImage *image, CvRect roi)
{
	IplImage *result;
	cvSetImageROI(image,roi);
	result = cvCreateImage( cvSize(roi.width, roi.height), image->depth, image->nChannels );
	cvCopy(image,result);
	cvResetImageROI(image); // release image ROI
	return result;
}
