#ifndef _OXSTANDALONE_ENTITY_
#define _OXSTANDALONE_ENTITY_

#include <winsock2.h>

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>
#include <Entities/OXEntityTypes.h>
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


namespace core
{
	namespace iprod
	{
		class OXStandAloneEntity : public iprod::Prod3DEntity
		{
			public:

				OXStandAloneEntity(){};
				OXStandAloneEntity(core::IEntityPersistence* ent, const float &pitch=1.0f, const float &amplitude=100.0f);
				virtual ~OXStandAloneEntity();
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
				double delta_time, lived_time;

				std::map<NatureOfEntity, float> otherEntities_feedback; //<natureofentity , karma> : karma = good(0.0) -> evil(1.0) 
				std::map<IEntity*, float> user_feedback;

		};
	}
}

#endif

