#ifndef _IMOTIONDETECTION_
#define _IMOTIONDETECTION_

#include <string>
#include <core/Export.h>
#include <core/types.h>

namespace core
{
	class _COREEXPORT_ IMotionDetection
	{
		public:

			virtual ~IMotionDetection(){}
			virtual void Delete()=0;
			virtual void Init()=0;

			//virtual bool Apply( Image input, const float &scale, int &pos_x, int &pos_y) = 0;
			virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data) = 0;
			virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			virtual char * GetCopyOfCurrentImageOpticalFlow(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			virtual std::vector<MotionElement> GetMotionElements() = 0;
			virtual corePoint3D<float> GetMotionAtCoords(corePoint2D<int> coords) = 0;
			
			//virtual void GetFaceCenterPos(corePoint2D<int> &pos) = 0;
			//virtual void GetFaceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b) = 0;
			//virtual void GetFeaturePos(const std::string &feature, corePoint2D<int> &pos) = 0;

	};
}


#endif

