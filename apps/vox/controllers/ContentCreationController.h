#ifndef _CONTENTCREATIONCONTROLLER_
#define _CONTENTCREATIONCONTROLLER_

#include <Application.h>

#include <Entities/OXStandAloneEntity.h>
#include <Entities/OXBoidsEntity.h>
#include <Entities/OXTreeEntity.h>

#include <boost/thread.hpp>
#include <core/types.h>
#include <vector>

enum NatureOfEntity {
      STANDALONE	= 1,
      BOID			= 2,
      TREE			= 3
   };

class Application;

class ContentCreationController
{
	public:

		static ContentCreationController *Instance();
		void SetApp(IApplication *app_) {app = app_;}

		void Update();

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
	
	protected:

		ContentCreationController();
		ContentCreationController(const ContentCreationController & ) ;
		ContentCreationController &operator = (const ContentCreationController & ) ;

	private:

		static IApplication *app;
		static ContentCreationController *instance;
		static double time_start;

		boost::shared_ptr<boost::thread> m_thread;
		boost::try_mutex m_mutex;

		float psique; //0-1 good-evil
		float energy; //0-1 calm-energetic

		void * ambient_soundsample;
		void * background_color;


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