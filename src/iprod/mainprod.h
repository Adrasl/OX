#ifndef _MAINPROD_
#define _MAINPROD_

#include <core/IProd/IProd.h>
#include <core/IApplication.h>
#include <iprod/Prod3DWindow.h>
#include <iprod/Prod3DEntity.h>
#include <iprod/MeshFactory.h>
#include <string>

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <modelPool.h>
#include <NodePathCollection.h>
#include <GeomVertexFormat.h>
#include <GeomVertexData.h>
#include <GeomVertexWriter.h>
#include <GeomTriangles.h>
#include <fog.h>
#include <collisionHandlerQueue.h>
#include <collisionTraverser.h>
#include <collisionSphere.h>
#include <collisionTube.h>
#include <collisionBox.h>
#include <collisionNode.h>
#include <boundingSphere.h>
//#include <audioManager.h>
//#include <audioSound.h>
//#include <virtualFileSystem.h>
#include <filename.h>
#include <texturePool.h>

#include <genericAsyncTask.h>
#include <asyncTaskManager.h>

#include "cIntervalManager.h"
#include "cLerpNodePathInterval.h"
#include "cMetaInterval.h"

#include <boost/thread.hpp>

#include <SFML/Audio.hpp>
//#include <SFML/Audio/SoundRecorder.hpp>

#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>
#include <core/IPersistence/IEntityPersistence.h>
#include <debugger.h>

//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>

namespace core
{
	namespace iprod
	{


		////--------------------------
		////SERIALIZE ME
		//class NodoSerio 
		//{
		//	private:
		//		friend class boost::serialization::access;
		//		template<class Archive>
		//		void serialize(Archive & ar, const unsigned int version)
		//		{
		//			ar & nodo;
		//		}
		//		//PandaNode nodo;
		//		NodePath nodo;
		//	public:
		//		NodoSerio(NodePath value) :nodo(value)	{}
		//};
		////--------------------------

		//--------------------------
		//SERIALIZE ME
		/*class NodoSerio
		{
			private:
				friend class boost::serialization::access;
				template<class Archive>
				void serialize(Archive & ar, const unsigned int version)
				{
					ar & mierda;
				}
				int mierda;
			public:
				NodoSerio()	: mierda(666) {}
		};*/
		//--------------------------



		class MainProd : public core::IProd
		{
			public:

				MainProd(IApplicationConfiguration *app_config_, int argc, char *argv[]);
				virtual ~MainProd();

				virtual void Delete();
				virtual void SetApp(IApplication *app_) {app = app_;}
				virtual void SetAppConfig(IApplicationConfiguration *app_config_) {app_config = app_config_;}
				virtual void Init();
				virtual void DoStuff();
				virtual void OpenWindow();

				void PostLogMessage(const std::string &message);
				bool RunWorld(core::IUserPersistence  *user, core::IWorldPersistence *world);
				void CloseWorld();

				virtual void SetUserPosition(const core::corePoint3D<double> &pos);
				virtual void SetCamerasPDU(core::corePDU3D<double> pdu);

				virtual void InsertEntityIntoCurrentWorld(core::IEntity * ent);
				virtual void RemoveEntityFromCurrentWorld(core::IEntity * ent);
				virtual void LoadEntityFromCurrentWorld(core::IEntity * ent); //if an insertion is not needed because it has been already registered
				virtual void InsertEntityIntoScene(core::IEntityPersistence* ent);
				virtual void InsertEntityIntoScene(core::IEntityPersistence* ent, std::vector<float> source_data, int row_step=3);
				virtual void SetAvatar(const std::string model);
				virtual void SetAvatar(std::vector<float> source_data, int row_step=3);
				virtual void SetAvatar(void *graphic_node);
				virtual void* CreateGraphicNode(std::vector<float> source_data, int row_step=3);
				virtual void* CreateGraphicNode(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data);
				virtual void RegisterEntity(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data); //retomar regiter entity
				virtual void CreateAndRegisterEntity(std::map< int, std::vector<corePDU3D<double>> > source_weighted_data); //retomar regiter entity

				static NodePath* CreateQuad();
				static void AddTestQuad();
			
			private:

				static void AddProd3DEntityToLoadQueue(Prod3DEntity* entity);
				static void ProcessProd3DEntitiesToBeLoadedQueue();
				static void PrivateRemoveEntityFromCurrentWorld(core::IEntity * ent);

				static core::IUserPersistence				*current_user;
				static core::IWorldPersistence				*current_world;
				static core::IEntityPersistence				*user_dummyPersistence;
				static Prod3DEntity							*user_entity;
				static NodePath								*user_nodepath;
				static std::vector<Prod3DEntity *>			scene_entities;
				static std::map<Prod3DEntity*, NodePath*>	scene_entities_nodepaths;
				static std::map<NodePath*, Prod3DEntity*>	scene_nodepaths_entities;
				static std::vector<NodePath*>				testnodepaths;
				static CollisionHandlerQueue				*collision_handler_queue;
				static CollisionTraverser					*collision_traverser;

				static std::map< Prod3DEntity *, CollisionNode * >					entity_collider_array;
				static std::map< const CollisionSolid *, core::corePDU3D<double> >	avatar_collider_array;
				static NodePath														*avatar_current_graphicNodePath;
				static std::map< const CollisionSolid *, Prod3DEntity* >			entities_collider_array;
				static std::vector< Prod3DEntity * >								entity_collidable_array_to_register;
				static std::vector< Prod3DEntity * >								entity_array_to_be_loaded;
				

				static core::corePoint3D<double> pt0, pt1, pti, vel, vel0, vel1, acc;
				static double last_interval, last_time, last_loop_t;
				static core::corePDU3D<double> last_pdu;
				static void CheckCollisions();
				static void UpdateEntities();
				static void ClearAvatarModel();
				void SetUpUser(void *graphic_node = NULL);

				//---destroy when ready---------------------
				//static NodePath environment, pandaActor, pandaActor2, pandaActor3, pandaActor4;
				//------------------------------------------

			private:

				static void LoadEntityIntoScene(Prod3DEntity * entity);
				static void RemoveEntityFromScene(Prod3DEntity * entity);



				static bool insert_now;

				IApplication* app;
				MeshFactory *mesh_factory;
				static IApplicationConfiguration *app_config;
				static void DoInit();
				static void DoDoStuff();
				static void PlaySoundCapture();
				static void DoMainLoop();
				static void Iterate();
				static void CreateDefaultWindows(int numWindows);
				static void LoadDefaultScene();
				static void LoadEmptyScene();
				static void ClearScene();

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
				static std::vector< NodePath >			camera_array;
				static int last_window_id, m_argc;
				static char **m_argv;
				static NodePath cam_viewpoint;
				static NodePath origin, up;

				static int dummy_erase_me;

				//static 		PT(AudioManager) AM;
				//static		PT(AudioSound) mySound;
				static		VirtualFileSystem* vfs;
				//static		string filename;

				//sfml
				static sf::SoundBuffer sound_Buffer;
				static double listener_position[];
				static double listener_target[];
				static double sound_pos[];
				static sf::Sound Sound;
				static PT(Fog) m_fog;

				static unsigned int num_windows;

				//static sf::SoundBufferRecorder Recorder;
				//static sf::SoundBuffer recordingBuffer;
				static float m_peo, m_caca;
				static bool use_master_display;
				static Prod3DWindow*	master_prod3Dwindow;
				static WindowFramework*	master_pandawindow;
				static NodePath master_camera;

				//music stuff
				std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > music_melody_samples;
				std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > music_base_samples;
				std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > music_decoration_samples;

		};

	}
}

#endif


