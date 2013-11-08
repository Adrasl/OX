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

void Prod3DEntity::OnStart()
{}
void Prod3DEntity::OnUpdate()
{}

void Prod3DEntity::OnCollisionCall(Prod3DEntity *otherEntity)
{
	//retomar, reimplementar por cada subentidad
}

void Prod3DEntity::OnUserCollisionCall(core::corePDU3D<double>)
{
	//retomar
}

/*_collider_Ptr->add_in_pattern( "into-%in" );
global::_framework.get_event_handler().add_hook( "into-" + _collider_Ptr->get_name(), &TestFunction );

void Prod3DEntity::TestFunction( const Event * e )
 {
    TypedWritableReferenceCount * value = e->get_parameter(0).get_ptr();
    PT(CollisionEntry) entry = DCAST(CollisionEntry, value); 
    nassertv(entry != NULL);
    
    cerr << "Collision from " << entry->get_from_node_path() 
         << " into " << entry->get_into_node_path() << "\n";
 }*/