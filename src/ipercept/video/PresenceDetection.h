#ifndef _PRESENCEDETECTION_
#define _PRESENCEDETECTION_

#include <core/IPercept/video/IPresenceDetection.h>
#include <core/IPercept/IPerceptVideo.h>
#include <ipercept/CamWindow.h>

#include <string>
#include <vector>
#include <math.h>

#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>

#include <boost/thread.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <core/types.h>

namespace core
{
	namespace ipercept
	{
		class PresenceDetection : public core::IPresenceDetection
		{
			public:

				PresenceDetection(IPerceptVideo* video_perception, const int camera_index);
				virtual ~PresenceDetection();
				virtual void Delete();
				virtual void Init();

				int GetCamIndex() { return cam_index; } ;
				virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data);

				virtual void GetPresenceCenterOfMass(corePoint2D<int> &pos);
				virtual void GetPresenceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b);
				virtual void GetPresenceArea(double &area_inpixel_units) {boost::try_mutex::scoped_lock lock(m_mutex); area_inpixel_units = presence_area; }
				virtual void GetPresenceOrientation(double &radians_counterclockwise) {boost::try_mutex::scoped_lock lock(m_mutex); radians_counterclockwise = presence_orientation; }
				virtual void GetPresenceEccentricity(double &cero_means_round) {boost::try_mutex::scoped_lock lock(m_mutex); cero_means_round = presence_eccentricity; }
				virtual bool PresenceDetected();
				virtual void TrainBackground();

			private:

				void DoInit();
				void DoMainLoop();
				void Iterate();
				void Capture();
				virtual bool Apply(/* Image input, const float &scale, int &pos_x, int &pos_y*/);
				virtual void DoTrainBackground();

				void detect_and_draw();

				boost::shared_ptr<boost::thread> m_thread;
				boost::try_mutex m_mutex;
				bool initialized, stop_requested;
				int cam_index;

				IPerceptVideo* v_perception;
				IplImage  *image, *latest_bkg; 
				IplImage  *background_image, *foreground_img;  //deprecated
				IplImage  *last_image, *last_foreground_image; //deprecated


				//CENCARA2_2Detector *ENCARAFaceDetector;
				 corePoint2D<int> presenceCenterPos, presenceRec_a, presenceRec_b;

				 CvGaussBGStatModelParams* background_params;
				 CvBGStatModel *background_model, *bg_trainning_model;
				 //CvMoments foreground_moments;
				 CvMoments *foreground_moments;
				 double presence_area, 
					    presence_orientation, 
						presence_eccentricity;
				 int background_trainning_frames, 
					 background_is_trained,
					 background_train_lapse;
				 bool updated, first_time;
		};
	}
}

#endif