#ifndef _ICOG_
#define _ICOG_

#include <string>
#include <core/Export.h>
#include <core/IApplication.h>

namespace core
{
	class _COREEXPORT_ ICog
	{
		public:

			virtual ~ICog(){}
			virtual void Delete()=0;
			virtual void SetApp(IApplication *app_) = 0 ;

	};
}


#endif

