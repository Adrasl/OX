#include <PerceptAudio.h>

#include <debugger.h> 

using namespace core::ipercept;

boost::try_mutex PerceptAudio::m_mutex;
boost::shared_ptr<boost::thread> PerceptAudio::m_thread;
bool PerceptAudio::initialized = false;
bool PerceptAudio::stop_requested = false;

sf::SoundBufferRecorder PerceptAudio::Recorder;
sf::SoundBuffer PerceptAudio::recordingBuffer;


PerceptAudio::PerceptAudio()
{
	if (!initialized)
	{
		//nothing so far
	}
}

PerceptAudio::~PerceptAudio()
{
	Recorder.Stop();
}

void PerceptAudio::Delete()
{
	stop_requested = true;
	assert(m_thread);
    m_thread->join();
}

void PerceptAudio::Init()
{
	PerceptAudio::DoInit();
}

void PerceptAudio::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&PerceptAudio::DoMainLoop)));
	}
}

void PerceptAudio::DoMainLoop()
{
	initialized = true;
	
	Recorder.Start();

	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(1000));
	}
}

void PerceptAudio::Iterate()
{
	boost::try_mutex::scoped_try_lock lock(m_mutex);
	if (lock)
	{
		Capture();
	}
}

void PerceptAudio::Capture()
{
	Recorder.Stop();
	recordingBuffer = Recorder.GetBuffer();
	Recorder.Start();
}