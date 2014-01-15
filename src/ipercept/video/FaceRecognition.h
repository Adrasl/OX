#ifndef _FACERECOGNITION_
#define _FACERECOGNITION_

#include <core/IPercept/video/IFaceRecognition.h>
#include <core/IPercept/IPerceptVideo.h>
#include <ipercept/CamWindow.h>

#include <string>
#include <vector>

#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>

#include <boost/thread.hpp>

namespace core
{
	namespace ipercept
	{
		class FaceRecognition : public core::IFaceRecognition
		{
			public:

				FaceRecognition(IApplicationConfiguration *appconfig);
				virtual ~FaceRecognition();

				virtual void Train();
				virtual int RecognizeFromImage(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step);
				virtual void AddUser(std::vector<core::Image> faces, const int &user_id);

			private:

				IApplicationConfiguration *app_config;
				CvHaarClassifierCascade* cascade;

				int     face_width, face_height;	
				float * projected_test_face;

				IplImage ** eigen_vector_pp;
				std::vector<IplImage *> eigen_vector;
				std::vector<IplImage *> face_vector; 
				CvMat	 *facepic_to_person_indexes;
				CvMat	 *projected_trainning_faces;
				CvMat	 *eigen_values;
				IplImage *average_image;

				bool	  initialized;
				bool LoadTrainningData();
				void StoreTrainningData();
				int  FindBestCandidate(float *projected_test_face_, float &likeness,  const bool &use_mahalanobis = false);
				IplImage* convertFloat32ToUchar8(const IplImage *src);
		};
	}
}

#endif

