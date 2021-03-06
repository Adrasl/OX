#include <ipercept/video/MotionDetection.h>

#include <debugger.h> 

using namespace core;
using namespace core::ipercept;
using namespace cv;

//seconds
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;

void  MotionDetection::UpdateMHI( IplImage* img, IplImage* dst, int diff_threshold )
{
	boost::try_mutex::scoped_lock lock(m_mutex);

	if (lock)
	{
		boost::mutex::scoped_lock lock2(m_mutex_motion_areas);

		motion_elements.clear();

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

		//OPTICAL FLOW
		//---------------------------
		//Updating previous and current frames
		if (!velx)
			velx = cvCreateImage(size,IPL_DEPTH_32F,1);
		if (!vely)
			vely = cvCreateImage(size,IPL_DEPTH_32F,1);	

		if (previous_frame == NULL)
		{	previous_frame = cvCreateImage(size,IPL_DEPTH_8U,1);
			cvCvtColor(img,previous_frame,CV_RGB2GRAY);
		} else
		{	cvReleaseImage(&previous_frame);
			previous_frame = current_frame;
		}
		current_frame = cvCreateImage(size,IPL_DEPTH_8U,1);
		cvCvtColor(img, current_frame, CV_RGB2GRAY);		
		
		if (!velx_Mat)
			velx_Mat = cvCreateMat(size.height, size.width, CV_32FC1); 
		if (!vely_Mat)
			vely_Mat = cvCreateMat(size.height, size.width, CV_32FC1);
		if (show_frame)
			cvReleaseImage(&show_frame);
		show_frame = cvCreateImage(size,IPL_DEPTH_8U,3);

		//Calculating dense Optical Flow (Lucas Kanade)
		block_size.height = block_size.width = 3;
		if (previous_frame && current_frame)
			cvCalcOpticalFlowLK(previous_frame, current_frame, block_size, velx, vely);
		cvConvert(velx, velx_Mat);
		cvConvert(vely, vely_Mat);

		//Preparing picture with lines showing optical flow
		if(show_frame)
		{	cvZero( show_frame );
			for (int y=0; y<size.height; y+=50) {
			for (int x=0; x<size.width; x+=50) {
				int vel_x_here = (int)cvGetReal2D( velx, y, x);
				int vel_y_here = (int)cvGetReal2D( vely, y, x);
				cvLine( show_frame, cvPoint(x, y), cvPoint(x+vel_x_here, y+vel_y_here), cvScalarAll(255));
			}}
		}

		//MHI
		//------------------------------------
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
		for( i = -1; i < seq->total; i++ ) 
		{

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
	boost::mutex::scoped_lock lock(m_mutex_motion_areas);

	return motion_elements;
}





MotionDetection::MotionDetection(IPerceptVideo* video_perception, const int camera_index)
	: v_perception(video_perception), cam_index(camera_index), initialized(false), stop_requested(false), image(NULL), 
	  N(4), buf(NULL), last(0), mhi(NULL), orient(NULL), mask(NULL), segmask(NULL), storage(NULL), motion(NULL), las_time(0), updated(false),
	  previous_frame(NULL), current_frame(NULL), velx(NULL), vely(NULL), show_frame(NULL), velx_Mat(NULL), vely_Mat(NULL)
{
	boost::mutex::scoped_lock lock(m_mutex);

	faceRec_a.x = -1; faceRec_a.y = -1;
	faceRec_b.x = -1; faceRec_b.y = -1;
	faceCenterPos.x = -1;
	faceCenterPos.y = -1;
}

MotionDetection::~MotionDetection()
{
	boost::mutex::scoped_lock lock(m_mutex);

	cvReleaseImage(&image);
	cvReleaseImage(&current_frame);
	cvReleaseImage(&previous_frame);
}

void MotionDetection::Delete()
{
	boost::mutex::scoped_lock lock(m_mutex);

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
	if ((updated))
	{
		if(updated) Process();
	}
}

void MotionDetection::Process()
{
    if( !motion )
    {	boost::mutex::scoped_lock lock(m_mutex);
        motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
        cvZero( motion );
        motion->origin = image->origin;
    }

    UpdateMHI( image, motion, 30 );

	{boost::mutex::scoped_lock lock(m_mutex);
	updated = false;}
}

void MotionDetection::Capture()
{
	char *old_image = (image) ? image->imageData : NULL;
	int size_x, size_y, n_channels, depth, width_step;
	char *new_image = NULL;
	
	{	new_image = (v_perception) ? v_perception->GetCopyOfCurrentImage(cam_index, size_x, size_y, n_channels, depth, width_step) : NULL;	}

	if(new_image)
	{
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

		//MOTION FEATURES
		//---------------------------
            if( !motion )
            {   motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
                cvZero( motion );
                motion->origin = image->origin;										}

            UpdateMHI( image, motion, 30 );
		//-------------------------------

		if(aux_img) 
		{	cvReleaseImage(&aux_img);
			delete aux_img;          }
		if(new_image) free(new_image);

		double timestamp = (double)clock()/CLOCKS_PER_SEC;
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

char * MotionDetection::GetCopyOfCurrentImageOpticalFlow(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb)
{
	boost::try_mutex::scoped_try_lock lock(m_mutex); 

	if ((lock)&&(show_frame))
	{
		boost::mutex::scoped_lock lock2(m_mutex_motion_areas);

		char *source = show_frame->imageData;
		char *copy;

		size_x     = show_frame->width;
		size_y     = show_frame->height;
		n_channels = show_frame->nChannels;
		depth      = show_frame->depth;
		width_step = show_frame->widthStep;

		copy = (char *)malloc(sizeof(char)*size_x*size_y*n_channels);

		for (int y = 0; y < show_frame->height; y++) {
			for (int x = 0; x < show_frame->width; x++) {
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
corePoint3D<float> MotionDetection::GetMotionAtCoords(corePoint2D<int> coords)
{
	corePoint3D<float> result;
	result.y = result.x = result.z = 0.0;
	if (velx && vely && velx_Mat && velx_Mat)
	{	//Faster than cvGetReal2D
		//result.x = cvmGet(velx_Mat, coords.x, coords.y); 
		//result.y = cvmGet(velx_Mat, coords.x, coords.y); 
		result.x = (int)cvGetReal2D( velx, coords.y, coords.x);
		result.y = (int)cvGetReal2D( vely, coords.y, coords.x);
	}

	return result;
}
