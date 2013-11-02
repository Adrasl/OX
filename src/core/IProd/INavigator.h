#ifndef _INAVIGATOR_
#define _INAVIGATOR_

#include <string>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ INavigator
	{
		public:

			virtual ~INavigator(){}
			virtual void Delete()=0;

	};
}


#endif

