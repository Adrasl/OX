#include <ProdSound.h>

#include <debugger.h> 

using namespace core::iprod;

boost::try_mutex ProdSound::m_mutex;
bool ProdSound::initialized = false;
bool ProdSound::stop_requested = false;
boost::shared_ptr<boost::thread> ProdSound::m_thread;

ProdSound::ProdSound()
{
}

ProdSound::~ProdSound()
{
	m_thread->join();
}

void ProdSound::Delete()
{
	stop_requested = true;
	m_thread->join();
	assert(m_thread);
}

void ProdSound::Init()
{
	ProdSound::DoInit();
}

void ProdSound::DoInit()
{
	if (!initialized)
	{
		assert(!m_thread);
		m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::function0<void>(&ProdSound::DoMainLoop)));
	}
}

void ProdSound::DoMainLoop()
{
	printf("Prueba SFML:\n");

	sf::SoundBuffer Buffer;
	if (!Buffer.LoadFromFile("f://etc//repos//OX//motor.wav"))
	{
		printf("Failed: LoadFromFile\n");
	}
	unsigned int chan = Buffer.GetChannelsCount();

	//sf::Sound Sound;
	//Sound.SetBuffer(Buffer);
	//Sound.SetLoop(true);
	////Sound.SetPitch(1.5f);
	////Sound.SetVolume(75.f);
	//Sound.SetVolume(100.f);

	//Sound.Play();
	//Sound.Pause();
	//Sound.Play();
	//Sound.Stop()

	float oyente_posicion[]	= {50.0f,50.0f,1.0f};
	float listener_target[]	= {0.0f,-1.0f,0.0f};
	float sound_pos[]		= {5.0f,5.0f,0.0f};

	sf::Listener::SetPosition(oyente_posicion[0], oyente_posicion[1], oyente_posicion[2]);
	sf::Listener::SetTarget(listener_target[0], listener_target[1], listener_target[2]);
	sf::Listener::SetGlobalVolume(100.f);

	sf::Sound Sound;
	Sound.SetBuffer(Buffer);
	Sound.SetLoop(true);
	Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);
	Sound.SetRelativeToListener(true);
	Sound.SetMinDistance(2.f);
	Sound.SetAttenuation(2.f);

	Sound.Play();

	int POS_limit = 10;

	printf("Listener se mueve en X:\n");
	sound_pos[0] = 0;
	while(sound_pos[0] < POS_limit)
	{	sound_pos[0] += 0.000005f; 
		Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);}
	sound_pos[0] = 0;
	printf("Listener se mueve en Y:\n");
	while(sound_pos[1] < POS_limit)
	{	sound_pos[1] += 0.000005f; 
		Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);}
	sound_pos[1] = 0;
	printf("Listener se mueve en Z:\n");
	while(sound_pos[2] < POS_limit)
	{	sound_pos[2] += 0.000005f; 
		Sound.SetPosition(sound_pos[0], sound_pos[1], sound_pos[2]);}


	initialized = true;

	while(!stop_requested)
	{
		Iterate();
		m_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(1000));
	}

    /* Paramos la fuente antes de finalizar */
    Sound.Stop();
}

void ProdSound::Iterate()
{

}
