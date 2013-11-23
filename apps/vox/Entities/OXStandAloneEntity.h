#ifndef _OXSTANDALONE_ENTITY_
#define _OXSTANDALONE_ENTITY_

#include <winsock2.h>

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>
#include <iprod/Prod3DEntity.h>
#include <ipersistence/EntityPersistence.h>

#include <controllers/ContentCreationController.h>

#include <string>
#include <vector>

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <modelPool.h>
#include <NodePathCollection.h>
#include <GeomVertexFormat.h>
#include <GeomVertexData.h>
#include <GeomVertexWriter.h>
#include <GeomTriangles.h>

//#include <pandaFramework.h>

//using namespace core;

namespace core
{
	namespace iprod
	{
		//retomar, esto debería ir en otro fichero común
		enum NatureOfEntity {
								STANDALONE	= 1,
								BOID			= 2,
								TREE			= 3};

		static enum IA_State {	BORN,
								HAPPY,
								ANGRY,
								SCARED,
								HUNGRY,
								HORNY,
								DEAD };

		static enum IA_Karma {	GOOD,
								EVIL};

		static enum IA_Energy{	CALM,
								EXITED};

		class OXStandAloneEntity : public iprod::Prod3DEntity
		{
			public:

				OXStandAloneEntity(core::IEntityPersistence* ent);
				virtual ~OXStandAloneEntity();

				float GetTimeToLive()					{ return timeToLive;		}
				void  SetTimeToLive(const float &value)	{ timeToLive = value;		}

				virtual void Delete();
				virtual void Destroy();
				virtual void OnStart();
				virtual void OnUpdate();
				virtual void OnDeath();
				virtual void OnCollisionCall(IEntity *otherEntity); 
				virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo);
				virtual void PlaySound(const std::string &label, const bool &loop);
				virtual void PlayAnimation(const std::string &label);

				virtual void ReceiveDamage();
				virtual void ReceivePleasure();
				virtual void BeEated();
				virtual void Copulate();


			private:

				float timeToLive;
				float psique; //type of entity
				float karma;  //good or evil
				float energy; //calm or energyc
				double start_timestamp, latestupdate_timestamp, current_timestamp;
				double delta_time, lived_time;

				std::map<NatureOfEntity, float> otherEntities_feedback; //good(0) --> evil(1) 

				//map::<double, std::string> feedback_from_otherEntities; //<feedback, EntityLabel>, feedback < 0.50 bad, >0.50 good;

		};
	}
}

#endif

