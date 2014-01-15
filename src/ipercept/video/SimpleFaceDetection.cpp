#include <ipercept/video/SimpleFaceDetection.h>


#include <debugger.h> 

using namespace core;
using namespace core::ipercept;
using namespace cv;

SimpleFaceDetection::SimpleFaceDetection(IApplicationConfiguration *appconfig, IPerceptVideo* video_perception, const int camera_index): app_config(appconfig), v_perception(video_perception), cam_index(camera_index), initialized(false), stop_requested(false),/* ENCARAFaceDetector(NULL),*/ image(NULL), face_img(NULL), updated(false), face_detected(false), cascade(NULL)
{
	faceRec_a.x = -1; faceRec_a.y = -1;
	faceRec_b.x = -1; faceRec_b.y = -1;
	faceCenterPos.x = -1;
	faceCenterPos.y = -1;

	std::stringstream cascade_name;
	if(app_config)
		cascade_name << app_config->GetExternDirectory() << "encara2/ENCARA2data/HaarClassifiers/haarcascade_frontalface_alt2.xml";
	else
		cascade_name << "./haarcascade_frontalface_alt2.xml";
	
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name.str().c_str(), 0, 0, 0 );
}

SimpleFaceDetection::~SimpleFaceDetection()
{
	cvReleaseImage(&image);
}

void SimpleFaceDetection::Delete()
{
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void SimpleFaceDetection::Init()
{
	SimpleFaceDetection::DoInit();
}

void SimpleFaceDetection::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&SimpleFaceDetection::DoMainLoop, this ) ));
	}
}

void SimpleFaceDetection::DoMainLoop()
{
	initialized = true;

	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}
}

void SimpleFaceDetection::Iterate()
{
	boost::mutex::scoped_try_lock lock(m_mutex);
	if (lock)
	{
		Process();
	}
}

void SimpleFaceDetection::Process()
{
	face_detected = Apply();
	updated = false;
}

bool SimpleFaceDetection::FaceDetected() 
{
	boost::mutex::scoped_lock lock(m_mutex);
	return face_detected; 
}

void SimpleFaceDetection::SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data)
{
	boost::mutex::scoped_try_lock lock(m_mutex);
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

		updated = true;
	}
}

void SimpleFaceDetection::Capture()
{}

bool SimpleFaceDetection::Apply()
{
	IplImage *h = image;
	if (h)
	{
		return detect_and_draw();
	}
	return false;
}

char * SimpleFaceDetection::GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::mutex::scoped_try_lock lock(m_mutex);
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
	return NULL;
}

char * SimpleFaceDetection::GetCopyOfAreaOfInterest(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(face_img))
	{
		char *source = face_img->imageData;
		char *copy;

		size_x     = face_img->width;
		size_y     = face_img->height;
		n_channels = face_img->nChannels;
		depth      = face_img->depth;
		width_step = size_x * n_channels;

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
//---------------------------------
 //Parameters for haar detection
 //From the API:
 //The default parameters (scale_factor=2, min_neighbors=3, flags=0) are tuned 
 //for accurate yet slow object detection. For a faster operation on real video 
 //images the settings are: 
 //scale_factor=1.2, min_neighbors=2, flags=CV_HAAR_DO_CANNY_PRUNING, 
 //min_size=<minimum possible face size

bool SimpleFaceDetection::detect_and_draw()
{
	//Accurate
	CvPoint pt1, pt2;
	CvSize min_size;
	min_size.width = 20;
	min_size.height= 20;
	int scale, image_scale;
	scale = image_scale	= 2;
	float haar_scale = 2;
	int min_neighbors = 2;
	int haar_flags = 0;
	cascade;

	////fast
	//CvPoint pt1, pt2;
	//CvSize min_size;
	//min_size.width = 20;
	//min_size.height= 20;
	//int scale, image_scale;
	//scale = image_scale	= 1.2;
	//float haar_scale = 1.2;
	//int min_neighbors = 2;
	//int haar_flags = CV_HAAR_DO_CANNY_PRUNING;
	//static CvHaarClassifierCascade* cascade = NULL;

	//const char* cascade_name = "C:/etc/ProjectOX/trunk/projectVOX/extern/encara2/ENCARA2data/HaarClassifiers/haarcascade_frontalface_alt2.xml";/*"haarcascade_profileface.xml";*/
    //cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

	
    // allocate temporary images
	IplImage *img = image;
    IplImage *gray = cvCreateImage(cvSize(img->width,img->height), 8, 1);
	CvSize si_size;
	si_size.width = cvRound(img->width / (double)image_scale);
	si_size.height= cvRound( (double)img->height / image_scale);
	IplImage *small_img = cvCreateImage(si_size, 8, 1);

	// convert color input image to grayscale
	cvCvtColor(img, gray, CV_BGR2GRAY);
    // scale input image for faster processing
    cvResize(gray, small_img, CV_INTER_LINEAR);
    cvEqualizeHist(small_img, small_img);

    if(cascade)
	{
        CvSeq * faces = cvHaarDetectObjects(small_img, cascade, cvCreateMemStorage(0), haar_scale, min_neighbors, haar_flags, min_size);
        if (faces)
			for( int i = 0; i < (faces ? faces->total : 0); i++ )
			{
				CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
				pt1.x = r->x*scale;
				pt2.x = (r->x+r->width)*scale;
				pt1.y = r->y*scale;
				pt2.y = (r->y+r->height)*scale;
				cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );

				{boost::mutex::scoped_lock lock(face_data_mutex);
				faceRec_a.x = pt1.x; faceRec_a.y = pt1.y;
				faceRec_b.x = pt2.x; faceRec_b.y = pt2.y;
				faceCenterPos.x = int((pt1.x + pt2.x)/2.0);
				faceCenterPos.y = int((pt1.y + pt2.y)/2.0);}

				IplImage *aux_img = face_img;
				face_img = Sub_Image(img, cvRect( faceRec_a.x, faceRec_a.y, faceRec_b.x-faceRec_a.x, faceRec_b.y-faceRec_a.y ));
				if (aux_img) cvReleaseImage(&aux_img);

				return true;
			}       
	}
	return false;
}

void SimpleFaceDetection::GetFaceCenterPos(corePoint2D<int> &pos)
{
	boost::mutex::scoped_lock lock(face_data_mutex);
	pos.x = faceCenterPos.x;
	pos.y = faceCenterPos.y;
}
void SimpleFaceDetection::GetFaceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b)
{
	boost::mutex::scoped_lock lock(face_data_mutex);
	corner_a.x = faceRec_a.x;
	corner_a.y = faceRec_a.y;
	corner_b.x = faceRec_b.x;
	corner_b.y = faceRec_b.y;
}
void SimpleFaceDetection::GetFeaturePos(const std::string &feature, corePoint2D<int> &pos)
{
	//Nothing, no features available
}

IplImage* SimpleFaceDetection::Sub_Image(IplImage *image, CvRect roi)
{
	IplImage *result;
	cvSetImageROI(image,roi);
	result = cvCreateImage( cvSize(roi.width, roi.height), image->depth, image->nChannels );
	cvCopy(image,result);
	cvResetImageROI(image);
	return result;
}