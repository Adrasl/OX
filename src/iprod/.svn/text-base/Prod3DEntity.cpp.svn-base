#include <Prod3DEntity.h>

#include <debugger.h> 

using namespace core::iprod;

Prod3DEntity::Prod3DEntity(core::IEntityPersistence* ent) : entity(ent), nodepath(NULL)
{
	if (entity != NULL )
	{
		data = entity->GetModelData();
	}
}

Prod3DEntity::~Prod3DEntity()
{
}

void Prod3DEntity::SetNodePath(NodePath *value)		
{ 
	nodepath = value; 
	if ( (entity != NULL) && (nodepath != NULL) )
	{
		float posx, posy, posz, rotx, roty, rotz, scale;
		entity->GetPosition(posx, posy, posz);
		entity->GetOrientation(rotx, roty, rotz);
		entity->GetScale(scale);
		nodepath->set_pos(posx,posy,posz);
		nodepath->set_hpr(rotx, roty, rotz);
		nodepath->set_scale(scale,scale,scale);
	}
}