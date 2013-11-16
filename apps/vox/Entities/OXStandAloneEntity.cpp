#include <Entities/OXStandAloneEntity.h>

#include <debugger.h> 

using namespace core::iprod;

OXStandAloneEntity::OXStandAloneEntity(core::IEntityPersistence* ent) : entity(ent), nodepath(NULL), collidable(false)
{
	if (entity != NULL )
	{
		data = entity->GetModelData();
	}
}

OXStandAloneEntity::~OXStandAloneEntity()
{
}

void OXStandAloneEntity::Delete()
{
}

void OXStandAloneEntity::SetNodePath(NodePath *value)		
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

void OXStandAloneEntity::OnStart()
{}
void OXStandAloneEntity::OnUpdate()
{
	this;
}

void OXStandAloneEntity::OnCollisionCall(IEntity *otherEntity)
{
	this;
	otherEntity;
	OXStandAloneEntity *prod3dntity = (OXStandAloneEntity *)otherEntity;
	//retomar
}

void OXStandAloneEntity::OnUserCollisionCall(core::corePDU3D<double> collisionInfo)
{
	this;
	//retomar
}

void OXStandAloneEntity::PlaySound(const string &label, const bool &loop)
{
}

void OXStandAloneEntity::PlayAnimation(const string &label)
{}

/*_collider_Ptr->add_in_pattern( "into-%in" );
global::_framework.get_event_handler().add_hook( "into-" + _collider_Ptr->get_name(), &TestFunction );

void OXStandAloneEntity::TestFunction( const Event * e )
 {
    TypedWritableReferenceCount * value = e->get_parameter(0).get_ptr();
    PT(CollisionEntry) entry = DCAST(CollisionEntry, value); 
    nassertv(entry != NULL);
    
    cerr << "Collision from " << entry->get_from_node_path() 
         << " into " << entry->get_into_node_path() << "\n";
 }*/