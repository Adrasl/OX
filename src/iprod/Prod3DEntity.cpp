#include <Prod3DEntity.h>

#include <debugger.h> 

using namespace core::iprod;

Prod3DEntity::Prod3DEntity(core::IEntityPersistence* ent) : entity(ent), nodepath(NULL), collidable(false)
{
	if (entity != NULL )
	{
		data = entity->GetModelData();
	}
}

Prod3DEntity::~Prod3DEntity()
{
}

void Prod3DEntity::Delete()
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
{
	this;
}

void Prod3DEntity::OnCollisionCall(IEntity *otherEntity)
{
	this;
	otherEntity;
	Prod3DEntity *prod3dntity = (Prod3DEntity *)otherEntity;
	//retomar
}

void Prod3DEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	this;
	//retomar
}

void Prod3DEntity::PlaySound(const string &label, const bool &loop)
{
}

void Prod3DEntity::PlayAnimation(const string &label)
{}

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