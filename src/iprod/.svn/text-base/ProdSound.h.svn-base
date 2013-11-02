#ifndef _SOUNDPROD_
#define _SOUNDPROD_

#include <string>

#include <SFML/Audio.hpp>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace core
{
	namespace iprod
	{
		class ProdSound
		{
			public:

				ProdSound();
				virtual ~ProdSound();
				virtual void Delete();
				virtual void Init();

			private:

				static boost::shared_ptr<boost::thread> m_thread;
				static boost::try_mutex m_mutex;
				
				static bool initialized, stop_requested;

				static void DoMainLoop();
				static void Iterate();
				static void DoInit();

		};

	}
}

#endif

