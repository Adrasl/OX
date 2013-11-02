#ifndef _IFACEDETECTION_
#define _IFACEDETECTION_

#include <string>
#include <core/Export.h>
#include <core/types.h>

namespace core
{
	class _COREEXPORT_ IFaceDetection
	{
		public:

			virtual ~IFaceDetection(){}
			virtual void Delete()=0;
			virtual void Init()=0;

			virtual bool FaceDetected() = 0;
			virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data) = 0;
			virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			virtual char * GetCopyOfAreaOfInterest(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			
			virtual void GetFaceCenterPos(corePoint2D<int> &pos) = 0;
			virtual void GetFaceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b) = 0;
			virtual void GetFeaturePos(const std::string &feature, corePoint2D<int> &pos) = 0;

		private:
			/** \brief Apply detection to image, return true if there is a face detected. **/
			virtual bool Apply() = 0;
	};
}


#endif

