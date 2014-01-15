#ifndef _IGUIWINDOW_
#define _IGUIWINDOW_

#include <string>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ IGuiWindow
	{
		public:

			virtual ~IGuiWindow(){}
			virtual void Show( const bool &value = true )=0;
			virtual void FullScreen( const bool &value = true )=0;

	};
}


#endif

