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
#include <filename.h>
#include <texturePool.h>

#include <genericAsyncTask.h>
#include <asyncTaskManager.h>

#include "ambientLight.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "spotlight.h"
#include "cIntervalManager.h"
#include "cLerpNodePathInterval.h"
#include "cMetaInterval.h"

#if PANDA_NUMERIC_VERSION >= 1008000
#define Colorf LColorf
#endif

#include "p3util/cCommonFilters.h"

#include <boost/thread.hpp>

#include <SFML/Audio.hpp>
//#include <SFML/Audio/SoundRecorder.hpp>

#include <core/IPersistence/IUserPersistence.h>
#include <core/IPersistence/IWorldPersistence.h>
#include <core/IPersistence/IEntityPersistence.h>
#include <debugger.h>

namespace core
{
	namespace iprod
	{
		class MainProd : public core::IProd
		{
			public:

				MainProd(IApplicationConfiguration *app_config_, int argc, char *argv[], bool enable_effects = false);
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

				virtual void AddNewEntitiesIntoCurrentWorld(const std::map<core::IEntity *, double> &new_entities_after_seconds);
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

				virtual std::vector<std::string> GetBackgroundSounds();
				virtual std::string  AddBackgroundSound(const std::string &file_name, const double &time_lerp=0.0);
				virtual void SetPitchBackgroundSound(const std::string &id, const float &value, const double &time_lerp=0.0);
				virtual void SetAmplitudeBackgroundSound(const std::string &id, const float &value, const double &time_lerp=0.0);
				virtual void RemoveBackgroundSound(const std::string &id, const double &time_lerp=0.0);
				virtual void RemoveAllBackgroundSound(const double &time_lerp=0.0);

				virtual void SetBackgroundColor(const float &R, const float &G, const float &B);
				virtual void SetFogColor(const float &R, const float &G, const float &B);
				virtual void SetFogIntensity(const float &intensity);
				virtual void SetBackgroundAndFog(const float &bg_R=0.5f, const float &bg_G=0.5f, const float &bg_B=0.5f, const float &f_R=0.5f, const float &f_G=0.5f, const float &f_B=0.5f, const float &intensity=0.5f, const float animation_time=.0f);

				//After activating these features it wont be possible to resize the windows and reorient them anymore
				virtual void EnableSimpleInverEffect(const bool &enable = true);
				virtual void EnableSimpleBloomEffect(const bool &enable = true);
				virtual void EnableSimpleToonEffect(const bool &enable = true);
				virtual void EnableSimpleBlurEffect(const bool &enable = true);
				virtual void EnableSimpleSSAOEffect(const bool &enable = true);
				virtual void EnableSimpleBackgroundVolumetricLightEffect(const bool &enable = true);

				static NodePath* CreateQuad();
				static void AddTestQuad();
			
			private:

				static void ProcessProd3DEntitiesToBeLoadedQueue();
				static void ProcessProd3DEntitiesToBeRemovedQueue();
				static void PrivateRemoveEntityFromCurrentWorld(core::IEntity * ent);
				static void InternalRemoveAllBackgroundSound(const double &time_lerp=0.0);

				static core::IUserPersistence				*current_user;
				static core::IUserPersistence				*default_user;
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

				static bool									enableEffects;
				static bool									enable_simpleINVERTEffect;
				static bool									enable_simpleBLOOMEffect;
				static bool									enable_simpleTOONEffect;
				static bool									enable_simpleBLUREffect;
				static bool									enable_simpleVOLUMETRICLIGHTSEffect;
				static bool									enable_simpleSSAOEffect;

				static std::map< Prod3DEntity *, CollisionNode * >					entity_collider_array;
				static std::map< const CollisionSolid *, core::corePDU3D<double> >	avatar_collider_array;
				static NodePath														*avatar_current_graphicNodePath;
				static std::map< const CollisionSolid *, Prod3DEntity* >			entities_collider_array;
				static std::vector< Prod3DEntity * >								entity_collidable_array_to_register;
				static std::map< Prod3DEntity *, double >							entity_array_to_be_loaded_afterseconds;

				static core::corePoint3D<double> pt0, pt1, pti, vel, vel0, vel1, acc;
				static double last_interval, last_time, last_loop_t;
				static core::corePDU3D<double> last_pdu;
				static void CheckCollisions();
				static void UpdateEntities();
				static void ClearAvatarModel();
				void SetUpUser(void *graphic_node = NULL);

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
				static void CheckBackgroundAndFogRanges();
				static bool CheckDesiredBackgroundAndFogRanges();

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
				static std::map<int, CCommonFilters*>	ccommonfilters_array;
				static std::map<int, DisplayRegion *>	displayregions_array;
				static std::vector< NodePath >			camera_array;
				static int last_window_id, m_argc;
				static char **m_argv;
				static NodePath cam_viewpoint;
				static NodePath origin, up;
				static int dummy_erase_me;
				static		VirtualFileSystem* vfs;

				//sfml
				static double listener_position[];
				static double listener_target[];
				//static sf::SoundBufferRecorder Recorder;
				//static sf::SoundBuffer recordingBuffer;				
				static sf::SoundBuffer sound_Buffer;
				static double sound_pos[];
				static sf::Sound Sound;

				static Fog m_fog;
				static corePoint3D<float> background_color, desired_background_color;
				static corePoint3D<float> fog_color, desired_fog_color;
				static float fog_intensity, desired_fog_intensity;
				static float current_timestamp, 
							 background_animation_starttimestamp, 
							 background_animation_endtimestamp;

				static unsigned int num_windows;

				static bool use_master_display;
				static Prod3DWindow*	master_prod3Dwindow;
				static WindowFramework*	master_pandawindow;
				static NodePath master_camera;

				//music stuff
				static std::map< std::string, core::coreSound<sf::Sound, sf::SoundBuffer> > music_melody_samples; //background sound tracks
				static std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > music_base_samples;
				static std::map< Prod3DEntity*, std::vector<core::coreSound<sf::Sound, sf::SoundBuffer>> > music_decoration_samples;

				static AnimControlCollection SceneAnimControlCollection;

				static NodePath* fake_background_quad; //Panda won't allow changing the background color while using
													  //post processing filters, so, we create a non-illuminated quad 
													  //relative to the cam, with that color...
				static NodePath* default_ambientLight;

		};

	}
}

#endif


