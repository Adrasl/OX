#ifndef _CONTENTCREATIONCONTROLLER_
#define _CONTENTCREATIONCONTROLLER_

#include <boost/parameter/aux_/parameter_requirements.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/thread.hpp>

#include <Application.h>

#include <Entities/OXStandAloneEntity.h>
#include <Entities/OXBoidsEntity.h>
#include <Entities/OXTreeEntity.h>
#include <RTreeTemplate/RTree.h>

#include <core/types.h>
#include <vector>
#include <random>

using namespace boost::accumulators;

enum NatureOfEntity {
      NONINTERACTIVE= 0,
	  STANDALONE	= 1,
      BOID			= 2,
      TREE			= 3
   };



class Application;

class ContentCreationController : public core::Observer
{
	static enum IA_Karma {	GOOD,
							NEUTRAL,
							EVIL};

	static enum IA_Energy{	CALM,
							EXITED};
	public:

		static ContentCreationController* Instance();
		static void SetApp(IApplication *app_, core::IApplicationConfiguration* iapp_config_=NULL, IPercept *app_mainpercept_=NULL, IProd *app_mainprod_=NULL);

		static void Update();
		static void Clear();
		static void Reset();

		static void SetAudioInfo(){};
		static void SetUserArea() {};
		static void SetUserFace() {};
		static void SetUserIsPresent() {};
		static void SetUserHeadPosition() {};
		static void SetUserCenterOfMass() {};
		static void SetUserExcentricity() {};
		static void SetUserAverageVelocity(){};

		static void GetAudioInfo(){};
		static void GetUserArea() {};
		static void GetUserFace() {};
		static void GetUserIsPresent() {};
		static void GetUserHeadPosition() {};
		static void GetUserCenterOfMass() {};
		static void GetUserExcentricity() {};
		static void GetUserAverageVelocity(){};

		static void GetAllEntities(){};
		static void GetLabeledEntities(const std::string label){};
		static void EntityHadAGoodUserFeedback(const bool &was_good);

		static void RemoveEntityFromCurrentWorld(core::IEntity *entity);

		virtual void Notified();
	
	protected:

		ContentCreationController();
		ContentCreationController(const ContentCreationController & ) ;
		ContentCreationController &operator = (const ContentCreationController & ) ;
		
		static void DoNotified();
		static void RestartCurrentUserBackgroundAndFog();

		static void CreatePresetOfEntities1(const double &time = 1.0f);
		static void CreatePresetOfEntities2(const double &time = 1.0f);

	private:

		//static boost::shared_ptr<boost::thread> m_thread;
		static boost::mutex m_mutex;

		static IApplication *app;
		static IApplicationConfiguration* iapp_config;
		static IPercept		*app_mainpercept;
		static IProd		*app_mainprod;

		static ContentCreationController *instance;
		static double start_timestamp, 
			          latest_timestamp, 
					  current_timestamp,
					  music_timestamp,
					  createdEntity_timesptamp,
					  recover_collisionevaluation_aftertime;
		static bool must_change_background, must_change_music;
		static int z_step;


		static float psique; //0-1 good-evil
		static float energy; //0-1 calm-energetic

		static std::map<int, std::vector<std::string>> psique_melody;
		static std::string current_melody;

		static int background_sound;
		void * ambient_soundsample;

		static std::map<IA_Karma, corePoint3D<float>> background_color;
		static std::map<IA_Karma, corePoint3D<float>> fog_color;
		static std::map<IA_Karma, float> fog_intensity;
		static corePoint3D<float> current_background_color;
		static corePoint3D<float> current_fog_color;
		static float			  current_fog_intensity;
		static IA_Karma			  i_am_being;

		static std::map<int, core::IEntityPersistence*> RTree_Entities_by_entityIDs;
		static std::map<NatureOfEntity, RTree<int, float, 3, float> *> RTree_Entities_SpatialIndexes;
		static std::map<NatureOfEntity, std::vector<core::IEntityPersistence*>> RTree_Entities_by_Psique;
		static IWorldPersistence* current_world;
		static IUserPersistence*  current_user;
		static int entity_id;

		static RTree<int, float, 3, float> RTree_Default;
		static RTree<int, float, 3, float> RTree_StandAlone;
		static RTree<int, float, 3, float> RTree_BOIDs;
		static RTree<int, float, 3, float> RTree_Tree;

		static std::map<core::IEntity *, double> new_timed_entities;


		std::tr1::ranlux64_base_01 randomEngine;
		//std::tr1::normal_distribution<double> randomDistribution(-20.0,20.0);

		//http://composicionmusical.com/oido-musical/elementos-musicales
		//sonido: duración/tiempo, timbre/textura, intensidad/volumen, altura(grave, agudo)
		//elementos: ritmo, melodía, contrapunto, armonía.
		//ritmo: patrón de repetición
		//melodía: combinación de sonidos a partir de un ritmo
		//contrapunto: melodías que se enfrentan a la principal, regido por las leyes de la armonía.
		


};

#endif