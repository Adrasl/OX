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

class ContentCreationController
{
	public:

		static ContentCreationController *Instance();
		void SetApp(IApplication *app_, IPercept *app_mainpercept_=NULL, IProd *app_mainprod_=NULL) 
		{app = app_; app_mainpercept=app_mainpercept_; app_mainprod=app_mainprod_;}

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

		static void RemoveEntityFromCurrentWorld(core::IEntity *entity);
	
	protected:

		ContentCreationController();
		ContentCreationController(const ContentCreationController & ) ;
		ContentCreationController &operator = (const ContentCreationController & ) ;

	private:

		static IApplication *app;
		static IPercept		*app_mainpercept;
		static IProd		*app_mainprod;

		static ContentCreationController *instance;
		static double time_start;
		static int z_step;

		//static boost::shared_ptr<boost::thread> m_thread;
		static boost::try_mutex m_mutex;

		float psique; //0-1 good-evil
		float energy; //0-1 calm-energetic

		void * ambient_soundsample;
		void * background_color;

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


		std::tr1::ranlux64_base_01 randomEngine;
		//std::tr1::normal_distribution<double> randomDistribution(-20.0,20.0);


		//void *current_ambient_channel_01;
		//void *current_ambient_channel_02;
		//void *current_ambient_channel_03;
		//map::<int, void *> calm_ambients;
		//map::<int, void *> energetic_ambients;
		//map::<int, void *> good_ambients;
		//map::<int, void *> evil_ambients;

		//http://composicionmusical.com/oido-musical/elementos-musicales
		//sonido: duración/tiempo, timbre/textura, intensidad/volumen, altura(grave, agudo)
		//elementos: ritmo, melodía, contrapunto, armonía.
		//ritmo: patrón de repetición
		//melodía: combinación de sonidos a partir de un ritmo
		//contrapunto: melodías que se enfrentan a la principal, regido por las leyes de la armonía.
		


};

#endif