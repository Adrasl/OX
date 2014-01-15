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

			/** \brief Feeds the detector with a new image. **/
			virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data) = 0;
			/** \brief Obtains current processed Motion History Image. **/
			virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			/** \brief Obtains current processed Optical Flow image. **/
			virtual char * GetCopyOfCurrentImageOpticalFlow(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			/** \brief Obtains currently detected motion elements **/
			virtual std::vector<MotionElement> GetMotionElements() = 0;
			/** \brief Obtains movement vector, given the coordinates **/
			virtual corePoint3D<float> GetMotionAtCoords(corePoint2D<int> coords) = 0;
		

	};
}


#endif

