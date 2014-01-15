#ifndef _OXBOID_ENTITY_
#define _OXBOID_ENTITY_

#include <winsock2.h>

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>
#include <icog/CommonSwarmIndividual.h>
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

#include <Entities/OXEntityTypes.h>


namespace core
{
	namespace iprod
	{
		class OXBoidsEntity : public iprod::Prod3DEntity, public core::icog::CommonSwarmIndividual
		{
			public:

				OXBoidsEntity(){};
				OXBoidsEntity(core::IEntityPersistence* ent, 
							  const int &type_ = Species::SPECIES1,
 							  const float &max_acceleration_	= 0.05f,
							  const float &max_velocity_		= 0.05f,
							  const float &perception_distance_	= 3.5f,
							  const float &separation_distance_	= 2.0f,
							  const float &avoidance_distance_	= 2.0f,
							  const float &randomness_factor_	= 1.0f,
							  const float &separation_factor_	= 1.5f,
							  const float &alignment_factor_	= 0.25f,
							  const float &cohesion_factor_		= 0.5f,
							  const float &attraction_factor_	= 1.0f,
							  const float &avoidance_factor_	= 1.5f,
							  const float &worldlimits_factor_	= 2.5f,
							  const float &pitch=1.0f, const float &amplitude=100.0f);
				virtual ~OXBoidsEntity();
				virtual void DeletePersistence();

				float GetTimeToLive()					{ boost::mutex::scoped_lock lock(m_mutex); return time_to_live;		}
				void  SetTimeToLive(const float &value)	{ boost::mutex::scoped_lock lock(m_mutex); time_to_live = value;	}

				virtual void OnStart();
				virtual void OnUpdate();
				virtual void OnDeath();
				virtual void OnDestroy();
				virtual void OnCollisionCall(IEntity *otherEntity); 
				virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo);


				virtual void ReceiveDamage();
				virtual void ReceivePleasure();
				virtual void BeEated();
				virtual void Copulate();

				virtual void SetPitch(const float &value);
				virtual void SetVolume(const float &value);


			protected:

				void KillMyself();

				float psique; //type of entity
				float karma;  //good or evil
				float energy; //calm or energyc
				bool  ignore_collisons, already_loaded_in_scene;
				double start_timestamp, latestupdate_timestamp, current_timestamp, killme_afterseconds, recovercollisions_afterseconds;
				double delta_time, lived_time, think_time;

				std::map<NatureOfEntity, float> otherEntities_feedback; //<natureofentity , karma> : karma = good(0.0) -> evil(1.0) 
				std::map<IEntity*, float> user_feedback;

		};
	}
}

#endif









