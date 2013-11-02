#ifndef _MAINPROD_
#define _MAINPROD_

#include <core/IProd/IProd.h>
#include <iprod/Prod3DWindow.h>
#include <string>

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <modelPool.h>
#include <audioManager.h>
#include <audioSound.h>
#include <virtualFileSystem.h>
#include <filename.h>

#include <genericAsyncTask.h>
#include <asyncTaskManager.h>

#include "cIntervalManager.h"
#include "cLerpNodePathInterval.h"
#include "cMetaInterval.h"

#include <boost/thread.hpp>

#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundRecorder.hpp>


namespace core
{
	namespace iprod
	{
		class MainProd : public core::IProd
		{
			public:

				MainProd(int argc, char *argv[]);
				virtual ~MainProd();

				virtual void Delete();
				virtual void Init();
				virtual void DoStuff();
				virtual void OpenWindow();

				static void DoInit();
				static void DoDoStuff();

				static void PlaySoundCapture();

			private:

				static void DoMainLoop();
				static void Iterate();
				static void CreateDefaultWindows(int numWindows);
				static void LoadDefaultScene();

				static AsyncTask::DoneStatus SpinCameraTask(GenericAsyncTask* task, void* data);


				static boost::shared_ptr<boost::thread> m_thread;
				static boost::try_mutex m_mutex;

				static PandaFramework framework;
				static Thread *graphic_thread;
				static PT(AsyncTaskManager) taskMgr;
				static PT(ClockObject) globalClock;

				static bool initialized, stop_requested;
				static std::map<int, Prod3DWindow*>		prod3Dwindow_array;
				static std::map<int, WindowFramework*>	pandawindows_array;
				static std::map<int, NodePath>			windowcamera_array;
				static int last_window_id, m_argc;
				static char **m_argv;
				static NodePath cam_viewpoint;
				static NodePath origin, up;

				//static 		PT(AudioManager) AM;
				//static		PT(AudioSound) mySound;
				static		VirtualFileSystem* vfs;
				//static		string filename;

				//////////////////////////////////////////////openal
				////////////////////////////////////////////static ALCdevice	*alDevice;
				////////////////////////////////////////////static ALCcontext	*alContext;
				////////////////////////////////////////////static ALenum		alError;

				////////////////////////////////////////////static ALuint  alBuffers[NUM_BUFFERS]; // Buffers hold sound data.
				////////////////////////////////////////////static ALuint  alSources[NUM_SOURCES]; // Sources are points of emitting sound.				
				////////////////////////////////////////////static ALfloat alSourcesPos[NUM_SOURCES][3]; // Position of the source sounds.
				////////////////////////////////////////////static ALfloat alSourcesVel[NUM_SOURCES][3]; // Velocity of the source sounds.
				////////////////////////////////////////////static ALfloat ListenerPos[];
				////////////////////////////////////////////static ALfloat ListenerVel[];
				////////////////////////////////////////////static ALfloat ListenerOri[]; // Orientation of the listener. (first 3 elements are "at", second 3 are "up")

				////////////////////////////////////////////static ALenum format;
				////////////////////////////////////////////static ALsizei size;
				////////////////////////////////////////////static ALvoid* data;
				////////////////////////////////////////////static ALsizei freq;
				////////////////////////////////////////////static ALboolean loop;

				//sfml
				static sf::SoundBuffer sound_Buffer;
				static double listener_position[];
				static double listener_target[];
				static double sound_pos[];
				static sf::Sound Sound;

				static sf::SoundBufferRecorder Recorder;
				static sf::SoundBuffer recordingBuffer;
				static float m_peo, m_caca;

		};

	}
}

#endif

