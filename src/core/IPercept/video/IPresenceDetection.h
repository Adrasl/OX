#ifndef _IPRESENCEDETECTION_
#define _IPRESENCEDETECTION_

#include <string>
#include <core/Export.h>
#include <core/types.h>

namespace core
{
	class _COREEXPORT_ IPresenceDetection
	{
		public:

			virtual ~IPresenceDetection(){}
			virtual void Delete()=0;
			virtual void Init()=0;
			virtual void TrainBackground() = 0;

			/** \brief Feeds the detector with a new image. **/
			virtual void SetCurrentImage(const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step, char * data) = 0;
			/** \brief Obtains current processed image. **/
			virtual char * GetCopyOfCurrentImage(int &size_x, int &size_y, int &n_channels, int &depth, int &width_step, const bool &switch_rb = false) = 0;
			/** \brief Obtains current center of mass coordinates. **/
			virtual void GetPresenceCenterOfMass(corePoint2D<int> &pos) = 0;
			/** \brief Obtains current presence rectangle. **/
			virtual void GetPresenceRec(corePoint2D<int> &corner_a, corePoint2D<int> &corner_b) = 0;
			/** \brief Obtains current presence area. **/
			virtual void GetPresenceArea(double &area_inpixel_units) = 0;
			/** \brief Obtains current presence orientation. Mayor axis, counterclockwise**/
			virtual void GetPresenceOrientation(double &radians_counterclockwise) = 0;
			/** \brief Obtains current presence eccentricity. Zero means rounded. One means a line.**/
			virtual void GetPresenceEccentricity(double &cero_means_round) = 0;
			/** \brief Obtains current presence rectangle. **/
			virtual bool PresenceDetected() = 0;

		private:
			/** \brief Apply detection to image, return true if there is a presence detected. **/
			virtual bool Apply() = 0;

	};
}


#endif

