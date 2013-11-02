#ifndef _MOTIONDETECTION_
#define _MOTIONDETECTION_

#include <core/IPercept/video/IMotionDetection.h>
#include <core/IPercept/IPerceptVideo.h>
#include <ipercept/CamWindow.h>

#include <string>
#include <vector>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <core/types.h>

namespace core
{
	namespace ipercept
	{
		class MotionDetection : public core::IMotionDetection
		{
			public:

				MotionDetection(IPerceptVideo* video_perception, const int camera_index);
				virtual ~MotionDetection();
				virtual void Delete();
				virtual void Init();
				//virtual bool Apply( Image input, const float &scale, int &pos_x, int &pos_y);

				int GetCamIndex() { return cam_index; } ;
				virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data);
				virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				virtual std::vector<MotionElement> GetMotionElements();
				//virtual void GetFeatureROIArray(const std::string &feature, std::vector<Rect3F> &result);

				//virtual void GetFaceCenterPos(corePoint2D<int> &pos);
				//virtual void GetFaceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b);
				//virtual void GetFeaturePos(const std::string &feature, corePoint2D<int> &pos);

			private:

				void DoInit();
				void DoMainLoop();
				void Iterate();
				void Capture();
				void Process();

				//void detect_and_draw();

				boost::shared_ptr<boost::thread> m_thread;
				boost::try_mutex m_mutex;
				boost::try_mutex m_mutex_motion_areas;
				bool initialized, stop_requested, updated;

				IPerceptVideo* v_perception;
				IplImage  *image;

				//---------------
				int cam_index, N; // number of cyclic frame buffer used for motion detection (should, probably, depend on FPS)
				IplImage **buf;
				int last;
				IplImage *mhi; // MHI
				IplImage *orient; // orientation
				IplImage *mask; // valid orientation mask
				IplImage *segmask; // motion segmentation map
				CvMemStorage* storage; // temporary storage
				IplImage* motion;
				void UpdateMHI( IplImage* img, IplImage* dst, int diff_threshold );
				double las_time;
				//---------------

				//CENCARA2_2Detector *ENCARAFaceDetector;
				 corePoint2D<int> faceCenterPos, faceRec_a, faceRec_b;
				 std::vector<MotionElement> motion_elements;
		};
	}
}

#endif