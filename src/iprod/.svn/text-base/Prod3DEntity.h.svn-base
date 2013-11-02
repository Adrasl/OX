#ifndef _IPROD_3D_ENTITY_
#define _IPROD_3D_ENTITY_

#include <winsock2.h>

#include <core/IPersistence/IEntityPersistence.h>
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
		class Prod3DEntity
		{
			public:

				Prod3DEntity(core::IEntityPersistence* ent);
				virtual ~Prod3DEntity();

				std::string GetData()					{ return data;		}
				NodePath*	GetNodePath()				{ return nodepath;	}
				core::IEntityPersistence* GetEntity()	{ return entity;	}
				void SetData(const std::string &value)	{ data = value;		}
				void SetNodePath(NodePath *value);

			private:

				std::string data;
				core::IEntityPersistence* entity;
				NodePath *nodepath;

		};
	}
}

#endif

