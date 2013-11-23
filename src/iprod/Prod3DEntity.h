#ifndef _IPROD_3D_ENTITY_
#define _IPROD_3D_ENTITY_

#include <winsock2.h>

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>

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

namespace core
{
	namespace iprod
	{
		class Prod3DEntity : public core::IEntity
		{
			public:

				Prod3DEntity();
				Prod3DEntity(core::IEntityPersistence* ent);
				virtual ~Prod3DEntity();

				std::string GetData()					{ return data;		}
				NodePath*	GetNodePath()				{ return nodepath;	}
				corePDU3D<double> GetPDU()				{ return pdu;		}
				core::IEntityPersistence* GetEntity()	{ return entity;	}
				void SetData(const std::string &value)	{ data = value;		}
				void SetPDU(const corePDU3D<double> &value){ pdu = value;	}
				void SetNodePath(NodePath *value);

				virtual void Delete();
				virtual void Load();
				virtual void Save();
				virtual void OnStart();
				virtual void OnUpdate();
				virtual void OnDestroy();
				virtual void OnCollisionCall(IEntity *otherEntity); 
				virtual void OnUserCollisionCall(core::corePDU3D<double> collisionInfo);
				virtual bool IsCollidable()						{ return collidable;}
				virtual void SetCollidable(const bool &value)	{ collidable =value;}
				//virtual void PlaySound(const std::string &label, const bool &loop);
				//virtual void PlayAnimation(const std::string &label);

			protected:

				std::string					data; //3d model file
				core::IEntityPersistence	*entity;
				NodePath					*nodepath;
				corePDU3D<double>			pdu; //pos, vel, acc
				bool						collidable;

		};
	}
}

#endif

