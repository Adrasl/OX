#ifndef _ENCARA2FACEDETECTION_
#define _ENCARA2FACEDETECTION_

#include <core/IPercept/video/IFaceDetection.h>
#include <core/IPercept/IPerceptVideo.h>
#include <ipercept/CamWindow.h>

#include <string>
#include <vector>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
//ENCARA2
#include <ENCARA2_2lib.h>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace core
{
	namespace ipercept
	{
		class Encara2FaceDetection : public core::IFaceDetection
		{
			public:

				Encara2FaceDetection(IPerceptVideo* video_perception, const int camera_index);
				virtual ~Encara2FaceDetection();
				virtual void Delete();
				virtual void Init();

				virtual bool FaceDetected();
				int GetCamIndex() { return cam_index; } ;
				virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				virtual char * GetCopyOfAreaOfInterest(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false);
				virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data);

				virtual void GetFaceCenterPos(corePoint2D<int> &pos);
				virtual void GetFaceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b);
				virtual void GetFeaturePos(const std::string &feature, corePoint2D<int> &pos);

			private:

				void DoInit();
				void DoMainLoop();
				void Iterate();
				void Capture();
				void Process();
				virtual bool Apply();

				IplImage* Sub_Image(IplImage *image, CvRect roi);

				boost::shared_ptr<boost::thread> m_thread;
				boost::try_mutex m_mutex, face_data_mutex;
				bool initialized, stop_requested, updated, face_detected;

				IPerceptVideo* v_perception;
				IplImage  *image, *face_img;
				int cam_index;
				double las_time;

				CENCARA2_2Detector *ENCARAFaceDetector;
				corePoint2D<int> faceCenterPos, faceRec_a, faceRec_b;
		};
	}
}

#endif

