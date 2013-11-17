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
		class OXStandAloneEntity : public iprod::Prod3DEntity
		{
			public:

				OXStandAloneEntity(core::IEntityPersistence* ent);
				virtual ~OXStandAloneEntity();

				float GetTimeToLive()					{ return timeToLive;		}
				void  SetTimeToLive(const float &value)	{ timeToLive = value;		}

				virtual void Delete();
				virtual void OnStart();
				virtual void OnUpdate();
				virtual void OnCollisionCall(IEntity *otherEntity); 
				virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo);
				virtual void PlaySound(const std::string &label, const bool &loop);
				virtual void PlayAnimation(const std::string &label);

			private:

				float timeToLive;
				float psique; //type of entity
				float karma;  //good or evil
				float energy; //calm or energyc
				//map::<double, std::string> feedback_from_otherEntities; //<feedback, EntityLabel>, feedback < 0.50 bad, >0.50 good;

		};
	}
}

#endif

