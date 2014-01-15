#include <ipercept/video/PresenceDetection.h>
#include <debugger.h> 

#define NUM_BG_TRAINNINGFRAMES 10
#define TRAIN_PERIOD 1000
#define PRESENCE_MIN_AREA 70000
#define PRESENCE_MAX_AREA 50000000

using namespace core;
using namespace core::ipercept;
using namespace cv;

PresenceDetection::PresenceDetection(IPerceptVideo* video_perception, const int camera_index)
: v_perception(video_perception), cam_index(camera_index), initialized(false), stop_requested(false),/* ENCARAFaceDetector(NULL),*/ 
  image(NULL), background_model(NULL), bg_trainning_model(NULL), background_image(NULL), foreground_img(NULL),  background_trainning_frames(0), updated(false), latest_bkg(NULL), background_is_trained(0), background_train_lapse(0),
  last_image(NULL), last_foreground_image(NULL), first_time(true), foreground_moments(NULL), presence_area(0.0)
{
	presenceRec_a.x = -1; presenceRec_a.y = -1;
	presenceRec_b.x = -1; presenceRec_b.y = -1;
	presenceCenterPos.x = -1;
	presenceCenterPos.y = -1;

	background_params = new CvGaussBGStatModelParams;
	background_params->win_size=10;	
	background_params->n_gauss=5;
	background_params->bg_threshold=0.8;
	background_params->std_threshold=3.5;
	background_params->minArea=40;
	background_params->weight_init=0.05;
	background_params->variance_init=45;
}

PresenceDetection::~PresenceDetection()
{
	m_thread->join();
	cvReleaseImage(&image);
	delete background_params;
	delete background_model;
}

void PresenceDetection::Delete()
{
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void PresenceDetection::Init()
{
	PresenceDetection::DoInit();
}

void PresenceDetection::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PresenceDetection::DoMainLoop, this ) ));
	}
}

void PresenceDetection::DoMainLoop()
{
	initialized = true;

	background_is_trained = 0;
	background_train_lapse = 0;

	while(!stop_requested)
	{
		Iterate();
		if( background_train_lapse > 0 )
			background_train_lapse--;
		else
		{	
			if (background_is_trained < 2) 
			{	DoTrainBackground();
				background_train_lapse = TRAIN_PERIOD;	
				background_is_trained++;
			}
		}
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}
}

void PresenceDetection::Iterate()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (lock && updated)
	{
		Capture();
	}
}


void PresenceDetection::Capture()
{
	Apply();
}

bool PresenceDetection::Apply()
{
	IplImage *h = image;
	updated = false;
	if (h && background_model)
	{
		bool do_train = (background_trainning_frames < NUM_BG_TRAINNINGFRAMES);

		if(!latest_bkg)
			latest_bkg  = cvCloneImage(image);

		if(do_train && first_time)
		{
			if(!bg_trainning_model)
				bg_trainning_model = cvCreateGaussianBGModel(latest_bkg ,background_params);

			CvSize size;
			size.width			= background_model->foreground->width;
			size.height			= background_model->foreground->height;
			int n_channels_bw	= background_model->foreground->nChannels;
			int depth_bw		= background_model->foreground->depth;
			int width_step_bw	= background_model->foreground->widthStep;
			int n_channels		= image->nChannels;
			int depth			= image->depth;
			int width_step		= image->widthStep;

			IplImage *imgA, *imgB, *notForeg, *foreg, *mix;
			imgA		= cvCreateImage(size, depth, n_channels);
			imgB		= cvCreateImage(size, depth, n_channels);
			notForeg	= cvCreateImage(size, depth, n_channels);
			foreg		= cvCreateImage(size, depth, n_channels);
			mix			= cvCreateImage(size, depth, n_channels);

			if (background_trainning_frames != 0 )
				cvConvertImage(background_model->foreground, foreg);
			else 
				cvConvertImage(bg_trainning_model->foreground, foreg);
			cvNot(foreg, notForeg);
			cvAnd(h, notForeg, imgB);
			cvAnd(latest_bkg, foreg, imgA);
			cvOr(imgA, imgB, mix);

			cvReleaseImage(&imgA);
			cvReleaseImage(&imgB);
			cvReleaseImage(&foreg);
			cvReleaseImage(&notForeg);
			//
			cvUpdateBGStatModel( mix, bg_trainning_model, -1);
			if(background_trainning_frames < NUM_BG_TRAINNINGFRAMES) 
				background_trainning_frames++;
			if(background_trainning_frames == NUM_BG_TRAINNINGFRAMES)
			{
				CvBGStatModel *aux_model;
				aux_model = background_model;
				background_model = bg_trainning_model;
				bg_trainning_model = aux_model;
				IplImage *img_Aux = latest_bkg;
				latest_bkg = mix;
				cvReleaseImage(&img_Aux);
				first_time = false;
			}
			else
				cvReleaseImage(&mix);
		}
		cvUpdateBGStatModel( h, background_model, 0 );

		IplImage *eroded;
		int size_x     = background_model->foreground->width;
		int size_y     = background_model->foreground->height;
		int n_channels = background_model->foreground->nChannels;
		int depth      = background_model->foreground->depth;
		int width_step = background_model->foreground->widthStep;

		CvSize size;
		size.width = size_x;
		size.height = size_y;
		eroded  = cvCreateImage(size, depth, n_channels);
		cvErode(background_model->foreground, eroded, 0, 3);
		CvMoments *old_moments = foreground_moments; //forgotten in memory
		CvMoments *new_moments = new CvMoments();
		cvMoments(eroded, new_moments); 
		foreground_moments = new_moments;

		cvReleaseImage(&eroded);
		delete old_moments;

		if (foreground_moments)
		{	
			//Area
			presence_area = 0.0;
			presence_area = (*foreground_moments).m00;
			
			//Center
			if(presence_area > 0.0)
			{	presenceCenterPos.x = (*foreground_moments).m10/presence_area;
				presenceCenterPos.y = (*foreground_moments).m01/presence_area;		}
			
			//Orientation
			presence_orientation = 0.0;
			double mu20_minus_mu02 = (*foreground_moments).mu20 - (*foreground_moments).mu02;
			double mu11 =(*foreground_moments).mu11;
			double base_presence_orientation = 0.5 * atan( 2 * mu11 /  mu20_minus_mu02 );
			if (mu20_minus_mu02 == 0.0)
			{	if (mu11 > 0) presence_orientation = M_PI * 0.25;
				if (mu11 < 0) presence_orientation = M_PI * -0.25;			
			} else if ( mu20_minus_mu02 > 0.0 )
			{	if ((mu11 > 0) || (mu11 < 0)) presence_orientation = base_presence_orientation;
			} else
			{	if (mu11 > 0) presence_orientation = base_presence_orientation + (M_PI*0.5);
				if (mu11 < 0) presence_orientation = base_presence_orientation - (M_PI*0.5);
			}
		
			//Eccentricity: 0=round, 1=line
			presence_eccentricity = 0.0;
			double mA = pow((*foreground_moments).mu20 - (*foreground_moments).mu02, 2);
			double mB = pow((*foreground_moments).mu11, 2) * 4.0;
			double mC = pow((*foreground_moments).mu20 + (*foreground_moments).mu02, 2);
			presence_eccentricity = (mA - mB) / mC ;
		}
		return true;
	}
	return false;
}

char * PresenceDetection::GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb) 
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(background_model)&&(background_model->foreground))
	{
		IplImage *eroded, *dilated;
		size_x     = background_model->foreground->width;
		size_y     = background_model->foreground->height;
		n_channels = background_model->foreground->nChannels;
		depth      = background_model->foreground->depth;
		width_step = background_model->foreground->widthStep;

		CvSize size;
		size.width = size_x;
		size.height = size_y;
		eroded  = cvCreateImage(size, depth, n_channels);
		dilated = cvCreateImage(size, depth, n_channels);
		cvErode(background_model->foreground, eroded, 0, 3);
		//cvDilate(eroded, dilated, 0, 1);

		char *source = eroded->imageData;
		char *copy;

		copy = (char *)malloc(sizeof(char)*size_x*size_y*n_channels);

		for (int y = 0; y < eroded->height; y++) {
			for (int x = 0; x < eroded->width; x++) {
				((uchar*)(copy + width_step*y))[x]   = ((uchar*)(source+width_step*y))[x];
			}
		}
		cvReleaseImage(&eroded);
		cvReleaseImage(&dilated);
		return copy;
	}
	return NULL;
}

void PresenceDetection::detect_and_draw()
{
}

bool PresenceDetection::PresenceDetected() 
{
	boost::mutex::scoped_lock lock(m_mutex);

	return ( ( presence_area > PRESENCE_MIN_AREA ) 
		  && ( presence_area < PRESENCE_MAX_AREA ) );
}
void PresenceDetection::GetPresenceCenterOfMass(corePoint2D<int> &pos)
{
	boost::mutex::scoped_lock lock(m_mutex);

	pos.x = presenceCenterPos.x;
	pos.y = presenceCenterPos.y;
}
void PresenceDetection::GetPresenceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b)
{
	boost::mutex::scoped_lock lock(m_mutex);

	corner_a.x = presenceRec_a.x;
	corner_a.y = presenceRec_a.y;
	corner_b.x = presenceRec_b.x;
	corner_b.y = presenceRec_b.y;
}

void PresenceDetection::DoTrainBackground()
{
	boost::try_mutex::scoped_lock lock(m_mutex);
	if (lock)
	{
		background_trainning_frames = 0;
		if (!background_model && image)
			background_model = cvCreateGaussianBGModel(image ,background_params);
	}
}

void PresenceDetection::TrainBackground()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (lock)
	{
		background_is_trained = 0;
		background_train_lapse = 0;
		first_time = true;
		cvReleaseBGStatModel(&background_model);
		cvReleaseBGStatModel(&bg_trainning_model);
		background_model = bg_trainning_model = NULL;
	}
}

void PresenceDetection::SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data)
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

		updated = true;
	}
}

