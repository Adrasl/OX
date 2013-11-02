#ifndef _ICAMERA_
#define _ICAMERA_

#include <string>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ ICamera
	{
		public:

			virtual ~ICamera(){}
			virtual void Delete()=0;

	};
}


#endif

