#ifndef _OXBOID_ENTITY_
#define _OXBOID_ENTITY_

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
		class OXBoidsEntity : public iprod::Prod3DEntity
		{
			public:

				OXBoidsEntity(core::IEntityPersistence* ent);
				virtual ~OXBoidsEntity();

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
		};
	}
}

#endif



//#ifndef _IPROD_3D_ENTITY_
//#define _IPROD_3D_ENTITY_
//
//#include <winsock2.h>
//
//#include <core/IPersistence/IEntityPersistence.h>
//#include <core/IProd/IEntity.h>
//#include <core/types.h>
//#include <iprod/Prod3DEntity.h>
//#include <ipersistence/EntityPersistence.h>
//
//#include <controllers/ContentCreationController.h>
//
//#include <string>
//#include <vector>
//
//#include <pandaFramework.h>
//#include <pandaSystem.h>
//#include <modelPool.h>
//#include <NodePathCollection.h>
//#include <GeomVertexFormat.h>
//#include <GeomVertexData.h>
//#include <GeomVertexWriter.h>
//#include <GeomTriangles.h>
//
////#include <pandaFramework.h>
//
//using namespace core;
//
////namespace core
////{
////	namespace iprod
////	{
//		class OXBoidsEntity : public iprod::Prod3DEntity
//		{
//			public:
//
//				OXBoidsEntity(core::IEntityPersistence* ent);
//				virtual ~OXBoidsEntity();
//
//				std::string GetData()					{ return data;		}
//				NodePath*	GetNodePath()				{ return nodepath;	}
//				core::IEntityPersistence* GetEntity()	{ return entity;	}
//				void SetData(const std::string &value)	{ data = value;		}
//				void SetNodePath(NodePath *value);
//
//				virtual void Delete();
//				virtual void OnStart();
//				virtual void OnUpdate();
//				virtual void OnCollisionCall(IEntity *otherEntity); 
//				virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo);
//				virtual bool IsCollidable()						{ return collidable;}
//				virtual void SetCollidable(const bool &value)	{ collidable =value;}
//				virtual void PlaySound(const std::string &label, const bool &loop);
//				virtual void PlayAnimation(const std::string &label);
//
//			private:
//
//				float psique; // < 0.50 negative attitude, >0.50 positive Attitude
//				//map::<double, std::string> feedback_from_otherEntities; //<feedback, EntityLabel>, feedback < 0.50 bad, >0.50 good;
//
//				std::string data;
//				core::IEntityPersistence* entity;
//				NodePath *nodepath;
//				bool collidable;
//
//		};
////	}
////}
//
//#endif
//
