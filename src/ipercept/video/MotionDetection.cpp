#include <ipercept/video/MotionDetection.h>

#include <debugger.h> 

using namespace core;
using namespace core::ipercept;
using namespace cv;


// various tracking parameters (in seconds)
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;

//const int N = 4;// number of cyclic frame buffer used for motion detection (should, probably, depend on FPS)
//// ring image buffer
//IplImage **buf = 0;
//int last = 0;

// temporary images
//IplImage *mhi = 0; // MHI
//IplImage *orient = 0; // orientation
//IplImage *mask = 0; // valid orientation mask
//IplImage *segmask = 0; // motion segmentation map
//CvMemStorage* storage = 0; // temporary storage

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
void  MotionDetection::UpdateMHI( IplImage* img, IplImage* dst, int diff_threshold )
{
	boost::try_mutex::scoped_lock lock(m_mutex);

	{	boost::try_mutex::scoped_lock lock(m_mutex_motion_areas);
		motion_elements.clear();	}

    double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
    CvSize size = cvSize(img->width,img->height); // get current frame size
    int i, idx1 = last, idx2;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    double count;
    double angle;
    CvPoint center;
    double magnitude;
    CvScalar color;

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
        if( buf == 0 ) {
            buf = (IplImage**)malloc(N*sizeof(buf[0]));
            memset( buf, 0, N*sizeof(buf[0]));
        }

        for( i = 0; i < N; i++ ) {
            cvReleaseImage( &buf[i] );
            buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            cvZero( buf[i] );
        }
        cvReleaseImage( &mhi );
        cvReleaseImage( &orient );
        cvReleaseImage( &segmask );
        cvReleaseImage( &mask );

        mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        cvZero( mhi ); // clear MHI at the beginning
        orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
    }

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames

    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it
    cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale( mhi, mask, 255./MHI_DURATION,
                (MHI_DURATION - timestamp)*255./MHI_DURATION );
    cvZero( dst );
    cvMerge( mask, 0, 0, 0, dst );

    // calculate motion gradient orientation and valid orientation mask
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );

    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
    //for( i = -1; i < seq->total; i++ ) {
	for( i = 0; i < seq->total; i++ ) {

        if( i < 0 ) { // case of the whole image
            comp_rect = cvRect( 0, 0, size.width, size.height );
            color = CV_RGB(255,255,255);
            magnitude = 100;
        }
        else { // i-th motion component
            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
            if( comp_rect.width + comp_rect.height < 100 ) // reject very small components
                continue;
            color = CV_RGB(255,0,0);
            magnitude = 30;
        }

        // select component ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

        // calculate orientation
        angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

        // check for the case of little motion
        if( count < comp_rect.width*comp_rect.height * 0.05 )
            continue;

        //// draw a clock with arrow indicating the direction
        center = cvPoint( (comp_rect.x + comp_rect.width/2), (comp_rect.y + comp_rect.height/2) );
        cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
        cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)), cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );

		{
			boost::try_mutex::scoped_lock lock(m_mutex_motion_areas);
			MotionElement me;
			me.magnitude = magnitude;
			me.rect.min[0] = comp_rect.x;
			me.rect.min[1] = comp_rect.y;
			me.rect.max[0] = comp_rect.x + comp_rect.width;
			me.rect.max[1] = comp_rect.y + comp_rect.height;
			me.direction = vector3F(magnitude*cos(angle*CV_PI/180), -1 * magnitude*sin(angle*CV_PI/180), 0);
			motion_elements.push_back(me);
		}
    }
}

std::vector<MotionElement> MotionDetection::GetMotionElements() 
{ 
	boost::try_mutex::scoped_lock lock(m_mutex_motion_areas);
	return motion_elements;
}





MotionDetection::MotionDetection(IPerceptVideo* video_perception, const int camera_index)
	: v_perception(video_perception), cam_index(camera_index), initialized(false), stop_requested(false), image(NULL), 
	  N(4), buf(NULL), last(0), mhi(NULL), orient(NULL), mask(NULL), segmask(NULL), storage(NULL), motion(NULL), las_time(0), updated(false)
{
	faceRec_a.x = -1; faceRec_a.y = -1;
	faceRec_b.x = -1; faceRec_b.y = -1;
	faceCenterPos.x = -1;
	faceCenterPos.y = -1;
}

MotionDetection::~MotionDetection()
{
	cvReleaseImage(&image);
}

void MotionDetection::Delete()
{
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void MotionDetection::Init()
{
	MotionDetection::DoInit();
}

void MotionDetection::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&MotionDetection::DoMainLoop, this ) ));
		//m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&MotionDetection::DoMainLoop)));
	}
}

void MotionDetection::DoMainLoop()
{
	initialized = true;

	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
	}
}

void MotionDetection::Iterate()
{
	//boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (/*(lock)&&*/(updated))
	{
		//Capture();
		if(updated) Process();
	}
}

void MotionDetection::Process()
{
    if( !motion )
    {	boost::try_mutex::scoped_lock lock(m_mutex);
        motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
        cvZero( motion );
        motion->origin = image->origin;
    }

    UpdateMHI( image, motion, 30 );

	{boost::try_mutex::scoped_lock lock(m_mutex);
	updated = false;}
}


void MotionDetection::Capture()
{
	char *old_image = (image) ? image->imageData : NULL;
	int size_x, size_y, n_channels, depth, width_step;
	char *new_image = NULL;
	
	//while(!new_image)
	{	new_image = (v_perception) ? v_perception->GetCopyOfCurrentImage(cam_index, size_x, size_y, n_channels, depth, width_step) : NULL;
	//	m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(10));
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

		//---------------------------
            if( !motion )
            {
                motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
                cvZero( motion );
                motion->origin = image->origin;
            }

            UpdateMHI( image, motion, 30 );
		//-------------------------------

		//Image fd_image(NULL, size_x, size_y, n_channels, depth, width_step);
		//int face_x, face_y;
		//Apply(fd_image, 2, face_x, face_y);

		if(aux_img) 
		{	cvReleaseImage(&aux_img);
			delete aux_img;          }
		if(new_image) free(new_image);

		double timestamp = (double)clock()/CLOCKS_PER_SEC;
		std::cout << "MOTION Period: " << timestamp-las_time << "\n";
		las_time = timestamp;
	}
	else
		std::cout << "MOTION FAILED!!\n";
}

void MotionDetection::SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data)
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

char * MotionDetection::GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if ((lock)&&(motion))
	{
		char *source = motion->imageData;
		char *copy;

		size_x     = motion->width;
		size_y     = motion->height;
		n_channels = motion->nChannels;
		depth      = motion->depth;
		width_step = motion->widthStep;

		copy = (char *)malloc(sizeof(char)*size_x*size_y*n_channels);

		for (int y = 0; y < motion->height; y++) {
			for (int x = 0; x < motion->width; x++) {
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
