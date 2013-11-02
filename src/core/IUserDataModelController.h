#ifndef _IUSERDATAMODELCONTROLLER_
#define _IUSERDATAMODELCONTROLLER_

#include <core/types.h>
#include <core/Export.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace core
{

	class IUserDataModelController
	{
		public:
			virtual ~IUserDataModelController() {};

			virtual void Clear() = 0;
			static IUserDataModelController* GetInstance() {return instance;}
			virtual void InsertUserDataElement(const core::UserDataElement &value) = 0;
			virtual std::vector<core::UserDataElement> GetUserDataElements(const core::Rect3F &search_rect) = 0;
			
			virtual void SetHeadPosition(const core::corePoint3D<double> &value) = 0;
			virtual void SetCenterOfMass(const core::corePoint3D<double> &value) = 0;
			virtual void SetCenterOfMassToHead(const core::corePoint3D<double> &value) = 0;
			virtual void SetBoundingBox(const core::corePoint3D<double> &min, const core::corePoint3D<double> &max) = 0;
			virtual void SetSpaceCenter(const core::corePoint3D<double> &value) = 0;
			virtual void SetThresholdDistanceToMinMax(const core::corePoint3D<double> &value) = 0;
			virtual void SetSpaceOffset(const core::corePoint3D<double> &value) = 0;
			virtual void SetPresenceDetected(const bool &value) = 0;
			virtual void SetMotionElements(const std::vector<MotionElement> &value) = 0;

			virtual std::vector<MotionElement> GetMotionElements() = 0;
			virtual bool GetPresenceDetected() = 0;
			virtual core::corePoint3D<double> GetHeadPosition() = 0;
			virtual core::corePoint3D<double> GetCenterOfMass() = 0;
			virtual core::corePoint3D<double> GetCenterOfMassToHead() = 0;
			virtual core::corePoint3D<double> GetSpaceCenter() = 0;
			virtual core::corePoint3D<double> GetThresholdDistanceToMinMax() = 0;
			virtual core::corePoint3D<double> GetSpaceOffset() = 0;
			virtual void GetBoundingBox( core::corePoint3D<double> &min, core::corePoint3D<double> &max) = 0;

	protected:
			static IUserDataModelController* instance;

	};
}
#endif