#ifndef _IPERCEPTAUDIO_
#define _IPERCEPTAUDIO_

#include <string>
#include <core/Export.h>

namespace core
{
	class _COREEXPORT_ IPerceptAudio
	{
		public:

			virtual ~IPerceptAudio(){}
			virtual void Delete()=0;
			virtual void Init()=0;

	};
}


#endif

