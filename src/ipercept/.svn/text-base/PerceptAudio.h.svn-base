#ifndef _PERCEPTAUDIO_
#define _PERCEPTAUDIO_

#include <core/IPercept/IPerceptAudio.h>

#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundRecorder.hpp>

namespace core
{
	namespace ipercept
	{
		class PerceptAudio : public core::IPerceptAudio
		{
			public:

				PerceptAudio();
				virtual ~PerceptAudio();
				virtual void Delete();
				virtual void Init();

			private:

				static void DoInit();
				static void DoMainLoop();
				static void Iterate();
				static void Capture();

				static boost::shared_ptr<boost::thread> m_thread;
				static boost::try_mutex m_mutex;
				static bool initialized, stop_requested;

				static sf::SoundBufferRecorder Recorder;
				static sf::SoundBuffer recordingBuffer;
		};
	}
}

#endif

