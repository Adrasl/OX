#ifndef _IFACERECOGNITION_
#define _IFACERECOGNITION_

#include <string>
#include <vector>
#include <core/Export.h>
#include <core/types.h>

namespace core
{
	class _COREEXPORT_ IFaceRecognition
	{
		public:

			virtual ~IFaceRecognition(){}
			virtual void Train() = 0;
			virtual int RecognizeFromImage(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step) = 0;
			virtual void AddUser(std::vector<core::Image> faces, const int &user_id) = 0;

		private:
	};
}

#endif

